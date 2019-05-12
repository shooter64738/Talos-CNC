/*
*  c_settings.cpp - NGC_RS274 controller.
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


#include "c_general.h"
#include "..\..\..\Common\NGC_RS274\NGC_Block.h"
#include "..\c_processor.h"
#include "..\..\..\Common\Serial\records_def.h"
#include "..\..\..\common\NGC_RS274\NGC_Errors.h"
#include "..\..\..\Common\NGC_RS274\NGC_Interpreter.h"
#include "..\Machine\c_machine.h"

void Settings::c_general::initialize()
{

};

void Settings::c_general::load_from_input(uint8_t setting_group, uint8_t sub_group)
{
	//Setting group corresponds to a group of values available for the user to set on the motion controller
	//or spindle controller.

	NGC_RS274::NGC_Binary_Block local_block;
	local_block.reset();
	
	
	uint16_t record_size = c_processor::host_serial.FindByte_Position(CR);
	NGC_RS274::Interpreter::Processor::Line = c_processor::host_serial.Buffer_Pointer() + c_processor::host_serial.TailPosition();
	uint16_t return_value = NGC_RS274::Interpreter::Processor::process_line(&local_block);
	c_processor::host_serial.AdvanceTail(record_size);
	c_processor::host_serial.SkipToEOL();

	//Did the interpreter have any errors with this block of data?
	if (return_value != NGC_RS274::Interpreter::Errors::OK)
	{
		//something was bad on the interpreter processing. Ignore these values.
		return;
	}
	float fAddress = 0.0;
	record_size = 0;
	//Motion Control Group Settings
	if (setting_group == 'M')
	{
		//sub groups are categorized like this: A=accel, B=backlash,M=Max rate,S=steps per mm,
		//P=Pulse length, J=Junction Deviation, T=Arc Tolerance
		c_processor::host_serial.print_string("Updating motion control ");

		if (sub_group == 'A')
		{
			c_processor::host_serial.print_string("acceleration\r");
			for (int i = 0; i < N_AXIS; i++)
			{
				if (local_block.get_defined(c_machine::machine_axis_names[i]))
				{
					c_processor::motion_control_setting_record.acceleration[i] = ((*local_block.axis_values.Loop[i]) *60 *60);
				}
			}
		}
		else if (sub_group == 'B')
		{
			c_processor::host_serial.print_string("backlash\r");
			for (int i = 0; i < N_AXIS; i++)
			{
				if (local_block.get_defined(c_machine::machine_axis_names[i]))
				{
					c_processor::motion_control_setting_record.back_lash_comp_distance[i] = *local_block.axis_values.Loop[i];
				}
			}
		}
		else if (sub_group == 'M')
		{
			c_processor::host_serial.print_string("max rate\r");
			for (int i = 0; i < N_AXIS; i++)
			{
				if (local_block.get_defined(c_machine::machine_axis_names[i]))
				{
					c_processor::motion_control_setting_record.max_rate[i] = *local_block.axis_values.Loop[i];
				}
			}
		}
		else if (sub_group == 'S')
		{
			c_processor::host_serial.print_string("steps per mm\r");
			for (int i = 0; i < N_AXIS; i++)
			{
				if (local_block.get_defined(c_machine::machine_axis_names[i]))
				{
					c_processor::motion_control_setting_record.steps_per_mm[i] = (int16_t)*local_block.axis_values.Loop[i];
				}
			}
		}
		if (local_block.get_value_defined('P', fAddress))
		{
			c_processor::host_serial.print_string("pulse length\r");
			c_processor::motion_control_setting_record.pulse_length = fAddress;
		}
		if (local_block.get_value_defined('J', fAddress))
		{
			c_processor::host_serial.print_string("junction deviation\r");
			c_processor::motion_control_setting_record.junction_deviation = fAddress;
		}
		if (local_block.get_value_defined('T', fAddress))
		{
			c_processor::host_serial.print_string("arc tolerance\r");
			c_processor::motion_control_setting_record.arc_tolerance = fAddress;
		}
		char setting_stream[sizeof(BinaryRecords::s_motion_control_settings)];
		record_size = sizeof(BinaryRecords::s_motion_control_settings);
		//copy updated block to stream
		memcpy(setting_stream, &c_processor::motion_control_setting_record, record_size);
		
		//memcpy(&motion_control_setting_record,setting_stream, record_size);	
		//c_processor::host_serial.print_string("motion_control_setting_record.record_type = "); c_processor::host_serial.print_int32((uint32_t)motion_control_setting_record.record_type); c_processor::host_serial.Write(CR);
		//for (uint8_t i = 0; i < N_AXIS; i++)
		//{
			//c_processor::host_serial.print_string("motion_control_setting_record.steps_per_mm[");
			//c_processor::host_serial.print_int32(i);
			//c_processor::host_serial.print_string("] = ");
			//c_processor::host_serial.print_float(motion_control_setting_record.steps_per_mm[i], 2);
			//c_processor::host_serial.Write(CR);
			//
			//
			//c_processor::host_serial.print_string("motion_control_setting_record.acceleration[");
			//c_processor::host_serial.print_int32(i);
			//c_processor::host_serial.print_string("] = ");
			//c_processor::host_serial.print_float(motion_control_setting_record.acceleration[i], 2);
			//c_processor::host_serial.Write(CR);
			//
			//c_processor::host_serial.print_string("motion_control_setting_record.max_rate[");
			//c_processor::host_serial.print_int32(i);
			//c_processor::host_serial.print_string("] = ");
			//c_processor::host_serial.print_float(motion_control_setting_record.max_rate[i], 2);
			//c_processor::host_serial.Write(CR);
			//
			//c_processor::host_serial.print_string("motion_control_setting_record.back_lash_comp_distance[");
			//c_processor::host_serial.print_int32(i);
			//c_processor::host_serial.print_string("] = ");
			//c_processor::host_serial.print_float(motion_control_setting_record.back_lash_comp_distance[i], 2);
			//c_processor::host_serial.Write(CR);
		//}
		//c_processor::host_serial.print_string("motion_control_setting_record.pulse_length = ");
		//c_processor::host_serial.print_int32(motion_control_setting_record.pulse_length);
		//c_processor::host_serial.Write(CR);
		//c_processor::host_serial.Write(CR);
		
		if (write_stream(setting_stream, record_size) == 0)
		{
			c_processor::host_serial.print_string("success.\r");
		}
		else
		{
			c_processor::host_serial.print_string("failed!\r");
		}
	}
}

uint8_t Settings::c_general::write_stream(char * stream, uint8_t record_size)
{
	//Send to motion controller
	uint8_t send_count = 0;
	while (1)
	{
		if (send_count > 4)
		{
			//We tried 4 times to send the record and it kept failing.. SUPER bad..
			return 2;
		}
		c_processor::controller_serial.Write_Record(stream, record_size);
		send_count++;
		//Now we need to wait for the motion controller to confirm it go the data
		if (c_processor::controller_serial.WaitFOrEOL(90000)) //<-- wait until the timeout
		{
			//We timed out. this is bad...
			return 1;
		}
		else
		{
			//get the response code from the controller
			uint8_t resp = c_processor::controller_serial.Get();
			//there should be a cr after this, we can throw it away
			c_processor::controller_serial.Get();
			//If we get a proceed resp, we can break the while. we are done.
			if (resp == SER_ACK_PROCEED)
			{
				break;
			}

			//if we get to here, we didnt get an ack and we need to resend.
			send_count++;
		}
	}
	return 0;
}

//// default constructor
//c_settings::c_settings()
//{
//} //c_settings
//
//// default destructor
//c_settings::~c_settings()
//{
//} //~c_settings
