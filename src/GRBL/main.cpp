#include <string.h>
#include "c_gcode_plus.h"
#include "c_report.h"
#include "c_protocol.h"
#include "hardware_def.h"
#include "Motion_Core\c_segment_arbitrator.h"
/*
* AVR_2560_GRBL.cpp
*
* Created: 3/5/2019 2:53:22 PM
* Author : jeff_d
*/


int main(void)
{
	Hardware_Abstraction_Layer::Core::initialize();
	c_protocol::control_serial = c_Serial(0, 115200);
	Hardware_Abstraction_Layer::Grbl::Stepper::initialize();
	Hardware_Abstraction_Layer::Grbl::Control::initialize();
	Hardware_Abstraction_Layer::Core::start_interrupts();
	Hardware_Abstraction_Layer::Grbl::Limits::initialize();

	for (;;)
	{
		c_gcode::gc_init(); // Set g-code parser to default state
		c_report::report_init_message();
		c_protocol::protocol_main_loop();
	}
	return 0; /* Never reached */
}

