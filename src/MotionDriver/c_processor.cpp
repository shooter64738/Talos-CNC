/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_processor.h"
#include "c_interpollation_software.h"
#include "c_segment_arbitrator.h"

BinaryRecords::Motion::s_input_block c_processor::motion_block;
uint8_t c_processor::remote = 0;

c_Serial c_processor::host_serial;
void c_processor::initialize()
{
	c_Serial host_serial = c_Serial(0, 115200); //<--Connect to host

	Hardware_Abstraction_Layer::Core::initialize();
	Hardware_Abstraction_Layer::MotionCore::Stepper::initialize();
	Motion_Core::initialize();
	
	////copy to block
	//memcpy(&test, Motion_Core::Software::Interpollation::stream, sizeof(Motion_Core::Software::Interpollation::stream));
	#ifdef MSVC
	//set some block stuff
	uint8_t record_size = sizeof(BinaryRecords::Motion::s_input_block);
	motion_block.motion_type = Motion_Core::e_motion_type::feed_linear;
	motion_block.feed_rate_mode = Motion_Core::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	motion_block.feed_rate = 5000;
	motion_block.axis_values[0] = 5; motion_block.axis_values[1] = 0; motion_block.axis_values[2] = 0;
	motion_block.axis_values[3] = 0; motion_block.axis_values[4] = 0; motion_block.axis_values[5] = 0;
	motion_block.line_number = 1;
	//copy updated block to stream
	memcpy(Motion_Core::Software::Interpollation::motion_stream, &motion_block, record_size);
	Hardware_Abstraction_Layer::Serial::add_to_buffer(0, Motion_Core::Software::Interpollation::motion_stream, record_size);
	#endif

	Hardware_Abstraction_Layer::Core::start_interrupts();
	uint8_t serial_try = 0;
	while (1)
	{

		//see if there is any data
		if (c_processor::host_serial.DataSize() > 0)
		{
			serial_try++;
			uint8_t record_type = c_processor::host_serial.Peek();
			if (record_type == 0 && serial_try>10)
			{
				//There are no record types of 0. if we see a zero, its probably bad serial data, so skip it.
				//Reset the serial buffer and tell the host to resend
				c_processor::host_serial.Reset();
				c_processor::host_serial.Write(SER_BAD_READ_RE_TRANSMIT);
				c_processor::host_serial.Write(CR);
				serial_try = 0;
				
			}
			if (record_type>0)
			{
				//we have data, determine its type and load it.
				record_type = c_processor::load_record(record_type);
				
				switch (record_type)
				{
					case MOTION_RECORD:
					{
						c_processor::host_serial.Write(SER_ACK_PROCEED);c_processor::host_serial.Write(CR);
						
						c_processor::host_serial.print_string("test.record_type = "); c_processor::host_serial.print_int32((uint32_t)record_type); c_processor::host_serial.Write(CR);
						c_processor::host_serial.print_string("test.motion_type = "); c_processor::host_serial.print_int32((uint32_t)c_processor::motion_block.motion_type); c_processor::host_serial.Write(CR);
						c_processor::host_serial.print_string("test.feed_rate_mode = "); c_processor::host_serial.print_int32((uint32_t)c_processor::motion_block.feed_rate_mode); c_processor::host_serial.Write(CR);
						c_processor::host_serial.print_string("test.feed_rate = "); c_processor::host_serial.print_int32(c_processor::motion_block.feed_rate); c_processor::host_serial.Write(CR);
						c_processor::host_serial.print_string("test.axis_values[0] = ");c_processor::host_serial.print_float(c_processor::motion_block.axis_values[0], 4); c_processor::host_serial.Write(CR);
						c_processor::host_serial.print_string("test.axis_values[1] = ");c_processor::host_serial.print_float(c_processor::motion_block.axis_values[1], 4); c_processor::host_serial.Write(CR);
						c_processor::host_serial.print_string("test.axis_values[2] = ");c_processor::host_serial.print_float(c_processor::motion_block.axis_values[2], 4); c_processor::host_serial.Write(CR);
						c_processor::host_serial.print_string("test.axis_values[3] = ");c_processor::host_serial.print_float(c_processor::motion_block.axis_values[3], 4); c_processor::host_serial.Write(CR);
						c_processor::host_serial.print_string("test.axis_values[4] = ");c_processor::host_serial.print_float(c_processor::motion_block.axis_values[4], 4); c_processor::host_serial.Write(CR);
						c_processor::host_serial.print_string("test.axis_values[5] = ");c_processor::host_serial.print_float(c_processor::motion_block.axis_values[5], 4); c_processor::host_serial.Write(CR);
						c_processor::host_serial.print_string("test.line_number = "); c_processor::host_serial.print_int32(c_processor::motion_block.line_number); c_processor::host_serial.Write(CR);
						Motion_Core::Software::Interpollation::load_block(c_processor::motion_block);


					}
					break;
					case SETTING_RECORD:
					{

					}
					break;
					default:
					{
						//We do not know what this record type is.
					}
					break;
				}
			}
		}
		
		#ifdef MSVC
		if (c_processor::remote)
		{
			c_processor::motion_block.line_number++;
			memcpy(Motion_Core::Software::Interpollation::motion_stream, &c_processor::motion_block, sizeof(Motion_Core::Software::Interpollation::motion_stream));
			Hardware_Abstraction_Layer::Serial::add_to_buffer(0, Motion_Core::Software::Interpollation::motion_stream, record_size);
			Motion_Core::Software::Interpollation::load_block(c_processor::motion_block);
			c_processor::remote = 0;
		}
		#endif

		//Let this continuously try to prep the step buffer. If theres no data to process nothing should happen
		Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
	}
}

uint8_t c_processor::load_record(uint8_t record_type)
{
	uint16_t record_size = 0;
	switch (record_type)
	{
		case MOTION_RECORD:
		{
			record_size = sizeof(BinaryRecords::Motion::s_input_block);
			//First byte indicates record type of motion. Make sure its all there before we start loading it.
			if (host_serial.HasRecord(record_size))
			{
				//Clear the stream
				memset(Motion_Core::Software::Interpollation::motion_stream, 0, record_size);
				//Clear the block
				memset(&c_processor::motion_block, 0, record_size);
				//Load the data into the stream
				for (int i = 0; i < record_size; i++)
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
