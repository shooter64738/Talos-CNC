#include "c_interpollation_hardware.h"
#include "../motion_hardware_def.h"
#include "c_motion_core.h"

#ifdef MSVC
#include <iostream>
#include <fstream>

using namespace std;
static ofstream myfile;

#endif // MSVC
#include "c_gateway.h"
#include "c_segment_arbitrator.h"
#include "../Processing/Events/EventHandlers/c_motion_control_event_handler.h"
#include "../Processing/Events/EventHandlers/c_system_event_handler.h"
#include "../Processing/Events/extern_events_types.h"

Motion_Core::Segment::Bresenham::Bresenham_Item *Motion_Core::Hardware::Interpolation::Change_Check_Exec_Timer_Bresenham; // Tracks the current st_block index. Change indicates new block.
Motion_Core::Segment::Timer::Timer_Item *Motion_Core::Hardware::Interpolation::Exec_Timer_Item;  // Pointer to the segment being executed

int32_t Motion_Core::Hardware::Interpolation::system_position[MACHINE_AXIS_COUNT];
uint32_t Motion_Core::Hardware::Interpolation::counter[MACHINE_AXIS_COUNT];

#ifdef STEP_PULSE_DELAY
uint8_t step_bits;  // Stores out_bits output to complete the step pulse delay
#endif

uint32_t Motion_Core::Hardware::Interpolation::Step_Pulse_Length;  // Step pulse reset time after step rise
uint8_t Motion_Core::Hardware::Interpolation::step_outbits;         // The next stepping-bits to be output
uint8_t Motion_Core::Hardware::Interpolation::dir_outbits;
uint32_t Motion_Core::Hardware::Interpolation::Current_Line = 0;

Motion_Core::Segment::Bresenham::Bresenham_Item *Change_Check_Exec_Timer_Bresenham; // Tracks the current st_block index. Change indicates new block.
Motion_Core::Segment::Bresenham::Bresenham_Item *Exec_Timer_Bresenham;   // Pointer to the block data for the segment being executed
Motion_Core::Segment::Timer::Timer_Item *Exec_Timer_Item;  // Pointer to the segment being executed

uint8_t Motion_Core::Hardware::Interpolation::step_port_invert_mask;
uint8_t Motion_Core::Hardware::Interpolation::dir_port_invert_mask;

uint8_t Motion_Core::Hardware::Interpolation::Step_Active = 0;
uint8_t Motion_Core::Hardware::Interpolation::Interpolation_Active = 0;
uint32_t Motion_Core::Hardware::Interpolation::Current_Sequence = 0;
uint32_t Motion_Core::Hardware::Interpolation::Last_Completed_Sequence = 0;

uint8_t Motion_Core::Hardware::Interpolation::direction_set = 0;
e_feed_modes Motion_Core::Hardware::Interpolation::drive_mode;
BinaryRecords::s_encoders * Motion_Core::Hardware::Interpolation::spindle_encoder;
uint32_t Motion_Core::Hardware::Interpolation::spindle_calculated_delay = 0;
uint8_t Motion_Core::Hardware::Interpolation::spindle_synch = 0;

void Motion_Core::Hardware::Interpolation::initialize(BinaryRecords::s_encoders * encoder_data)
{
	Motion_Core::Hardware::Interpolation::spindle_encoder = encoder_data;
}

void Motion_Core::Hardware::Interpolation::interpolation_begin_new_block(BinaryRecords::s_motion_data_block block )
{
	#ifdef MSVC
	myfile.open("acceleration.txt");
	#endif // MSVC

	

	//If we are already active, there isnt anything we need to do here.
	if (!Motion_Core::Hardware::Interpolation::Interpolation_Active)
	{
		Motion_Core::Hardware::Interpolation::Interpolation_Active = 1;
		Motion_Core::Hardware::Interpolation::step_outbits = step_port_invert_mask;
		Motion_Core::Hardware::Interpolation::spindle_encoder->target_rpm = block.spindle_state;
		Motion_Core::Hardware::Interpolation::drive_mode = block.feed_rate_mode;
		
		Motion_Core::Hardware::Interpolation::interpolation_begin();

	}

}

void Motion_Core::Hardware::Interpolation::interpolation_begin()
{
	switch (Motion_Core::Hardware::Interpolation::drive_mode)
	{
		//G93 and G94 feed modes
		case e_feed_modes::FEED_RATE_MINUTES_PER_UNIT_MODE:
		case e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE:
		{

			//This is driven internally by the motion controllers timer
			Hardware_Abstraction_Layer::MotionCore::Stepper::wake_up();
			break;
		}
		//G95 feed mode
		case e_feed_modes::FEED_RATE_UNITS_PER_ROTATION:
		{
			//TODO:Setup a timer for the specified delay time. Error if synch is not complete by then
			//on time out call Motion_Core::Hardware::Interpollation::spindle_synch_timeout()
			
			//Start the delay timer that will timeout if we have reached the
			//delay period before we reach specified spindle speed.
			Hardware_Abstraction_Layer::MotionCore::Spindle::configure_timer_for_at_speed_delay();
			
			//set the waiting event for spindle synch. the event manager will handle when/if
			//interpolation can start.
			
			extern_motion_control_events.event_manager.set((int)s_motion_controller_events::e_event_type::SpindleToSpeedWait);
			
			//This is driven externally by the encoder input from the spindle
			//If feedmode is spindle synch, what for spindle to get to speed if
			//spindle wait is configured
			Motion_Core::Hardware::Interpolation::check_spindle_at_speed();
			
			break;
		}
		default:
		/* Your code here */
		break;
	}
}

void Motion_Core::Hardware::Interpolation::spindle_at_speed_timeout(uint32_t parameter)
{
	extern_motion_control_events.event_manager.set((int)s_motion_controller_events::e_event_type::SpindleToSpeedTimeOut);
	extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemCritical);

	Motion_Core::Hardware::Interpolation::Shutdown();
	Motion_Core::Hardware::Interpolation::Exec_Timer_Item = NULL;
	Motion_Core::Segment::Timer::Buffer::Reset();
}

void Motion_Core::Hardware::Interpolation::Shutdown()
{
	#ifdef MSVC
	myfile.flush();
	myfile.close();
	#endif // MSVC

	Hardware_Abstraction_Layer::MotionCore::Stepper::st_go_idle();

	Motion_Core::Hardware::Interpolation::Interpolation_Active = 0; // Flag main program for cycle end
	Motion_Core::Hardware::Interpolation::Last_Completed_Sequence = Motion_Core::Hardware::Interpolation::Current_Sequence;
	Motion_Core::Hardware::Interpolation::Current_Sequence = 0;
	Motion_Core::Hardware::Interpolation::direction_set = 0;
	Motion_Core::Hardware::Interpolation::step_outbits = 0;
	Motion_Core::Hardware::Interpolation::spindle_synch = false;

	//Hardware_Abstraction_Layer::Grbl::Stepper::port_disable(pin_state);

}

void Motion_Core::Hardware::Interpolation::send_hardware_outputs()
{
	//We do not need to set the direction pins on every interrupt. These should only change when a new block is loaded.
	if (!Motion_Core::Hardware::Interpolation::direction_set)
	{
		Hardware_Abstraction_Layer::MotionCore::Stepper::port_direction((DIRECTION_PORT & ~DIRECTION_MASK)
		| (Motion_Core::Hardware::Interpolation::dir_outbits & DIRECTION_MASK));
		Motion_Core::Hardware::Interpolation::direction_set = 1;
	}

	//Hardware_Abstraction_Layer::MotionCore::Stepper::port_step((STEP_PORT & ~STEP_MASK) | Motion_Core::Hardware::Interpollation::step_outbits);
	Hardware_Abstraction_Layer::MotionCore::Stepper::port_step(Motion_Core::Hardware::Interpolation::step_outbits);
}

uint8_t Motion_Core::Hardware::Interpolation::check_spindle_at_speed()
{
	
	//Do we wait until we are at speed before interpolation?
	if (Motion_Core::Settings::_Settings.Hardware_Settings.spindle_encoder.wait_spindle_at_speed
	&& !Motion_Core::Hardware::Interpolation::spindle_synch)
	{
		//Are we near that speed yet?
		if (!Motion_Core::Hardware::Interpolation::spindle_encoder->near(
		Motion_Core::Settings::_Settings.Hardware_Settings.spindle_encoder.current_rpm
		, Motion_Core::Settings::_Settings.Hardware_Settings.spindle_encoder.target_rpm
		, Motion_Core::Settings::_Settings.Hardware_Settings.spindle_encoder.variable_percent)
		)
		{
			return false;
		}
	}

	/*
	We are still relying on the arbitrator to calculate the acceleration and deceleration.
	The speed is only adjusted AFTER acceleration has completed, and BEFORE deceleration begins
	*/
	//set the at speed event
	extern_motion_control_events.event_manager.set((int)s_motion_controller_events::e_event_type::SpindleAtSpeed);

	return true;
}

void Motion_Core::Hardware::Interpolation::step_tick()
{
	//Send outputs to physical hardware
	Motion_Core::Hardware::Interpolation::send_hardware_outputs();

	// Enable step pulse reset timer so that The Stepper Port Reset Interrupt can reset the signal after
	// exactly settings.pulse_microseconds microseconds, independent of the main Timer1 prescaler.
	Hardware_Abstraction_Layer::MotionCore::Stepper::pulse_reset_timer();
	//c_processor::debug_serial.print_string("t\r");
	Motion_Core::Hardware::Interpolation::Step_Active = 1;

	//sei();
	Hardware_Abstraction_Layer::Core::start_interrupts();

	// If there is no step segment, attempt to pop one from the stepper buffer

	if (Motion_Core::Hardware::Interpolation::Exec_Timer_Item == NULL)
	{
		Motion_Core::Hardware::Interpolation::Exec_Timer_Item = Motion_Core::Segment::Timer::Buffer::Current();
		//c_processor::debug_serial.print_string("n\r");
		// Anything in the buffer? If so, load and initialize next step segment.
		if (Motion_Core::Hardware::Interpolation::Exec_Timer_Item != NULL)
		{
			#ifdef MSVC
			myfile << Motion_Core::Hardware::Interpolation::Exec_Timer_Item->steps_to_execute_in_this_segment << ",";
			myfile << Motion_Core::Hardware::Interpolation::Exec_Timer_Item->timer_delay_value << ",";
			myfile << '0' + Motion_Core::Hardware::Interpolation::Exec_Timer_Item->timer_prescaler << ",";
			myfile << '0' + Motion_Core::Hardware::Interpolation::Exec_Timer_Item->line_number;
			myfile << "\r";
			myfile.flush();
			#endif

			//Hardware_Abstraction_Layer::MotionCore::Stepper::TCCR1B_set
			//(Motion_Core::Hardware::Interpollation::Exec_Timer_Item->timer_prescaler);
			
			Hardware_Abstraction_Layer::MotionCore::Stepper::OCR1A_set
			(Motion_Core::Hardware::Interpolation::Exec_Timer_Item->timer_delay_value);
			
			// If the new segment starts a new planner block, initialize stepper variables and counters.
			// NOTE: When the segment data index changes, this indicates a new planner block.
			if (Motion_Core::Hardware::Interpolation::Change_Check_Exec_Timer_Bresenham !=
			Motion_Core::Hardware::Interpolation::Exec_Timer_Item->bresenham_in_item)
			{
				//if we are loading a new block directions MIGHT change, so clear the set flag
				Motion_Core::Hardware::Interpolation::direction_set = 0;

				//We are starting a new 'block' of gcode. If we were already executing one, report it as complete.

				//New line segment, so this should be the start of a block.
				Motion_Core::Hardware::Interpolation::Current_Line
				= Motion_Core::Hardware::Interpolation::Exec_Timer_Item->line_number;

				Motion_Core::Hardware::Interpolation::Last_Completed_Sequence =
				Motion_Core::Hardware::Interpolation::Current_Sequence;

				Motion_Core::Hardware::Interpolation::Current_Sequence
				= Motion_Core::Hardware::Interpolation::Exec_Timer_Item->sequence;

				Motion_Core::Hardware::Interpolation::Change_Check_Exec_Timer_Bresenham =
				Motion_Core::Hardware::Interpolation::Exec_Timer_Item->bresenham_in_item;

				// Initialize Bresenham line and distance counters
				for (int i = 0; i < MACHINE_AXIS_COUNT; i++)
				Motion_Core::Hardware::Interpolation::counter[i] =
				Motion_Core::Hardware::Interpolation::Exec_Timer_Item->bresenham_in_item->step_event_count;
			}
			Motion_Core::Hardware::Interpolation::dir_outbits
			= Motion_Core::Hardware::Interpolation::Exec_Timer_Item->bresenham_in_item->direction_bits
			^ Motion_Core::Hardware::Interpolation::dir_port_invert_mask;

		}
		else
		{
			// Segment buffer empty. Shutdown.
			Motion_Core::Hardware::Interpolation::Shutdown();
			return; // Nothing to do but exit.
		}
	}
	//seems stable to here.
	// Reset step out bits.
	Motion_Core::Hardware::Interpolation::step_outbits = 0;
	for (int i = 0; i < MACHINE_AXIS_COUNT; i++)
	{
		// Execute step displacement profile by Bresenham line algorithm
		Motion_Core::Hardware::Interpolation::counter[i] += Motion_Core::Hardware::Interpolation::Exec_Timer_Item->bresenham_in_item->steps[i];
		if (Motion_Core::Hardware::Interpolation::counter[i] >
		Motion_Core::Hardware::Interpolation::Exec_Timer_Item->bresenham_in_item->step_event_count)
		{
			Motion_Core::Hardware::Interpolation::step_outbits |= (1 << i);
			Motion_Core::Hardware::Interpolation::counter[i] -=
			Motion_Core::Hardware::Interpolation::Exec_Timer_Item->bresenham_in_item->step_event_count;

			//We arent changing system position on a backlash comp motion.
			if (!Motion_Core::Hardware::Interpolation::Exec_Timer_Item->flag.get((int)BinaryRecords::e_block_flag::block_state_skip_sys_pos_update))
			{
				if (Motion_Core::Hardware::Interpolation::Exec_Timer_Item->bresenham_in_item->direction_bits & (1 << i))
				Motion_Core::Hardware::Interpolation::system_position[i]--;
				else
				Motion_Core::Hardware::Interpolation::system_position[i]++;
			}
		}
	}

	Motion_Core::Hardware::Interpolation::Exec_Timer_Item->steps_to_execute_in_this_segment--;

	//If feedmode is spindle synch, calculate the correct delay value for
	//the feedrate, based on spindle current speed
	//TODO: is there a better way to do this without several if statements?
	if (Motion_Core::Hardware::Interpolation::drive_mode == e_feed_modes::FEED_RATE_UNITS_PER_ROTATION)
	{
		//only adjust the delay value if we are in 'cruise' state.
		//The arbitrator still controls motion during accel and decel
		if (Motion_Core::Hardware::Interpolation::Exec_Timer_Item->flag.get((int)BinaryRecords::e_block_flag::motion_state_cruising))
		{
			
			if (Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->feedrate_delay <1)
			{
				//UART->UART_THR = 'A';
			}
			else
			{
				//UART->UART_THR = 'X';
				Hardware_Abstraction_Layer::MotionCore::Stepper::OCR1A_set
				(Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->feedrate_delay);
			}
		}
	}

	if (Motion_Core::Hardware::Interpolation::Exec_Timer_Item->steps_to_execute_in_this_segment == 0)
	{
		// Segment is complete. Discard current segment and advance segment indexing.
		Motion_Core::Hardware::Interpolation::Exec_Timer_Item = NULL;
		Motion_Core::Segment::Timer::Buffer::Advance();
		//c_processor::debug_serial.print_string("a\r");
	}
	Motion_Core::Hardware::Interpolation::step_outbits
	^= Motion_Core::Hardware::Interpolation::step_port_invert_mask;  // Apply step port invert mask

	Motion_Core::Hardware::Interpolation::Step_Active = 0;
	
	

}