/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_processor.h"

Motion_Core::Software::Interpollation::s_input_block c_processor::motion_block;

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
	
	
	
	uint8_t record_size = sizeof(Motion_Core::Software::Interpollation::motion_stream);
	////copy to block
	//memcpy(&test, Motion_Core::Software::Interpollation::stream, sizeof(Motion_Core::Software::Interpollation::stream));
	#ifdef MSVC
	//set some block stuff
	motion_block.motion_type = Motion_Core::e_motion_type::feed_linear;
	motion_block.feed_rate_mode = Motion_Core::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	motion_block.feed_rate = 5000;
	motion_block.axis_values[0] = 1;motion_block.axis_values[1] = 0;motion_block.axis_values[2] = 0;
	motion_block.axis_values[3] = 0; motion_block.axis_values[4] =0 ; motion_block.axis_values[5] = 0;
	motion_block.line_number = 1;
	//copy updated block to stream
	memcpy(Motion_Core::Software::Interpollation::motion_stream, &motion_block,sizeof(Motion_Core::Software::Interpollation::motion_stream));
	Hardware_Abstraction_Layer::Serial::add_to_buffer(0, Motion_Core::Software::Interpollation::motion_stream, record_size);
	#endif
	
	Hardware_Abstraction_Layer::Core::start_interrupts();
	while (1)
	{

		//see if there is any data
		if (host_serial.DataSize()>0)
		{
			uint8_t record_type = host_serial.Peek();
			if (record_type == 0)
			{
				host_serial.Get();
				continue;
			}
			
			host_serial.print_string("PEEK record_type = ");host_serial.print_int32((uint32_t)record_type);host_serial.Write(CR);
			//we have data, determine its type and load it.
			record_type = load_record(record_type);
			switch (record_type)
			{
				case MOTION_RECORD:
				{
					host_serial.print_string("test.record_type = ");host_serial.print_int32((uint32_t)record_type);host_serial.Write(CR);
					host_serial.print_string("test.motion_type = ");host_serial.print_int32((uint32_t)c_processor::motion_block.motion_type);host_serial.Write(CR);
					host_serial.print_string("test.feed_rate_mode = ");host_serial.print_int32((uint32_t)c_processor::motion_block.feed_rate_mode);host_serial.Write(CR);
					host_serial.print_string("test.feed_rate = ");host_serial.print_int32(c_processor::motion_block.feed_rate);host_serial.Write(CR);
					host_serial.print_string("test.axis_values[0] = ");host_serial.print_float(c_processor::motion_block.axis_values[0],4);host_serial.Write(CR);
					host_serial.print_string("test.axis_values[1] = ");host_serial.print_float(c_processor::motion_block.axis_values[1],4);host_serial.Write(CR);
					host_serial.print_string("test.axis_values[2] = ");host_serial.print_float(c_processor::motion_block.axis_values[2],4);host_serial.Write(CR);
					host_serial.print_string("test.axis_values[3] = ");host_serial.print_float(c_processor::motion_block.axis_values[3],4);host_serial.Write(CR);
					host_serial.print_string("test.axis_values[4] = ");host_serial.print_float(c_processor::motion_block.axis_values[4],4);host_serial.Write(CR);
					host_serial.print_string("test.axis_values[5] = ");host_serial.print_float(c_processor::motion_block.axis_values[5],4);host_serial.Write(CR);
					host_serial.print_string("test.line_number = ");host_serial.print_int32(c_processor::motion_block.line_number);host_serial.Write(CR);
					Motion_Core::Software::Interpollation::load_block(c_processor::motion_block);
					
					
				}
				break;
				case SETTING_RECORD:
				{
					
				}
				break;
			}
		}
		//if (host_serial.HasRecord(record_size))
		//{
		////Clear the stream
		//memset(Motion_Core::Software::Interpollation::motion_stream, 0, record_size);
		////Clear the block
		//memset(&motion_block, 0, record_size);
		//for (int i=0;i<record_size;i++)
		//{
		//Motion_Core::Software::Interpollation::motion_stream[i] = host_serial.Get();
		//}
		//memcpy(&motion_block, Motion_Core::Software::Interpollation::motion_stream, record_size);
		//
		////host_serial.print_string("test.motion_type = ");host_serial.print_int32((uint32_t)test.motion_type);host_serial.Write(CR);
		////host_serial.print_string("test.feed_rate_mode = ");host_serial.print_int32((uint32_t)test.feed_rate_mode);host_serial.Write(CR);
		////host_serial.print_string("test.feed_rate = ");host_serial.print_int32(test.feed_rate);host_serial.Write(CR);
		////host_serial.print_string("test.axis_values[0] = ");host_serial.print_float(test.axis_values[0],4);host_serial.Write(CR);
		////host_serial.print_string("test.axis_values[1] = ");host_serial.print_float(test.axis_values[1],4);host_serial.Write(CR);
		////host_serial.print_string("test.axis_values[2] = ");host_serial.print_float(test.axis_values[2],4);host_serial.Write(CR);
		////host_serial.print_string("test.axis_values[3] = ");host_serial.print_float(test.axis_values[3],4);host_serial.Write(CR);
		////host_serial.print_string("test.axis_values[4] = ");host_serial.print_float(test.axis_values[4],4);host_serial.Write(CR);
		////host_serial.print_string("test.axis_values[5] = ");host_serial.print_float(test.axis_values[5],4);host_serial.Write(CR);
		////host_serial.print_string("test.line_number = ");host_serial.print_int32(test.line_number);host_serial.Write(CR);
		//
		//Motion_Core::Software::Interpollation::load_block(motion_block);
		//#ifdef MSVC
		//motion_block.line_number++;
		//memcpy(Motion_Core::Software::Interpollation::motion_stream, &motion_block, sizeof(Motion_Core::Software::Interpollation::motion_stream));
		//Hardware_Abstraction_Layer::Serial::add_to_buffer(0, Motion_Core::Software::Interpollation::motion_stream, record_size);
		//#endif
		//}
		//Let this continuously try to prep the step buffer. If theres no data to process nothing should happen
		Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
	}
}

uint8_t c_processor::load_record(uint8_t record_type)
{
	uint16_t record_size =0;
	switch (record_type)
	{
		case MOTION_RECORD:
		{
			record_size = sizeof(Motion_Core::Software::Interpollation::motion_stream);
			//First byte indicates record type of motion. Make sure its all there before we start loading it.
			if (host_serial.HasRecord(record_size))
			{
				//Clear the stream
				memset(Motion_Core::Software::Interpollation::motion_stream, 0, record_size);
				//Clear the block
				memset(&c_processor::motion_block, 0, record_size);
				//Load the data into the stream
				for (int i=0;i<record_size;i++)
				{
					Motion_Core::Software::Interpollation::motion_stream[i] = host_serial.Get();
				}
				//Put the stream into the struct
				memcpy(&c_processor::motion_block, Motion_Core::Software::Interpollation::motion_stream, record_size);
				return MOTION_RECORD;
			}
		}
		break;
		case SETTING_RECORD:
		{
		}
		break;
		
		default:
		return 0;
		break;
	}
}
