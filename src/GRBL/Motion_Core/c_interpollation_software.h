#include <stdint.h>
#include "c_planner_compute_block.h"
#include "../../Common/NGC_RS274/NGC_Block.h"
namespace Motion_Core
{
	namespace Software
	{
#ifndef __C_SOFTWARE_INTERPOLLATION
#define __C_SOFTWARE_INTERPOLLATION
		class Interpollation
		{
		public:
			static void mc_line(float *target, c_planner::plan_line_data_t *pl_data, NGC_RS274::NGC_Binary_Block *target_block);
			static void mc_arc(float *target, c_planner::plan_line_data_t *pl_data, float *position, float *offset, float radius
				, uint8_t axis_0, uint8_t axis_1, uint8_t axis_linear, uint8_t is_clockwise_arc, NGC_RS274::NGC_Binary_Block *target_block);
		public:
			Interpollation();
			~Interpollation();
		};
	};
};
#endif
