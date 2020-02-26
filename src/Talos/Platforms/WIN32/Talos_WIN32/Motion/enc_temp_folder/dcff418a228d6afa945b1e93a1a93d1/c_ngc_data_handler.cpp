/*
*  c_data_events.cpp - NGC_RS274 controller.
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
#include "c_ngc_data_handler.h"
#include <string.h>
#include "../../Events/extern_events_types.h"
#include "../../../../Shared Data/_s_motion_record.h"


static s_motion_data_block loaded_block;
static uint8_t write_count = 0;

void(*c_ngc_data_handler::pntr_data_handler_release)(c_ring_buffer<char> * buffer);

ret_pointer c_ngc_data_handler::assign_read_handler(c_ring_buffer <char> * buffer)
{
	return c_ngc_data_handler::ngc_read_handler;
}

ret_pointer c_ngc_data_handler::assign_write_handler(c_ring_buffer <char> * buffer)
{
	//Convert the loaded_block that is in here into a a stream. 
	//place the stream into the buffer. the write handler will do the rest.
	
	//If this is the first time here, the blocks station value will be zero. 
	//The station value indicates which record we are expecting to get from the coordiantor.
	loaded_block.station++;
	write_count = loaded_block._size;
	memcpy(buffer->_storage_pointer, &loaded_block, loaded_block._size);
	return c_ngc_data_handler::ngc_write_handler;
}

void c_ngc_data_handler::ngc_read_handler(c_ring_buffer <char> * buffer)
{
	buffer->pntr_write(1, *buffer->_storage_pointer++);
	write_count--;
	//when write count reaches zero we have written all data for the record
	if (!write_count)
	{
		//for good measure, lets reset the buffer
		buffer->reset();
		//call the release method.. Remember back in the serial handler we assigned a call back function to release?
		c_ngc_data_handler::__release(buffer);
	}
}

void c_ngc_data_handler::ngc_write_handler(c_ring_buffer <char> * buffer)
{
	buffer->pntr_write(1, *buffer->_storage_pointer++);
	write_count--;
	//when write count reaches zero we have written all data for the record
	if (!write_count)
	{
		//for good measure, lets reset the buffer
		buffer->reset();
		//call the release method.. Remember back in the serial handler we assigned a call back function to release?
		c_ngc_data_handler::__release(buffer);
	}
}

void c_ngc_data_handler::__release(c_ring_buffer <char> * buffer_source)
{
	//release the handler because we should be done with it now, but pass a flag in indicating if
	//there is more data to read from this buffer
	c_ngc_data_handler::pntr_data_handler_release(buffer_source);
	//set the handler release to null now. we dont need it
	c_ngc_data_handler::pntr_data_handler_release = NULL;
}
