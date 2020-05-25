/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/

/*

All control flags for motion are in here. If its a flag that has something to do with the motion
it is controlled by flags in here. If I find a missed flag I move it to here. I want a central
location to control all motion output from.

*/


#include "c_state_control.h"
#include "c_ngc_to_block.h"
#include "c_block_to_segment.h"
#include "c_segment_to_hardware.h"
#include "../../Configuration/c_configuration.h"

//temporary until I get the hal tied to this.
#define HAL_SYS_TICK_TIME() 0
#define HAL_MOTION_ON() 0
#define HAL_MOTION_OFF() 0
#define HAL_SPINDLE_ON() 0
#define HAL_SPINDLE_OFF() 0
#define HAL_SPINDLE_SPEED() 0

namespace mtn_cfg = Talos::Configuration::Motion;
namespace mtn_out = Talos::Motion::Core::Output;

namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace States
			{
				/*--------------------------------------------------------------------------*/
#pragma region Global/All state control wrapper

				void Talos::Motion::Core::States::execute()
				{
					execute(e_state_class::Process);
					execute(e_state_class::Motion);
					execute(e_state_class::Output);

				}

				void Talos::Motion::Core::States::execute(e_state_class st_class)
				{
					switch (st_class)
					{
					case e_state_class::Motion:
					{
						Motion::execute();
						break;
					}
					case e_state_class::Process:
					{
						Process::execute();
						break;
					}
					case e_state_class::Output:
					{
						Output::execute();
						break;
					}
					default:
						break;
					}
				}

#pragma endregion
				/*--------------------------------------------------------------------------*/
#pragma region Motion class state control
				/*
				This is the central point to control all the motion output.
				Buffers are serviced in here, segments are processed, if anything
				produces motion it goes through here so that it can be controlled
				from here by setting state flags.
				*/

				s_bit_flag_controller<Motion::e_states> Motion::states;
				s_bit_flag_controller<Motion::e_internal_states> Motion::__internal_states;
				uint32_t Motion::spindle_request_time = 0;

				void Motion::execute()
				{
					if (!Motion::states.get(Motion::e_states::running))
						if (Motion::states.get(Motion::e_states::cycle_start))
							__cycle_start();

					if (Motion::states.get(Motion::e_states::hold))
						__cycle_hold();

					if (Motion::states.get(Motion::e_states::ready))
						if (Motion::states.get(Motion::e_states::running))
							__run();

					if (Motion::states.get(Motion::e_states::wait_for_spindle_at_speed))
					{
						//the output director has requested the spindle come on, and run at a
						//certain speed.
						__config_spindle();

					}
				}

				//called when cycle start bit is set.
				void Motion::__cycle_start()
				{
					//set internal state flags for holding a motion. The mmajor state flag will control
					//the motion output.
					Motion::__internal_states.clear(Motion::e_internal_states::held);
					Motion::__internal_states.clear(Motion::e_internal_states::release);
					Motion::states.set(Motion::e_states::ready);
					
					//should we just assume running? do we need to check other stuff?
					Motion::states.set(Motion::e_states::running);
					
					//we about to start motion so fill the segment buffer.
					Core::Process::Segment::fill_step_segment_buffer();

					mtn_out::Segment::gate_keeper();
				}

				//called when running flag is set. fills segment buffer that is consumed by the 
				//motion executor
				void Motion::__run()
				{
					//we are in run mode so we can fill the segement buffer. that does NOT mean
					//that motion will execute. If hold is active then we are going to get no
					//motion until it is released
					Core::Process::Segment::fill_step_segment_buffer();
				}

				//called when feed hold bit is set. running bit is not cleared.
				void Motion::__cycle_hold()
				{
					//set internal state flags for holding a motion. The mmajor state flag will control
					//the motion output.
					Motion::__internal_states.set(Motion::e_internal_states::held);
					Motion::__internal_states.clear(Motion::e_internal_states::release);

					Core::Process::Segment::st_update_plan_block_parameters();
					//step_event_control.set(e_step_control_type::Step_motion_hold);
				}

				//called when release bit it set. feed hold bit is cleared automatically
				void Motion::__cycle_release()
				{
					Motion::__internal_states.set(Motion::e_internal_states::release);
					Motion::__internal_states.clear(Motion::e_internal_states::held);

					//step_event_control.reset();
					Core::Process::Segment::st_update_plan_block_parameters();
					Core::Process::Segment::fill_step_segment_buffer();
					//restart interpollation
					//Motion_Core::Hardware::Interpolation::interpolation_begin();
					//Motion_Core::System::clear_control_state_mode(STATE_MOTION_CONTROL_RESUME);
				}

				//called when terminate bit is set. resets hold, relase, running bits
				void Motion::__cycle_reset()
				{
					Motion::__internal_states.clear(Motion::e_internal_states::release);
					Motion::__internal_states.clear(Motion::e_internal_states::held);
					Motion::states.clear(Motion::e_states::running);
				}

				//called when the spindle activate flags are set. sets up the spindle
				void Motion::__config_spindle()
				{
					//We are here to setup the spindle. the current timer record in output director
					//should have the spindle information.
					//Output::spindle_target_speed = Talos::Motion::Core::Output::Segment::active_block->spindle_rate;

					//if the spindle is not on, turn it on.
					if (!Motion::states.get(Motion::e_states::spindle_on))
					{
						//flag output to turn the spindle on.
						Output::states.set(Output::e_states::spindle_requested_on);

						//record the time when the wait for speed began
						//call into hardware layer here and get a sys tick timer.
						spindle_request_time = HAL_SYS_TICK_TIME();
					}
					else
					{
						int32_t target = Output::spindle_target_speed +
							(Output::spindle_target_speed * (mtn_cfg::Controller.Settings.hardware.spindle_encoder.rpm_tolerance / 100));

						//spindle is on, what is its speed?
						if ((Output::spindle_last_checked_speed >
							(Output::spindle_target_speed - mtn_cfg::Controller.Settings
								.hardware.spindle_encoder.rpm_tolerance))
							&& (Output::spindle_last_checked_speed <
							(Output::spindle_target_speed + mtn_cfg::Controller.Settings
								.hardware.spindle_encoder.rpm_tolerance)))
						{
							//spindle is at speed and within the request rpm range. 

							//Clear the wait flag
							Motion::states.clear(Motion::e_states::wait_for_spindle_at_speed);
							//set the at speed flag
							Motion::states.clear(Motion::e_states::spindle_at_speed);
						}
						else
						{
							//still waiting on the spindle. 
							//have we timed out yet?
							uint32_t elapsed = (HAL_SYS_TICK_TIME() - spindle_request_time);
							if (mtn_cfg::Controller.Settings.hardware.spindle_encoder
								.wait_spindle_at_speed - elapsed)
							{
								//Out of time. This is a fault

								//request spindle off.
								Output::states.set(Output::e_states::spindle_requested_off);
								Output::states.set(Output::e_states::hardware_fault);

								Motion::states.set(Motion::e_states::spindle_failure);
							}
						}
					}

				}
#pragma endregion
				/*--------------------------------------------------------------------------*/
#pragma region Process class state control
				/*
				This is the data process controller. If ngc data comes in this
				will determine when to process it and what to do with it when
				it is processed.
				*/
				s_bit_flag_controller<Process::e_states> Process::states;

				void Process::execute()
				{
					//looks like ngc data has come in. 
					if (Process::states.get_clr(Process::e_states::ngc_buffer_not_empty))
						__load_ngc();

					//if there is data in the segment buffer we are 'running'
					if (Talos::Motion::Core::Input::Block::motion_buffer.has_data())
						Motion::states.set(Motion::e_states::ready);
				}

				void Process::__load_ngc()
				{
					//load ngc buffer it into segment buffer
					bool more_data = Talos::Motion::Core::Input::Block::ngc_buffer_process();
					if (more_data)
						Process::states.set(Process::e_states::ngc_buffer_not_empty);
				}

#pragma endregion
				/*--------------------------------------------------------------------------*/
#pragma region Output class state control
				/*
				This is the data process controller. If ngc data comes in this
				will determine when to process it and what to do with it when
				it is processed.
				*/
				s_bit_flag_controller<Output::e_states> Output::states;
				int32_t Output::spindle_last_checked_speed = 0;
				int32_t Output::spindle_target_speed = 0;

				void Output::execute()
				{
					//if we have a hardware failure, shut it all down.
					if (Output::states.get(Output::e_states::hardware_fault))
					{
						__spindle_off();
						__motion_off();

						//set motion states
						Motion::states.set(Motion::e_states::terminate);
						Motion::states.set(Motion::e_states::hard_fault);
						Output::states.clear(Output::e_states::interpolation_running);
					}

					if (Output::states.get_clr(Output::e_states::spindle_requested_on))
						__spindle_on();

					if (Output::states.get_clr(Output::e_states::spindle_requested_off))
						__spindle_off();

					if (Output::states.get_clr(Output::e_states::spindle_get_speed))
						Output::spindle_last_checked_speed = __spindle_speed();
				}

				void Output::__motion_off()
				{
					Motion::states.set(Motion::e_states::motion_off);
					//call into the hardware layer here and turn the spindle on
					HAL_MOTION_OFF();
				}

				void Output::__motion_on()
				{
					Motion::states.set(Motion::e_states::motion_on);
					//call into the hardware layer here and turn the spindle on
					HAL_MOTION_ON();
				}

				void Output::__spindle_on()
				{
					Motion::states.set(Motion::e_states::spindle_on);
					//call into the hardware layer here and turn the spindle on
					HAL_SPINDLE_ON();
				}

				void Output::__spindle_off()
				{
					Motion::states.set(Motion::e_states::spindle_off);
					//call into the hardware layer here and turn the spindle off
					HAL_SPINDLE_OFF();
				}

				int32_t Output::__spindle_speed()
				{
					//call into hardware layer to get spindle speed
					return HAL_SPINDLE_SPEED();
				}

#pragma endregion
				/*--------------------------------------------------------------------------*/
			}
		}
	}
}