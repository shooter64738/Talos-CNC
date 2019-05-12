/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_processor.h"
#include "c_interpollation_software.h"
#include "c_segment_arbitrator.h"
#include "c_motion_core.h"
#include "..\Common\Hardware_Abstraction_Layer\AVR_2560\c_serial_avr_2560.h"

BinaryRecords::s_motion_data_block c_processor::motion_block;
BinaryRecords::s_motion_control_settings c_processor::settings_block;


uint8_t c_processor::remote = 0;

c_Serial c_processor::host_serial;
void c_processor::initialize()
{
	c_processor::host_serial = c_Serial(0, 115200); //<--Connect to host

	Hardware_Abstraction_Layer::Core::initialize();
	Hardware_Abstraction_Layer::MotionCore::Stepper::initialize();
	Motion_Core::initialize();

	////copy to block
	//memcpy(&test, Motion_Core::Software::Interpollation::stream, sizeof(Motion_Core::Software::Interpollation::stream));
	#define TEST_MOTION
	#ifdef MSVC
	#ifdef TEST_MOTION
	//set some block stuff
	uint8_t record_size = sizeof(BinaryRecords::s_motion_data_block);
	motion_block.motion_type = BinaryRecords::e_motion_type::feed_linear;
	motion_block.feed_rate_mode = BinaryRecords::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	motion_block.feed_rate = 5000;
	motion_block.axis_values[0] = 1; motion_block.axis_values[1] = 0; motion_block.axis_values[2] = 0;
	motion_block.axis_values[3] = 0; motion_block.axis_values[4] = 0; motion_block.axis_values[5] = 0;
	motion_block.line_number = 1;
	//copy updated block to stream
	char motion_stream[sizeof(BinaryRecords::s_motion_data_block)];
	
	memcpy(motion_stream, &motion_block, record_size);
	Hardware_Abstraction_Layer::Serial::add_to_buffer(0, motion_stream, record_size);
	#endif
	#ifdef TEST_SETTINGS
	uint8_t record_size = sizeof(BinaryRecords::s_motion_control_settings);
	for (uint8_t i = 0; i < N_AXIS; i++)
	{
		c_processor::settings_block.steps_per_mm[i] = 160;
		c_processor::settings_block.acceleration[i] = (100.0 * 60 * 60);
		c_processor::settings_block.max_rate[i] = 5000;
		c_processor::settings_block.pulse_length = 10;
		//arbitrary for testing
		c_processor::settings_block.back_lash_comp_distance[i] = 55;
	}
	c_processor::settings_block.pulse_length = 10;
	char setting_stream[sizeof(BinaryRecords::s_motion_control_settings)];
	memcpy(setting_stream, &c_processor::settings_block, record_size);
	Hardware_Abstraction_Layer::Serial::add_to_buffer(0, setting_stream, record_size);
	#endif
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
			//when a motion is running it ma take several loops for the serial data to arrive.
			//the serial_try is a way to wait a reasonable amount of times before the failure
			//is declared.
			if (record_type == 0 && serial_try > 300)
			{
				//There are no record types of 0. if we see a zero, its probably bad serial data, so skip it.
				//Reset the serial buffer and tell the host to resend
				c_processor::host_serial.Reset();
				c_processor::host_serial.Write(SER_BAD_READ_RE_TRANSMIT);
				c_processor::host_serial.Write(CR);
				serial_try = 0;

			}
			else
			{
				//we have data, determine its type and load it (if its all there yet).
				record_type = c_processor::load_record(record_type);
			}

			if (record_type > 0)
			{
				switch (record_type)
				{
					case MOTION_RECORD:
					{
						c_processor::host_serial.Write(SER_ACK_PROCEED); c_processor::host_serial.Write(CR);

						//c_processor::host_serial.print_string("test.record_type = "); c_processor::host_serial.print_int32((uint32_t)record_type); c_processor::host_serial.Write(CR);
						//c_processor::host_serial.print_string("test.motion_type = "); c_processor::host_serial.print_int32((uint32_t)c_processor::motion_block.motion_type); c_processor::host_serial.Write(CR);
						//c_processor::host_serial.print_string("test.feed_rate_mode = "); c_processor::host_serial.print_int32((uint32_t)c_processor::motion_block.feed_rate_mode); c_processor::host_serial.Write(CR);
						//c_processor::host_serial.print_string("test.feed_rate = "); c_processor::host_serial.print_int32(c_processor::motion_block.feed_rate); c_processor::host_serial.Write(CR);
						//c_processor::host_serial.print_string("test.axis_values[0] = ");c_processor::host_serial.print_float(c_processor::motion_block.axis_values[0], 4); c_processor::host_serial.Write(CR);
						//c_processor::host_serial.print_string("test.axis_values[1] = ");c_processor::host_serial.print_float(c_processor::motion_block.axis_values[1], 4); c_processor::host_serial.Write(CR);
						//c_processor::host_serial.print_string("test.axis_values[2] = ");c_processor::host_serial.print_float(c_processor::motion_block.axis_values[2], 4); c_processor::host_serial.Write(CR);
						//c_processor::host_serial.print_string("test.axis_values[3] = ");c_processor::host_serial.print_float(c_processor::motion_block.axis_values[3], 4); c_processor::host_serial.Write(CR);
						//c_processor::host_serial.print_string("test.axis_values[4] = ");c_processor::host_serial.print_float(c_processor::motion_block.axis_values[4], 4); c_processor::host_serial.Write(CR);
						//c_processor::host_serial.print_string("test.axis_values[5] = ");c_processor::host_serial.print_float(c_processor::motion_block.axis_values[5], 4); c_processor::host_serial.Write(CR);
						//c_processor::host_serial.print_string("test.line_number = "); c_processor::host_serial.print_int32(c_processor::motion_block.line_number); c_processor::host_serial.Write(CR);
						Motion_Core::Software::Interpollation::load_block(c_processor::motion_block);
					}
					break;
					case MOTION_CONTROL_SETTING_RECORD:
					{
						c_processor::host_serial.Write(SER_ACK_PROCEED); c_processor::host_serial.Write(CR);
						memcpy(&Motion_Core::Settings::_Settings, &c_processor::settings_block, sizeof(BinaryRecords::s_motion_control_settings));

						//c_processor::host_serial.print_string("test.record_type = "); c_processor::host_serial.print_int32((uint32_t)record_type); c_processor::host_serial.Write(CR);
						//for (uint8_t i = 0; i < N_AXIS; i++)
						//{
						//c_processor::host_serial.print_string("test.steps_per_mm[");
						//c_processor::host_serial.print_int32(i);
						//c_processor::host_serial.print_string("] = ");
						//c_processor::host_serial.print_float(Motion_Core::Settings::_Settings.steps_per_mm[i], 2);
						//c_processor::host_serial.Write(CR);
						//
						//
						//c_processor::host_serial.print_string("test.acceleration[");
						//c_processor::host_serial.print_int32(i);
						//c_processor::host_serial.print_string("] = ");
						//c_processor::host_serial.print_float(Motion_Core::Settings::_Settings.acceleration[i], 2);
						//c_processor::host_serial.Write(CR);
						//
						//c_processor::host_serial.print_string("test.max_rate[");
						//c_processor::host_serial.print_int32(i);
						//c_processor::host_serial.print_string("] = ");
						//c_processor::host_serial.print_float(Motion_Core::Settings::_Settings.max_rate[i], 2);
						//c_processor::host_serial.Write(CR);
						//
						//c_processor::host_serial.print_string("test.back_lash_comp_distance[");
						//c_processor::host_serial.print_int32(i);
						//c_processor::host_serial.print_string("] = ");
						//c_processor::host_serial.print_float(Motion_Core::Settings::_Settings.back_lash_comp_distance[i], 2);
						//c_processor::host_serial.Write(CR);
						//}
						//c_processor::host_serial.print_string("test.pulse_length = ");
						//c_processor::host_serial.print_int32(Motion_Core::Settings::_Settings.pulse_length);
						//c_processor::host_serial.Write(CR);
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
		//if (c_processor::remote)
		{
			c_processor::motion_block.line_number++;
			memcpy(motion_stream, &c_processor::motion_block, sizeof(motion_stream));
			Hardware_Abstraction_Layer::Serial::add_to_buffer(0, motion_stream, record_size);
			Hardware_Abstraction_Layer::Serial::rxBuffer[0];
			Motion_Core::Software::Interpollation::load_block(c_processor::motion_block);
			c_processor::remote = 0;
		}
		#endif
		//while(1)
		{
			//Let this continuously try to prep the step buffer. If theres no data to process nothing should happen
			Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
		}
	}
}

uint8_t c_processor::load_record(uint8_t record_type)
{
	uint16_t record_size = 0;
	switch (record_type)
	{
		case MOTION_RECORD:
		{
			record_size = sizeof(BinaryRecords::s_motion_data_block);
			//First byte indicates record type of motion. Make sure its all there before we start loading it.
			if (host_serial.HasRecord(record_size))
			{
				//Clear the struct
				memset(&c_processor::motion_block, 0, record_size);
				//Put the stream into the struct
				memcpy(&c_processor::motion_block
					, c_processor::host_serial.Buffer_Pointer() + c_processor::host_serial.TailPosition()
					, record_size);
				//Move the tail forward to the position we stopped reading the serial buffer from. (probably the head position)
				c_processor::host_serial.AdvanceTail(record_size);
				return MOTION_RECORD;
			}
		}
		break;
		case MOTION_CONTROL_SETTING_RECORD:
		{
			record_size = sizeof(BinaryRecords::s_motion_control_settings);
			//First byte indicates record type of motion. Make sure its all there before we start loading it.
			if (host_serial.HasRecord(record_size))
			{
				//Clear the struct
				memset(&c_processor::settings_block, 0, record_size);
				//Put the stream into the struct
				memcpy(&c_processor::settings_block
					, c_processor::host_serial.Buffer_Pointer() + c_processor::host_serial.TailPosition()
					, record_size);
				//Move the tail forward to the position we stopped reading the serial buffer from. (probably the head position)
				c_processor::host_serial.AdvanceTail(record_size);
				return MOTION_CONTROL_SETTING_RECORD;
			}
		}
		break;

		default:
		return 0;
		break;
	}
}
