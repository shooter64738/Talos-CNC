#ifndef C_SEGMENTED_MOTION_BLOCK_H
#define C_SEGMENTED_MOTION_BLOCK_H

#include <stdint.h>
#include "s_motion_axis_detail.h"
#include "s_common_objects.h"
#include "e_state_flag.h"



struct __s_motion_block
{
	// Fields used by the bresenham algorithm for tracing the line
			// NOTE: Used by stepper algorithm to execute the block correctly. Do not alter these values.
	s_motion_axis axis_data;
	//s_bit_flag_controller<e_block_state> states;

	float entry_speed_sqr;     // The current planned entry speed at block junction in (mm/min)^2
	float max_entry_speed_sqr; // Maximum allowable entry speed based on the minimum of junction limit and
							   //   neighboring nominal speeds with overrides in (mm/min)^2
	float acceleration;        // Axis-limit adjusted line acceleration in (mm/min^2). Does not change.
	float millimeters;         // The remaining distance for this block to be executed in (mm).
							   // NOTE: This value may be altered by stepper algorithm during execution.

							   // Stored rate limiting data used by planner when changes occur.
	float max_junction_speed_sqr; // Junction entry speed limit based on direction vectors in (mm/min)^2
	float rapid_rate;             // Axis-limit adjusted maximum rate for this block direction in (mm/min)
	float programmed_rate;        // Programmed rate of this block (mm/min).
	uint8_t Station;
	
	s_common_motion_block_object common;
	

};

#endif
