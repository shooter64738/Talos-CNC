/*
* c_grbl_avr_2560_limits.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

#include "../Talos.h"

#ifndef __DEFAULTS_H__
#define __DEFAULTS_H__

#define DEFAULTS_GENERIC

#ifdef DEFAULTS_GENERIC
#define DEFAULT_X_STEPS_PER_MM  160.0
#define DEFAULT_X_MAX_RATE     10000.0 // mm/min
#define DEFAULT_X_ACCELERATION (100.0 * 60 * 60) // 10 * 60 * 60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_X_MAX_TRAVEL   2000.0 // mm

#define DEFAULT_Y_STEPS_PER_MM 160.0
#define DEFAULT_Y_MAX_RATE     10000.0 // mm/min
#define DEFAULT_Y_ACCELERATION (100.0 * 60 * 60) // 10 * 60 * 60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_Y_MAX_TRAVEL   2000.0 // mm

#define DEFAULT_Z_STEPS_PER_MM 160.0
#define DEFAULT_Z_MAX_RATE     10000.0 // mm/min
#define DEFAULT_Z_ACCELERATION (100.0 * 60 * 60) // 10 * 60 * 60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_Z_MAX_TRAVEL   2000.0 // mm

#define DEFAULT_A_STEPS_PER_MM 160.0
#define DEFAULT_A_MAX_RATE     10000.0 // mm/min
#define DEFAULT_A_ACCELERATION (100.0 * 60 * 60) // 10 * 60 * 60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_A_MAX_TRAVEL   2000.0 // mm

#define DEFAULT_B_STEPS_PER_MM 160.0
#define DEFAULT_B_MAX_RATE     10000.0 // mm/min
#define DEFAULT_B_ACCELERATION (100.0 * 60 * 60) // 10 * 60 * 60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_B_MAX_TRAVEL   2000.0 // mm

#define DEFAULT_C_STEPS_PER_MM 160.0
#define DEFAULT_C_MAX_RATE     10000.0 // mm/min
#define DEFAULT_C_ACCELERATION (100.0 * 60 * 60) // 10 * 60 * 60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_C_MAX_TRAVEL   2000.0 // mm

#define DEFAULT_SPINDLE_RPM_MAX         1000.0 // rpm
#define DEFAULT_SPINDLE_RPM_MIN         0.0 // rpm

#define DEFAULT_STEP_PULSE_MICROSECONDS 10
#define DEFAULT_STEPPING_INVERT_MASK    0
#define DEFAULT_DIRECTION_INVERT_MASK   0
#define DEFAULT_STEPPER_IDLE_LOCK_TIME  25 // msec (0-254, 255 keeps steppers enabled)

#define DEFAULT_STATUS_REPORT_MASK      1 // MPos enabled

#define DEFAULT_JUNCTION_DEVIATION      0.01 // mm
#define DEFAULT_ARC_TOLERANCE           0.002 // mm
#define DEFAULT_REPORT_INCHES           0 // false

#define DEFAULT_INVERT_ST_ENABLE        0 // false
#define DEFAULT_INVERT_LIMIT_PINS       0 // false
#define DEFAULT_SOFT_LIMIT_ENABLE       0 // false
#define DEFAULT_HARD_LIMIT_ENABLE       0 // false
#define DEFAULT_INVERT_PROBE_PIN        0 // false
#define DEFAULT_LASER_MODE              0 // false

#define DEFAULT_HOMING_ENABLE           0 // false
#define DEFAULT_HOMING_DIR_MASK         0 // move positive dir
#define DEFAULT_HOMING_FEED_RATE        25.0 // mm/min
#define DEFAULT_HOMING_SEEK_RATE        1000.0 // mm/min
#define DEFAULT_HOMING_DEBOUNCE_DELAY   250 // msec (0-65k)
#define DEFAULT_HOMING_PULLOFF          1.0 // mm
#else
#error No defaults found
#endif

#endif
