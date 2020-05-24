#ifndef _C_MOTION_SYSTEM_INTERNAL_H
#define _C_MOTION_SYSTEM_INTERNAL_H

#include <stdint.h>

#define SOME_LARGE_VALUE 1.0E+38
#define PREP_FLAG_DECEL_OVERRIDE bit(3)
#define PREP_FLAG_RECALCULATE bit(0)

//PACK(
struct s_motion_internal_settings
{
	float ARC_ANGULAR_TRAVEL_EPSILON;
	float N_ARC_CORRECTION;
	float MINIMUM_JUNCTION_SPEED;
	float MINIMUM_JUNCTION_SPEED_SQ;
	float MINIMUM_FEED_RATE;
	uint16_t ACCELERATION_TICKS_PER_SECOND;
	float REQ_MM_INCREMENT_SCALAR;
	float DT_SEGMENT()
	{
		return (1.0 / (this->ACCELERATION_TICKS_PER_SECOND * 60.0)); // min/segment
	};
	
};
//);
#endif