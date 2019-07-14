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

static BinaryRecords::s_motion_data_block jog_mot;
c_Serial Motion_Core::c_processor::coordinator_serial;
c_Serial Motion_Core::c_processor::debug_serial;

static uint32_t serial_try = 0;
static uint32_t last_serial_size = 0;

void Motion_Core::c_processor::initialize()
{
	Motion_Core::c_processor::coordinator_serial = c_Serial(1, 115200); //<--Connect to coordinator
	Motion_Core::c_processor::debug_serial = c_Serial(0, 115200); //<--Connect to host

	Hardware_Abstraction_Layer::Core::initialize();
	Hardware_Abstraction_Layer::MotionCore::Stepper::initialize();
	Hardware_Abstraction_Layer::MotionCore::Inputs::initialize();
	Motion_Core::initialize();
	Hardware_Abstraction_Layer::Core::start_interrupts();
	
	Motion_Core::c_processor::debug_serial.print_string("motion driver ready\r");
	
	
	Motion_Core::c_processor::run();
	Hardware_Abstraction_Layer::Core::critical_shutdown();
	Motion_Core::c_processor::debug_serial.print_string("Critical system fault occurred!\r");
}
//static BinaryRecords::s_motion_data_block mot;
static BinaryRecords::s_motion_data_block mots[10];
static uint16_t mot_index = 0;
void Motion_Core::c_processor::run()
{
	while (!Motion_Core::System::get_control_state_mode(STATE_CRITICAL_FAILURE))
	{
		Motion_Core::c_processor::process_loop();
	}
}

void Motion_Core::c_processor::process_loop()
{
	//Do we have any axis faults reported?
	Motion_Core::c_processor::check_hardware_faults();

	Motion_Core::c_processor::check_control_states();

	//See if there is a record in the serial buffer. If there is load it.
	Motion_Core::c_processor::check_process_record();
	
	//Let this continuously try to fill the segment buffer. If theres no data to process nothing should happen
	Motion_Core::Segment::Arbitrator::Fill_Step_Segment_Buffer();
	
	//If a segment has completed, we should report it.
	Motion_Core::c_processor::check_sequence_complete();
}

void Motion_Core::c_processor::send_status(BinaryRecords::e_system_state_record_types State
,BinaryRecords::e_system_sub_state_record_types SubState ,uint32_t numeric_message
,char* char_message, uint32_t SET_ControlState, uint32_t CLEAR_ControlState)
{
	BinaryRecords::s_status_message new_stat;
	new_stat.system_state = State;
	new_stat.system_sub_state = SubState;
	new_stat.num_message = numeric_message;
	if (char_message !=NULL)
	{
		memcpy(&new_stat.chr_message,char_message,sizeof(new_stat.chr_message));
	}
	
	Motion_Core::c_processor::send_status(new_stat, SET_ControlState, CLEAR_ControlState);
}

BinaryRecords::e_binary_responses Motion_Core::c_processor::send_status(BinaryRecords::s_status_message new_stat, uint32_t SET_ControlState, uint32_t CLEAR_ControlState)
{
	float unit_factor = 1.0;
	//translate the machine position at the time into mm/inches
	unit_factor = ((Motion_Core::Settings::_Settings.machine_units == BinaryRecords::e_unit_types::MM) ? 1.0 : 25.4);
	for (uint8_t i = 0;i<MACHINE_AXIS_COUNT;i++)
	{
		new_stat.position[i] =
		((float)Motion_Core::Hardware::Interpollation::system_position[i]
		/(float)Motion_Core::Settings::_Settings.steps_per_mm[i])* unit_factor;
		c_processor::debug_serial.print_int32(Motion_Core::Hardware::Interpollation::system_position[i]);
		c_processor::debug_serial.Write(CR);
	}
	
	BinaryRecords::e_binary_responses resp =
	c_record_handler::handle_outbound_record(&new_stat,c_processor::coordinator_serial);
	
	//CLEAR mode.
	if (CLEAR_ControlState!=STATE_STATUS_IGNORE)
	Motion_Core::System::clear_control_state_mode(CLEAR_ControlState);
	//SET mode.
	if (SET_ControlState!=STATE_STATUS_IGNORE)
	Motion_Core::System::set_control_state_mode(SET_ControlState);
	return resp;
}

void Motion_Core::c_processor::check_control_states()
{
	if (!Motion_Core::Hardware::Interpollation::Interpolation_Active)
	Motion_Core::System::clear_control_state_mode(STATE_EXEC_MOTION_INTERPOLATION);
	
	//Were we running a jog interpolation?
	if (Motion_Core::System::get_control_state_mode(STATE_EXEC_MOTION_JOG))
	{
		//Is interpolation complete?
		if (!Motion_Core::Hardware::Interpollation::Interpolation_Active)
		{
			Motion_Core::c_processor::send_status(BinaryRecords::e_system_state_record_types::Motion_Complete
			,BinaryRecords::e_system_sub_state_record_types::Jog_Complete
			,0,NULL,STATE_STATUS_IGNORE,STATE_EXEC_MOTION_JOG);
		}
		
	}
}

void Motion_Core::c_processor::process_motion(BinaryRecords::s_motion_data_block *mot)
{
	Motion_Core::System::new_sequence = mot->sequence;
	
	Motion_Core::c_processor::debug_serial.print_string("\ttest.motion_type = "); Motion_Core::c_processor::debug_serial.print_int32((uint32_t)mot->motion_type); Motion_Core::c_processor::debug_serial.Write(CR);
	Motion_Core::c_processor::debug_serial.print_string("\ttest.feed_rate_mode = "); Motion_Core::c_processor::debug_serial.print_int32((uint32_t)mot->feed_rate_mode); Motion_Core::c_processor::debug_serial.Write(CR);
	Motion_Core::c_processor::debug_serial.print_string("\ttest.feed_rate = "); Motion_Core::c_processor::debug_serial.print_int32(mot->feed_rate); Motion_Core::c_processor::debug_serial.Write(CR);
	Motion_Core::c_processor::debug_serial.print_string("\t***axis_data_begin***\r");
	for(uint8_t i=0;i<MACHINE_AXIS_COUNT;i++)
	{
		Motion_Core::c_processor::debug_serial.print_string("\t\ttest.axis_values[");
		Motion_Core::c_processor::debug_serial.print_int32(i);
		Motion_Core::c_processor::debug_serial.print_string("] = ");
		Motion_Core::c_processor::debug_serial.print_float(mot->axis_values[i], 3);
		Motion_Core::c_processor::debug_serial.Write(CR);
	}
	Motion_Core::c_processor::debug_serial.print_string("\t***axis_data_end***\r");
	Motion_Core::c_processor::debug_serial.print_string("\t***arc_data_begin***\r");
	Motion_Core::c_processor::debug_serial.print_string("\t\ttest.arc_values.horizontal_center = ");Motion_Core::c_processor::debug_serial.print_float(mot->arc_values.horizontal_offset, 3); Motion_Core::c_processor::debug_serial.Write(CR);
	Motion_Core::c_processor::debug_serial.print_string("\t\ttest.arc_values.vertical_center = ");Motion_Core::c_processor::debug_serial.print_float(mot->arc_values.vertical_offset, 3); Motion_Core::c_processor::debug_serial.Write(CR);
	Motion_Core::c_processor::debug_serial.print_string("\t\ttest.arc_values.radius = ");Motion_Core::c_processor::debug_serial.print_float(mot->arc_values.Radius, 3); Motion_Core::c_processor::debug_serial.Write(CR);
	Motion_Core::c_processor::debug_serial.print_string("\t***arc_data_end***\r");
	Motion_Core::c_processor::debug_serial.print_string("\ttest.line_number = "); Motion_Core::c_processor::debug_serial.print_int32(mot->line_number); Motion_Core::c_processor::debug_serial.Write(CR);
	
	Motion_Core::Software::Interpollation::load_block(mot);
	Motion_Core::System::set_control_state_mode(STATE_EXEC_MOTION_INTERPOLATION);
	
}

void Motion_Core::c_processor::check_process_record()
{
	BinaryRecords::e_binary_record_types stream_type = (BinaryRecords::e_binary_record_types)Motion_Core::c_processor::coordinator_serial.Peek();
	if ((uint8_t)stream_type == 0)
	return;
	BinaryRecords::e_binary_responses resp_value;
	switch (stream_type)
	{
		case BinaryRecords::e_binary_record_types::Motion:
		{
			BinaryRecords::s_motion_data_block _blk = mots[mot_index];
			resp_value = c_record_handler::handle_inbound_record(&_blk,&Motion_Core::c_processor::coordinator_serial);
			
			if (resp_value != BinaryRecords::e_binary_responses::Ok)
			{
				Motion_Core::c_processor::debug_serial.print_string("Motion Error : ");
				Motion_Core::c_processor::debug_serial.print_int32((int) resp_value);
				return;
			}
			
			if (resp_value == BinaryRecords::e_binary_responses::Ok)
			{
				mot_index++;
				if (mot_index==10)
				{mot_index = 0;}
				Motion_Core::c_processor::process_motion(&_blk);
			}
		}
		break;
		case BinaryRecords::e_binary_record_types::Motion_Control_Setting:
		{
			
			BinaryRecords::s_motion_control_settings _blk;
			resp_value = c_record_handler::handle_inbound_record(&_blk,&Motion_Core::c_processor::coordinator_serial);
			
			if (resp_value != BinaryRecords::e_binary_responses::Ok)
			{
				Motion_Core::c_processor::debug_serial.print_string("Setting Error : ");
				Motion_Core::c_processor::debug_serial.print_int32((int) resp_value);
				return;
			}
			
			if (resp_value == BinaryRecords::e_binary_responses::Ok)
			{
				uint16_t recsize = sizeof(BinaryRecords::s_motion_control_settings);
				uint8_t setting_error = 0;
				for (uint8_t i=0;i<MACHINE_AXIS_COUNT;i++)
				{
					uint32_t new_rate = (_blk.max_rate[i] * _blk.steps_per_mm[i])/60;
					if (new_rate> MAX_STEP_RATE)
					{
						Motion_Core::c_processor::debug_serial.print_string("Setting Error : ");
						Motion_Core::c_processor::debug_serial.print_string("value exceeded.");
						
						
						Motion_Core::c_processor::send_status(BinaryRecords::e_system_state_record_types::System_Error
						, BinaryRecords::e_system_sub_state_record_types::Error_Setting_Max_Rate_Exceeded
						, new_rate,"setting reject\0",STATE_STATUS_IGNORE,STATE_STATUS_IGNORE);
						setting_error = 1;
					}
				}
				if (!setting_error)
				{
					_blk._check_sum = 0;
					//Motion_Core::Settings::_Settings.pulse_length = _blk.pulse_length;
					memcpy(&Motion_Core::Settings::_Settings,&_blk,recsize);
					//reset hardware parameters after update.
					Hardware_Abstraction_Layer::MotionCore::Stepper::initialize();
				}
				
				
				
			}
		}
		break;
		case BinaryRecords::e_binary_record_types::Jog:
		{
			BinaryRecords::s_jog_data_block _blk;
			resp_value = c_record_handler::handle_inbound_record(&_blk,&Motion_Core::c_processor::coordinator_serial);
			if (resp_value != BinaryRecords::e_binary_responses::Ok)
			{
				Motion_Core::c_processor::debug_serial.print_string("Jog Error : ");
				Motion_Core::c_processor::debug_serial.print_int32((int) resp_value);
				Motion_Core::c_processor::debug_serial.print_string("\r");
				return;
			}
			
			if (resp_value == BinaryRecords::e_binary_responses::Ok)
			{
				//Let em know what we are doing.
				//Motion_Core::System::set_control_state_mode(STATE_EXEC_MOTION_JOG);
				Motion_Core::c_processor::send_status(BinaryRecords::e_system_state_record_types::Motion_Active
				,BinaryRecords::e_system_sub_state_record_types::Jog_Running
				,0,NULL,STATE_EXEC_MOTION_JOG,STATE_STATUS_IGNORE);
				
				jog_mot.feed_rate_mode = BinaryRecords::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
				jog_mot.motion_type = BinaryRecords::e_motion_type::rapid_linear;
				
				uint32_t jog_steps = labs(_blk.axis_value * Motion_Core::Settings::_Settings.steps_per_mm[_blk.axis]);
				if (jog_steps<1)
				{
					_blk.axis_value = (float)(1.0/(float)Motion_Core::Settings::_Settings.steps_per_mm[_blk.axis])
					* _blk.axis_value<0.0?-1.0:1.0;
				}
				Motion_Core::c_processor::debug_serial.print_string("run jog");
				jog_mot.axis_values[_blk.axis] = _blk.axis_value;
				
				jog_mot.flag = _blk.flag;
				jog_mot.sequence = 0;
				//send the jog motion. when it completes, check_State_status will send a new
				//status message to the coordinator.
				Motion_Core::c_processor::process_motion(&jog_mot);
				
			}
		}
		break;
		default:
		/* Your code here */
		break;
	}
	
	
}

void Motion_Core::c_processor::check_hardware_faults()
{
	BinaryRecords::s_status_message *status;
	
	//See if there is a hardware alarm from a stepper/servo driver
	if (Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms >0)
	{
		Motion_Core::c_processor::debug_serial.print_string("drive fault\r");
		Motion_Core::System::set_control_state_mode(STATE_ERR_AXIS_DRIVE_FAULT);
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
				
				Motion_Core::c_processor::debug_serial.print_string("Drive on Axis ");
				Motion_Core::c_processor::debug_serial.print_int32(i);
				Motion_Core::c_processor::debug_serial.print_string(" reported a motion fault\r");
			}
			
		}
		c_record_handler::handle_outbound_record(status,Motion_Core::c_processor::coordinator_serial);
		Hardware_Abstraction_Layer::MotionCore::Inputs::Driver_Alarms = 0;
	}
}

void Motion_Core::c_processor::check_sequence_complete()
{
	//If we are holding, or resuming then we cant be complete can we...
	if (Motion_Core::Hardware::Interpollation::Last_Completed_Sequence != 0
	&& !Motion_Core::System::get_control_state_mode(STATE_MOTION_CONTROL_HOLD))
	//&& !Motion_Core::System::get_control_state_mode(STATE_MOTION_CONTROL_RESUME))
	{
		BinaryRecords::s_status_message new_stat;
		
		if (Motion_Core::Hardware::Interpollation::Interpolation_Active)
		{
			new_stat.system_state = BinaryRecords::e_system_state_record_types::Motion_Active;
		}
		else
		{
			new_stat.system_state = BinaryRecords::e_system_state_record_types::Motion_Idle;
		}

		new_stat.system_sub_state = BinaryRecords::e_system_sub_state_record_types::Block_Complete;
		
		new_stat.num_message = Motion_Core::Hardware::Interpollation::Last_Completed_Sequence;
		Motion_Core::Hardware::Interpollation::Last_Completed_Sequence = 0;
		
		BinaryRecords::e_binary_responses resp =
		Motion_Core::c_processor::send_status(new_stat, STATE_STATUS_IGNORE, STATE_STATUS_IGNORE);
		//c_record_handler::handle_outbound_record(status_record,c_processor::coordinator_serial);
		
	}
}




