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


c_Serial c_processor::coordinator_serial;
c_Serial c_processor::debug_serial;
BinaryRecords::s_peripheral_panel c_processor::peripheral_record_data;

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
	}
}

void c_processor::check_panel_inputs()
{
	uint8_t shift_mode = Hardware_Abstraction_Layer::PeripheralPanel::key_pressed(SHIFT_MODE);
	
	if (!shift_mode)
	{
		//Check for a change in the jog wheel position
		int8_t MPG_Change = Hardware_Abstraction_Layer::Manual_Pulse_Generator::check_moved();
		
		if (MPG_Change !=0)
		{
			c_processor::debug_serial.print_string("MPG move\r");
			c_jogger::send_jog(MPG_Change);
		}
		
		//See if the jog scale button was pressed
		if (Hardware_Abstraction_Layer::PeripheralPanel::key_pressed(JOG_SCALE) != 0)
		{
			c_processor::peripheral_record_data.Jogging.Scale = c_processor::peripheral_record_data.Jogging.Scale/10.0;
			if (c_processor::peripheral_record_data.Jogging.Scale<0.001)
			{
				c_processor::peripheral_record_data.Jogging.Scale = 1;
			}
		}
		
		//See if the jog axis button was pressed
		if (Hardware_Abstraction_Layer::PeripheralPanel::key_pressed(JOG_AXIS) != 0)
		{
			c_processor::peripheral_record_data.Jogging.Axis++;
			if (c_processor::peripheral_record_data.Jogging.Axis>MACHINE_AXIS_COUNT)
			{
				c_processor::peripheral_record_data.Jogging.Axis = 0;
			}
		}
	}
}