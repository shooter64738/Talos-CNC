/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_processor.h"
c_Serial c_processor::host_serial;
void c_processor::initialize()
{
	c_Serial host_serial = c_Serial(0, 115200); //<--Connect to host

	Hardware_Abstraction_Layer::Core::initialize();
	//c_protocol::control_serial = c_Serial(0, 115200);
	Hardware_Abstraction_Layer::Grbl::Stepper::initialize();
	//Hardware_Abstraction_Layer::Grbl::Control::initialize();
	
	//Hardware_Abstraction_Layer::Grbl::Limits::initialize();
	
	Motion_Core::initialize();
	
	
	Motion_Core::Software::Interpollation::s_input_block test;

	//copy to block
	memcpy(&test, Motion_Core::Software::Interpollation::stream, sizeof(Motion_Core::Software::Interpollation::stream));
	//set some block stuff
	test.motion_type = Motion_Core::e_motion_type::feed_linear;
	test.feed_rate_mode = Motion_Core::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	test.feed_rate = 5000;
	test.axis_values[0] = 10;test.axis_values[1] = 10;test.axis_values[2] = 10;
	test.axis_values[3] = 10; test.axis_values[4] =10 ; test.axis_values[5] = 10;
	test.line_number = 1;
	//copy updated block to stream
	memcpy(Motion_Core::Software::Interpollation::stream, &test,sizeof(Motion_Core::Software::Interpollation::stream));
	//clear block
	memset(&test, 0, sizeof(Motion_Core::Software::Interpollation::s_input_block));
	//copy back to block from stream
	memcpy(&test, Motion_Core::Software::Interpollation::stream, sizeof(Motion_Core::Software::Interpollation::stream));
	


	host_serial.print_string("hello");
	host_serial.Write(CR);
	Hardware_Abstraction_Layer::Core::start_interrupts();
	uint8_t updated = 1;
	while (1)
	{
		if (!Motion_Core::Hardware::Interpollation::Interpolation_Active && !updated)
		{
			host_serial.print_string("End:");
			for (uint8_t i=0;i<N_AXIS;i++)
			{
				host_serial.print_float(Motion_Core::Hardware::Interpollation::system_position[i]);
				host_serial.Write(',');
			}
			host_serial.Write(CR);
			updated = 1;
		}
		
		#ifndef MSVC
		if (host_serial.HasEOL())
		#else
		if (1==1)
		#endif
		{
			
			updated = 0;
			//host_serial.print_string("buff\r");
			host_serial.SkipToEOL();
			host_serial.print_string("Line:");
			host_serial.print_int32(test.line_number);
			host_serial.Write(CR);
			host_serial.print_string("Start:");
			for (uint8_t i=0;i<N_AXIS;i++)
			{
				host_serial.print_float(Motion_Core::Hardware::Interpollation::system_position[i]);
				host_serial.Write(',');
			}
			host_serial.Write(CR);
			
			Motion_Core::Software::Interpollation::load_block(test);
			test.line_number++;
		}
		//Let this continuously try to prep the step buffer. If theres no data to process nothing shoudl happen
		Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
	}
}
