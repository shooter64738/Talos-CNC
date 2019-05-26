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
#include "ARM_MotionDriver\ARM_3X8E_MotionDriver\c_record_handler.h"
#include "ARM_MotionDriver\ARM_3X8E_MotionDriver\c_system.h"





uint8_t c_processor::remote = 0;

c_Serial c_processor::coordinator_serial;
c_Serial c_processor::debug_serial;

static uint32_t serial_try = 0;

void c_processor::initialize()
{
	c_processor::coordinator_serial = c_Serial(1, 115200); //<--Connect to coordinator
	c_processor::debug_serial = c_Serial(0, 115200); //<--Connect to host

	Hardware_Abstraction_Layer::Core::initialize();
	Hardware_Abstraction_Layer::MotionCore::Stepper::initialize();
	Hardware_Abstraction_Layer::MotionCore::Inputs::initialize();
	Motion_Core::initialize();
	Hardware_Abstraction_Layer::Core::start_interrupts();
	
	c_processor::debug_serial.print_string("motion driver ready\r");
	
	c_processor::run();
	
}

void c_processor::run()
{
	while (1)
	{
		//Do we have any axis faults reported?
		c_processor::check_hardware_faults();

		//If we were processing a jog record, we will let the coordinator know when we are done with the jog
		//so that the jog messages do not 'pile up' and over run the serial buffer. (Coordinator will not send
		//another jog command until we acknowledge we are done with the current one)
		c_processor::check_jog_completed();

		//See if there is a record in the serial buffer. If there is load it.
		c_processor::check_process_record(c_processor::check_serial_input());
		
		//Let this continuously try to fill the segment buffer. If theres no data to process nothing should happen
		Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
		
		//If a segment has completed, we should report it.
		c_processor::check_sequence_complete();
		
	}
}

void c_processor::check_jog_completed()
{
	//Were we running a jog interpolation?
	if (Motion_Core::System::control_state_modes == CONTROL_MODE_JOG)
	{
		//Is interpolation complete?
		if (Motion_Core::Hardware::Interpollation::Interpolation_Active == 0)
		{
			//Let the coordinator know we are clear to receive another jog command.
			c_processor::coordinator_serial.Write((char)BinaryRecords::e_binary_responses::Jog_Complete); c_processor::coordinator_serial.Write(CR);
			//clear the jog mode.
			Motion_Core::System::control_state_modes = CONTROL_MODE_NONE;
		}
	}
}

void c_processor::check_process_record(BinaryRecords::e_binary_record_types record_type)
{
	
	if (record_type != BinaryRecords::e_binary_record_types::Unknown)
	{
		switch (record_type)
		{
			case BinaryRecords::e_binary_record_types::Jog:
			{
				c_processor::debug_serial.print_string("processing jog \r");
				c_record_handler::Inbound::handle_jog();
			}
			break;
			case BinaryRecords::e_binary_record_types::Motion:
			{
				c_processor::debug_serial.print_string("processing motion \r");
				c_record_handler::Inbound::handle_motion();
				
			}
			break;
			case BinaryRecords::e_binary_record_types::Motion_Control_Setting:
			{
				c_processor::debug_serial.print_string("processing motion control setting \r");
				c_record_handler::Inbound::handle_motion_control_settings();
				
			}
			break;
			default:
			{
				//We do not know what this record type is.
			}
			break;
		}
	}
	else
	{
		//c_processor::debug_serial.print_string("processing unknown\r")	;
	}
}

BinaryRecords::e_binary_record_types c_processor::check_serial_input()
{
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
			return BinaryRecords::e_binary_record_types::Unknown;

		}
		else
		{
			c_processor::debug_serial.print_string("loading\r");
			//we have data, determine its type and load it (if its all there yet).
			record_type = c_record_handler::Inbound::handle_record(record_type,c_processor::coordinator_serial);
		}
		
		return record_type;
	}
	return BinaryRecords::e_binary_record_types::Unknown;
}

void c_processor::check_hardware_faults()
{
	//See if there is a hardware alarm from a stepper/servo driver
	if (Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms >0)
	{
		//Immediately stop motion
		Motion_Core::Segment::Arbitrator::cycle_hold(); //<--decelerate to a soft stop
		
		c_record_handler::status_record.system_state = BinaryRecords::e_system_state_record_types::System_Error;
		for (int i=0;i<MACHINE_AXIS_COUNT;i++)
		{
			//Which axis has faulted?
			if (Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms & (1<<i))
			{
				c_record_handler::status_record.system_sub_state =  (BinaryRecords::e_system_sub_state_record_types) i;
				
				c_processor::debug_serial.print_string("Drive on Axis ");
				c_processor::debug_serial.print_int32(i);
				c_processor::debug_serial.print_string(" reported a motion fault\r");
			}
			
		}
		Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms = 0;
	}
}

void c_processor::check_sequence_complete()
{
	if (Motion_Core::Hardware::Interpollation::Last_Completed_Sequence != 0 )
	{
		c_processor::debug_serial.print_string("Block ");
		c_processor::debug_serial.print_int32(Motion_Core::Hardware::Interpollation::Last_Completed_Sequence);
		c_processor::debug_serial.print_string(" check_sequence_complete()\r");
		
		if (Motion_Core::Hardware::Interpollation::Interpolation_Active)
		{
			c_record_handler::status_record.system_state = BinaryRecords::e_system_state_record_types::Motion_Active;
		}
		else
		{
			c_record_handler::status_record.system_state = BinaryRecords::e_system_state_record_types::Motion_Idle;
		}
		
		if (!Motion_Core::System::StepControl)
		{
			//if step control is zero, lets see if we reported this as a hold previously
			if (c_record_handler::status_record.system_sub_state == BinaryRecords::e_system_sub_state_record_types::Block_Holding)
			{
				//We were holding, this is just a resuming block
				c_record_handler::status_record.system_sub_state = BinaryRecords::e_system_sub_state_record_types::Block_Resuming;
			}
			else
			{
				//We werent holding, this is just a completed block
				c_record_handler::status_record.system_sub_state = BinaryRecords::e_system_sub_state_record_types::Block_Complete;
			}
		}
		//TODO: expand this to show different hold states.
		else if (Motion_Core::System::StepControl > 0 )
		{
			c_record_handler::status_record.system_sub_state = BinaryRecords::e_system_sub_state_record_types::Block_Holding;
		}
		
		c_record_handler::status_record.num_message = Motion_Core::Hardware::Interpollation::Last_Completed_Sequence;
		Motion_Core::Hardware::Interpollation::Last_Completed_Sequence = 0;
		
		c_processor::debug_serial.print_string("state = ");
		c_processor::debug_serial.print_int32((int32_t)c_record_handler::status_record.system_state);c_processor::debug_serial.Write(CR);
		c_processor::debug_serial.print_string("sub state = ");
		c_processor::debug_serial.print_int32((int32_t)c_record_handler::status_record.system_sub_state);c_processor::debug_serial.Write(CR);
		c_processor::debug_serial.print_string("num message = ");
		c_processor::debug_serial.print_int32(c_record_handler::status_record.num_message);c_processor::debug_serial.Write(CR);
		c_processor::debug_serial.print_string("chr message = ");
		c_processor::debug_serial.print_string(c_record_handler::status_record.chr_message);c_processor::debug_serial.Write(CR);
		
		c_record_handler::Outbound::handle_status(c_record_handler::status_record,c_processor::coordinator_serial);
		
		
	}
}




