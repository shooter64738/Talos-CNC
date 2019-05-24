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

#include "c_interpollation_hardware.h"

BinaryRecords::s_motion_data_block c_processor::motion_block;
BinaryRecords::s_motion_control_settings c_processor::settings_block;
BinaryRecords::s_jog_data_block c_processor::jog_block;
BinaryRecords::s_status_message c_processor::status_message;


uint8_t c_processor::remote = 0;

c_Serial c_processor::coordinator_serial;
c_Serial c_processor::debug_serial;

void c_processor::initialize()
{
	c_processor::coordinator_serial = c_Serial(1, 115200); //<--Connect to coordinator
	c_processor::debug_serial = c_Serial(0, 115200); //<--Connect to host

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
	for (uint8_t i = 0; i < MACHINE_AXIS_COUNT; i++)
	{
		c_processor::settings_block.steps_per_mm[i] = 160;
		c_processor::settings_block.acceleration[i] = (500.0 * 60 * 60);
		c_processor::settings_block.max_rate[i] = 10000;
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
	uint32_t serial_try = 0;
	uint8_t control_mode = 0;
	
	c_processor::debug_serial.print_string("motion driver ready\r");
	
	//We just want to send the coordinator something so it knows we are alive. It just has to end with a CR
	c_processor::coordinator_serial.print_string("on\r");
	uint32_t new_sequence = 0;
	
	while (1)
	{

		//If we were processing a jog record, we will let the coordinator know when we are done with the jog
		//so that the jog messages do not 'pile up' and over run the serial buffer. (Coordinator will not send
		//another jog command until we acknowledge we are done with the current one)
		if (Motion_Core::Hardware::Interpollation::Interpolation_Active == 0)
		{
			if (control_mode ==1)
			{
				//Let the coordinator know we are clear to receive another jog command.
				c_processor::coordinator_serial.Write((char)BinaryRecords::e_binary_responses::Jog_Complete); c_processor::coordinator_serial.Write(CR);
				control_mode = 0;
			}
		}
		//see if there is any data
		if (c_processor::coordinator_serial.DataSize() > 0)
		{
			serial_try++;
			BinaryRecords::e_binary_record_types record_type = (BinaryRecords::e_binary_record_types)c_processor::coordinator_serial.Peek();
			
			//when a motion is running it may take several loops for the serial data to arrive.
			//the serial_try is a way to wait a reasonable amount of times before the failure
			//is declared.
			
			if (record_type == BinaryRecords::e_binary_record_types::Unknown && serial_try > (300 * BINARY_SERIAL_CYCLE_TIMEOUT))
			{
				//There are no record types of 0. if we see a zero, its probably bad serial data, so skip it.
				//Reset the serial buffer and tell the host to resend
				c_processor::coordinator_serial.Reset();
				c_processor::coordinator_serial.Write((char)BinaryRecords::e_binary_responses::Data_Error);
				c_processor::coordinator_serial.Write(CR);
				serial_try = 0;

			}
			else
			{
				//we have data, determine its type and load it (if its all there yet).
				record_type = c_processor::load_record(record_type);
			}

			if (record_type != BinaryRecords::e_binary_record_types::Unknown)
			{
				switch (record_type)
				{
					case BinaryRecords::e_binary_record_types::Jog:
					{
						if (!Motion_Core::Hardware::Interpollation::Interpolation_Active)
						{
							control_mode = 1;
							//Tell the host we got the record, and its ok.
							c_processor::coordinator_serial.Write((char)BinaryRecords::e_binary_responses::Ok); c_processor::coordinator_serial.Write(CR);

							c_processor::debug_serial.print_string("jog value = "); c_processor::debug_serial.print_float(c_processor::jog_block.axis_value); c_processor::debug_serial.Write(CR);							//Convert jog to standard motion block.
							c_processor::motion_block.feed_rate_mode = BinaryRecords::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
							c_processor::motion_block.motion_type = BinaryRecords::e_motion_type::rapid_linear;
							c_processor::motion_block.axis_values[c_processor::jog_block.axis] = c_processor::jog_block.axis_value;
							Motion_Core::Software::Interpollation::load_block(c_processor::motion_block);
							
						}
						else
						{
							
						}
					}
					break;
					case BinaryRecords::e_binary_record_types::Motion:
					{
						c_processor::coordinator_serial.Write((char)BinaryRecords::e_binary_responses::Ok); c_processor::coordinator_serial.Write(CR);

						c_processor::motion_block.motion_type = BinaryRecords::e_motion_type::rapid_linear;
						c_processor::coordinator_serial.print_string("test.record_type = "); c_processor::coordinator_serial.print_int32((uint32_t)record_type); c_processor::coordinator_serial.Write(CR);
						c_processor::coordinator_serial.print_string("test.motion_type = "); c_processor::coordinator_serial.print_int32((uint32_t)c_processor::motion_block.motion_type); c_processor::coordinator_serial.Write(CR);
						c_processor::coordinator_serial.print_string("test.feed_rate_mode = "); c_processor::coordinator_serial.print_int32((uint32_t)c_processor::motion_block.feed_rate_mode); c_processor::coordinator_serial.Write(CR);
						c_processor::coordinator_serial.print_string("test.feed_rate = "); c_processor::coordinator_serial.print_int32(c_processor::motion_block.feed_rate); c_processor::coordinator_serial.Write(CR);
						c_processor::coordinator_serial.print_string("test.axis_values[0] = ");c_processor::coordinator_serial.print_float(c_processor::motion_block.axis_values[0], 4); c_processor::coordinator_serial.Write(CR);
						c_processor::coordinator_serial.print_string("test.axis_values[1] = ");c_processor::coordinator_serial.print_float(c_processor::motion_block.axis_values[1], 4); c_processor::coordinator_serial.Write(CR);
						c_processor::coordinator_serial.print_string("test.axis_values[2] = ");c_processor::coordinator_serial.print_float(c_processor::motion_block.axis_values[2], 4); c_processor::coordinator_serial.Write(CR);
						c_processor::coordinator_serial.print_string("test.axis_values[3] = ");c_processor::coordinator_serial.print_float(c_processor::motion_block.axis_values[3], 4); c_processor::coordinator_serial.Write(CR);
						c_processor::coordinator_serial.print_string("test.axis_values[4] = ");c_processor::coordinator_serial.print_float(c_processor::motion_block.axis_values[4], 4); c_processor::coordinator_serial.Write(CR);
						c_processor::coordinator_serial.print_string("test.axis_values[5] = ");c_processor::coordinator_serial.print_float(c_processor::motion_block.axis_values[5], 4); c_processor::coordinator_serial.Write(CR);
						c_processor::coordinator_serial.print_string("test.line_number = "); c_processor::coordinator_serial.print_int32(c_processor::motion_block.line_number); c_processor::coordinator_serial.Write(CR);
						c_processor::motion_block.sequence = ++new_sequence;
						
						//c_processor::status_message.system_state =
						//(Motion_Core::Hardware::Interpollation::Interpolation_Active?
						//BinaryRecords::e_system_state_record_types::Motion_Active
						//: BinaryRecords::e_system_state_record_types::Motion_Idle);
						//c_processor::status_message.system_sub_state = BinaryRecords::e_system_sub_state_record_types::Block_Queuing;
						//c_processor::status_message.num_message = c_processor::motion_block.sequence;
												//
						//c_processor::send_status(c_processor::status_message);
						
						//Tell the host we got the record, and its ok.
						//c_processor::coordinator_serial.Write((char)BinaryRecords::e_binary_responses::Ok); c_processor::coordinator_serial.Write(CR);

						
						Motion_Core::Software::Interpollation::load_block(c_processor::motion_block);
					}
					break;
					case BinaryRecords::e_binary_record_types::Motion_Control_Setting:
					{
						c_processor::coordinator_serial.Write((char)BinaryRecords::e_binary_responses::Ok); c_processor::coordinator_serial.Write(CR);
						
						
						memcpy(&Motion_Core::Settings::_Settings, &c_processor::settings_block, sizeof(BinaryRecords::s_motion_control_settings));

						c_processor::debug_serial.print_string("test.record_type = "); c_processor::debug_serial.print_int32((uint32_t)record_type); c_processor::debug_serial.Write(CR);
						for (uint8_t i = 0; i < MACHINE_AXIS_COUNT; i++)
						{
							c_processor::debug_serial.print_string("test.steps_per_mm[");
							c_processor::debug_serial.print_int32(i);
							c_processor::debug_serial.print_string("] = ");
							c_processor::debug_serial.print_float(Motion_Core::Settings::_Settings.steps_per_mm[i], 2);
							c_processor::debug_serial.Write(CR);
							
							c_processor::debug_serial.print_string("test.acceleration[");
							c_processor::debug_serial.print_int32(i);
							c_processor::debug_serial.print_string("] = ");
							c_processor::debug_serial.print_float(Motion_Core::Settings::_Settings.acceleration[i]/60/60, 2);
							c_processor::debug_serial.print_string("mm/s");
							c_processor::debug_serial.Write(CR);
							
							c_processor::debug_serial.print_string("test.max_rate[");
							c_processor::debug_serial.print_int32(i);
							c_processor::debug_serial.print_string("] = ");
							c_processor::debug_serial.print_float(Motion_Core::Settings::_Settings.max_rate[i], 2);
							c_processor::debug_serial.print_string("mm/m");
							c_processor::debug_serial.Write(CR);
							
							c_processor::debug_serial.print_string("test.back_lash_comp_distance[");
							c_processor::debug_serial.print_int32(i);
							c_processor::debug_serial.print_string("] = ");
							c_processor::debug_serial.print_float(Motion_Core::Settings::_Settings.back_lash_comp_distance[i], 2);
							c_processor::debug_serial.Write(CR);
						}
						c_processor::debug_serial.print_string("test.pulse_length = ");
						c_processor::debug_serial.print_int32(Motion_Core::Settings::_Settings.pulse_length);
						c_processor::debug_serial.Write(CR);
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
		if (Motion_Core::Hardware::Interpollation::Last_Completed_Sequence != 0)
		{
			c_processor::debug_serial.print_string("Block ");
			c_processor::debug_serial.print_int32(Motion_Core::Hardware::Interpollation::Last_Completed_Sequence);
			c_processor::debug_serial.print_string(" completed\r");
			
			c_processor::status_message.system_state =
			(Motion_Core::Hardware::Interpollation::Interpolation_Active?
			BinaryRecords::e_system_state_record_types::Motion_Active
			: BinaryRecords::e_system_state_record_types::Motion_Idle);
			c_processor::status_message.system_sub_state = BinaryRecords::e_system_sub_state_record_types::Block_Complete;
			c_processor::status_message.num_message = Motion_Core::Hardware::Interpollation::Last_Completed_Sequence;
			Motion_Core::Hardware::Interpollation::Last_Completed_Sequence = 0;
			
			c_processor::debug_serial.print_string("state = ");
			c_processor::debug_serial.print_int32((int32_t)status_message.system_state);c_processor::debug_serial.Write(CR);
			c_processor::debug_serial.print_string("sub state = ");
			c_processor::debug_serial.print_int32((int32_t)status_message.system_sub_state);c_processor::debug_serial.Write(CR);
			c_processor::debug_serial.print_string("num message = ");
			c_processor::debug_serial.print_int32(status_message.num_message);c_processor::debug_serial.Write(CR);
			c_processor::debug_serial.print_string("chr message = ");
			c_processor::debug_serial.print_string(status_message.chr_message);c_processor::debug_serial.Write(CR);
			
			c_processor::send_status(c_processor::status_message);
			
			
		}
	}
}

BinaryRecords::e_binary_record_types c_processor::load_record(BinaryRecords::e_binary_record_types record_type)
{
	uint16_t record_size = 0;
	switch (record_type)
	{
		case BinaryRecords::e_binary_record_types::Jog:
		{
			record_size = sizeof(BinaryRecords::s_jog_data_block);
			//First byte indicates record type of motion. Make sure its all there before we start loading it.
			if (coordinator_serial.HasRecord(record_size))
			{
				//Clear the struct
				memset(&c_processor::jog_block, 0, record_size);
				//Put the stream into the struct
				memcpy(&c_processor::jog_block
				, c_processor::coordinator_serial.Buffer_Pointer() + c_processor::coordinator_serial.TailPosition()
				, record_size);
				//Move the tail forward to the position we stopped reading the serial buffer from. (probably the head position)
				c_processor::coordinator_serial.AdvanceTail(record_size);
				c_processor::coordinator_serial.Reset();
				return BinaryRecords::e_binary_record_types::Jog;
			}
			else
			{
				//uint32_t data_size = coordinator_serial.DataSize();
				//c_processor::debug_serial.print_string("***record too small(");
				//c_processor::debug_serial.print_int32(data_size);
				//c_processor::debug_serial.print_string(") bytes. expecting (");
				//c_processor::debug_serial.print_int32(record_size);
				//c_processor::debug_serial.print_string(") bytes\r\r");
			}
		}
		break;
		case BinaryRecords::e_binary_record_types::Motion:
		{
			record_size = sizeof(BinaryRecords::s_motion_data_block);
			
			//First byte indicates record type of motion. Make sure its all there before we start loading it.
			if (coordinator_serial.HasRecord(record_size))
			{
				//Clear the struct
				memset(&c_processor::motion_block, 0, record_size);
				//Put the stream into the struct
				memcpy(&c_processor::motion_block
				, c_processor::coordinator_serial.Buffer_Pointer() + c_processor::coordinator_serial.TailPosition()
				, record_size);
				//Move the tail forward to the position we stopped reading the serial buffer from. (probably the head position)
				c_processor::coordinator_serial.AdvanceTail(record_size);
				c_processor::coordinator_serial.Reset();
				return BinaryRecords::e_binary_record_types::Motion;
			}
			else
			{
				//uint32_t data_size = coordinator_serial.DataSize();
				//c_processor::debug_serial.print_string("***record too small(");
				//c_processor::debug_serial.print_int32(data_size);
				//c_processor::debug_serial.print_string(") bytes. expecting (");
				//c_processor::debug_serial.print_int32(record_size);
				//c_processor::debug_serial.print_string(") bytes\r\r");
			}
		}
		break;
		case BinaryRecords::e_binary_record_types::Motion_Control_Setting :
		{
			record_size = sizeof(BinaryRecords::s_motion_control_settings);
			//First byte indicates record type of motion. Make sure its all there before we start loading it.
			if (coordinator_serial.HasRecord(record_size))
			{
				//Clear the struct
				memset(&c_processor::settings_block, 0, record_size);
				//Put the stream into the struct
				memcpy(&c_processor::settings_block
				, c_processor::coordinator_serial.Buffer_Pointer() + c_processor::coordinator_serial.TailPosition()
				, record_size);
				//Move the tail forward to the position we stopped reading the serial buffer from. (probably the head position)
				c_processor::coordinator_serial.AdvanceTail(record_size);
				c_processor::coordinator_serial.Reset();
				return BinaryRecords::e_binary_record_types::Motion_Control_Setting;
			}
		}
		break;

		default:
		return BinaryRecords::e_binary_record_types::Unknown;
		break;
	}
}

BinaryRecords::e_binary_responses c_processor::send_status(BinaryRecords::s_status_message status_data)
{
	char motion_stream[sizeof(BinaryRecords::s_status_message)];
	memcpy(motion_stream, &status_data,sizeof(BinaryRecords::s_motion_data_block));
	
	//Send to record and wait for response.
	BinaryRecords::e_binary_responses resp
	= c_processor::write_stream(motion_stream,sizeof(BinaryRecords::s_motion_data_block), BinaryRecords::e_binary_responses::Ok);

	return resp ;
	
}

BinaryRecords::e_binary_responses c_processor::write_stream(char * stream, uint8_t record_size,BinaryRecords::e_binary_responses Ack_Resp)
{
	uint8_t send_count = 0;
	while (1)
	{
		if (send_count > 4)
		{
			//We tried 4 times to send the record and it kept failing.. SUPER bad..
			return BinaryRecords::e_binary_responses::Data_Error;
		}
		c_processor::coordinator_serial.Write_Record(stream, record_size);
		send_count++;
		//Now we need to wait for the motion controller to confirm it got the data
		if (c_processor::coordinator_serial.WaitForEOL(90000)) //<-- wait until the timeout
		{
			//We timed out. this is bad...
			return BinaryRecords::e_binary_responses::Response_Time_Out;
		}
		else
		{
			//get the response code from the controller
			BinaryRecords::e_binary_responses resp
			= (BinaryRecords::e_binary_responses)c_processor::coordinator_serial.Get();
			
			//there should be a cr after this, we can throw it away
			c_processor::coordinator_serial.Get();
			//If we get a proceed resp, we can break the while. we are done.
			if (resp == Ack_Resp)
			{
				break;
			}

			//if we get to here, we didnt get an ack and we need to resend.
			send_count++;
		}
	}
	return BinaryRecords::e_binary_responses::Ok;
}
