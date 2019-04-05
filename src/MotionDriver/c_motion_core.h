//#include "../all_includes.h"
#include <math.h>
#include <stdint.h>
#include <stddef.h>
#define N_AXIS 6
namespace Motion_Core
{
	#define TICKS_PER_MICROSECOND (F_CPU/1000000)
	#define ARC_ANGULAR_TRAVEL_EPSILON 5E-7
	#define N_ARC_CORRECTION 12
	#define SOME_LARGE_VALUE 1.0E+38
	#define MINIMUM_JUNCTION_SPEED 0.0
	#define MINIMUM_FEED_RATE 1.0
	#define ACCELERATION_TICKS_PER_SECOND 100
	#define PREP_FLAG_DECEL_OVERRIDE bit(3)
	#define PREP_FLAG_RECALCULATE bit(0)
	#define STEP_CONTROL_EXECUTE_HOLD         bit(1)
	#define REQ_MM_INCREMENT_SCALAR 1.25
	#define DT_SEGMENT (1.0/(ACCELERATION_TICKS_PER_SECOND*60.0)) // min/segment
	#ifndef __C_RAMP_TYPE
	#define __C_RAMP_TYPE
	enum class Ramp_Type
	{
		Accel, Cruise, Decel, Decel_Override
	};
	#endif
	float convert_delta_vector_to_unit_vector(float *vector);
	float limit_value_by_axis_maximum(float *max_value, float *unit_vec);
	uint8_t get_direction_pin_mask(uint8_t axis_idx);
	void initialize();
	
	#ifndef __C_SETTINGS
	#define __C_SETTINGS
	class Settings
	{
		public:
		static float acceleration[N_AXIS];
		static float max_rate[N_AXIS];
		static int16_t steps_per_mm[N_AXIS];
		static float junction_deviation;
		static float arc_tolerance;
		static uint16_t pulse_length;
	};
	#endif

};