namespace Motion_Core
{
#define PREP_FLAG_DECEL_OVERRIDE bit(3)
#define PREP_FLAG_RECALCULATE bit(0)
#define STEP_CONTROL_EXECUTE_HOLD         bit(1)
#define REQ_MM_INCREMENT_SCALAR 1.25
#define DT_SEGMENT (1.0/(ACCELERATION_TICKS_PER_SECOND*60.0)) // min/segment
	enum class Ramp_Type
	{
		Accel, Cruise, Decel, Decel_Override
	};
};