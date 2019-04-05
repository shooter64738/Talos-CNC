/*
 * MotionDriver.cpp
 *
 * Created: 4/3/2019 1:32:16 PM
 * Author : Family
 */ 

#include <avr/io.h>
#include "../Common/NGC_RS274/NGC_Interpreter.h"
#include "../Common/NGC_RS274/NGC_Block.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_core_avr_2560.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560_stepper.h"
#include "../GRBL/Motion_Core/c_segment_arbitrator.h"
#include "../GRBL/Motion_Core/c_interpollation_software.h"
#include "../GRBL/Motion_Core/c_motion_core.h"
//#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560_control.h"
//#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560_limits.h"


int main(void)
{
	Hardware_Abstraction_Layer::Core::initialize();
	//c_protocol::control_serial = c_Serial(0, 115200);
	Hardware_Abstraction_Layer::Grbl::Stepper::initialize();
	//Hardware_Abstraction_Layer::Grbl::Control::initialize();
	Hardware_Abstraction_Layer::Core::start_interrupts();
	//Hardware_Abstraction_Layer::Grbl::Limits::initialize();
Motion_Core::initialize();

    NGC_RS274::NGC_Binary_Block block = NGC_RS274::NGC_Binary_Block();
    NGC_RS274::Interpreter::Processor::Line[0] = 'G';
    NGC_RS274::Interpreter::Processor::Line[1] = '0';
    NGC_RS274::Interpreter::Processor::Line[2] = 'X';
    NGC_RS274::Interpreter::Processor::Line[3] = '1';
	NGC_RS274::Interpreter::Processor::Line[4] = '0';
    NGC_RS274::Interpreter::Processor::Line[5] = '\r';
    NGC_RS274::Interpreter::Processor::process_line(&block);
	Motion_Core::Software::Interpollation::load_block(&block);
    while (1) 
    {
		Motion_Core::Segment::Arbitrator::st_prep_buffer();
    }
}

