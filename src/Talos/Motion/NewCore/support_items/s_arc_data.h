#ifndef __C_MOTION_CORE_ARC_ITEM_H
#define __C_MOTION_CORE_ARC_ITEM_H

#include <stdint.h>

struct s_arc_data
{
	float radius;
	float offset_horizontal;
	float offset_vertical;
	float offset_normal;

	float center_axis0;
	float center_axis1;
	//float center_horizontal;
	//float center_vertical;
	float center_normal;

	uint32_t segments;
	float angular_travel;
};
#endif