/*
* c_processor.cpp
*
* Created: 2/12/2019 3:40:46 PM
* Author: jeff_d
*/


#include "c_processor.h"
#include "../Common/Hardware_Abstraction_Layer/c_hal.h"
#include "c_encoder.h"
#include "c_driver.h"
#include "../Common/NGC_RS274/NGC_Interpreter.h"
#define CONTROL_TYPE_SPINDLE
c_Serial Spindle_Controller::c_processor::host_serial;

void Spindle_Controller::c_processor::startup()
{
	c_hal::initialize();
	Spindle_Controller::c_processor::host_serial = c_Serial(0, 115200); //<--Connect to host
	Spindle_Controller::c_driver::initialize();
	Spindle_Controller::c_encoder::initialize(400);
	Spindle_Controller::c_processor::host_serial.print_string("spindle on line");
	
	NGC_RS274::Interpreter::Processor::initialize();
	
	while (1)
	{
		Spindle_Controller::c_processor::host_serial.print_float(Spindle_Controller::c_encoder::current_rpm());
		Spindle_Controller::c_processor::host_serial.print_string("rpm  ");
		Spindle_Controller::c_processor::host_serial.Write(CR);
	}
}

//// default constructor
//c_processor::c_processor()
//{
//} //c_processor
//
//// default destructor
//c_processor::~c_processor()
//{
//} //~c_processor
