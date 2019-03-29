#include "c_interpollation_hardware.h"
#include "../hardware_def.h"

#ifdef MSVC
#include <iostream>
#include <fstream>

using namespace std;
static ofstream myfile;

#endif // MSVC

Motion_Core::Segment::Bresenham::Bresenham_Item *Motion_Core::Hardware::Interpollation::Change_Check_Exec_Timer_Bresenham; // Tracks the current st_block index. Change indicates new block.
Motion_Core::Segment::Bresenham::Bresenham_Item *Motion_Core::Hardware::Interpollation::Exec_Timer_Bresenham;   // Pointer to the block data for the segment being executed
Motion_Core::Segment::Timer::Timer_Item *Motion_Core::Hardware::Interpollation::Exec_Timer_Item;  // Pointer to the segment being executed

uint32_t Motion_Core::Hardware::Interpollation::system_position[N_AXIS];
uint32_t Motion_Core::Hardware::Interpollation::counter[N_AXIS];
#ifdef STEP_PULSE_DELAY
uint8_t step_bits;  // Stores out_bits output to complete the step pulse delay
#endif

uint8_t Motion_Core::Hardware::Interpollation::step_pulse_time;  // Step pulse reset time after step rise
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

void Motion_Core::Hardware::Interpollation::Initialize()
{
#ifdef MSVC
	myfile.open("acceleration.txt");
#endif // MSVC
	Motion_Core::Hardware::Interpollation::Interpolation_Active = 1;
	Motion_Core::Hardware::Interpollation::step_outbits = step_port_invert_mask;
	Hardware_Abstraction_Layer::Grbl::Stepper::wake_up();

}

void Motion_Core::Hardware::Interpollation::Shutdown()
{
#ifdef MSVC
	myfile.flush();
	myfile.close();
#endif // MSVC
	Hardware_Abstraction_Layer::Grbl::Stepper::st_go_idle();
	
	//Hardware_Abstraction_Layer::Grbl::Stepper::port_disable(pin_state);

}

void Motion_Core::Hardware::Interpollation::step_tick()
{
	if (Motion_Core::Hardware::Interpollation::Step_Active)
	{
		return;
	} // The busy-flag is used to avoid reentering this interrupt

	  // Set the direction pins a couple of nanoseconds before we step the steppers
	  //DIRECTION_PORT = (DIRECTION_PORT & ~DIRECTION_MASK) | (st.dir_outbits & DIRECTION_MASK);
	Hardware_Abstraction_Layer::Grbl::Stepper::port_direction((DIRECTION_PORT & ~DIRECTION_MASK)
		| (Motion_Core::Hardware::Interpollation::dir_outbits & DIRECTION_MASK));

	// Then pulse the stepping pins
#ifdef STEP_PULSE_DELAY
	st.step_bits = (STEP_PORT & ~STEP_MASK) | st.step_outbits; // Store out_bits to prevent overwriting.
#else  // Normal operation
	//STEP_PORT = (STEP_PORT & ~STEP_MASK) | st.step_outbits;
	Hardware_Abstraction_Layer::Grbl::Stepper::port_step((STEP_PORT & ~STEP_MASK) | Motion_Core::Hardware::Interpollation::step_outbits);
#endif

	// Enable step pulse reset timer so that The Stepper Port Reset Interrupt can reset the signal after
	// exactly settings.pulse_microseconds microseconds, independent of the main Timer1 prescaler.
	Hardware_Abstraction_Layer::Grbl::Stepper::pulse_reset_timer();

	Motion_Core::Hardware::Interpollation::Step_Active = 1;
	//sei();
	Hardware_Abstraction_Layer::Core::start_interrupts();
	// Re-enable interrupts to allow Stepper Port Reset Interrupt to fire on-time.
	// NOTE: The remaining code in this ISR will finish before returning to main program.

	// If there is no step segment, attempt to pop one from the stepper buffer

	if (Motion_Core::Hardware::Interpollation::Exec_Timer_Item == NULL)
	{

		// Anything in the buffer? If so, load and initialize next step segment.
		if ((Motion_Core::Hardware::Interpollation::Exec_Timer_Item = Motion_Core::Segment::Timer::Buffer::Current()) != NULL)
		{
#ifdef MSVC
			myfile << Motion_Core::Hardware::Interpollation::Exec_Timer_Item->steps_to_execute_in_this_segment << ",";
			myfile << Motion_Core::Hardware::Interpollation::Exec_Timer_Item->timer_delay_value << ",";
			myfile << '0' + Motion_Core::Hardware::Interpollation::Exec_Timer_Item->timer_prescaler << ",";
			myfile << '0' + Motion_Core::Hardware::Interpollation::Exec_Timer_Item->line_number;
			myfile << "\r";
			myfile.flush();	
#endif
			Hardware_Abstraction_Layer::Grbl::Stepper::TCCR1B_set
			(Motion_Core::Hardware::Interpollation::Exec_Timer_Item->timer_prescaler);

			// Initialize step segment timing per step and load number of steps to execute.
			Hardware_Abstraction_Layer::Grbl::Stepper::OCR1A_set
			(Motion_Core::Hardware::Interpollation::Exec_Timer_Item->timer_delay_value);
			//st.step_count = st.Exec_Timer_Item->steps_to_execute_in_this_segment; // NOTE: Can sometimes be zero when moving slow.

			// If the new segment starts a new planner block, initialize stepper variables and counters.
			// NOTE: When the segment data index changes, this indicates a new planner block.
			if (Motion_Core::Hardware::Interpollation::Change_Check_Exec_Timer_Bresenham !=
				Motion_Core::Hardware::Interpollation::Exec_Timer_Item->bresenham_in_item)
			{
				//New line segment, so this should be the start of a block. 
				Motion_Core::Hardware::Interpollation::Current_Line
					= Motion_Core::Hardware::Interpollation::Exec_Timer_Item->line_number;

				Motion_Core::Hardware::Interpollation::Change_Check_Exec_Timer_Bresenham = 
					Motion_Core::Hardware::Interpollation::Exec_Timer_Item->bresenham_in_item;
				Motion_Core::Hardware::Interpollation::Exec_Timer_Bresenham = 
					Motion_Core::Hardware::Interpollation::Exec_Timer_Item->bresenham_in_item;

				// Initialize Bresenham line and distance counters

				Motion_Core::Hardware::Interpollation::counter[X_AXIS]
					= Motion_Core::Hardware::Interpollation::counter[Y_AXIS] 
					= Motion_Core::Hardware::Interpollation::counter[Z_AXIS]
					= Motion_Core::Hardware::Interpollation::counter[A_AXIS]
					= Motion_Core::Hardware::Interpollation::counter[B_AXIS]
					= Motion_Core::Hardware::Interpollation::counter[C_AXIS]
					=(Motion_Core::Hardware::Interpollation::Exec_Timer_Bresenham->step_event_count >> 1);
			}
			Motion_Core::Hardware::Interpollation::dir_outbits 
				= Motion_Core::Hardware::Interpollation::Exec_Timer_Bresenham->direction_bits
				^ Motion_Core::Hardware::Interpollation::dir_port_invert_mask;

		}
		else
		{
			// Segment buffer empty. Shutdown.
			Motion_Core::Hardware::Interpollation::Shutdown();
			Motion_Core::Hardware::Interpollation::Interpolation_Active = 0; // Flag main program for cycle end

			//c_planner::block_complete = true;
			//c_planner::completed_block = Motion_Core::Hardware::Interpollation::Current_Line;
			return; // Nothing to do but exit.
		}
	}


	//// Check probing state.
	//if (c_system::sys_probe_state == PROBE_ACTIVE)
	//{
	//	c_probe::probe_state_monitor();
	//}

	// Reset step out bits.
	Motion_Core::Hardware::Interpollation::step_outbits = 0;
	for (int i = 0; i < N_AXIS; i++)
	{
		// Execute step displacement profile by Bresenham line algorithm
#ifdef ADAPTIVE_MULTI_AXIS_STEP_SMOOTHING
		st.counter[i] += st.steps[i];
#else
		Motion_Core::Hardware::Interpollation::counter[i] += Motion_Core::Hardware::Interpollation::Exec_Timer_Bresenham->steps[i];
#endif
		if (Motion_Core::Hardware::Interpollation::counter[i] > 
			Motion_Core::Hardware::Interpollation::Exec_Timer_Bresenham->step_event_count)
		{
			Motion_Core::Hardware::Interpollation::step_outbits |= (1 << i);
			Motion_Core::Hardware::Interpollation::counter[i] -=
				Motion_Core::Hardware::Interpollation::Exec_Timer_Bresenham->step_event_count;
			if (Motion_Core::Hardware::Interpollation::Exec_Timer_Bresenham->direction_bits & (1 << i))
				Motion_Core::Hardware::Interpollation::system_position[i]--;
			else
				Motion_Core::Hardware::Interpollation::system_position[i]++;
		}
	}

	//// During a homing cycle, lock out and prevent desired axes from moving.
	//if (Motion_Core::Hardware::Interpollation::sys.state == STATE_HOMING)
	//{
	//	Motion_Core::Hardware::Interpollation::step_outbits &= c_system::sys.homing_axis_lock;
	//}


	//st.step_count--; // Decrement step events count

	if (Motion_Core::Hardware::Interpollation::Exec_Timer_Item->steps_to_execute_in_this_segment > 0)
		Motion_Core::Hardware::Interpollation::Exec_Timer_Item->steps_to_execute_in_this_segment--;
	else
	{
		int x = 0;
	}

	//if (st.step_count == 0)
	if (Motion_Core::Hardware::Interpollation::Exec_Timer_Item->steps_to_execute_in_this_segment == 0)
	{

		// Segment is complete. Discard current segment and advance segment indexing.
		Motion_Core::Hardware::Interpollation::Exec_Timer_Item = NULL;
		Motion_Core::Segment::Timer::Buffer::Advance();

	}
	Motion_Core::Hardware::Interpollation::step_outbits
		^= Motion_Core::Hardware::Interpollation::step_port_invert_mask;  // Apply step port invert mask
	Motion_Core::Hardware::Interpollation::Step_Active = 0;
}