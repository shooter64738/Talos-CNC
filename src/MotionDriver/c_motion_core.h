
#include <math.h>
#include <stdint.h>
#include <stddef.h>
#include "..\Common\Serial\records_def.h"



//#define MOTION_RECORD 1
//#define SETTING_RECORD 2
//#define SER_ACK_PROCEED 100 //proceed with more instructions
//#define SER_BAD_READ_RE_TRANSMIT 101 //data is bad, re-send
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



	float convert_delta_vector_to_unit_vector(float *vector);
	float limit_value_by_axis_maximum(float *max_value, float *unit_vec);
	uint8_t get_direction_pin_mask(uint8_t axis_idx);
	void initialize();


#ifndef __C_SETTINGS
#define __C_SETTINGS
	class Settings
	{
	public:

		static BinaryRecords::s_settings _Settings;
	};
#endif

};