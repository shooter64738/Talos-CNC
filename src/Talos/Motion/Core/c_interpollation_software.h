#include <stdint.h>
#include "c_planner_compute_block.h"
#include "../../Shared Data/_peripheral_panel_control_enums.h"

#ifndef __C_SOFTWARE_INTERPOLLATION
#define __C_SOFTWARE_INTERPOLLATION
namespace Motion_Core
{
	namespace Software
	{
		
		class Interpolation
		{
			public:

			static int32_t system_position[MACHINE_AXIS_COUNT];
			
			struct s_backlash_comp
			{
				uint8_t needs_comp = 0;
				int8_t last_directions[MACHINE_AXIS_COUNT];
			};
			
			static s_backlash_comp back_comp;
			
			static uint16_t load_block(s_motion_data_block * block);
			
			static uint8_t _mc_line(s_motion_data_block *target_block);
			static uint8_t _mc_arc(s_motion_data_block *target_block);

			//static void mc_line(float *target, c_planner::plan_line_data_t *pl_data, NGC_RS274::NGC_Binary_Block *target_block);
			//static void mc_arc(float *target, c_planner::plan_line_data_t *pl_data, float *position, float *offset, float radius
			//, uint8_t axis_0, uint8_t axis_1, uint8_t axis_linear, uint8_t is_clockwise_arc, NGC_RS274::NGC_Binary_Block *target_block);


			//public:
			//static NGC_RS274::NGC_Binary_Block previous_state;

			//Interpollation();
			//~Interpollation();
		};
	};
};
#endif
