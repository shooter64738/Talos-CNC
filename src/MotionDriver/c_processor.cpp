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
	
	Motion_Core::Software::Interpollation::s_input_block in_block[10];
	
	host_serial.print_string("hello");
	host_serial.Write(CR);
	Hardware_Abstraction_Layer::Core::start_interrupts();
	int8_t t = 0;
	
	in_block[t].motion_type = Motion_Core::e_motion_type::feed_linear;
	in_block[t].feed_rate_mode = Motion_Core::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	in_block[t].feed_rate = 1000;
	memset(in_block[t].axis_values, 0, sizeof(in_block[t].axis_values));
	in_block[t].line_number = 1;
	in_block[t].axis_values[0]=10;in_block[t++].axis_values[1]=10;
	//Motion_Core::Software::Interpollation::load_block(in_block[t++]);
	
	in_block[t].motion_type = Motion_Core::e_motion_type::feed_linear;
	in_block[t].feed_rate_mode = Motion_Core::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	in_block[t].feed_rate = 2000;
	memset(in_block[t].axis_values,0,sizeof(in_block[t].axis_values));
	in_block[t].line_number = 2;
	in_block[t].axis_values[0]=10;in_block[t++].axis_values[1]=-10;
	//Motion_Core::Software::Interpollation::load_block(in_block[t++]);
	
	in_block[t].motion_type = Motion_Core::e_motion_type::feed_linear;
	in_block[t].feed_rate_mode = Motion_Core::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	in_block[t].feed_rate = 1000;
	memset(in_block[t].axis_values, 0, sizeof(in_block[t].axis_values));
	in_block[t].line_number = 3;
	in_block[t].axis_values[0]=10;in_block[t++].axis_values[1]=-10;
	//Motion_Core::Software::Interpollation::load_block(in_block[t++]);

	in_block[t].motion_type = Motion_Core::e_motion_type::feed_linear;
	in_block[t].feed_rate_mode = Motion_Core::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	in_block[t].feed_rate = 2000;
	memset(in_block[t].axis_values, 0, sizeof(in_block[t].axis_values));
	in_block[t].line_number = 4;
	in_block[t].axis_values[0]=10;in_block[t++].axis_values[1]=10;
	//Motion_Core::Software::Interpollation::load_block(in_block[t++]);

	in_block[t].motion_type = Motion_Core::e_motion_type::feed_linear;
	in_block[t].feed_rate_mode = Motion_Core::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	in_block[t].feed_rate = 1000;
	memset(in_block[t].axis_values, 0, sizeof(in_block[t].axis_values));
	in_block[t].line_number = 5;
	in_block[t].axis_values[0]=10;in_block[t++].axis_values[1]=-10;
	//Motion_Core::Software::Interpollation::load_block(in_block[t++]);

	in_block[t].motion_type = Motion_Core::e_motion_type::feed_linear;
	in_block[t].feed_rate_mode = Motion_Core::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	in_block[t].feed_rate = 2000;
	memset(in_block[t].axis_values, 0, sizeof(in_block[t].axis_values));
	in_block[t].line_number = 6;
	in_block[t].axis_values[0]=10;in_block[t++].axis_values[1]=10;
	//Motion_Core::Software::Interpollation::load_block(in_block[t++]);
	t=0;
	while (1)
	{
		#ifndef MSVC
		if (host_serial.HasEOL())
		#else
		if (1==1)
		#endif
		{
			if (Motion_Core::Hardware::Interpollation::Interpolation_Active)
			{
				host_serial.print_string("End:");
				for (uint8_t i=0;i<N_AXIS;i++)
				{
					host_serial.print_float(Motion_Core::Hardware::Interpollation::system_position[i]);
					host_serial.Write(',');
				}
				host_serial.Write(CR);
			}
			
			//host_serial.print_string("buff\r");
			host_serial.SkipToEOL();
			host_serial.print_string("Line:");
			host_serial.print_int32(in_block[t].line_number);
			host_serial.Write(CR);
			host_serial.print_string("Start:");
			for (uint8_t i=0;i<N_AXIS;i++)
			{
				host_serial.print_float(Motion_Core::Hardware::Interpollation::system_position[i]);
				host_serial.Write(',');
			}
			host_serial.Write(CR);
			
			Motion_Core::Software::Interpollation::load_block(in_block[t]);
			t++;//UDR0='G';
		}
		Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
	}
}
