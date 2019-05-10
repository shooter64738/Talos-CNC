/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_processor.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_serial_win.h"
c_Serial c_processor::host_serial;
void c_processor::initialize()
{
	c_Serial host_serial = c_Serial(0, 115200); //<--Connect to host

	Hardware_Abstraction_Layer::Core::initialize();
	//c_protocol::control_serial = c_Serial(0, 115200);
	Hardware_Abstraction_Layer::MotionCore::Stepper::initialize();
	//Hardware_Abstraction_Layer::Grbl::Control::initialize();
	
	//Hardware_Abstraction_Layer::Grbl::Limits::initialize();
	
	Motion_Core::initialize();
	
	
	Motion_Core::Software::Interpollation::s_input_block test;
	uint8_t record_size = sizeof(Motion_Core::Software::Interpollation::motion_stream);
	////copy to block
	//memcpy(&test, Motion_Core::Software::Interpollation::stream, sizeof(Motion_Core::Software::Interpollation::stream));
#ifdef MSVC
	//set some block stuff
	test.motion_type = Motion_Core::e_motion_type::feed_linear;
	test.feed_rate_mode = Motion_Core::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	test.feed_rate = 5000;
	test.axis_values[0] = 1;test.axis_values[1] = 0;test.axis_values[2] = 0;
	test.axis_values[3] = 0; test.axis_values[4] =0 ; test.axis_values[5] = 0;
	test.line_number = 1;
	//copy updated block to stream
	memcpy(Motion_Core::Software::Interpollation::motion_stream, &test,sizeof(Motion_Core::Software::Interpollation::motion_stream));
	Hardware_Abstraction_Layer::Serial::add_to_buffer(0, Motion_Core::Software::Interpollation::motion_stream, record_size);
#endif
	//host_serial.Write_Record(Motion_Core::Software::Interpollation::stream,record_size);
	//clear block
	//memset(&test, 0, sizeof(Motion_Core::Software::Interpollation::s_input_block));
	//copy back to block from stream
	//memcpy(&test, Motion_Core::Software::Interpollation::stream, sizeof(Motion_Core::Software::Interpollation::stream));
	


	//host_serial.print_string("hello");
	//host_serial.Write(CR);
	Hardware_Abstraction_Layer::Core::start_interrupts();
	uint8_t last_size=0;
	while (1)
	{

uint8_t current_size = host_serial.DataSize();
		//if ( current_size != last_size)
		//{
			//last_size = current_size;
			//host_serial.print_string("size=");
			//host_serial.print_int32(last_size);
			//host_serial.Write(CR);
		//}
		if (host_serial.HasRecord(record_size))
		{
			//host_serial.print_string("sH=");host_serial.print_int32(host_serial.HeadPosition());host_serial.Write(CR);
			//host_serial.print_string("sT=");host_serial.print_int32(host_serial.TailPosition());host_serial.Write(CR);
			//host_serial.print_string("HAVE RECORD!");
			//host_serial.Write(CR);
			//Clear the stream
			memset(Motion_Core::Software::Interpollation::motion_stream, 0, record_size);
			//Clear the block
			memset(&test, 0, record_size);
			for (int i=0;i<record_size;i++)
			{
				Motion_Core::Software::Interpollation::motion_stream[i] = host_serial.Get();
			}
			
			
			memcpy(&test, Motion_Core::Software::Interpollation::motion_stream, record_size);
			
			//host_serial.print_string("test.motion_type = ");host_serial.print_int32((uint32_t)test.motion_type);host_serial.Write(CR);
			//host_serial.print_string("test.feed_rate_mode = ");host_serial.print_int32((uint32_t)test.feed_rate_mode);host_serial.Write(CR);
			//host_serial.print_string("test.feed_rate = ");host_serial.print_int32(test.feed_rate);host_serial.Write(CR);
			//host_serial.print_string("test.axis_values[0] = ");host_serial.print_float(test.axis_values[0],4);host_serial.Write(CR);
			//host_serial.print_string("test.axis_values[1] = ");host_serial.print_float(test.axis_values[1],4);host_serial.Write(CR);
			//host_serial.print_string("test.axis_values[2] = ");host_serial.print_float(test.axis_values[2],4);host_serial.Write(CR);
			//host_serial.print_string("test.axis_values[3] = ");host_serial.print_float(test.axis_values[3],4);host_serial.Write(CR);
			//host_serial.print_string("test.axis_values[4] = ");host_serial.print_float(test.axis_values[4],4);host_serial.Write(CR);
			//host_serial.print_string("test.axis_values[5] = ");host_serial.print_float(test.axis_values[5],4);host_serial.Write(CR);
			//host_serial.print_string("test.line_number = ");host_serial.print_int32(test.line_number);host_serial.Write(CR);
			//
			//host_serial.print_string("eH=");host_serial.print_int32(host_serial.HeadPosition());host_serial.Write(CR);
			//host_serial.print_string("eT=");host_serial.print_int32(host_serial.TailPosition());host_serial.Write(CR);
			
			Motion_Core::Software::Interpollation::load_block(test);
#ifdef MSVC
			test.line_number++;
			memcpy(Motion_Core::Software::Interpollation::motion_stream, &test, sizeof(Motion_Core::Software::Interpollation::motion_stream));
			Hardware_Abstraction_Layer::Serial::add_to_buffer(0, Motion_Core::Software::Interpollation::motion_stream, record_size);
#endif
		}
		//Let this continuously try to prep the step buffer. If theres no data to process nothing shoudl happen
		Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
	}
}
