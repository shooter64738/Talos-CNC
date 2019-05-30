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
#include "..\c_processor.h"
#include "..\MotionController\c_motion_controller.h"
#include "..\..\..\Common\Serial\records_def.h"
#include "c_motion_events.h"
#include "..\..\..\Common\Serial\c_record_handler.h"

uint32_t c_data_events::event_flags;


void c_data_events::check_events()
{
	
	if (c_data_events::event_flags ==0)
	{
		return;
	}
	//c_processor::host_serial.print_string("e check\r");
	if (c_data_events::get_event(e_Data_Events::NGC_BUFFER_READY))
	{
		
	}
	
	if (c_data_events::get_event(e_Data_Events::Peripheral_Record_InQueue))
	{
		c_data_events::clear_event(e_Data_Events::Peripheral_Record_InQueue);
		BinaryRecords::e_binary_record_types record_type = (BinaryRecords::e_binary_record_types)c_processor::peripheral_serial.Peek();
		if ((uint8_t)record_type == 255)
		{
			c_processor::controller_serial.Get();
			c_data_events::clear_event(e_Data_Events::Peripheral_Record_InQueue);
		}
		c_processor::host_serial.print_string("rec type = ");
		c_processor::host_serial.print_int32((int)record_type);
		
		if (record_type == BinaryRecords::e_binary_record_types::Jog )
		{
			BinaryRecords::s_jog_data_block mot;
			BinaryRecords::e_binary_responses resp_value = c_record_handler::handle_inbound_record(&mot,&c_processor::peripheral_serial);
			if (c_motion_events::get_event(Motion_Events::JOG_IN_QUEUE))
			{
				c_processor::host_serial.print_string("jog in progress. skipping\r");
				return;
			}
			c_motion_events::set_event(Motion_Events::JOG_IN_QUEUE);
			
			if (resp_value == BinaryRecords::e_binary_responses::Check_Sum_Failure)
			{
				c_data_events::clear_event(e_Data_Events::Peripheral_Record_InQueue);
			}
			if (resp_value == BinaryRecords::e_binary_responses::Ok)
			{
				c_data_events::clear_event(e_Data_Events::Peripheral_Record_InQueue);
				
				c_processor::host_serial.print_string("jog forward\r");
				BinaryRecords::e_binary_responses resp_value = c_record_handler::handle_outbound_record(&mot,c_processor::controller_serial);
			}
		}
	}
	if (c_data_events::get_event(e_Data_Events::Motion_Record_InQueue))
	{
		BinaryRecords::e_binary_record_types record_type = (BinaryRecords::e_binary_record_types)c_processor::controller_serial.Peek();
		if ((uint8_t)record_type == 255)
		{
			c_processor::controller_serial.Get();
			c_data_events::clear_event(e_Data_Events::Motion_Record_InQueue);
		}
		
		if (record_type == BinaryRecords::e_binary_record_types::Status )
		{
			BinaryRecords::s_status_message mot;
			BinaryRecords::s_status_message *p_mot = &mot;
			
			BinaryRecords::e_binary_responses resp_value = c_record_handler::handle_inbound_record(p_mot,&c_processor::controller_serial);
			
			if (resp_value == BinaryRecords::e_binary_responses::Check_Sum_Failure)
			{
				c_data_events::clear_event(e_Data_Events::Motion_Record_InQueue);
			}
			if (resp_value == BinaryRecords::e_binary_responses::Ok)
			{
				c_data_events::clear_event(e_Data_Events::Motion_Record_InQueue);
				
				//c_processor::host_serial.print_string("controller status:\r");
				//c_processor::host_serial.print_string("\tstate = ");
				//c_processor::host_serial.print_int32((int32_t)p_mot->system_state);c_processor::host_serial.Write(CR);
				//c_processor::host_serial.print_string("\tsub state = ");
				//c_processor::host_serial.print_int32((int32_t)p_mot->system_sub_state);c_processor::host_serial.Write(CR);
				//c_processor::host_serial.print_string("\tnum message = ");
				//c_processor::host_serial.print_int32(p_mot->num_message);c_processor::host_serial.Write(CR);
				//c_processor::host_serial.print_string("\tchr message = ");
				//c_processor::host_serial.print_string(p_mot->chr_message);c_processor::host_serial.Write(CR);
				
				if (p_mot->system_sub_state == BinaryRecords::e_system_sub_state_record_types::Block_Complete)
				{
					c_motion_events::set_event(Motion_Events::MOTION_COMPLETE);
					c_motion_events::last_reported_block = (uint32_t) p_mot->num_message;
				}
				else if (p_mot->system_sub_state == BinaryRecords::e_system_sub_state_record_types::Jog_Complete)
				{
					c_motion_events::clear_event(Motion_Events::JOG_IN_QUEUE);
					
					BinaryRecords::s_status_message new_stat;
					new_stat.system_state = BinaryRecords::e_system_state_record_types::Motion_Complete;
					new_stat.system_sub_state = BinaryRecords::e_system_sub_state_record_types::Jog_Complete;
					
					BinaryRecords::e_binary_responses resp =
					c_record_handler::handle_outbound_record(&new_stat,c_processor::peripheral_serial);
					//c_processor::host_serial.print_string("jog back ack\r");
					//c_processor::host_serial.print_int32((int)resp);
					//c_processor::host_serial.print_string("\r");
					
					
				}
			}
		}
		
		//
		//c_processor::controller_serial.Reset();
		//
		//}
	}
	else
	{
		//trashed record.
		c_processor::controller_serial.Reset();
		c_data_events::clear_event(e_Data_Events::Motion_Record_InQueue);
	}
}



void c_data_events::set_event(e_Data_Events EventFlag)
{
	c_data_events::event_flags=(BitSet(c_data_events::event_flags,((int)EventFlag)));
}

uint8_t c_data_events::get_event(e_Data_Events EventFlag)
{
	return (BitGet(c_data_events::event_flags,((int)EventFlag)));
}

void c_data_events::clear_event(e_Data_Events EventFlag)
{
	c_data_events::event_flags=BitClr(c_data_events::event_flags,((int)EventFlag));
}

//// default constructor
//c_data_events::c_data_events()
//{
//} //c_data_events
//
//// default destructor
//c_data_events::~c_data_events()
//{
//} //~c_data_events
