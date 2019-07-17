/*
* c_processor.cpp
*
* Created: 5/19/2019 7:57:52 AM
* Author: Family
*/


#include "c_processor.h"
#include "hardware_def.h"
#include "c_jogger.h"
#include "..\..\..\talos.h"
#include "..\..\..\Common\Serial\c_record_handler.h"


c_Serial c_processor::coordinator_serial;
c_Serial c_processor::debug_serial;
BinaryRecords::s_peripheral_panel c_processor::peripheral_record_data;

static uint32_t state_modes = 0;

void c_processor::startup()
{
	Hardware_Abstraction_Layer::Core::initialize();
	
	Hardware_Abstraction_Layer::Manual_Pulse_Generator::initialize();
	Hardware_Abstraction_Layer::PeripheralPanel::initialize();
	c_processor::debug_serial = c_Serial(0, 115200); //<--Connect to host
	c_processor::coordinator_serial = c_Serial(1, 115200); //<--Connect to coordinator
	
	Hardware_Abstraction_Layer::Core::start_interrupts();
	c_processor::peripheral_record_data.Jogging.Axis = 0;
	c_processor::peripheral_record_data.Jogging.Scale = 0;
	c_processor::debug_serial.print_string("peripheral ready\r");
	
	
	uint32_t changed_bits = 0;
	
	while (1)
	{
		c_processor::check_panel_inputs();
		c_processor::check_serial_input();
	}
}

void c_processor::check_panel_inputs()
{
	uint8_t shift_mode = 0 ;// Hardware_Abstraction_Layer::PeripheralPanel::key_pressed(SHIFT_MODE);
	
	if (!shift_mode)
	{
		//Check for a change in the jog wheel position
		int8_t MPG_Change = Hardware_Abstraction_Layer::Manual_Pulse_Generator::check_moved();
		
		if (MPG_Change !=0 && !c_processor::get_control_state_mode(EXEC_MOTION_JOG))
		{
			c_processor::set_control_state_mode(EXEC_MOTION_JOG);
			c_processor::debug_serial.print_string("MPG move\r");
			c_jogger::send_jog(MPG_Change);
		}
		
		//See if the jog scale button was pressed
		if (Hardware_Abstraction_Layer::PeripheralPanel::key_pressed(JOG_SCALE) != 0)
		{
			c_processor::peripheral_record_data.Jogging.Scale *= 0.1;
			
			if (c_processor::peripheral_record_data.Jogging.Scale<0.001)
			{
				c_processor::peripheral_record_data.Jogging.Scale = 1;
			}
			c_processor::debug_serial.print_string("jog scale = ");
			c_processor::debug_serial.print_float(c_processor::peripheral_record_data.Jogging.Scale);
			c_processor::debug_serial.Write(CR);
		}
		
		//See if the jog axis button was pressed
		if (Hardware_Abstraction_Layer::PeripheralPanel::key_pressed(JOG_AXIS) != 0)
		{
			c_processor::debug_serial.print_string("jog axis = ");
			c_processor::peripheral_record_data.Jogging.Axis++;
			if (c_processor::peripheral_record_data.Jogging.Axis>MACHINE_AXIS_COUNT)
			{
				c_processor::peripheral_record_data.Jogging.Axis = 0;
			}
		}
	}
}

void c_processor::check_serial_input()
{
	if (c_processor::coordinator_serial.DataSize()==0)
	return;
	
	BinaryRecords::e_binary_record_types record_type = (BinaryRecords::e_binary_record_types)c_processor::coordinator_serial.Peek();
	if ((uint8_t)record_type == 255)
	{
		c_processor::coordinator_serial.Get();
	
	}
	
	if (record_type == BinaryRecords::e_binary_record_types::Status )
	{
		BinaryRecords::s_status_message mot;
		BinaryRecords::e_binary_responses resp_value = c_record_handler::handle_inbound_record(&mot,&c_processor::coordinator_serial);
		
		if (resp_value == BinaryRecords::e_binary_responses::Ok)
		{
			if (mot.system_sub_state == BinaryRecords::e_system_sub_state_record_types::Jog_Complete
			|| mot.system_sub_state == BinaryRecords::e_system_sub_state_record_types::Jog_Failed)
			{
				//BinaryRecords::s_status_message new_stat;
				//new_stat.system_state = BinaryRecords::e_system_state_record_types::Motion_Complete;
				//new_stat.system_sub_state = BinaryRecords::e_system_sub_state_record_types::Jog_Complete;
				
				c_processor::clear_control_state_mode(EXEC_MOTION_JOG);
			}
		}
	}
}
void c_processor::set_control_state_mode(uint8_t flag)
{
	BitSet_(state_modes,flag);
}

void c_processor::clear_control_state_mode(uint8_t flag)
{
	BitClr_(state_modes,flag);
}

uint8_t c_processor::get_control_state_mode(uint8_t flag)
{
	return bool (state_modes & (1UL<<flag));
}
