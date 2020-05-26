/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/

#include "..//..//Configuration/c_configuration.h"
#include "../../talos_hardware_def.h"
#include "c_segment_to_hardware.h"
#include "c_ngc_to_block.h"
#include "support_items/e_state_flag.h"
#include "c_block_to_segment.h"
#include "c_state_control.h"
#include <math.h>

#ifdef MSVC
#include <iostream>
#include <fstream>

using namespace std;
static ofstream myfile;

#endif // MSVC

namespace mtn_cfg = Talos::Configuration::Motion;
namespace int_cfg = Talos::Configuration::Interpreter;
namespace mtn_ctl = Talos::Motion::Core::States;
namespace mot_dat = Talos::Motion::Core::Input;
namespace seg_dat = Talos::Motion::Core::Process;


#define HAL_SET_DIRECTION_PINS()
#define HAL_RELEASE_DRIVE_BREAKS()


namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Output
			{
				/*
				Before interpolation can begin:

				What feed mode are we in? is this a time based motion that we can use a timer on, or
				is it dependent on other components like spindle rpm

				Do we have to wait on other hardware? Spindle up to speed, servo brakes released, etc.
				*/

				//s_common_segment_items Segment::previous_flags{ 0 };
				//__s_motion_block* Segment::active_block = NULL;

				//we are reading/writing to the same buffer in the block_to_segment code
				//so we can use the same storage pointer but we need a new ring buffer to
				//'wrap' around it
				/*c_ring_buffer<__s_motion_block> Segment::motion_buffer
				(mot_dat::Block::motion_buffer._storage_pointer , MOTION_BUFFER_SIZE);*/

				//default the gate to new motion.
				void(*Segment::pntr_next_gate)(void) = Segment::__new_motion;

				static s_timer_item* active_timer_item = NULL;
				static s_bresenham* active_bresenham = NULL;
				static uint32_t active_line_number = 0;
				static uint32_t last_complete_sequence = 0;
				static uint32_t active_sequence = 0;
				static uint16_t step_outbits = 0;
				static int32_t bresenham_counter[MACHINE_AXIS_COUNT]{ 0 };

				void Segment::gate_keeper()
				{
					//This should always point to something... 
					if (Segment::pntr_next_gate != NULL)
						Segment::pntr_next_gate();
				}

				void Segment::__new_motion()
				{
					
#ifdef MSVC
					myfile.open("acceleration.txt");
#endif // MSVC
					__configure_spindle();
				}

				void Segment::__configure_spindle()
				{
					//if there is a spindle record in the buffer, it was put there to run with the current
					//motion block. the same spindle settings may persist over several motion blocks. When
					//a new record appears in the spindle buffer, that indicates a change to the spindle
					//configuration was detected whent he block was processed and that spindle record should
					//be processed before the block motion starts.
					if (!mot_dat::Block::spindle_buffer.has_data())
						return;

					__spindle_start();
				}

				void Segment::__spindle_start()
				{
					__s_spindle_block* spindle_block = mot_dat::Block::spindle_buffer.peek();

					//if this motion requires spindle synch, point he gate keepr to the spindle synch waiter
					if (spindle_block->states.get(e_spindle_state::synch_with_motion))
					{
						//call the synch function
						__spindle_wait_synch();
						//point the gate keeper to synch function so that it calls over and over until
						//1. the timeout waiting for spindle synch, and an error occurs
						//2. the spindle comes on and reaches the target speed.
						Segment::pntr_next_gate = __spindle_wait_synch;
					}
					else
					{
						__release_brakes();
						Segment::pntr_next_gate = Segment::__motion_start;
					}
					
				}

				void Segment::__spindle_wait_synch()
				{
					//loop until timeout or spindle reaches speed
					
					//@speed start motion
					//Segment::pntr_next_gate = __start_motion;

					//@timeout set error
					//Segment::pntr_next_gate = __spindle_wait_synch;
				}

				void Segment::__motion_start()
				{
					mtn_ctl::Output::states.set(mtn_ctl::Output::e_states::interpolation_running);
					//start the timer, this will set all of the wheel in motion
					Hardware_Abstraction_Layer::MotionCore::Stepper::wake_up();
					
					//null gate keeper??? no sure what to do yet... 
					//Segment::pntr_next_gate = NULL;
					
				}

				void Segment::__release_brakes()
				{
					//release output drive breaks
					HAL_RELEASE_DRIVE_BREAKS();

					//The breaks usually have some kind of delay. we need to determine
					//how long the delay should be, set the timer to that period and
					//re-point the next gate to the next logical step.
					Segment::pntr_next_gate = Segment::__run_interpolation;
				}

				void Segment::__run_interpolation()
				{
					//this has been called from an ISR or thread

					//Send outputs to physical hardware
					//Motion_Core::Hardware::Interpolation::send_hardware_outputs();

					// Enable step pulse reset timer so that The Stepper Port Reset Interrupt can reset the signal after
					// exactly settings.pulse_microseconds microseconds, independent of the main Timer1 prescaler.
					//Hardware_Abstraction_Layer::MotionCore::Stepper::pulse_reset_timer();
					//c_processor::debug_serial.print_string("t\r");
					//Motion_Core::Hardware::Interpolation::Step_Active = 1;

					//sei();
					//Hardware_Abstraction_Layer::Core::start_interrupts();

					// If there is no step segment, attempt to pop one from the stepper buffer

					if (active_timer_item == NULL)
					{
						bool end = __config_timer();
						//were there any more segments? if not we are done.
						if (end)
						{
							//set this to null in case the timer fires will we are in here.
							Segment::pntr_next_gate = NULL;
							Segment::__end_interpolation();
						}
					}
					//seems stable to here.
					// Reset step out bits.
					step_outbits = 0;
					for (int i = 0; i < MACHINE_AXIS_COUNT; i++)
					{
						// Execute step displacement profile by Bresenham line algorithm
						bresenham_counter[i] += active_timer_item->common.bres_obj->steps[i];
						if (bresenham_counter[i] >
							active_timer_item->common.bres_obj->step_event_count)
						{
							step_outbits |= (1 << i);
							bresenham_counter[i] -=
								active_timer_item->common.bres_obj->step_event_count;

							//We arent changing system position on a backlash comp motion.
							if (!active_timer_item->common.system.get(e_system_block_state::block_state_skip_sys_pos_update))
							{
								/*if (active_timer_item->common.bres_obj->direction_bits._flag & (1 << i))
									Motion_Core::Hardware::Interpolation::system_position[i]--;
								else
									Motion_Core::Hardware::Interpolation::system_position[i]++;*/
							}
						}
					}

					active_timer_item->steps_to_execute_in_this_segment--;

					//If feedmode is spindle synch, calculate the correct delay value for
					//the feedrate, based on spindle current speed
					//TODO: is there a better way to do this without several if statements?
					if (active_timer_item->common.feed.get(e_feed_block_state::feed_mode_units_per_rotation))
					{
						//only adjust the delay value if we are in 'cruise' state.
						//The arbitrator still controls motion during accel and decel
						if (active_timer_item->common.speed.get(e_speed_block_state::motion_state_cruising))
						{
							//Hardware_Abstraction_Layer::MotionCore::Stepper::OCR1A_set
							//(Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->feedrate_delay);
						}
					}

					if (active_timer_item->steps_to_execute_in_this_segment == 0)
					{
						//We are done with this timer item. Calling a get will move the indexes so the 
						//buffer is empty if it was full before.
						seg_dat::Segment::timer_buffer.get();
						active_timer_item = NULL;
					}
					//step_outbits ^= Motion_Core::Hardware::Interpolation::step_port_invert_mask;  // Apply step port invert mask
				}

				bool Segment::__config_timer()
				{
					if (seg_dat::Segment::timer_buffer.has_data())
					{
						//dont 'get' here otherwise the item we are pointing at will get over written
						//by the segment loader in block_to_segment
						active_timer_item = seg_dat::Segment::timer_buffer.peek();
					}
					else
						int x = 0;

					bool done = true;
					if (active_timer_item != NULL)
					{
						done = false;
#ifdef MSVC
						myfile << active_timer_item->steps_to_execute_in_this_segment << ",";
						myfile << active_timer_item->timer_delay_value << ",";
						myfile << '0' + active_timer_item->common.tracking.line_number << ",";
						myfile << '0' + active_timer_item->common.tracking.sequence;
						myfile << "\r\n";
						myfile.flush();
#endif

						//set timer delay value to the active_timer objects delay time
						/*Hardware_Abstraction_Layer::MotionCore::Stepper::OCR1A_set
						(Motion_Core::Hardware::Interpolation::Exec_Timer_Item->timer_delay_value);*/

						// If the new segment starts a new planner block, initialize stepper variables and counters.
						// NOTE: When the segment data index changes, this indicates a new planner block.
						if (active_bresenham != active_timer_item->common.bres_obj)
						{
							//if we are loading a new block directions MIGHT change, so clear the set flag
							//Motion_Core::Hardware::Interpolation::direction_set = 0;

							//New line segment, so this should be the start of a block.
							active_line_number = active_timer_item->common.tracking.line_number;
							last_complete_sequence = active_sequence;
							active_sequence = active_timer_item->common.tracking.sequence;
							active_bresenham = active_timer_item->common.bres_obj;

							// Initialize Bresenham line and distance counters
							for (int i = 0; i < MACHINE_AXIS_COUNT; i++)
								bresenham_counter[i] = active_timer_item->common.bres_obj->step_event_count;
						}

						/*Motion_Core::Hardware::Interpolation::dir_outbits
							= Motion_Core::Hardware::Interpolation::Exec_Timer_Item->bresenham_in_item->direction_bits
							^ Motion_Core::Hardware::Interpolation::dir_port_invert_mask;*/
					}
					return done;
				}

				void Segment::__end_interpolation()
				{
#ifdef MSVC
					myfile.flush();
					myfile.close();
#endif // MSVC
					__set_brakes();
				}

				void Segment::__set_brakes()
				{
					Segment::pntr_next_gate = Segment::__new_motion;
				}
			}
		}
	}
}
