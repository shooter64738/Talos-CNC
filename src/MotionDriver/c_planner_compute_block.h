
#ifndef __C_PLANNER_CALCULATOR
#define __C_PLANNER_CALCULATOR
#include <stdint.h>
#include <stddef.h>
//#include "../all_includes.h"
#include "c_block.h"
#include "..\Common\NGC_RS274\NGC_Block.h"

namespace Motion_Core
{
	namespace Planner
	{
		class Calculator
		{

		public:


			//static int32_t position[N_AXIS];          // The planner position of the tool in absolute steps. Kept separate
													  // from g-code position for movements requiring multiple line motions,
													  // i.e. arcs, canned cycles, and backlash compensation.
			static float previous_unit_vec[N_AXIS];   // Unit vector of previous path line segment
			static float previous_nominal_speed;  // Nominal speed of previous path line segment
			static Motion_Core::Planner::Block_Item *block_buffer_planned;
			static uint8_t _plan_buffer_line(NGC_RS274::NGC_Binary_Block *target_block);
			//static uint8_t plan_buffer_line(float *target, c_planner::plan_line_data_t *pl_data, NGC_RS274::NGC_Binary_Block *target_block);
			static float plan_compute_profile_nominal_speed(Motion_Core::Planner::Block_Item *block);
			static void plan_compute_profile_parameters(Motion_Core::Planner::Block_Item *block, float nominal_speed, float prev_nominal_speed);
			static float plan_get_exec_block_exit_speed_sqr();
			static void planner_recalculate();
		};
	};
};
#endif
