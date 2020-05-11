#ifndef _C_ENCODER_STRUCT
#define _C_ENCODER_STRUCT
#include <stdint.h>
#include "Kernel/c_kernel_utils.h"
__PACK__(
struct s_encoder_meta
{
	int32_t reg_tc0_cv1; //4
	int32_t reg_tc0_ra0; //4
	float speed_rps; //4
	float speed_rpm; //4
	//16 bytes
}
);

__PACK__(
struct s_encoders
{
	s_encoder_meta meta_data; //16
	uint32_t feedrate_delay; //4
	uint16_t samples_per_second; //2 How frequently are we going to check the encoder signal
	uint16_t ticks_per_revolution;//2 How many ticks are in a rev
	int32_t current_rpm;//4 what is the current spindle rpm
	int32_t target_rpm;//4 what is the target spindle rpm
	uint8_t variable_percent;//1 how far from the target can it be?
	uint8_t wait_spindle_at_speed;//1 do we wait for the spindle to get to speed before we start interpolation? 1 = yes, 0 = no
	uint8_t lock_to_axis;//1 if spindle lock_to_axis is set ('X','Y','Z','A','B','C','U','V','W') moving that axis will cause the spindle to rotate
	int32_t spindle_synch_wait_time_ms;//4 when we are waiting for spindle at speed, how many milliseconds do we wait for synch to occur.
}
);
#endif // _C_ENCODER_STRUCT