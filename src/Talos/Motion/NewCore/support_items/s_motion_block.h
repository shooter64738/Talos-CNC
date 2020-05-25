#ifndef C_SEGMENTED_MOTION_BLOCK_H
#define C_SEGMENTED_MOTION_BLOCK_H

#include <stdint.h>
#include "s_segment_timer_common.h"
#include "../../../physical_machine_parameters.h"
#include "../../../_bit_flag_control.h"


struct __s_motion_block
{
	// Fields used by the bresenham algorithm for tracing the line
			// NOTE: Used by stepper algorithm to execute the block correctly. Do not alter these values.
	uint32_t steps[MACHINE_AXIS_COUNT];    // Step count along each axis
	uint32_t step_event_count; // The maximum step axis count and number of steps required to complete this block.
	s_bit_flag_controller<uint16_t> direction_bits;    // The direction bit set for this block (refers to *_DIRECTION_BIT in config.h)
	s_bit_flag_controller<uint16_t> bl_comp_bits;//Direction changes are tracked and bit flags are set per axis for bl comp.
	s_bit_flag_controller<e_block_state> states;//Direction changes are tracked and bit flags are set per axis for bl comp.

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
	
	s_common_segment_items common;
	

};

#endif
