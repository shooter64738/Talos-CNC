#ifndef _C_ENCODER_STRUCT
#define _C_ENCODER_STRUCT
#include <stdint.h>
struct s_encoder_meta
{
	int32_t reg_tc0_cv1 = 0;
	int32_t reg_tc0_ra0 = 0;
	float speed_rps = 0;
	float speed_rpm = 0;
};

struct s_encoders
{
	s_encoder_meta meta_data;
	uint32_t feedrate_delay;
	uint16_t samples_per_second; //How frequently are we going to check the encoder signal
	uint16_t ticks_per_revolution;//How many ticks are in a rev
	int32_t current_rpm;//what is the current spindle rpm
	int32_t target_rpm;//what is the target spindle rpm
	uint8_t variable_percent;//how far from the target can it be?
	uint8_t wait_spindle_at_speed;//do we wait for the spindle to get to speed before we start interpolation? 1 = yes, 0 = no
	uint8_t lock_to_axis;//if spindle lock_to_axis is set ('X','Y','Z','A','B','C','U','V','W') moving that axis will cause the spindle to rotate
	int32_t spindle_synch_wait_time_ms;//when we are waiting for spindle at speed, how many milliseconds do we wait for synch to occur.

	//function to determine if we are near enough to the rpm to start interpolation
	uint8_t near(uint32_t current_rpm, uint32_t target_rpm, uint8_t variable_percent)
	{
		if (current_rpm == 0 || target_rpm == 0)
		{
			return 0;
		}
		//5% = 0.05
		//1000rpm * .05 = 50
		uint8_t target_percent = (float)target_rpm * (float)variable_percent / 100.0;
		//return 1 if the current RPM is within the % range of the target. otherwise 0
		return (current_rpm >= (target_rpm - target_percent)) && (current_rpm <= (target_rpm + target_percent));
	};
};
#endif // _C_ENCODER_STRUCT