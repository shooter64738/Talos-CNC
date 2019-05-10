
#include <math.h>
#include <stdint.h>
#include <stddef.h>
#define N_AXIS 3
#define  G_CODE_MULTIPLIER 100
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
	enum class e_ramp_type
	{Accel, Cruise, Decel, Decel_Override};
	#endif
	
	#ifndef __C_MOTION_TYPE
	#define __C_MOTION_TYPE
	enum class e_motion_type : uint8_t
	{rapid_linear = 0,feed_linear = 1,arc_cw = 2,arc_ccw = 3};
	#endif
	
	#ifndef __C_BLOCK_FLAG
	#define __C_BLOCK_FLAG
	enum class e_block_flag : uint8_t
	{normal = 0, compensation = 1};
	#endif
	
	#ifndef __C_FEED_MODES
	#define __C_FEED_MODES
	enum class e_feed_modes : uint16_t
	{	FEED_RATE_MINUTES_PER_UNIT_MODE = 93 * G_CODE_MULTIPLIER,
		FEED_RATE_UNITS_PER_MINUTE_MODE = 94 * G_CODE_MULTIPLIER,
		FEED_RATE_UNITS_PER_ROTATION = 95 * G_CODE_MULTIPLIER,
		FEED_RATE_CONSTANT_SURFACE_SPEED = 96 * G_CODE_MULTIPLIER,
		FEED_RATE_RPM_MODE = 97 * G_CODE_MULTIPLIER
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
		static float back_lash_comp_distance[N_AXIS];
	};
	#endif

};