/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_gateway.h"
#include "c_interpollation_software.h"
#include "c_segment_arbitrator.h"
#include "c_motion_core.h"
#include "c_interpollation_hardware.h"
#include "c_system.h"
#include "../../communication_def.h"
#include <string.h>
#include "../Processing/Events/EventHandlers/c_motion_event_handler.h"
#include "../Processing/Events/EventHandlers/c_motion_control_event_handler.h"
#include "../../Shared Data/Event/extern_events_types.h"
#include "../../Shared Data/_s_status_record.h"

#define MOTION_BUFFER_SIZE 2

static s_motion_data_block jog_mot;
//c_Serial Motion_Core::c_processor::coordinator_serial;
c_Serial *Motion_Core::Gateway::local_serial;

//static uint32_t serial_try = 0;
//static uint32_t last_serial_size = 0;
static s_motion_data_block mots[MOTION_BUFFER_SIZE];
static uint16_t mot_index = 0;
static uint16_t motion_buffer_head = 0;
static uint16_t motion_buffer_tail = 0;

void Motion_Core::Gateway::add_motion(s_motion_data_block new_blk)
{
	

	//If the buffer is full we cant add a new motion yet
	if (extern_motion_control_events.event_manager.get((int)s_motion_controller_events::e_event_type::MotionBufferFull))
	{
		return; //<--no space for a new item right now
	}

	s_motion_data_block *_blk = &mots[motion_buffer_head++];
	memcpy(_blk,&new_blk,sizeof(s_motion_data_block));

	if (motion_buffer_head == MOTION_BUFFER_SIZE)
	{
		motion_buffer_head = 0;
	}
	if (motion_buffer_head == motion_buffer_tail)
	{
		extern_motion_control_events.event_manager.set((int)s_motion_controller_events::e_event_type::MotionBufferFull);
	}
	//the buffer defaults to empty, but since we jsut added an item, its not empty anymore
	extern_motion_control_events.event_manager.clear((int)s_motion_controller_events::e_event_type::MotionBufferEmpty);

	extern_motion_control_events.event_manager.set((int)s_motion_controller_events::e_event_type::MotionBufferAdd);
	
	//Motion_Core::Gateway::process_motion(&_blk);
}

void Motion_Core::Gateway::process_loop()
{
	//Do we have any axis faults reported?
	//Motion_Core::Gateway::check_hardware_faults();

	//Motion_Core::Gateway::check_control_states();

	//See if there is a record in the serial buffer. If there is load it.
	//Motion_Core::c_processor::check_process_record();
	
	//Let this continuously try to fill the segment buffer. If theres no data to process nothing should happen
	Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
	
	//If a segment has completed, we should report it.
	Motion_Core::Gateway::check_sequence_complete();
}

void Motion_Core::Gateway::check_control_states()
{
	if (!Motion_Core::Hardware::Interpolation::Interpolation_Active)
	Motion_Core::System::state_mode.control_modes.clear((int) Motion_Core::System::e_control_event_type::Control_motion_interpolation);
	
	//Were we running a jog interpolation?
	if (Motion_Core::System::state_mode.control_modes.get((int)Motion_Core::System::e_control_event_type::Control_jog_motion))
	{
		//Is interpolation complete?
		if (!Motion_Core::Hardware::Interpolation::Interpolation_Active)
		{
			//Motion_Core::Gateway::send_status(BinaryRecords::e_system_state_record_types::Motion_Complete
			//,BinaryRecords::e_system_sub_state_record_types::Jog_Complete
			//,0,NULL,STATE_STATUS_IGNORE,STATE_EXEC_MOTION_JOG);
		}
		
	}
}

void Motion_Core::Gateway::process_motion()
{
	//If head and tail are equal, and the empty flag is set, nothing has been put in the buffer.
	if ((motion_buffer_head == motion_buffer_tail)
		&& extern_motion_control_events.event_manager.get((int)s_motion_controller_events::e_event_type::MotionBufferEmpty))
	{
		return; //<--nothing in the buffer, just return
	}

	//Grab the block at tail position to process, and incriment the tail
	s_motion_data_block *_blk = &mots[motion_buffer_tail++];
	//process the block at tail position
	Motion_Core::Gateway::process_motion(_blk);
	
	
	//since we processed a block, we have freed a space in the buffer, so its no longer 'full'
	extern_motion_control_events.event_manager.clear((int)s_motion_controller_events::e_event_type::MotionBufferFull);

	//See if we are wrapping the buffer
	if (motion_buffer_tail == MOTION_BUFFER_SIZE)
	{
		motion_buffer_tail = 0;
	}

	//If head and tail are equal we have nothing left to consume so set the buffer to empty
	if (motion_buffer_head == motion_buffer_tail)
	{
		extern_motion_control_events.event_manager.set((int)s_motion_controller_events::e_event_type::MotionBufferEmpty);
	}
	
}

void Motion_Core::Gateway::process_motion(s_motion_data_block *mot)
{
	Motion_Core::System::new_sequence = mot->station;
	
	//mot->axis_values[0]=50;
	#ifdef DEBUG_REPORTING
	Motion_Core::Gateway::local_serial->print_string("\ttest.motion_type = "); Motion_Core::Gateway::local_serial->print_int32((uint32_t)mot->motion_type); Motion_Core::Gateway::local_serial->Write(CR);
	Motion_Core::Gateway::local_serial->print_string("\ttest.feed_rate_mode = "); Motion_Core::Gateway::local_serial->print_int32((uint32_t)mot->feed_rate_mode); Motion_Core::Gateway::local_serial->Write(CR);
	Motion_Core::Gateway::local_serial->print_string("\ttest.feed_rate = "); Motion_Core::Gateway::local_serial->print_int32(mot->feed_rate); Motion_Core::Gateway::local_serial->Write(CR);
	Motion_Core::Gateway::local_serial->print_string("\t***axis_data_begin***\r");
	for(uint8_t i=0;i<MACHINE_AXIS_COUNT;i++)
	{
		Motion_Core::Gateway::local_serial->print_string("\t\ttest.axis_values[");
		Motion_Core::Gateway::local_serial->print_int32(i);
		Motion_Core::Gateway::local_serial->print_string("] = ");
		Motion_Core::Gateway::local_serial->print_float(mot->axis_values[i], 3);
		Motion_Core::Gateway::local_serial->Write(CR);
	}
	Motion_Core::Gateway::local_serial->print_string("\t***axis_data_end***\r");
	Motion_Core::Gateway::local_serial->print_string("\t***arc_data_begin***\r");
	Motion_Core::Gateway::local_serial->print_string("\t\ttest.arc_values.horizontal_center = ");Motion_Core::Gateway::local_serial->print_float(mot->arc_values.horizontal_offset, 3); Motion_Core::Gateway::local_serial->Write(CR);
	Motion_Core::Gateway::local_serial->print_string("\t\ttest.arc_values.vertical_center = ");Motion_Core::Gateway::local_serial->print_float(mot->arc_values.vertical_offset, 3); Motion_Core::Gateway::local_serial->Write(CR);
	Motion_Core::Gateway::local_serial->print_string("\t\ttest.arc_values.radius = ");Motion_Core::Gateway::local_serial->print_float(mot->arc_values.Radius, 3); Motion_Core::Gateway::local_serial->Write(CR);
	Motion_Core::Gateway::local_serial->print_string("\t***arc_data_end***\r");
	Motion_Core::Gateway::local_serial->print_string("\ttest.line_number = "); Motion_Core::Gateway::local_serial->print_int32(mot->line_number); Motion_Core::Gateway::local_serial->Write(CR);
	#endif
	//If cycle start is set then start executing the motion. Otherwise jsut hold it while the buffer fills. 
	if (Motion_Core::System::state_mode.control_modes.get((int)Motion_Core::System::e_control_event_type::Control_auto_cycle_start))
	{
		uint16_t return_code = Motion_Core::Software::Interpolation::load_block(mot);
		if (return_code)
		{
			extern_motion_control_events.event_manager.set((int)s_motion_controller_events::e_event_type::BlockExecuting);
			Motion_Core::System::state_mode.control_modes.set((int)Motion_Core::System::e_control_event_type::Control_motion_interpolation);
		}
		else if (return_code == 0)
		{
			//Events::Motion_Controller::events_statistics.num_message = mot->sequence;
			extern_motion_control_events.event_manager.set((int)s_motion_controller_events::e_event_type::BlockDiscarded);
		}
	}
	
}

void Motion_Core::Gateway::check_hardware_faults()
{
	s_status_message *status;
	
	//See if there is a hardware alarm from a stepper/servo driver
	if (Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms >0)
	{
		Motion_Core::System::state_mode.control_modes.set((int)Motion_Core::System::e_control_event_type::Control_axis_drive_fault);
		//Hardware faults but not interpolating... Very strange..
		if (Motion_Core::Hardware::Interpolation::Interpolation_Active)
		{
			//Immediately stop motion
			Motion_Core::Segment::Arbitrator::cycle_hold(); //<--decelerate to a soft stop
		}
		
		
		status->system_state = e_motion_state::System_Error;
		
		for (int i=0;i<MACHINE_AXIS_COUNT;i++)
		{
			//Which axis has faulted?
			if (Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms & (1<<i))
			{
				uint8_t axis_id = (uint8_t) e_motion_sub_state::Error_Axis_Drive_Fault_X
				+ i;
				status->system_sub_state =
				(e_motion_sub_state)axis_id;
				
				//Motion_Core::Gateway::local_serial->print_string("Drive on Axis ");
				//Motion_Core::Gateway::local_serial->print_int32(i);
				//Motion_Core::Gateway::local_serial->print_string(" reported a motion fault\r");
			}
			
		}
		//		c_record_handler::handle_outbound_record(status,Motion_Core::c_processor::coordinator_serial);
		Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms = 0;
	}
}

void Motion_Core::Gateway::check_sequence_complete()
{
	//If we are holding, or resuming then we cant be complete can we...
	if (Motion_Core::Hardware::Interpolation::Last_Completed_Sequence != 0
	&& !Motion_Core::System::state_mode.control_modes.get((int)Motion_Core::System::e_control_event_type::Control_hold_motion))
	//&& !Motion_Core::System::get_control_state_mode(STATE_MOTION_CONTROL_RESUME))
	{
		if (Motion_Core::Hardware::Interpolation::Interpolation_Active)
		{
			//Events::Motion_Controller::events_statistics.system_state = BinaryRecords::e_system_state_record_types::Motion_Active;
		}
		else
		{
			//Events::Motion_Controller::events_statistics.system_state = BinaryRecords::e_system_state_record_types::Motion_Idle;
		}

		//Events::Motion_Controller::events_statistics.system_sub_state = BinaryRecords::e_system_sub_state_record_types::Block_Complete;
		
		//Events::Motion_Controller::events_statistics.num_message = Motion_Core::Hardware::Interpolation::Last_Completed_Sequence;
		Motion_Core::Hardware::Interpolation::Last_Completed_Sequence = 0;
		
		//flag an event so that Main_Processing can pick it up.
		extern_motion_control_events.event_manager.set((int)s_motion_controller_events::e_event_type::BlockComplete);

	}
}




