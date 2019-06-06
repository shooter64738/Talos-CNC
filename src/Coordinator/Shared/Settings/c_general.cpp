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
#include "..\..\..\common\NGC_RS274\NGC_Errors.h"
#include "..\..\..\Common\NGC_RS274\NGC_Interpreter.h"
#include "..\Machine\c_machine.h"
#include "..\MotionController\c_motion_controller.h"
#include "..\..\..\Common\Serial\c_record_handler.h"

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
			for (int i = 0; i < MACHINE_AXIS_COUNT; i++)
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
			for (int i = 0; i < MACHINE_AXIS_COUNT; i++)
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
			for (int i = 0; i < MACHINE_AXIS_COUNT; i++)
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
			for (int i = 0; i < MACHINE_AXIS_COUNT; i++)
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
		
		BinaryRecords::e_binary_responses resp = Settings::c_general::update_controller(&c_processor::motion_control_setting_record);
		if (resp == BinaryRecords::e_binary_responses::Ok)
		{
			c_processor::host_serial.print_string("success.\r");
		}
		else
		{
			c_processor::host_serial.print_string("failed!\r");
		}
	}
}

BinaryRecords::e_binary_responses Settings::c_general::update_controller(BinaryRecords::s_motion_control_settings * settings)
{
	return c_record_handler::handle_outbound_record(settings, c_processor::controller_serial);
	
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
