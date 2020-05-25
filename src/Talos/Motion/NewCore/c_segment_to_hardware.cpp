/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/

#include "..//..//Configuration/c_configuration.h"
#include "../../talos_hardware_def.h"
#include "c_segment_to_hardware.h"
#include "c_block_to_segment.h"
#include "c_ngc_to_block.h"
#include "support_items/e_block_state.h"
#include "support_items/s_segment_timer_common.h"
#include "c_state_control.h"
#include <math.h>

namespace mtn_cfg = Talos::Configuration::Motion;
namespace int_cfg = Talos::Configuration::Interpreter;
namespace mtn_ctl = Talos::Motion::Core::States;
namespace seg_dat = Talos::Motion::Core::Process;


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
				
				s_common_segment_items Segment::previous_flags{ 0 };
				__s_motion_block* active_block = NULL;

				void Segment::init_new_motion()
				{
					//have we already been here and set flags?
					//waiting on spindle to get to speed
					if (!mtn_ctl::Motion::states.get(mtn_ctl::Motion::e_states::wait_for_spindle_at_speed))
						return;
					if (Segment::active_block == NULL)
						Segment::active_block = seg_dat::Segment::buffer.get();

					//does this block depend on the spindle running and being at a certain speed?
					if (Segment::active_block->common.flag.get(e_block_state::feed_on_spindle))
					{
						//does the controller say spindle is at speed?
						if (!mtn_ctl::Motion::states.get(mtn_ctl::Motion::e_states::spindle_at_speed))
						{
							mtn_ctl::Motion::states.set(mtn_ctl::Motion::e_states::wait_for_spindle_at_speed);
							//cant do anythign until spindle is up to speed
							return;
						}
					}

					//store off the execution flags for this block.
					previous_flags = Segment::active_block->common;

					//if we get passed all the checking above, we are ready to actually do something
					s_timer_item * timer_item = seg_dat::Segment::timer_buffer.get();
				}
			}
		}
	}
}