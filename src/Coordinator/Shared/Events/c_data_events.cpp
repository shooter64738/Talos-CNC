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
		uint16_t record_size = 0;
		//c_processor::host_serial.print_string("got data!\r");
		//get the record type
		BinaryRecords::e_binary_record_types record_type = (BinaryRecords::e_binary_record_types)c_processor::peripheral_serial.Peek();
		if (record_type == BinaryRecords::e_binary_record_types::Jog)
		{
			record_size= sizeof(BinaryRecords::s_jog_data_block);
			c_processor::host_serial.print_string("writing ");
			c_processor::host_serial.print_int32(record_size);
			c_processor::host_serial.print_string(" bytes\r");
			//First byte indicates record type of motion. Make sure its all there before we start loading it.
			if (c_processor::peripheral_serial.HasRecord(record_size))
			{
				//clear the peripheral record event
				c_data_events::clear_event(e_Data_Events::Peripheral_Record_InQueue);
				BinaryRecords::s_jog_data_block jog_block;
				//Clear the struct
				memset(&jog_block, 0, record_size);
				//Put the stream into the struct
				memcpy(&jog_block
				, c_processor::peripheral_serial.Buffer_Pointer() + c_processor::peripheral_serial.TailPosition()
				, record_size);
				
				//Tell the peripheral controller we got the data
				c_processor::peripheral_serial.Write((char)BinaryRecords::e_binary_responses::Ok);c_processor::peripheral_serial.Write(CR);
				
				if (c_motion_controller::send_jog(jog_block) == BinaryRecords::e_binary_responses::Response_Time_Out)
				{
					c_processor::host_serial.print_string("Jog ACK error\r");
				}
				else
				{
					//Tell the peripheral controller the jog is complete
					c_processor::peripheral_serial.Write((char)BinaryRecords::e_binary_responses::Jog_Complete);c_processor::peripheral_serial.Write(CR);
					//c_processor::host_serial.print_string("Jog Success\r");
				}
				c_processor::peripheral_serial.Reset();
				//return BinaryRecords::e_binary_record_types::Jog;
			}
		}
		if (record_type == BinaryRecords::e_binary_record_types::Peripheral_Control_Setting)
		{
			
		}
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
