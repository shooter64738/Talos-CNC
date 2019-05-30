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
#include "../Common/Serial/c_record_handler.h"
#include "c_system.h"





uint8_t c_processor::remote = 0;

c_Serial c_processor::coordinator_serial;
c_Serial c_processor::debug_serial;

static uint32_t serial_try = 0;
static uint32_t last_serial_size = 0;

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
	Hardware_Abstraction_Layer::Core::critical_shutdown();
	c_processor::debug_serial.print_string("Critical system fault occurred!\r");
}
//static BinaryRecords::s_motion_data_block mot;
static BinaryRecords::s_motion_data_block mots[10];
static uint16_t mot_index = 0;
void c_processor::run()
{
	while (!Motion_Core::System::get_control_state_mode(CRITICAL_FAILURE))
	{
		//Do we have any axis faults reported?
		c_processor::check_hardware_faults();

		c_processor::check_control_states();

		//See if there is a record in the serial buffer. If there is load it.
		check_process_record();
		
		//Let this continuously try to fill the segment buffer. If theres no data to process nothing should happen
		Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
		
		//If a segment has completed, we should report it.
		c_processor::check_sequence_complete();
	}
}

void c_processor::send_state_control_status()
{
	//if (Motion_Core::Hardware::Interpollation::Interpolation_Active)
	//{
	//c_record_handler::status_record.system_state = BinaryRecords::e_system_state_record_types::Motion_Active;
	//}
	//else
	//{
	//c_record_handler::status_record.system_state = BinaryRecords::e_system_state_record_types::Motion_Idle;
	//}
	//
	////Default sub state to complete
	//c_record_handler::status_record.system_sub_state = BinaryRecords::e_system_sub_state_record_types::Block_Complete;
	//
	//if (Motion_Core::System::get_control_state_mode(MOTION_CONTROL_HOLD))
	//c_record_handler::status_record.system_sub_state = BinaryRecords::e_system_sub_state_record_types::Block_Holding;
	//if (Motion_Core::System::get_control_state_mode(MOTION_CONTROL_RESUME))
	//c_record_handler::status_record.system_sub_state = BinaryRecords::e_system_sub_state_record_types::Block_Resuming;
	//
	//c_record_handler::status_record.num_message = (float)Motion_Core::Segment::Arbitrator::Active_Block->sequence;
	//
	////c_processor::debug_serial.print_string("state = ");
	////c_processor::debug_serial.print_int32((int32_t)c_record_handler::status_record.system_state);c_processor::debug_serial.Write(CR);
	////c_processor::debug_serial.print_string("sub state = ");
	////c_processor::debug_serial.print_int32((int32_t)c_record_handler::status_record.system_sub_state);c_processor::debug_serial.Write(CR);
	////c_processor::debug_serial.print_string("num message = ");
	////c_processor::debug_serial.print_float(c_record_handler::status_record.num_message,3);c_processor::debug_serial.Write(CR);
	////c_processor::debug_serial.print_string("chr message = ");
	////c_processor::debug_serial.print_string(c_record_handler::status_record.chr_message);c_processor::debug_serial.Write(CR);
	//
	//c_record_handler::handle_outbound_record(c_record_handler::status_record,c_processor::coordinator_serial);
}

void c_processor::check_control_states()
{
	if (!Motion_Core::Hardware::Interpollation::Interpolation_Active)
	Motion_Core::System::clear_control_state_mode(EXEC_MOTION_INTERPOLATION);
	//debug_serial.print_string("state = ");
	//debug_serial.print_int32(Motion_Core::System::control_state_modes);
	//debug_serial.print_string("\r");
	//Were we running a jog interpolation?
	if (Motion_Core::System::get_control_state_mode(EXEC_MOTION_JOG))
	{
		//Is interpolation complete?
		if (!Motion_Core::Hardware::Interpollation::Interpolation_Active)
		{
			//Let the coordinator know we are clear to receive another jog command.
			BinaryRecords::s_status_message new_stat;
			new_stat.system_state = BinaryRecords::e_system_state_record_types::Motion_Complete;
			new_stat.system_sub_state = BinaryRecords::e_system_sub_state_record_types::Jog_Complete;
			
			BinaryRecords::e_binary_responses resp =
			c_record_handler::handle_outbound_record(&new_stat,c_processor::coordinator_serial);
			
			//clear the jog mode.
			Motion_Core::System::clear_control_state_mode(EXEC_MOTION_JOG);
		}
		
	}
}

void c_processor::process_motion(BinaryRecords::s_motion_data_block *mot)
{
	//c_processor::debug_serial.print_string("\t***mot test\r");
	//c_processor::debug_serial.print_string("processing motion \r");
	mot->motion_type = BinaryRecords::e_motion_type::rapid_linear;
	//Did we already process this motion command? We may have had a serial issue that
	//caused the motion to send twice
	if (Motion_Core::System::new_sequence == mot->sequence)
	{
		////c_processor::debug_serial.print_string("\t***duplicate motion \r");
		////c_processor::debug_serial.print_string("\t***motion ignored \r");
		//return;//<--Break out of the switch. We already have this motion.
	}
	
	Motion_Core::System::new_sequence = mot->sequence;
	
	//c_processor::debug_serial.print_string("\ttest.motion_type = "); c_processor::debug_serial.print_int32((uint32_t)mot->motion_type); c_processor::debug_serial.Write(CR);
	//c_processor::debug_serial.print_string("\ttest.feed_rate_mode = "); c_processor::debug_serial.print_int32((uint32_t)mot->feed_rate_mode); c_processor::debug_serial.Write(CR);
	//c_processor::debug_serial.print_string("\ttest.feed_rate = "); c_processor::debug_serial.print_int32(mot->feed_rate); c_processor::debug_serial.Write(CR);
	//c_processor::debug_serial.print_string("\ttest.axis_values[0] = ");c_processor::debug_serial.print_float(mot->axis_values[0], 4); c_processor::debug_serial.Write(CR);
	//c_processor::debug_serial.print_string("\ttest.axis_values[1] = ");c_processor::debug_serial.print_float(mot->axis_values[1], 4); c_processor::debug_serial.Write(CR);
	//c_processor::debug_serial.print_string("\ttest.axis_values[2] = ");c_processor::debug_serial.print_float(mot->axis_values[2], 4); c_processor::debug_serial.Write(CR);
	//c_processor::debug_serial.print_string("\ttest.axis_values[3] = ");c_processor::debug_serial.print_float(mot->axis_values[3], 4); c_processor::debug_serial.Write(CR);
	//c_processor::debug_serial.print_string("\ttest.axis_values[4] = ");c_processor::debug_serial.print_float(mot->axis_values[4], 4); c_processor::debug_serial.Write(CR);
	//c_processor::debug_serial.print_string("\ttest.axis_values[5] = ");c_processor::debug_serial.print_float(mot->axis_values[5], 4); c_processor::debug_serial.Write(CR);
	//c_processor::debug_serial.print_string("\ttest.line_number = "); c_processor::debug_serial.print_int32(mot->line_number); c_processor::debug_serial.Write(CR);
	//
	
	
	Motion_Core::Software::Interpollation::load_block(mot);
	Motion_Core::System::set_control_state_mode(EXEC_MOTION_INTERPOLATION);
	
}

void c_processor::check_process_record()
{
	BinaryRecords::e_binary_record_types stream_type = (BinaryRecords::e_binary_record_types)c_processor::coordinator_serial.Peek();
	if ((uint8_t)stream_type == 0)
	return;
	BinaryRecords::e_binary_responses resp_value;
	switch (stream_type)
	{
		case BinaryRecords::e_binary_record_types::Motion:
		{
			BinaryRecords::s_motion_data_block _blk = mots[mot_index];
			resp_value = c_record_handler::handle_inbound_record(&_blk,&c_processor::coordinator_serial);
			
			if (resp_value != BinaryRecords::e_binary_responses::Ok)
			{
				debug_serial.print_string("Motion Error : ");
				debug_serial.print_int32((int) resp_value);
				return;
			}
			
			if (resp_value == BinaryRecords::e_binary_responses::Ok)
			{
				mot_index++;
				if (mot_index==10)
				{mot_index = 0;}
				process_motion(&_blk);
			}
		}
		break;
		case BinaryRecords::e_binary_record_types::Motion_Control_Setting:
		{
			BinaryRecords::s_motion_control_settings _blk;
			resp_value = c_record_handler::handle_inbound_record(&_blk,&c_processor::coordinator_serial);
			
			if (resp_value != BinaryRecords::e_binary_responses::Ok)
			{
				debug_serial.print_string("Setting Error : ");
				debug_serial.print_int32((int) resp_value);
				return;
			}
			
			if (resp_value == BinaryRecords::e_binary_responses::Ok)
			{
				uint8_t recsize = sizeof(BinaryRecords::s_motion_control_settings);
				memcpy(&Motion_Core::Settings::_Settings,&_blk,recsize);
			}
		}
		break;
		case BinaryRecords::e_binary_record_types::Jog:
		{
			BinaryRecords::s_jog_data_block _blk;
			resp_value = c_record_handler::handle_inbound_record(&_blk,&c_processor::coordinator_serial);
			if (resp_value != BinaryRecords::e_binary_responses::Ok)
			{
				debug_serial.print_string("Jog Error : ");
				debug_serial.print_int32((int) resp_value);
				debug_serial.print_string("\r");
				return;
			}
			
			if (resp_value == BinaryRecords::e_binary_responses::Ok)
			{
				
				Motion_Core::System::set_control_state_mode(EXEC_MOTION_JOG);
				
				BinaryRecords::s_motion_data_block mot;
				mot.feed_rate_mode = BinaryRecords::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
				mot.motion_type = BinaryRecords::e_motion_type::rapid_linear;
				//make sure the jog distance wont be less than 1 step
				//debug_serial.print_string("value = ");
				//debug_serial.print_float(_blk.axis,3);
				//debug_serial.Write(CR);
				uint32_t jog_steps = labs(_blk.axis_value * Motion_Core::Settings::_Settings.steps_per_mm[_blk.axis]);
				//debug_serial.print_string("steps = ");
				//debug_serial.print_int32(jog_steps);
				//debug_serial.Write(CR);
				if (jog_steps<1)
				{
					debug_serial.print_string("recalc\r");
					_blk.axis_value = (float)(1.0/(float)Motion_Core::Settings::_Settings.steps_per_mm[_blk.axis])
					* _blk.axis_value<0.0?-1.0:1.0;
				}
				//debug_serial.print_string("dist = ");
				//debug_serial.print_float(_blk.axis_value,3);
				//debug_serial.Write(CR);
				mot.axis_values[_blk.axis] = _blk.axis_value;
				
				mot.flag = _blk.flag;
				mot.sequence = 0;
				process_motion(&mot);
				
			}
		}
		break;
		default:
		/* Your code here */
		break;
	}
	
	
}

void c_processor::check_hardware_faults()
{
	BinaryRecords::s_status_message *status;
	
	//See if there is a hardware alarm from a stepper/servo driver
	if (Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms >0)
	{
		Motion_Core::System::set_control_state_mode(ERR_AXIS_DRIVE_FAULT);
		//Hardware faults but not interpolating... Very strange..
		if (Motion_Core::Hardware::Interpollation::Interpolation_Active)
		{
			//Immediately stop motion
			Motion_Core::Segment::Arbitrator::cycle_hold(); //<--decelerate to a soft stop
		}
		
		
		status->system_state = BinaryRecords::e_system_state_record_types::System_Error;
		
		for (int i=0;i<MACHINE_AXIS_COUNT;i++)
		{
			//Which axis has faulted?
			if (Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms & (1<<i))
			{
				uint8_t axis_id = (uint8_t) BinaryRecords::e_system_sub_state_record_types::Error_Axis_Drive_Fault_X
				+ i;
				status->system_sub_state =
				(BinaryRecords::e_system_sub_state_record_types)axis_id;
				
				c_processor::debug_serial.print_string("Drive on Axis ");
				c_processor::debug_serial.print_int32(i);
				c_processor::debug_serial.print_string(" reported a motion fault\r");
			}
			
		}
		c_record_handler::handle_outbound_record(status,c_processor::coordinator_serial);
		Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms = 0;
	}
}

void c_processor::check_sequence_complete()
{
	
	
	//If we are holding, or resuming then we cant be complete can we...
	if (Motion_Core::Hardware::Interpollation::Last_Completed_Sequence != 0
	&& !Motion_Core::System::get_control_state_mode(MOTION_CONTROL_HOLD)
	&& !Motion_Core::System::get_control_state_mode(MOTION_CONTROL_RESUME))
	{
		BinaryRecords::s_status_message new_stat;
		BinaryRecords::s_status_message *status_record = &new_stat;
		
		if (Motion_Core::Hardware::Interpollation::Interpolation_Active)
		{
			status_record->system_state = BinaryRecords::e_system_state_record_types::Motion_Active;
		}
		else
		{
			status_record->system_state = BinaryRecords::e_system_state_record_types::Motion_Idle;
		}

		status_record->system_sub_state = BinaryRecords::e_system_sub_state_record_types::Block_Complete;
		
		status_record->num_message = Motion_Core::Hardware::Interpollation::Last_Completed_Sequence;
		Motion_Core::Hardware::Interpollation::Last_Completed_Sequence = 0;
		
		BinaryRecords::e_binary_responses resp =
		c_record_handler::handle_outbound_record(status_record,c_processor::coordinator_serial);
		
	}
}




