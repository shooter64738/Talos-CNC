#include "c_interpollation_hardware.h"
#include "hardware_def.h"
#include "c_motion_core.h"

#ifdef MSVC
#include <iostream>
#include <fstream>

using namespace std;
static ofstream myfile;

#endif // MSVC
#include "c_processor.h"
#include "c_segment_arbitrator.h"

Motion_Core::Segment::Bresenham::Bresenham_Item *Motion_Core::Hardware::Interpollation::Change_Check_Exec_Timer_Bresenham; // Tracks the current st_block index. Change indicates new block.
Motion_Core::Segment::Timer::Timer_Item *Motion_Core::Hardware::Interpollation::Exec_Timer_Item;  // Pointer to the segment being executed

int32_t Motion_Core::Hardware::Interpollation::system_position[MACHINE_AXIS_COUNT];
uint32_t Motion_Core::Hardware::Interpollation::counter[MACHINE_AXIS_COUNT];

#ifdef STEP_PULSE_DELAY
uint8_t step_bits;  // Stores out_bits output to complete the step pulse delay
#endif

uint32_t Motion_Core::Hardware::Interpollation::Step_Pulse_Length;  // Step pulse reset time after step rise
uint8_t Motion_Core::Hardware::Interpollation::step_outbits;         // The next stepping-bits to be output
uint8_t Motion_Core::Hardware::Interpollation::dir_outbits;
uint32_t Motion_Core::Hardware::Interpollation::Current_Line = 0;

Motion_Core::Segment::Bresenham::Bresenham_Item *Change_Check_Exec_Timer_Bresenham; // Tracks the current st_block index. Change indicates new block.
Motion_Core::Segment::Bresenham::Bresenham_Item *Exec_Timer_Bresenham;   // Pointer to the block data for the segment being executed
Motion_Core::Segment::Timer::Timer_Item *Exec_Timer_Item;  // Pointer to the segment being executed

uint8_t Motion_Core::Hardware::Interpollation::step_port_invert_mask;
uint8_t Motion_Core::Hardware::Interpollation::dir_port_invert_mask;

uint8_t Motion_Core::Hardware::Interpollation::Step_Active = 0;
uint8_t Motion_Core::Hardware::Interpollation::Interpolation_Active = 0;
uint32_t Motion_Core::Hardware::Interpollation::Current_Sequence = 0;
uint32_t Motion_Core::Hardware::Interpollation::Last_Completed_Sequence = 0;

uint8_t Motion_Core::Hardware::Interpollation::direction_set = 0;
BinaryRecords::e_feed_modes Motion_Core::Hardware::Interpollation::drive_mode;


void Motion_Core::Hardware::Interpollation::Initialize()
{
	#ifdef MSVC
	myfile.open("acceleration.txt");
	#endif // MSVC

//memset(&Motion_Core::Hardware::Interpollation::system_position,0,sizeof(Motion_Core::Hardware::Interpollation::system_position));
	
	//if (!Motion_Core::Hardware::Interpollation::Interpolation_Active)
	{
		Motion_Core::Hardware::Interpollation::Interpolation_Active = 1;
		Motion_Core::Hardware::Interpollation::step_outbits = step_port_invert_mask;
		
		switch (Motion_Core::Hardware::Interpollation::drive_mode)
		{
			//G93 and G94 feed modes
			case BinaryRecords::e_feed_modes::FEED_RATE_MINUTES_PER_UNIT_MODE:
			case BinaryRecords::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE:
			{
				
				//This is driven internally by the motion controllers timer
				Hardware_Abstraction_Layer::MotionCore::Stepper::wake_up();
			}
			//G95 feed mode
			case BinaryRecords::e_feed_modes::FEED_RATE_UNITS_PER_ROTATION:
			{
				//Motion_Core::Hardware::Interpollation::Drive_With_Timer();
			}
			break;
			default:
			/* Your code here */
			break;
		}
		
	}

}

void Motion_Core::Hardware::Interpollation::Shutdown()
{
	#ifdef MSVC
	myfile.flush();
	myfile.close();
	#endif // MSVC
	
	Hardware_Abstraction_Layer::MotionCore::Stepper::st_go_idle();
	
	Motion_Core::Hardware::Interpollation::Interpolation_Active = 0; // Flag main program for cycle end
	Motion_Core::Hardware::Interpollation::Last_Completed_Sequence = Motion_Core::Hardware::Interpollation::Current_Sequence;
	Motion_Core::Hardware::Interpollation::Current_Sequence = 0;
	Motion_Core::Hardware::Interpollation::direction_set = 0;
	Motion_Core::Hardware::Interpollation::step_outbits = 0;

	//Hardware_Abstraction_Layer::Grbl::Stepper::port_disable(pin_state);

}

void Motion_Core::Hardware::Interpollation::step_tick()
{
	
	//We do not need to set the direction pins on every interrupt. These should only change when a new block is loaded.
	if (!Motion_Core::Hardware::Interpollation::direction_set)
	{
		Hardware_Abstraction_Layer::MotionCore::Stepper::port_direction((DIRECTION_PORT & ~DIRECTION_MASK)
		| (Motion_Core::Hardware::Interpollation::dir_outbits & DIRECTION_MASK));
		Motion_Core::Hardware::Interpollation::direction_set = 1;
	}
	
	//Hardware_Abstraction_Layer::MotionCore::Stepper::port_step((STEP_PORT & ~STEP_MASK) | Motion_Core::Hardware::Interpollation::step_outbits);
	Hardware_Abstraction_Layer::MotionCore::Stepper::port_step(Motion_Core::Hardware::Interpollation::step_outbits);
	//c_processor::debug_serial.print_string("t\r");

	// Enable step pulse reset timer so that The Stepper Port Reset Interrupt can reset the signal after
	// exactly settings.pulse_microseconds microseconds, independent of the main Timer1 prescaler.
	Hardware_Abstraction_Layer::MotionCore::Stepper::pulse_reset_timer();
//c_processor::debug_serial.print_string("t\r");
	Motion_Core::Hardware::Interpollation::Step_Active = 1;

	//sei();
	Hardware_Abstraction_Layer::Core::start_interrupts();

	// If there is no step segment, attempt to pop one from the stepper buffer

	if (Motion_Core::Hardware::Interpollation::Exec_Timer_Item == NULL)
	{
		Motion_Core::Hardware::Interpollation::Exec_Timer_Item = Motion_Core::Segment::Timer::Buffer::Current();
		//c_processor::debug_serial.print_string("n\r");
		// Anything in the buffer? If so, load and initialize next step segment.
		if (Motion_Core::Hardware::Interpollation::Exec_Timer_Item != NULL)
		{
			//c_processor::debug_serial.print_string("step");
			//c_processor::debug_serial.print_int32(Motion_Core::Hardware::Interpollation::Exec_Timer_Item->steps_to_execute_in_this_segment);
			//c_processor::debug_serial.print_string(" tail");
			//c_processor::debug_serial.print_int32(Motion_Core::Segment::Timer::Buffer::_tail);
			//c_processor::debug_serial.print_string(" head");
			//c_processor::debug_serial.print_int32(Motion_Core::Segment::Timer::Buffer::_head);
			//c_processor::debug_serial.Write(CR);
			//if (Motion_Core::Hardware::Interpollation::Exec_Timer_Item->steps_to_execute_in_this_segment ==0)
			//{
				//Motion_Core::Segment::Timer::Buffer::_tail--;
				//Motion_Core::Hardware::Interpollation::Shutdown();
			//}
			//
			#ifdef MSVC
			myfile << Motion_Core::Hardware::Interpollation::Exec_Timer_Item->steps_to_execute_in_this_segment << ",";
			myfile << Motion_Core::Hardware::Interpollation::Exec_Timer_Item->timer_delay_value << ",";
			myfile << '0' + Motion_Core::Hardware::Interpollation::Exec_Timer_Item->timer_prescaler << ",";
			myfile << '0' + Motion_Core::Hardware::Interpollation::Exec_Timer_Item->line_number;
			myfile << "\r";
			myfile.flush();
			#endif

			//Hardware_Abstraction_Layer::MotionCore::Stepper::TCCR1B_set
			//(Motion_Core::Hardware::Interpollation::Exec_Timer_Item->timer_prescaler);

			Hardware_Abstraction_Layer::MotionCore::Stepper::OCR1A_set
			(Motion_Core::Hardware::Interpollation::Exec_Timer_Item->timer_delay_value);

			// If the new segment starts a new planner block, initialize stepper variables and counters.
			// NOTE: When the segment data index changes, this indicates a new planner block.
			if (Motion_Core::Hardware::Interpollation::Change_Check_Exec_Timer_Bresenham !=
			Motion_Core::Hardware::Interpollation::Exec_Timer_Item->bresenham_in_item)
			{
				//if we are loading a new block directions MIGHT change, so clear the set flag
				Motion_Core::Hardware::Interpollation::direction_set = 0;
				
				//We are starting a new 'block' of gcode. If we were already executing one, report it as complete.
				
				//New line segment, so this should be the start of a block.
				Motion_Core::Hardware::Interpollation::Current_Line
				= Motion_Core::Hardware::Interpollation::Exec_Timer_Item->line_number;
				
				Motion_Core::Hardware::Interpollation::Last_Completed_Sequence =
				Motion_Core::Hardware::Interpollation::Current_Sequence;
				
				Motion_Core::Hardware::Interpollation::Current_Sequence
				= Motion_Core::Hardware::Interpollation::Exec_Timer_Item->sequence;

				Motion_Core::Hardware::Interpollation::Change_Check_Exec_Timer_Bresenham =
				Motion_Core::Hardware::Interpollation::Exec_Timer_Item->bresenham_in_item;

				// Initialize Bresenham line and distance counters
				for (int i = 0; i < MACHINE_AXIS_COUNT; i++)
				Motion_Core::Hardware::Interpollation::counter[i] =
				Motion_Core::Hardware::Interpollation::Exec_Timer_Item->bresenham_in_item->step_event_count;
			}
			Motion_Core::Hardware::Interpollation::dir_outbits
			= Motion_Core::Hardware::Interpollation::Exec_Timer_Item->bresenham_in_item->direction_bits
			^ Motion_Core::Hardware::Interpollation::dir_port_invert_mask;

		}
		else
		{
			// Segment buffer empty. Shutdown.
			Motion_Core::Hardware::Interpollation::Shutdown();
			return; // Nothing to do but exit.
		}
	}
//seems stable to here.
	// Reset step out bits.
	Motion_Core::Hardware::Interpollation::step_outbits = 0;
	for (int i = 0; i < MACHINE_AXIS_COUNT; i++)
	{
		// Execute step displacement profile by Bresenham line algorithm
		Motion_Core::Hardware::Interpollation::counter[i] += Motion_Core::Hardware::Interpollation::Exec_Timer_Item->bresenham_in_item->steps[i];
		if (Motion_Core::Hardware::Interpollation::counter[i] >
		Motion_Core::Hardware::Interpollation::Exec_Timer_Item->bresenham_in_item->step_event_count)
		{
			Motion_Core::Hardware::Interpollation::step_outbits |= (1 << i);
			Motion_Core::Hardware::Interpollation::counter[i] -=
			Motion_Core::Hardware::Interpollation::Exec_Timer_Item->bresenham_in_item->step_event_count;
			
			//We arent changing system position on a backlash comp motion.
			if (Motion_Core::Hardware::Interpollation::Exec_Timer_Item->flag == BinaryRecords::e_block_flag::normal)
			{
				if (Motion_Core::Hardware::Interpollation::Exec_Timer_Item->bresenham_in_item->direction_bits & (1 << i))
				Motion_Core::Hardware::Interpollation::system_position[i]--;
				else
				Motion_Core::Hardware::Interpollation::system_position[i]++;
			}
		}
	}

	Motion_Core::Hardware::Interpollation::Exec_Timer_Item->steps_to_execute_in_this_segment--;
	
	if (Motion_Core::Hardware::Interpollation::Exec_Timer_Item->steps_to_execute_in_this_segment == 0)
	{
		// Segment is complete. Discard current segment and advance segment indexing.
		Motion_Core::Hardware::Interpollation::Exec_Timer_Item = NULL;
		Motion_Core::Segment::Timer::Buffer::Advance();
//c_processor::debug_serial.print_string("a\r");
	}
	Motion_Core::Hardware::Interpollation::step_outbits
	^= Motion_Core::Hardware::Interpollation::step_port_invert_mask;  // Apply step port invert mask
	
	Motion_Core::Hardware::Interpollation::Step_Active = 0;
	
}