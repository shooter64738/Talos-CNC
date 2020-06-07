/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/

#include "../../Configuration/c_configuration.h"
#include "../../talos_hardware_def.h"
#include "c_segment_to_hardware.h"
#include "c_ngc_to_block.h"
#include "c_block_to_segment.h"
#include "../Processing/State_Control/c_motion_state_control.h"
#include "c_hardware_out.h"
#include <math.h>

#ifdef MSVC
#include <iostream>
#include <fstream>

using namespace std;
static ofstream myfile;

#endif // MSVC

namespace mtn_cfg = Talos::Configuration::Motion;
namespace int_cfg = Talos::Configuration::Interpreter;
namespace mtn_ctl_sta = Talos::Motion::Core::States;
namespace mot_dat = Talos::Motion::Core::Input;
namespace seg_dat = Talos::Motion::Core::Process;
namespace hrd_out = Talos::Motion::Core::Output;
namespace hal_mtn = Hardware_Abstraction_Layer::MotionCore;

#define AXIS1 0
#define AXIS2 1
#define AXIS3 2
#define AXIS4 3
#define AXIS5 4
#define AXIS6 5



//#define HAL_SET_DIRECTION_PINS()
//#define HAL_RELEASE_DRIVE_BREAKS()


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

				//default the gate to new motion.
				void(*Segment::pntr_next_gate)(void) = Segment::__new_motion;
				uint16_t(*Segment::pntr_driver)(void) = NULL;

				Segment::s_persisted Segment::_persisted{};

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
					//__configure_spindle();
					//__release_brakes();
					__motion_start();
				}

				void Segment::__configure_spindle()
				{
					//if there is a spindle record in the buffer, it was put there to run with the current
					//motion block. the same spindle settings may persist over several motion blocks. When
					//a new record appears in the spindle buffer, that indicates a change to the spindle
					//configuration was detected when the block was processed and that spindle record should
					//be processed before the motion block.
					if (!mot_dat::Block::spindle_buffer.has_data())
						return;

					memcpy(&_persisted.spindle_block, mot_dat::Block::spindle_buffer.get(), sizeof(__s_spindle_block));

					hrd_out::Hardware::Spindle::start(_persisted.spindle_block);

					if (_persisted.spindle_block.states.get(e_spindle_state::synch_with_motion))
					{
						//call the synch function
						__spindle_wait_synch();
						//point the gate keeper to synch function so that it calls over and over until
						//1. the timeout waiting for spindle synch, and an error occurs
						//2. the spindle comes on and reaches the target speed.
						Segment::pntr_next_gate = __spindle_wait_synch;
					}
				}

				void Segment::__spindle_wait_synch()
				{
					//loop until timeout or spindle reaches speed

					//@speed start motion
					uint32_t rpm = hrd_out::Hardware::Spindle::get_speed(_persisted.spindle_block);
					if (rpm = _persisted.spindle_block.rpm)
					{
						//Segment::pntr_next_gate = __start_motion;
					}

					//@timeout set error
					//Segment::pntr_next_gate = __spindle_wait_synch;
				}

				void Segment::__motion_start()
				{
					/*
					Clear the gate keeper so that no new calls come in here.
					The motion will execute everything thats coming in, as it
					comes in. We do not need to 'start' motion again until:
					1. The feed mode changes to something requiring a different
					drive mode. Such as spindle synch. Motion will appear to
					just continue to the operator, but in reality we have:
						a.'stopped'
						b. 'reconfigured the spindle for the new feed mode
						c. started the motion again, all within about 5us
					2. All motion is executed and the interpollation 'stops'.
					At the stop point, the gatekeeper is reset so that new motions
					can be started again.

					Effectively this will run as one continuous motion until 1 of
					the 2 conditions above are met.
					*/

					Segment::pntr_next_gate = NULL;

					//The timer isr will call 'driver' 
					Segment::pntr_driver = __run_interpolation;

					mtn_ctl_sta::Output::states.set(mtn_ctl_sta::Output::e_states::interpolation_running);
					mtn_ctl_sta::Output::states.clear(mtn_ctl_sta::Output::e_states::interpolation_complete);
					
					//start the timer, this will set all of the wheel in motion
					//hrd_out::Hardware::Motion::initialize();

					hrd_out::Hardware::Motion::enable();
					hrd_out::Hardware::Motion::start();
					//null gate keeper??? not sure what to do yet... 
					//Segment::pntr_next_gate = NULL;

				}

				void Segment::__release_brakes()
				{
					//release output drive breaks
					hrd_out::Hardware::Motion::brakes(0);

					//The breaks usually have some kind of delay. we need to determine
					//how long the delay should be, set the timer to that period and
					//re-point the next gate to the next logical step.
					//Segment::pntr_next_gate = Segment::__run_interpolation;
				}

				static uint32_t holdit = 0;
				static uint32_t axis_loop = MACHINE_AXIS_COUNT;

				uint16_t Segment::__run_interpolation()
				{
					uint32_t pending_pin_shutoff;

					//this has been called from an ISR or thread
					//Send outputs to physical hardware
					//hrd_out::Hardware::Motion::step(&_persisted.step_outbits);
					//hal_mtn::Stepper::step_port(_persisted.step_outbits);
					/*STEPPER_PUL_PORT_DIRECT_REGISTER =
						(STEPPER_PUL_PORT_DIRECT_REGISTER & ~STEP_MASK) | _persisted.step_outbits;*/

					if (Segment::_persisted.seg == NULL)
					{
						bool end = __load_segment();
						//were there any more segments? if not we are done.
						if (end)
						{
							__end_interpolation();
							return 0;
						}
					}
					//seems stable to here.
					// Reset step out bits.
					_persisted.step_outbits = 1;

					//This is ugly because its all single line, but its twice as fast as a for loop

					/*STEPPER_PUL_PORT_DIRECT_REGISTER =
						(STEPPER_PUL_PORT_DIRECT_REGISTER & ~STEP_MASK) | 1;*/


						_persisted.bresenham_counter[AXIS1] += _persisted.seg->common.bres_obj->steps[AXIS1];
						if (_persisted.bresenham_counter[AXIS1] > _persisted.seg->common.bres_obj->major_axis)
						{
							_persisted.step_outbits |= (1 << AXIS1);
							_persisted.bresenham_counter[AXIS1] -= _persisted.seg->common.bres_obj->major_axis;
						}
						//return; //5.47us here

						_persisted.bresenham_counter[AXIS2] += _persisted.seg->common.bres_obj->steps[AXIS2];
						if (_persisted.bresenham_counter[AXIS2] > _persisted.seg->common.bres_obj->major_axis)
						{
							_persisted.step_outbits |= (1 << AXIS2);
							_persisted.bresenham_counter[AXIS2] -= _persisted.seg->common.bres_obj->major_axis;
						}
						//return; //7.13us here ~0.93
						_persisted.bresenham_counter[AXIS3] += _persisted.seg->common.bres_obj->steps[AXIS3];
						if (_persisted.bresenham_counter[AXIS3] > _persisted.seg->common.bres_obj->major_axis)
						{
							_persisted.step_outbits |= (1 << AXIS3);
							_persisted.bresenham_counter[AXIS3] -= _persisted.seg->common.bres_obj->major_axis;
						}

						_persisted.bresenham_counter[AXIS4] += _persisted.seg->common.bres_obj->steps[AXIS4];
						if (_persisted.bresenham_counter[AXIS4] > _persisted.seg->common.bres_obj->major_axis)
						{
							_persisted.step_outbits |= (1 << AXIS4);
							_persisted.bresenham_counter[AXIS4] -= _persisted.seg->common.bres_obj->major_axis;
						}

						_persisted.bresenham_counter[AXIS5] += _persisted.seg->common.bres_obj->steps[AXIS5];
						if (_persisted.bresenham_counter[AXIS5] > _persisted.seg->common.bres_obj->major_axis)
						{
							_persisted.step_outbits |= (1 << AXIS5);
							_persisted.bresenham_counter[AXIS5] -= _persisted.seg->common.bres_obj->major_axis;
						}

						_persisted.bresenham_counter[AXIS6] += _persisted.seg->common.bres_obj->steps[AXIS6];
						if (_persisted.bresenham_counter[AXIS6] > _persisted.seg->common.bres_obj->major_axis)
						{
							_persisted.step_outbits |= (1 << AXIS6);
							_persisted.bresenham_counter[AXIS6] -= _persisted.seg->common.bres_obj->major_axis;
						}

					_persisted.seg->major_axis--;

					if (_persisted.seg->major_axis == 0)
					{
						//We are done with this timer item. Calling a get will move the indexes so the 
						//buffer is not full if it was full before.
						seg_dat::Segment::timer_buffer.get();
						//seg_dat::Segment::bres_offload_buffer.get();
						_persisted.seg = NULL;

					}
					return _persisted.step_outbits;
				}

				bool Segment::__load_segment()
				{
					bool done = true;

					if (seg_dat::Segment::timer_buffer.has_data())
					{
						//dont 'get' here otherwise the item we are pointing at will get over written
						//by the segment loader in block_to_segment
						_persisted.seg = seg_dat::Segment::timer_buffer.peek();
					}
					else
						int x = 0;


					if (_persisted.seg != NULL)
					{
						done = false;
						//does this new timer indicate a feed mode change
						if (_persisted.seg->common.control_bits.feed.get(e_feed_block_state::feed_mode_change))
						{
#ifdef MSVC
							myfile << "*****feed mode change*****" << "\r";
#endif
							//does the change require spindle synch
							if (_persisted.seg->common.control_bits.feed.get(
								e_feed_block_state::feed_mode_units_per_rotation))
							{
#ifdef MSVC
								myfile << "*****units per rotation*****" << "\r";
#endif
								//configure for new feed mode
							}
							else
							{
#ifdef MSVC
								myfile << "*****normal*****" << "\r";
#endif
							}
							//save off flags
							_persisted.control_bits.feed._flag =
								_persisted.seg->common.control_bits.feed._flag;
						}
#ifdef MSVC
						myfile << _persisted.seg->major_axis << ",";
						myfile << _persisted.seg->timer_delay_value << ",";
						myfile << _persisted.seg->common.tracking.line_number << ",";
						myfile << _persisted.seg->common.tracking.sequence << ",";
						//myfile << total_steps << ",";
						myfile << "\r";
						myfile.flush();
#endif
						//set timer delay value to the active_timer objects delay time						
						//hrd_out::Hardware::Motion::time_adjust(&_persisted.seg->timer_delay_value);
						STEP_TIMER_DELAY_DIRECT_REGISTER = max(_persisted.seg->timer_delay_value, LOWEST_STEP_DELAY_VALUE);
						//We need to reset this when we change the delay time because if it gets
						//ahead of the ARR time we have to wait for it to wrap around which is about 30 seconds
						//STEP_STP_TIMER_COUNT_DIRECT_REGISTER = 0;

						// If the new segment starts a new planner block, initialize stepper variables and counters.
						// NOTE: When the segment data index changes, this indicates a new planner block.
						if (_persisted.active_bresenham != _persisted.seg->common.bres_obj)
						{

							//New line segment, so this should be the start of a block.
							mtn_ctl_sta::Output::states.set(mtn_ctl_sta::Output::e_states::ngc_block_done);
							_persisted.active_line_number = _persisted.seg->common.tracking.line_number;
							_persisted.last_complete_sequence = _persisted.active_sequence;

							mtn_ctl_sta::Output::block_stats.common.sequence = _persisted.last_complete_sequence;
							mtn_ctl_sta::Output::block_stats.common.line_number = _persisted.active_line_number;
							//mtn_ctl_sta::Output::block_stats.start_time = *Hardware_Abstraction_Layer::Core::cpu_tick_ms;

							_persisted.active_sequence = _persisted.seg->common.tracking.sequence;
							_persisted.active_bresenham = _persisted.seg->common.bres_obj;

							//if we are loading a new block directions MIGHT change, so clear the set flag
							uint16_t non_vol = _persisted.seg->common.bres_obj->direction_bits._flag;
							hrd_out::Hardware::Motion::direction(&non_vol);

							//a new block has been detected. that means there is a potential for new ngc settings
							//for the spindle. better go check
							//Segment::__configure_spindle();

							_persisted.bresenham_counter[AXIS1]
								= _persisted.seg->common.bres_obj->major_axis;
							_persisted.bresenham_counter[AXIS2]
								= _persisted.seg->common.bres_obj->major_axis;
							_persisted.bresenham_counter[AXIS3]
								= _persisted.seg->common.bres_obj->major_axis;
							_persisted.bresenham_counter[AXIS4]
								= _persisted.seg->common.bres_obj->major_axis;
							_persisted.bresenham_counter[AXIS5]
								= _persisted.seg->common.bres_obj->major_axis;
							_persisted.bresenham_counter[AXIS6]
								= _persisted.seg->common.bres_obj->major_axis;
						}
					}
					return done;
				}

				void Segment::__end_interpolation()
				{
					hrd_out::Hardware::Motion::stop();
#ifdef MSVC
					myfile << 0 << ",";
					myfile << 0 << ",";
					myfile << 0 << ",";
					myfile << _persisted.active_line_number << ",";
					myfile << _persisted.active_sequence << ",";
					//myfile << total_steps << ",";
					myfile << "\r";
					myfile.flush();
					myfile.close();
#endif // MSVC
					__set_brakes();
					hrd_out::Hardware::Motion::disable();
					
					//if motion is on a gold state we arent 'done' we are just holding.
					if (!mtn_ctl_sta::Motion::states.get(mtn_ctl_sta::Motion::e_states::hold))
					{
						mtn_ctl_sta::Output::states.set(mtn_ctl_sta::Output::e_states::ngc_block_done);
						mtn_ctl_sta::Output::block_stats.common.sequence = _persisted.last_complete_sequence;
						mtn_ctl_sta::Output::block_stats.common.line_number = _persisted.active_line_number;

						mtn_ctl_sta::Output::states.clear(mtn_ctl_sta::Output::e_states::interpolation_running);
						mtn_ctl_sta::Output::states.set(mtn_ctl_sta::Output::e_states::interpolation_complete);
					}
				}

				void Segment::__set_brakes()
				{
					hrd_out::Hardware::Motion::brakes(0);

					//set this to null in case the timer fires while we are in here.
					Segment::pntr_driver = NULL;

					Segment::pntr_next_gate = Segment::__new_motion;
				}
			}
		}
	}
}
