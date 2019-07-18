/*
* c_motion_core_win_spindle.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

#include "c_motion_core_win_spindle.h"
#include "..\..\..\..\physical_machine_parameters.h"
#include "..\..\..\..\Motion_Core\c_interpollation_hardware.h"

#define TIME_OUT_TIMER_CLOCK TC_CMR_TCCLKS_TIMER_CLOCK4
#define UPDATE_INTERVAL_TIMER_CLOCK TC_CMR_TCCLKS_TIMER_CLOCK1
#define STEP_CLOCK_DIVIDER 1
#define SYS_TICKS (84)
volatile uint32_t time_out_ticks = 0;

BinaryRecords::s_encoders * Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder;

/***************************************************************************************************/
/*                          QDEC Speed mode by polling in loop()                                   */
/***************************************************************************************************/

/*
In speed mode, the results are accumulated within the time base (100 ms in this example).
Every 100 ms, we get an edge that is connected internally thru TIOA8 to TC6. Depending on the user
application and the requirement, the user must modify the time base with respect to their speed range.
For example, if the application requires low speed measurement, the time base can be increased to get
the fine results. For higher speed measurement, the user must choose the lesser time base so that the
number of pulses counted does not overflow beyond the channel 0 counter (32-bit). If this is not taken
care, the results may be inaccurate.
*/
//const uint32_t ENCODER_CPR = 150;                            // Cycles per revolution; this depends on your encoder
//const uint32_t ENCODER_EDGES_PER_ROTATION = ENCODER_CPR * 4; // PPR = CPR * 4
//const uint32_t ENCODER_SAMPLES_PER_SECOND = 10;              // this will need to be tuned depending on your use case...


void Hardware_Abstraction_Layer::MotionCore::Spindle::initialize(BinaryRecords::s_encoders encoder_data)
{
	Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder = &encoder_data;
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::configure_timer_for_at_speed_delay()
{
	if (Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->spindle_synch_wait_time_ms)
	{
	}
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::stop_at_speed_timer()
{
	time_out_ticks = 0;
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::configure_timer_for_rpm_update(uint32_t interval)
{
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::OCR1A_set(uint32_t delay)
{
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::get_rpm() {
	double dSpeedRPS, dSpeedRPM;
	int32_t iSpeedPPP;

	iSpeedPPP = 128;

	dSpeedRPS =
	((iSpeedPPP /
	(Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->ticks_per_revolution * 1.0))
	* Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->samples_per_second);
	dSpeedRPM =  dSpeedRPS * 60;
	Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->current_rpm = (int32_t) dSpeedRPM;
}
