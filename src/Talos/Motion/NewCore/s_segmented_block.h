#ifndef C_SEGMENTED_BLOCK_H
#define C_SEGMENTED_BLOCK_H

#include <stdint.h>
#include "../../physical_machine_parameters.h"
#include "../../_bit_flag_control.h"

enum class e_axis_direction
{
	forward = 0,
	backward = 1,
};

struct s_segmented_block
{
	// Fields used by the bresenham algorithm for tracing the line
			// NOTE: Used by stepper algorithm to execute the block correctly. Do not alter these values.
	uint32_t steps[MACHINE_AXIS_COUNT];    // Step count along each axis
	uint32_t step_event_count; // The maximum step axis count and number of steps required to complete this block.
	uint8_t direction_bits;    // The direction bit set for this block (refers to *_DIRECTION_BIT in config.h)

							   // Block condition data to ensure correct execution depending on states and overrides.
	uint8_t condition;      // Block bitflag variable defining block run conditions. Copied from pl_line_data.
	int32_t line_number;  // Block line number for real-time reporting. Copied from pl_line_data.

						  // Fields used by the motion planner to manage acceleration. Some of these values may be updated
						  // by the stepper module during execution of special motion cases for replanning purposes.
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

								  // Stored spindle speed data used by spindle overrides and resuming methods.
	uint8_t Station;
	uint32_t sequence;
	s_bit_flag_controller<uint32_t> flag;
	float programmed_spindle_speed;
	uint8_t spindle_state;

};

#endif
