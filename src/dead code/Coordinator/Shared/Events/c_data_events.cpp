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

void c_data_events::send_status(BinaryRecords::e_system_state_record_types state
,BinaryRecords::e_system_sub_state_record_types sub_state
,c_Serial outgoing_serial)
{
	BinaryRecords::s_status_message new_stat;
	new_stat.system_state = state;
	new_stat.system_sub_state = sub_state;
	
	BinaryRecords::e_binary_responses resp =
	c_record_handler::handle_outbound_record(&new_stat,outgoing_serial);
}

BinaryRecords::e_binary_responses c_data_events::handle_periperal_record()
{
	//Clear the flag indicating theres a record to service
	c_data_events::clear_event(e_Data_Events::Peripheral_Record_InQueue);
	//Load the peripheral record type from serial
	BinaryRecords::e_binary_record_types record_type = (BinaryRecords::e_binary_record_types)c_processor::peripheral_serial.Peek();
	//Test for junk record
	if ((uint8_t)record_type == 255)
	{
		c_processor::controller_serial.Get();
		return BinaryRecords::e_binary_responses::Ok;
	}
	
	//Peripheral has sent a jog request
	if (record_type == BinaryRecords::e_binary_record_types::Jog )
	{
		c_processor::host_serial.print_string("send jog\r");
		//Setup to receive jog information
		BinaryRecords::s_jog_data_block mot;
		//Load the peripheral record data from serial. We must pull this serial
		//data off to reset the serial buffer, even if we arent going to use it
		BinaryRecords::e_binary_responses resp_value = c_record_handler::handle_inbound_record(&mot,&c_processor::peripheral_serial);
		//Already running a jog? (This should not occur because peripheral wont
		//send another jog command until the previous command completes.)
		if (c_motion_events::get_event(Motion_Events::JOG_IN_QUEUE))
		{
			return BinaryRecords::e_binary_responses::Ok;
		}
		//If the read produced an error we need to notify peripheral
		if (resp_value != BinaryRecords::e_binary_responses::Ok)
		{
			c_motion_events::clear_event(Motion_Events::JOG_IN_QUEUE);
			c_data_events::send_status(BinaryRecords::e_system_state_record_types::Motion_Discarded
			,BinaryRecords::e_system_sub_state_record_types::Jog_Failed
			,c_processor::peripheral_serial);
			return resp_value;
		}
		//Jog requests get forwarded to the motion controller. Send the request on.
		resp_value = c_record_handler::handle_outbound_record
		(&mot,c_processor::controller_serial);
		//If motion controller had no errors reading our request, set the jog in the queue and wait.
		if (resp_value == BinaryRecords::e_binary_responses::Ok)
		{
			c_motion_events::set_event(Motion_Events::JOG_IN_QUEUE);
			return BinaryRecords::e_binary_responses::Ok;
		}
		else
		{
			//Motion controller could not process our jog request. Notify peripheral of failure.
			c_data_events::send_status(BinaryRecords::e_system_state_record_types::Motion_Discarded
			,BinaryRecords::e_system_sub_state_record_types::Jog_Failed
			,c_processor::peripheral_serial);
			return BinaryRecords::e_binary_responses::Ok;
		}
		
	}
}

BinaryRecords::e_binary_responses c_data_events::handle_controller_record()
{
	c_data_events::clear_event(e_Data_Events::Motion_Record_InQueue);
	//Load the record type from the controller
	BinaryRecords::e_binary_record_types record_type = (BinaryRecords::e_binary_record_types)c_processor::controller_serial.Peek();
	if ((uint8_t)record_type == 255)
	{
		c_processor::controller_serial.Get();
		return BinaryRecords::e_binary_responses::Ok;
	}
	
	if (record_type == BinaryRecords::e_binary_record_types::Status )
	{
		//Setup to receive status record
		BinaryRecords::s_status_message mot;
		//Load the control record data from serial. We must pull this serial
		//data off to reset the serial buffer, even if we arent going to use it
		BinaryRecords::e_binary_responses resp_value = c_record_handler::handle_inbound_record(&mot,&c_processor::controller_serial);
		//If the read produced an error we need to notify controller
		if (resp_value != BinaryRecords::e_binary_responses::Ok)
		{
			//Status records cant be resent because they change constantly on the controller.
			
			//c_data_events::send_status(BinaryRecords::e_system_state_record_types::Motion_Discarded
			//,BinaryRecords::e_system_sub_state_record_types::Jog_Failed
			//,c_processor::controller_serial);
			return BinaryRecords::e_binary_responses::Ok;
		}
		
		//Debug print the status data for testing
		//********************************************************************************
		c_processor::host_serial.print_string("***controller status:\r");
		c_processor::host_serial.print_string("\tstate = ");
		c_processor::host_serial.print_int32((int32_t)mot.system_state);c_processor::host_serial.Write(CR);
		c_processor::host_serial.print_string("\tsub state = ");
		c_processor::host_serial.print_int32((int32_t)mot.system_sub_state);c_processor::host_serial.Write(CR);
		c_processor::host_serial.print_string("\tnum message = ");
		c_processor::host_serial.print_int32(mot.num_message);c_processor::host_serial.Write(CR);
		c_processor::host_serial.print_string("\tchr message = ");
		c_processor::host_serial.print_string(mot.chr_message);c_processor::host_serial.Write(CR);
		for (uint8_t i=0;i<MACHINE_AXIS_COUNT;i++)
		{
			c_processor::host_serial.print_string("Axis ");c_processor::host_serial.print_int32(i);
			c_processor::host_serial.print_string("= ");c_processor::host_serial.print_float(mot.position[i],3);
			c_processor::host_serial.Write(CR);
		}
		c_processor::host_serial.print_string("*********************\r");
		//********************************************************************************
		
		//Determine what the status is telling us
		switch (mot.system_sub_state)
		{
			case BinaryRecords::e_system_sub_state_record_types::Block_Complete:
			{
				c_motion_events::set_event(Motion_Events::MOTION_COMPLETE);
				c_motion_events::last_reported_block = (uint32_t) mot.num_message;
			}
			break;
			case BinaryRecords::e_system_sub_state_record_types::Jog_Complete:
			{
				c_motion_events::clear_event(Motion_Events::JOG_IN_QUEUE);
				c_data_events::send_status(BinaryRecords::e_system_state_record_types::Motion_Complete
				,BinaryRecords::e_system_sub_state_record_types::Jog_Complete
				,c_processor::peripheral_serial);
			}
			break;
			default:
			{
				
			}
			break;
		}
		
	}
	
	//
	//c_processor::controller_serial.Reset();
	//
	//}
}
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
	
	//If there is a peripheral record, handle it
	if (c_data_events::get_event(e_Data_Events::Peripheral_Record_InQueue))
	{
		c_data_events::handle_periperal_record();
	}
	
	//If there is a controller record, handle it
	if (c_data_events::get_event(e_Data_Events::Motion_Record_InQueue))
	{
		c_data_events::handle_controller_record();
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
