/*
*  c_gcode_buffer.cpp - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "c_gcode_buffer.h"
#include "..\Events\c_data_events.h"
#include "..\c_processor.h"
#include "Stager_Errors.h"
#include "..\status\c_status.h"
#include "..\..\..\Common\NGC_RS274\NGC_Interpreter.h"
#include "c_stager.h"
c_block c_gcode_buffer::collection[NGC_BUFFER_SIZE];
uint8_t c_gcode_buffer::buffer_head=0;
uint8_t c_gcode_buffer::buffer_tail=0;
uint8_t c_gcode_buffer::queued_count = 0;

uint8_t c_gcode_buffer::initialize()
{
	//buffer head/tail are moed from 0 to 1 initialy. the 0 position holds the start up default values. 
	c_gcode_buffer::buffer_head=1;
	c_gcode_buffer::buffer_tail=1;
	c_gcode_buffer::queued_count = 0;
	return 0;
}

/**
* \brief
* Returns the count of items in the queue
*
* \return uint8_t
*/
uint8_t c_gcode_buffer::count()
{
	//If anything is in the queue buffer, tail and head will not match.
	return c_gcode_buffer::buffer_head +1;
}

/**
* \brief
* Returns the index number of the current block for the PLANNER.
* Returns -1 if no data is available in the PLANNER
*
* \return int8_t
*/
c_block *c_gcode_buffer::get()
{
	
	//Do we have anything queued right now?
	if (c_gcode_buffer::buffer_tail == c_gcode_buffer::buffer_head && !c_data_events::get_event(Data_Events::NGC_BUFFER_FULL))
	{
		//There is nothing queued so we cannot have an active block
		return NULL;
	}
	c_gcode_buffer::queued_count--;
	//If there was a pending full event clear it.
	//its faster to just run this instead of an if operation to check, then conditionally call it.
	c_data_events::clear_event(Data_Events::NGC_BUFFER_FULL);
	//return what is at the tail and move the tail forward one. If we are on the last item, we will reset the tail
	c_block *data = &collection[c_gcode_buffer::buffer_tail];
	c_gcode_buffer::buffer_tail++;
	if (c_gcode_buffer::buffer_tail == NGC_BUFFER_SIZE)
	c_gcode_buffer::buffer_tail=0;
	return data;
	
}

/**
* \brief
* Dual purpose method. If there is an open slot to add planner data, this will return the
* head position. If a value <0 is returned then we can not add yet.
* Returns 0-(BUFFER_SIZE-1) if we can add. -1 if data cannot be added.
*
* \return int8_t
*/
int8_t c_gcode_buffer::can_add()
{
	return 0;
	
	//If adding 1 to the buffer makes it equal the buffer size we will be exceeding the buffer. Check to see if it can wrap
	if (c_gcode_buffer::buffer_head + 1 > NGC_BUFFER_SIZE)
	{
		//If wrapping and adding 1 will exceed the tail, then we are full
		if ((c_gcode_buffer::buffer_head-NGC_BUFFER_SIZE) + 1 >= c_gcode_buffer::buffer_tail)
		{
			return -1;
		}
		//We are wrapping but we wont be over writing the tail data.
		c_gcode_buffer::buffer_head = 0;
	}
	//If the above code does not cause a return, check to see if we are not going to wrap and if we will exceed the tail.
	if (c_gcode_buffer::buffer_tail == c_gcode_buffer::buffer_head+1)
	{
		return -1;
	}

	//If neither condition above causes a return -1, then we should be good to add to the buffer.
	return c_gcode_buffer::buffer_head;
}

/**
* \brief
* Converts a raw char[] of gcode into an NGC equivalent binary data and stores it in the planner buffer
* Assumes buffer space is checked before calling to add the data.
* \return int16_t
*/
int16_t c_gcode_buffer::add()
{
	c_block * local_block = &c_gcode_buffer::collection[c_gcode_buffer::buffer_head];

	/*
	Copy forward the states into the next block from the last modal states, and persisted values
	As the line data is processed in the interpreter, some (or all of these values) may be changed.
	We are only defaulting these to what they are now, so modal states and persisted values are retained
	
	This had to be moved to here, in case an error occurs in the interpreter and the block gets reset.
	If we do not reload these values each time, an error in the interpreter will cause the persisted
	states to be lost.
	*/
	memcpy(local_block->g_group, c_stager::previous_block->g_group, COUNT_OF_G_CODE_GROUPS_ARRAY*sizeof(uint16_t));
	memcpy(local_block->m_group, c_stager::previous_block->m_group, COUNT_OF_M_CODE_GROUPS_ARRAY*sizeof(uint16_t));
	memcpy(&local_block->persisted_values, &c_stager::previous_block->persisted_values, sizeof(c_block::s_persisted_values));
	memcpy(local_block->block_word_values, c_stager::previous_block->block_word_values, COUNT_OF_BLOCK_WORDS_ARRAY * sizeof(float));
	
	//Clear the non modal codes if there were any. These would have been carried over by the stager, and non modals are 'not modal' obviously
	local_block->g_group[NGC_RS274::Groups::G::NON_MODAL] = 0;
	
	//Process data and convert it into NGC binary data.
	//The data will go into the buffer head position
	//uint16_t return_value = c_interpreter::process_serial(local_block);
	//prime the interpreter 'Line' value
	uint8_t index = 0;
	while (c_processor::host_serial.Peek() != CR && c_processor::host_serial.Peek() != NULL)
	{
		NGC_RS274::Interpreter::Processor::Line[index] =  toupper(c_processor::host_serial.Get());
		index++;
	}
	//did the buffer end with CR or NULL?
	if (c_processor::host_serial.Peek() == CR)
		c_processor::host_serial.Get();

	uint16_t return_value = NGC_RS274::Interpreter::Processor::process_line(local_block);
	
	//Did the interpreter have any errors with this block of data?
	if (return_value!= NGC_RS274::Interpreter::Errors::OK)
	{
		//We had an interpreter error.
		//send the error code
		c_status::error('i',return_value,NGC_RS274::Interpreter::Processor::Line);
		/*
		since we could not process this block theres nothing else for us to do with it.
		we should notify the host of the error though.
		*/
		return return_value;
	}
	
	//No errors from interpreter so increment the head. That keeps it ready for the next data block when it comes in.
	//If we are already at the last position for head and we go past it now, it will get reset on the next 'add'
	c_gcode_buffer::buffer_head++;
	
	//This buffer must be a 2's compliment size so that bit wise & will fold it over.
	if (c_gcode_buffer::buffer_head == NGC_BUFFER_SIZE)
	c_gcode_buffer::buffer_head = 0;
	
	if (c_gcode_buffer::buffer_head == c_gcode_buffer::buffer_tail)
	{
		c_status::error('i',Stager_Errors::NGC_Buffer_Full,"ngc buffer full");
		//We set this event so that the caller can check for a buffer full condition before calling add.
		c_data_events::set_event(Data_Events::NGC_BUFFER_FULL);
		//Rollover the tail.. This might not be good...
		//++c_gcode_buffer::buffer_tail &= PLANNER_BLOCK_BUFFER_SIZE-1;
	}

	return return_value;
}

void c_gcode_buffer::drop()
{
	//c_gcode_buffer::queued_count--;
	//c_gcode_buffer::buffer_tail++;
}
//
//// default constructor
//c_buffer::c_buffer()
//{
//} //c_buffer
//
//// default destructor
//c_buffer::~c_buffer()
//{
//} //~c_buffer
