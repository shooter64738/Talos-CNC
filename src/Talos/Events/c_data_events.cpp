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

#include "c_data_events.h"
#include "c_motion_events.h"
#include "c_system_events.h"
#include "..\Planner\c_stager.h"
#include "..\Planner\Stager_Errors.h"
#include "..\GCode_Process.h"
#include "..\NGC_RS274\NGC_Errors.h"

BinaryRecords::s_bit_flag_controller_32 Events::Data::event_manager;
c_Serial * Events::Data::local_serial;

void Events::Data::set_events()
{
	if (Events::Data::local_serial->HasEOL())
	{
		//Set inbound serial event
		Events::Data::event_manager.set((int)Events::Data::e_event_type::Serial_data_inbound);
	}

}

void Events::Data::check_events()
{
	uint16_t return_value = 0;
	
	if (Events::Data::event_manager._flag ==0)
	{
		if (!Events::Data::event_manager.get((int)Events::Data::e_event_type::NGC_Data_Idle))
		{
			Events::Data::event_manager.set((int)Events::Data::e_event_type::NGC_Data_Idle);
			local_serial->print_string("Data idle.\r\n");
		}
		//this indicates a motion is ready to be processed by 'machine'
		return;
	}
	
	//If there is inbound serial data, check and see what type of data it is
	if (Events::Data::event_manager.get_clr((int)Events::Data::e_event_type::Serial_data_inbound)) //<--get&clear
	{
		Events::Data::event_manager.clear((int)Events::Data::e_event_type::NGC_Data_Idle);

		//If the first byte is not a printable character, this is not expected to
		//be gcode data.
		if (local_serial->Peek()<32)
		{
			//This would be a settings binary record of some type
		}
		else
		{
			//see if our buffer is full before we try.
			if (!Events::Data::event_manager.get((int)Events::Data::e_event_type::NGC_buffer_full))
			{
				
				//Get data from the specified source
				return_value = Talos::GCode_Process::load_data(NULL);
				if (Events::System::event_manager.get((int)Events::System::e_event_type::NGC_Error))
				{
					Events::System::events_statistics.num_message = return_value;
					Events::System::events_statistics.chr_message = "Interpreter error.\r\n";
					//A system error has occured. Probably poorly formed gcode data. 
					//This if block is here in case we want to act on it. Right now I
					//cant think of anything that we need to do except tell the user.
				}
			}
			else
			{
				
				local_serial->print_string("Ngc buffer full.\r\n");
			}
		}
	}
	
	//If was a new ngc item was added, handle it
	if (Events::Data::event_manager.get((int)Events::Data::e_event_type::NGC_item_added))
	{
		//When an ngc item is added we 'stage' the block. (final error check and get it prepped for the machine)
		return_value = Events::Data::process_ngc_item_added_event();
		if (return_value == Stager_Errors::OK)
		{
			//Everything passed the stager so set the event
			//Events::Motion::event_manager.set((int)Events::Motion::e_event_type::Motion_in_queue);
			//clear this event so we dont process it again.
			Events::Data::event_manager.clear((int)Events::Data::e_event_type::NGC_item_added);
		}
	}
	
	
	if (Events::Data::event_manager.get_clr((int)Events::Data::e_event_type::NGC_buffer_ready))
	{
		
	}
	
	
	if (Events::Data::event_manager.get_clr((int)Events::Data::e_event_type::Peripheral_record_in_queue))
	{
	}
	
	
	
}

uint16_t Events::Data::process_ngc_item_added_event()
{
	uint16_t return_value = Stager_Errors::OK;
	
//	if (!Events::Data::event_manager.get((int)Events::Data::e_event_type::Staging_buffer_full))
	{
		NGC_RS274::NGC_Binary_Block* current_block = c_stager::get_added_block();

		return_value = c_stager::stage_validation(current_block); //<--Currently does nothing, but you never know
		if (return_value == Stager_Errors::OK)
		{
			return_value = c_stager::stage_updates(current_block); //<--set tool id, cutter comp, parameters, etc...
			if (return_value == Stager_Errors::OK)
			{
				return_value = c_stager::stage_final(current_block);
				if (return_value == Stager_Errors::OK)
				{
					
					
				}
			}
		}
	}
	return return_value;
}

