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

				void Talos::Motion::Core::States::states_execute()
				{
					states_execute(e_state_class::Process);
					states_execute(e_state_class::Motion);

				}

				void Talos::Motion::Core::States::states_execute(e_state_class st_class)
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
					default:
						break;
					}
				}

#pragma endregion
/*--------------------------------------------------------------------------*/
#pragma region Motion class state control

				s_bit_flag_controller<Motion::e_states> Motion::states;
				s_bit_flag_controller<Motion::e_internal_states> Motion::__internal_states;
				void Motion::execute()
				{
					if (Motion::states.get(Motion::e_states::hold))
						__cycle_hold();

					if (Motion::states.get(Motion::e_states::running))
						__run();
				}

				void Motion::__run()
				{
					//we are in run mode so we can fill the segement buffer. that does NOT mean
					//that motion will execute. If hold is active then we are going to get no
					//motion until it is released
					Core::Process::Segment::fill_step_segment_buffer();
				}

				void Motion::__cycle_hold()
				{
					//set internal state flags for holding a motion. The mmajor state flag will control
					//the motion output.
					Motion::__internal_states.set(Motion::e_internal_states::held);
					Motion::__internal_states.clear(Motion::e_internal_states::release);

					Core::Process::Segment::st_update_plan_block_parameters();
					//step_event_control.set(e_step_control_type::Step_motion_hold);
				}

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

#pragma endregion
/*--------------------------------------------------------------------------*/
#pragma region Process class state control

				s_bit_flag_controller<Process::e_states> Process::states;

				void Process::execute()
				{
					//looks like ngc data has come in. 
					if (Process::states.get_clr(Process::e_states::ngc_buffer_not_empty))
						__load_ngc();

					//if there is data in the segment buffer we are 'running'
					if (Talos::Motion::Core::Input::Block::motion_buffer.has_data())
						Motion::states.set(Motion::e_states::running);
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
			}
		}
	}
}