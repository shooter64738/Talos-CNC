/*
* c_motion_core_arm_3x8e_spindle.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

//#include "..\..\..\..\physical_machine_parameters.h"
//#include "..\..\..\..\Motion_Core\c_interpollation_hardware.h"
#include "c_core_avr_2560.h"
#include "c_coordinator_avr2560_spindle.h"

//#include "..\..\..\..\records_def.h"
//#include "../../../..\Motion_Core\c_interpollation_hardware.h"
//#include "..\..\..\..\Motion\motion_hardware_def.h"

volatile uint32_t time_out_ticks = 0;

s_encoders * Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder;

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
#define qdec TC0

void Hardware_Abstraction_Layer::MotionCore::Spindle::initialize(s_encoders * encoder_data)
{
	Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder = encoder_data;
	
	Hardware_Abstraction_Layer::MotionCore::Spindle::qdec_speed_config();
	
	Hardware_Abstraction_Layer::MotionCore::Spindle::pwm_drive_configure();
}


void Hardware_Abstraction_Layer::MotionCore::Spindle::pwm_drive_configure()
{
	
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::set_speed(int32_t new_speed)
{
	
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::configure_timer_for_at_speed_delay()
{
	
	
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::stop_at_speed_timer()
{
}

void Hardware_Abstraction_Layer::MotionCore::Spindle::configure_timer_for_rpm_update(uint32_t interval)
{
}


int32_t Hardware_Abstraction_Layer::MotionCore::Spindle::get_rpm_qdec()
{
}

