#include "c_processor.h"

/*
* MotionDriver.cpp
*
* Created: 4/3/2019 1:32:16 PM
* Author : Family
*/



int main()
{
	c_processor::initialize();
}

//int main(void)
//{
	//c_Serial host_serial = c_Serial(0, 115200); //<--Connect to host
//
	//Hardware_Abstraction_Layer::Core::initialize();
	////c_protocol::control_serial = c_Serial(0, 115200);
	//Hardware_Abstraction_Layer::Grbl::Stepper::initialize();
	////Hardware_Abstraction_Layer::Grbl::Control::initialize();
	//
	////Hardware_Abstraction_Layer::Grbl::Limits::initialize();
	//
	//Motion_Core::initialize();
	//
	//Motion_Core::Software::Interpollation::s_input_block in_block[10];
	//
	//host_serial.print_string("hello");
	//Hardware_Abstraction_Layer::Core::start_interrupts();
	//int8_t t = 0;
	//
	//in_block[t].motion_type = Motion_Core::Software::Interpollation::e_motion_type::feed_linear;
	//in_block[t].feed_rate_mode = Motion_Core::Software::Interpollation::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//in_block[t].feed_rate = 1000;
	//memset(in_block[t].axis_values, 0, sizeof(in_block[t].axis_values));
	//in_block[t].line_number = 1;
	//in_block[t++].axis_values[0]=10;
	////Motion_Core::Software::Interpollation::load_block(in_block[t++]);
	//
	//in_block[t].motion_type = Motion_Core::Software::Interpollation::e_motion_type::feed_linear;
	//in_block[t].feed_rate_mode = Motion_Core::Software::Interpollation::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//in_block[t].feed_rate = 2000;
	//memset(in_block[t].axis_values,0,sizeof(in_block[t].axis_values));
	//in_block[t].line_number = 2;
	//in_block[t++].axis_values[0]=-10;
	////Motion_Core::Software::Interpollation::load_block(in_block[t++]);
	//
	//in_block[t].motion_type = Motion_Core::Software::Interpollation::e_motion_type::feed_linear;
	//in_block[t].feed_rate_mode = Motion_Core::Software::Interpollation::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//in_block[t].feed_rate = 1000;
	//memset(in_block[t].axis_values, 0, sizeof(in_block[t].axis_values));
	//in_block[t].line_number = 3;
	//in_block[t++].axis_values[0]=10;
	////Motion_Core::Software::Interpollation::load_block(in_block[t++]);
//
	//in_block[t].motion_type = Motion_Core::Software::Interpollation::e_motion_type::feed_linear;
	//in_block[t].feed_rate_mode = Motion_Core::Software::Interpollation::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//in_block[t].feed_rate = 2000;
	//memset(in_block[t].axis_values, 0, sizeof(in_block[t].axis_values));
	//in_block[t].line_number = 4;
	//in_block[t++].axis_values[0]=-10;
	////Motion_Core::Software::Interpollation::load_block(in_block[t++]);
//
	//in_block[t].motion_type = Motion_Core::Software::Interpollation::e_motion_type::feed_linear;
	//in_block[t].feed_rate_mode = Motion_Core::Software::Interpollation::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//in_block[t].feed_rate = 1000;
	//memset(in_block[t].axis_values, 0, sizeof(in_block[t].axis_values));
	//in_block[t].line_number = 5;
	//in_block[t++].axis_values[0]=10;
	////Motion_Core::Software::Interpollation::load_block(in_block[t++]);
//
	//in_block[t].motion_type = Motion_Core::Software::Interpollation::e_motion_type::feed_linear;
	//in_block[t].feed_rate_mode = Motion_Core::Software::Interpollation::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//in_block[t].feed_rate = 2000;
	//memset(in_block[t].axis_values, 0, sizeof(in_block[t].axis_values));
	//in_block[t].line_number = 6;
	//in_block[t++].axis_values[0]=-10;
	////Motion_Core::Software::Interpollation::load_block(in_block[t++]);
	//t=0;
	//while (1)
	//{
		//if (host_serial.HasEOL())
		////if (Motion_Core::Hardware::Interpollation::Interpolation_Active)
		//{
			////host_serial.print_string("buff\r");
			//host_serial.SkipToEOL();
			//host_serial.print_string("Line:");
			//host_serial.print_int32(in_block[t].line_number);
			//host_serial.Write(CR);
			//Motion_Core::Software::Interpollation::load_block(in_block[t]);
			//t++;//UDR0='G';
		//}
		//Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
	//}
//}

