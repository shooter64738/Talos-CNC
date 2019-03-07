

/*
* all_includes.h
*
* Created: 3/5/2019 3:37:29 PM
*  Author: jeff_d
*/


#ifndef ALL_INCLUDES_H_
#define ALL_INCLUDES_H_

#define SOME_LARGE_VALUE 1.0E+38

// Axis array index values. Must start with 0 and be continuous.
#define N_AXIS 6 // Number of axes
#define X_AXIS 0 // Axis indexing value.
#define Y_AXIS 1
#define Z_AXIS 2
#define A_AXIS 3
#define B_AXIS 4
#define C_AXIS 5

// CoreXY motor assignments. DO NOT ALTER.
// NOTE: If the A and B motor axis bindings are changed, this effects the CoreXY equations.
#ifdef COREXY
#define A_MOTOR X_AXIS // Must be X_AXIS
#define B_MOTOR Y_AXIS // Must be Y_AXIS
#endif

// Conversions
#define MM_PER_INCH (25.40)
#define INCH_PER_MM (0.0393701)
#define TICKS_PER_MICROSECOND (F_CPU/1000000)

#define DELAY_MODE_DWELL       0
#define DELAY_MODE_SYS_SUSPEND 1

// Useful macros
#define clear_vector(a) memset(a, 0, sizeof(a))
#define clear_vector_float(a) memset(a, 0.0, sizeof(float)*N_AXIS)
// #define clear_vector_long(a) memset(a, 0.0, sizeof(long)*N_AXIS)
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))
#define isequal_position_vector(a,b) !(memcmp((a), (b), sizeof(float)*N_AXIS))



#include <avr/io.h>
#include "../helpers.h"
// Serial port interrupt vectors
#define SERIAL_RX USART0_RX_vect
#define SERIAL_UDRE USART0_UDRE_vect

// Define step pulse output pins. NOTE: All step bit pins must be on the same port.
#define STEP_DDR      DDRA
#define STEP_PORT     PORTA
#define STEP_PIN      PINA
//  #define X_STEP_BIT    2 // MEGA2560 Digital Pin 24
//  #define Y_STEP_BIT    3 // MEGA2560 Digital Pin 25
//  #define Z_STEP_BIT    4 // MEGA2560 Digital Pin 26
//  #define STEP_MASK ((1 << X_STEP_BIT) | (1 << Y_STEP_BIT) | (1 << Z_STEP_BIT)) // All step bits
#define X_STEP_BIT        0 // MEGA2560 Digital Pin 22
#define Y_STEP_BIT        1 // MEGA2560 Digital Pin 23
#define Z_STEP_BIT        2 // MEGA2560 Digital Pin 24
#define A_STEP_BIT        3 // MEGA2560 Digital Pin 25
#define B_STEP_BIT        4 // MEGA2560 Digital Pin 26
#define C_STEP_BIT        5 // MEGA2560 Digital Pin 27
#define STEP_MASK ((1 << X_STEP_BIT) | (1 << Y_STEP_BIT) | (1 << Z_STEP_BIT) | (1 << A_STEP_BIT) | (1 << B_STEP_BIT) | (1 << C_STEP_BIT)) // All step bits

// Define step direction output pins. NOTE: All direction pins must be on the same port.
#define DIRECTION_DDR     DDRC
#define DIRECTION_PORT    PORTC
#define DIRECTION_PIN     PINC
//  #define X_DIRECTION_BIT   7 // MEGA2560 Digital Pin 30
//  #define Y_DIRECTION_BIT   6 // MEGA2560 Digital Pin 31
//  #define Z_DIRECTION_BIT   5 // MEGA2560 Digital Pin 32
//  #define DIRECTION_MASK ((1 << X_DIRECTION_BIT) | (1 << Y_DIRECTION_BIT) | (1 << Z_DIRECTION_BIT)) // All direction bits
#define X_DIRECTION_BIT   0 // MEGA2560 Digital Pin 37
#define Y_DIRECTION_BIT   1 // MEGA2560 Digital Pin 36
#define Z_DIRECTION_BIT   2 // MEGA2560 Digital Pin 35
#define A_DIRECTION_BIT   3 // MEGA2560 Digital Pin 34
#define B_DIRECTION_BIT   4 // MEGA2560 Digital Pin 33
#define C_DIRECTION_BIT   5 // MEGA2560 Digital Pin 32
#define DIRECTION_MASK ((1 << X_DIRECTION_BIT) | (1 << Y_DIRECTION_BIT) | (1 << Z_DIRECTION_BIT) | (1 << A_DIRECTION_BIT) | (1 << B_DIRECTION_BIT) | (1 << C_DIRECTION_BIT)) // All direction bits

// Define stepper driver enable/disable output pin.
#define STEPPERS_DISABLE_DDR   DDRB
#define STEPPERS_DISABLE_PORT  PORTB
#define STEPPERS_DISABLE_BIT   7 // MEGA2560 Digital Pin 13
#define STEPPERS_DISABLE_MASK (1 << STEPPERS_DISABLE_BIT)

// Define homing/hard limit switch input pins and limit interrupt vectors.
// NOTE: All limit bit pins must be on the same port
#define LIMIT_INT       PCIE0  // Pin change interrupt enable pin
#define LIMIT_INT_vect  PCINT0_vect
#define LIMIT_PCMSK     PCMSK0 // Pin change interrupt register
#define LIMIT_DDR       DDRB
#define LIMIT_PORT      PORTB
#define LIMIT_PIN       PINB
//    #define X_LIMIT_BIT     4 // MEGA2560 Digital Pin 10
//    #define Y_LIMIT_BIT     5 // MEGA2560 Digital Pin 11
//    #define Z_LIMIT_BIT     6 // MEGA2560 Digital Pin 12
//    #define LIMIT_MASK ((1 << X_LIMIT_BIT) | (1 << Y_LIMIT_BIT) | (1 << Z_LIMIT_BIT)) // All limit bits
#define X_LIMIT_BIT     0 // MEGA2560 Digital Pin 53
#define Y_LIMIT_BIT     1 // MEGA2560 Digital Pin 52
#define Z_LIMIT_BIT     2 // MEGA2560 Digital Pin 51
#define A_LIMIT_BIT     3 // MEGA2560 Digital Pin 50
#define B_LIMIT_BIT     4 // MEGA2560 Digital Pin 10
#define C_LIMIT_BIT     5 // MEGA2560 Digital Pin 11
#define LIMIT_MASK ((1 << X_LIMIT_BIT) | (1 << Y_LIMIT_BIT) | (1 << Z_LIMIT_BIT) | (1 << A_LIMIT_BIT) | (1 << B_LIMIT_BIT) | (1 << C_LIMIT_BIT)) // All limit bits

// Define spindle enable and spindle direction output pins.
#define SPINDLE_ENABLE_DDR      DDRH
#define SPINDLE_ENABLE_PORT     PORTH
#define SPINDLE_ENABLE_BIT      3 // MEGA2560 Digital Pin 6
#define SPINDLE_DIRECTION_DDR   DDRE
#define SPINDLE_DIRECTION_PORT  PORTE
#define SPINDLE_DIRECTION_BIT   3 // MEGA2560 Digital Pin 5

// Define flood and mist coolant enable output pins.
#define COOLANT_FLOOD_DDR   DDRH
#define COOLANT_FLOOD_PORT  PORTH
#define COOLANT_FLOOD_BIT   5 // MEGA2560 Digital Pin 8
#define COOLANT_MIST_DDR    DDRH
#define COOLANT_MIST_PORT   PORTH
#define COOLANT_MIST_BIT    6 // MEGA2560 Digital Pin 9

// Define user-control CONTROLs (cycle start, reset, feed hold) input pins.
// NOTE: All CONTROLs pins must be on the same port and not on a port with other input pins (limits).
#define CONTROL_DDR       DDRK
#define CONTROL_PIN       PINK
#define CONTROL_PORT      PORTK
#define CONTROL_RESET_BIT         0  // MEGA2560 Analog Pin 8
#define CONTROL_FEED_HOLD_BIT     1  // MEGA2560 Analog Pin 9
#define CONTROL_CYCLE_START_BIT   2  // MEGA2560 Analog Pin 10
#define CONTROL_SAFETY_DOOR_BIT   3  // MEGA2560 Analog Pin 11
#define CONTROL_INT       PCIE2  // Pin change interrupt enable pin
#define CONTROL_INT_vect  PCINT2_vect
#define CONTROL_PCMSK     PCMSK2 // Pin change interrupt register
#ifdef RTC_H_
#define CONTROL_MASK      ((1 << CONTROL_RESET_BIT) | (1 << CONTROL_FEED_HOLD_BIT) | (1 << CONTROL_CYCLE_START_BIT) | (1 << CONTROL_SAFETY_DOOR_BIT) | (1<<CONTROL_SPINDLE_DIRECTION_BIT) | (1<<CONTROL_SPINDLE_DRIVER_BIT))
#else
#define CONTROL_MASK      ((1 << CONTROL_RESET_BIT) | (1 << CONTROL_FEED_HOLD_BIT) | (1 << CONTROL_CYCLE_START_BIT) | (1 << CONTROL_SAFETY_DOOR_BIT) )
#endif // RTC_H_




// Define probe switch input pin.
#define PROBE_DDR       DDRK
#define PROBE_PIN       PINK
#define PROBE_PORT      PORTK
#define PROBE_BIT       7  // MEGA2560 Analog Pin 15
#define PROBE_MASK      (1 << PROBE_BIT)

// Advanced Configuration Below You should not need to touch these variables
// Set Timer up to use TIMER4B which is attached to Digital Pin 7
#define SPINDLE_PWM_MAX_VALUE       1024.0 // Translates to about 1.9 kHz PWM frequency at 1/8 prescaler
#ifndef SPINDLE_PWM_MIN_VALUE
#define SPINDLE_PWM_MIN_VALUE   1   // Must be greater than zero.
#endif
#define SPINDLE_PWM_OFF_VALUE       0
#define SPINDLE_PWM_RANGE           (SPINDLE_PWM_MAX_VALUE - SPINDLE_PWM_MIN_VALUE)
#define SPINDLE_TCCRA_REGISTER		TCCR4A
#define SPINDLE_TCCRB_REGISTER		TCCR4B
#define SPINDLE_OCR_REGISTER	  	OCR4B
#define SPINDLE_COMB_BIT			COM4B1

// 1/8 Prescaler, 16-bit Fast PWM mode
#define SPINDLE_TCCRA_INIT_MASK ((1 << WGM40) | (1 << WGM41))
#define SPINDLE_TCCRB_INIT_MASK ((1 << WGM42) | (1 << WGM43) | (1 << CS41))
#define SPINDLE_OCRA_REGISTER   OCR4A // 16-bit Fast PWM mode requires top reset value stored here.
#define SPINDLE_OCRA_TOP_VALUE  0x0400 // PWM counter reset value. Should be the same as PWM_MAX_VALUE in hex.

// Define spindle output pins.
#define SPINDLE_PWM_DDR		DDRH
#define SPINDLE_PWM_PORT    PORTH
#define SPINDLE_PWM_BIT		4 // MEGA2560 Digital Pin 7

#define DEFAULT_FEED_OVERRIDE           100 // 100%. Don't change this value.
#define MAX_FEED_RATE_OVERRIDE          200 // Percent of programmed feed rate (100-255). Usually 120% or 200%
#define MIN_FEED_RATE_OVERRIDE           10 // Percent of programmed feed rate (1-100). Usually 50% or 1%
#define FEED_OVERRIDE_COARSE_INCREMENT   10 // (1-99). Usually 10%.
#define FEED_OVERRIDE_FINE_INCREMENT      1 // (1-99). Usually 1%.

#define DEFAULT_RAPID_OVERRIDE  100 // 100%. Don't change this value.
#define RAPID_OVERRIDE_MEDIUM    50 // Percent of rapid (1-99). Usually 50%.
#define RAPID_OVERRIDE_LOW       25 // Percent of rapid (1-99). Usually 25%.
// #define RAPID_OVERRIDE_EXTRA_LOW 5 // *NOT SUPPORTED* Percent of rapid (1-99). Usually 5%.

#define DEFAULT_SPINDLE_SPEED_OVERRIDE    100 // 100%. Don't change this value.
#define MAX_SPINDLE_SPEED_OVERRIDE        200 // Percent of programmed spindle speed (100-255). Usually 200%.
#define MIN_SPINDLE_SPEED_OVERRIDE         10 // Percent of programmed spindle speed (1-100). Usually 10%.
#define SPINDLE_OVERRIDE_COARSE_INCREMENT  10 // (1-99). Usually 10%.
#define SPINDLE_OVERRIDE_FINE_INCREMENT     1 // (1-99). Usually 1%.

/*
#ifdef CPU_MAP_CUSTOM_PROC
// For a custom pin map or different processor, copy and edit one of the available cpu
// map files and modify it to your needs. Make sure the defined name is also changed in
// the config.h file.
#endif
*/

// Sets which axis the tool length offset is applied. Assumes the spindle is always parallel with
// the selected axis with the tool oriented toward the negative direction. In other words, a positive
// tool length offset value is subtracted from the current location.
#define TOOL_LENGTH_OFFSET_AXIS Z_AXIS // Default z-axis. Valid values are X_AXIS, Y_AXIS, or Z_AXIS.

// Define bit flag masks for the boolean settings in settings.flag.
#define BITFLAG_REPORT_INCHES      bit(0)
#define BITFLAG_LASER_MODE         bit(1)
#define BITFLAG_INVERT_ST_ENABLE   bit(2)
#define BITFLAG_HARD_LIMIT_ENABLE  bit(3)
#define BITFLAG_HOMING_ENABLE      bit(4)
#define BITFLAG_SOFT_LIMIT_ENABLE  bit(5)
#define BITFLAG_INVERT_LIMIT_PINS  bit(6)
#define BITFLAG_INVERT_PROBE_PIN   bit(7)

// Version of the EEPROM data. Will be used to migrate existing data from older versions of Grbl
// when firmware is upgraded. Always stored in byte 0 of eeprom
#define SETTINGS_VERSION 10  // NOTE: Check settings_reset() when moving to next version.

// Define bit flag masks for the boolean settings in settings.flag.
#define BITFLAG_REPORT_INCHES      bit(0)
#define BITFLAG_LASER_MODE         bit(1)
#define BITFLAG_INVERT_ST_ENABLE   bit(2)
#define BITFLAG_HARD_LIMIT_ENABLE  bit(3)
#define BITFLAG_HOMING_ENABLE      bit(4)
#define BITFLAG_SOFT_LIMIT_ENABLE  bit(5)
#define BITFLAG_INVERT_LIMIT_PINS  bit(6)
#define BITFLAG_INVERT_PROBE_PIN   bit(7)

// Define status reporting boolean enable bit flags in settings.status_report_mask
#define BITFLAG_RT_STATUS_POSITION_TYPE     bit(0)
#define BITFLAG_RT_STATUS_BUFFER_STATE      bit(1)

// Define settings restore bitflags.
#define SETTINGS_RESTORE_DEFAULTS      bit(0)
#define SETTINGS_RESTORE_PARAMETERS    bit(1)
#define SETTINGS_RESTORE_STARTUP_LINES bit(2)
#define SETTINGS_RESTORE_BUILD_INFO    bit(3)
#ifndef SETTINGS_RESTORE_ALL
#define SETTINGS_RESTORE_ALL 0xFF // All bitflags
#endif

// Define EEPROM memory address location values for Grbl settings and parameters
#define EEPROM_ADDR_GLOBAL         ((uint8_t *)1)
#define EEPROM_ADDR_PARAMETERS     ((uint8_t *)512)
#define EEPROM_ADDR_STARTUP_BLOCK  ((uint8_t *)768)
#define EEPROM_ADDR_BUILD_INFO     ((uint8_t *)942)

// Define EEPROM address indexing for coordinate parameters
#define N_COORDINATE_SYSTEM 6  // Number of supported work coordinate systems (from index 1)
#define SETTING_INDEX_NCOORD N_COORDINATE_SYSTEM+1 // Total number of system stored (from index 0)
// NOTE: Work coordinate indices are (0=G54, 1=G55, ... , 6=G59)
#define SETTING_INDEX_G28    N_COORDINATE_SYSTEM    // Home position 1
#define SETTING_INDEX_G30    N_COORDINATE_SYSTEM+1  // Home position 2
// #define SETTING_INDEX_G92    N_COORDINATE_SYSTEM+2  // Coordinate offset (G92.2,G92.3 not supported)

// Define Grbl axis settings numbering scheme. Starts at START_VAL, every INCREMENT, over N_SETTINGS.
#define AXIS_N_SETTINGS          4
#define AXIS_SETTINGS_START_VAL  100 // NOTE: Reserving settings values >= 100 for axis settings. Up to 255.
#define AXIS_SETTINGS_INCREMENT  10  // Must be greater than the number of axis settings


// After the safety door switch has been toggled and restored, this setting sets the power-up delay
// between restoring the spindle and coolant and resuming the cycle.
#define SAFETY_DOOR_SPINDLE_DELAY 4.0 // Float (seconds)
#define SAFETY_DOOR_COOLANT_DELAY 1.0 // Float (seconds)

// Minimum planner junction speed. Sets the default minimum junction speed the planner plans to at
// every buffer block junction, except for starting from rest and end of the buffer, which are always
// zero. This value controls how fast the machine moves through junctions with no regard for acceleration
// limits or angle between neighboring block line move directions. This is useful for machines that can't
// tolerate the tool dwelling for a split second, i.e. 3d printers or laser cutters. If used, this value
// should not be much greater than zero or to the minimum value necessary for the machine to work.
#define MINIMUM_JUNCTION_SPEED 0.0 // (mm/min)

// Sets the minimum feed rate the planner will allow. Any value below it will be set to this minimum
// value. This also ensures that a planned motion always completes and accounts for any floating-point
// round-off errors. Although not recommended, a lower value than 1.0 mm/min will likely work in smaller
// machines, perhaps to 0.1mm/min, but your success may vary based on multiple factors.
#define MINIMUM_FEED_RATE 1.0 // (mm/min)

// Number of arc generation iterations by small angle approximation before exact arc trajectory
// correction with expensive sin() and cos() calcualtions. This parameter maybe decreased if there
// are issues with the accuracy of the arc generations, or increased if arc execution is getting
// bogged down by too many trig calculations.
#define N_ARC_CORRECTION 12 // Integer (1-255)

// The arc G2/3 g-code standard is problematic by definition. Radius-based arcs have horrible numerical
// errors when arc at semi-circles(pi) or full-circles(2*pi). Offset-based arcs are much more accurate
// but still have a problem when arcs are full-circles (2*pi). This define accounts for the floating
// point issues when offset-based arcs are commanded as full circles, but get interpreted as extremely
// small arcs with around machine epsilon (1.2e-7rad) due to numerical round-off and precision issues.
// This define value sets the machine epsilon cutoff to determine if the arc is a full-circle or not.
// NOTE: Be very careful when adjusting this value. It should always be greater than 1.2e-7 but not too
// much greater than this. The default setting should capture most, if not all, full arc error situations.
#define ARC_ANGULAR_TRAVEL_EPSILON 5E-7 // Float (radians)

// Time delay increments performed during a dwell. The default value is set at 50ms, which provides
// a maximum time delay of roughly 55 minutes, more than enough for most any application. Increasing
// this delay will increase the maximum dwell time linearly, but also reduces the responsiveness of
// run-time command executions, like status reports, since these are performed between each dwell
// time step. Also, keep in mind that the Arduino delay timer is not very accurate for long delays.
#define DWELL_TIME_STEP 50 // Integer (1-255) (milliseconds)

#define HOMING_CYCLE_0 (1<<Z_AXIS)                // REQUIRED: First move Z to clear workspace.
#define HOMING_CYCLE_1 ((1<<X_AXIS)|(1<<Y_AXIS))  // OPTIONAL: Then move X,Y at the same time.

// Number of homing cycles performed after when the machine initially jogs to limit switches.
// This help in preventing overshoot and should improve repeatability. This value should be one or
// greater.
#define N_HOMING_LOCATE_CYCLE 1 // Integer (1-128)

// Define homing/hard limit switch input pins and limit interrupt vectors.
// NOTE: All limit bit pins must be on the same port
#define LIMIT_INT       PCIE0  // Pin change interrupt enable pin
#define LIMIT_INT_vect  PCINT0_vect
#define LIMIT_PCMSK     PCMSK0 // Pin change interrupt register
#define LIMIT_DDR       DDRB
#define LIMIT_PORT      PORTB
#define LIMIT_PIN       PINB

// Define realtime command special characters. These characters are 'picked-off' directly from the
// serial read data stream and are not passed to the grbl line execution parser. Select characters
// that do not and must not exist in the streamed g-code program. ASCII control characters may be
// used, if they are available per user setup. Also, extended ASCII codes (>127), which are never in
// g-code programs, maybe selected for interface programs.
// NOTE: If changed, manually update help message in report.c.

#define CMD_RESET 0x18 // ctrl-x.
#define CMD_STATUS_REPORT '?'
#define CMD_CYCLE_START '~'
#define CMD_FEED_HOLD '!'

// NOTE: All override realtime commands must be in the extended ASCII character set, starting
// at character value 128 (0x80) and up to 255 (0xFF). If the normal set of realtime commands,
// such as status reports, feed hold, reset, and cycle start, are moved to the extended set
// space, serial.c's RX ISR will need to be modified to accomodate the change.
// #define CMD_RESET 0x80
// #define CMD_STATUS_REPORT 0x81
// #define CMD_CYCLE_START 0x82
// #define CMD_FEED_HOLD 0x83
#define CMD_SAFETY_DOOR 0x84
#define CMD_JOG_CANCEL  0x85
#define CMD_DEBUG_REPORT 0x86 // Only when DEBUG enabled, sends debug report in '{}' braces.
#define CMD_FEED_OVR_RESET 0x90         // Restores feed override value to 100%.
#define CMD_FEED_OVR_COARSE_PLUS 0x91
#define CMD_FEED_OVR_COARSE_MINUS 0x92
#define CMD_FEED_OVR_FINE_PLUS  0x93
#define CMD_FEED_OVR_FINE_MINUS  0x94
#define CMD_RAPID_OVR_RESET 0x95        // Restores rapid override value to 100%.
#define CMD_RAPID_OVR_MEDIUM 0x96
#define CMD_RAPID_OVR_LOW 0x97
// #define CMD_RAPID_OVR_EXTRA_LOW 0x98 // *NOT SUPPORTED*
#define CMD_SPINDLE_OVR_RESET 0x99      // Restores spindle override value to 100%.
#define CMD_SPINDLE_OVR_COARSE_PLUS 0x9A
#define CMD_SPINDLE_OVR_COARSE_MINUS 0x9B
#define CMD_SPINDLE_OVR_FINE_PLUS 0x9C
#define CMD_SPINDLE_OVR_FINE_MINUS 0x9D
#define CMD_SPINDLE_OVR_STOP 0x9E
#define CMD_COOLANT_FLOOD_OVR_TOGGLE 0xA0
#define CMD_COOLANT_MIST_OVR_TOGGLE 0xA1

#define CMD_MPG_JOG_X 'X'
#define CMD_MPG_JOG_Y 'Y'


//for pstr
#include <avr/pgmspace.h>

#define F_CPU 16000000
#define BAUD_RATE 115200


#ifndef DEFAULTS_H_
#define DEFAULTS_H_
#define DEFAULTS_GENERIC

#ifdef DEFAULTS_GENERIC
#define DEFAULT_X_STEPS_PER_MM  250.0
#define DEFAULT_X_MAX_RATE     500.0 // mm/min
#define DEFAULT_X_ACCELERATION (10.0 * 60 * 60) // 10 * 60 * 60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_X_MAX_TRAVEL   200.0 // mm

#define DEFAULT_Y_STEPS_PER_MM 250.0
#define DEFAULT_Y_MAX_RATE     500.0 // mm/min
#define DEFAULT_Y_ACCELERATION (10.0 * 60 * 60) // 10 * 60 * 60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_Y_MAX_TRAVEL   200.0 // mm

#define DEFAULT_Z_STEPS_PER_MM 250.0
#define DEFAULT_Z_MAX_RATE     500.0 // mm/min
#define DEFAULT_Z_ACCELERATION (10.0 * 60 * 60) // 10 * 60 * 60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_Z_MAX_TRAVEL   200.0 // mm

#define DEFAULT_A_STEPS_PER_MM 250.0
#define DEFAULT_A_MAX_RATE     500.0 // mm/min
#define DEFAULT_A_ACCELERATION (10.0 * 60 * 60) // 10 * 60 * 60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_A_MAX_TRAVEL   200.0 // mm

#define DEFAULT_B_STEPS_PER_MM 250.0
#define DEFAULT_B_MAX_RATE     500.0 // mm/min
#define DEFAULT_B_ACCELERATION (10.0 * 60 * 60) // 10 * 60 * 60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_B_MAX_TRAVEL   200.0 // mm

#define DEFAULT_C_STEPS_PER_MM 250.0
#define DEFAULT_C_MAX_RATE     500.0 // mm/min
#define DEFAULT_C_ACCELERATION (10.0 * 60 * 60) // 10 * 60 * 60 mm/min^2 = 10 mm/sec^2
#define DEFAULT_C_MAX_TRAVEL   200.0 // mm

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
#define DEFAULT_HOMING_SEEK_RATE        500.0 // mm/min
#define DEFAULT_HOMING_DEBOUNCE_DELAY   250 // msec (0-65k)
#define DEFAULT_HOMING_PULLOFF          1.0 // mm
#else
#error No defaults found
#endif

#endif /* DEFAULTS_H_ */

// Number of floating decimal points printed by Grbl for certain value types. These settings are
// determined by realistic and commonly observed values in CNC machines. For example, position
// values cannot be less than 0.001mm or 0.0001in, because machines can not be physically more
// precise this. So, there is likely no need to change these, but you can if you need to here.
// NOTE: Must be an integer value from 0 to ~4. More than 4 may exhibit round-off errors.
#define N_DECIMAL_COORDVALUE_INCH 4 // Coordinate or position value in inches
#define N_DECIMAL_COORDVALUE_MM   3 // Coordinate or position value in mm
#define N_DECIMAL_RATEVALUE_INCH  1 // Rate or velocity value in in/min
#define N_DECIMAL_RATEVALUE_MM    0 // Rate or velocity value in mm/min
#define N_DECIMAL_SETTINGVALUE    3 // Decimals for floating point setting values
#define N_DECIMAL_RPMVALUE        0 // RPM value in rotations per min.

/// Grbl versioning system
#define GRBL_VERSION "1.1h Atmega 2560 6-axis + Sync/MPG"
#define GRBL_VERSION_BUILD "20180311"
// Line buffer size from the serial input stream to be executed.
#ifndef LINE_BUFFER_SIZE
#define LINE_BUFFER_SIZE 256
#endif

// Define Grbl status codes. Valid values (0-255)
#define STATUS_OK 0
#define STATUS_EXPECTED_COMMAND_LETTER 1
#define STATUS_BAD_NUMBER_FORMAT 2
#define STATUS_INVALID_STATEMENT 3
#define STATUS_NEGATIVE_VALUE 4
#define STATUS_SETTING_DISABLED 5
#define STATUS_SETTING_STEP_PULSE_MIN 6
#define STATUS_SETTING_READ_FAIL 7
#define STATUS_IDLE_ERROR 8
#define STATUS_SYSTEM_GC_LOCK 9
#define STATUS_SOFT_LIMIT_ERROR 10
#define STATUS_OVERFLOW 11
#define STATUS_MAX_STEP_RATE_EXCEEDED 12
#define STATUS_CHECK_DOOR 13
#define STATUS_LINE_LENGTH_EXCEEDED 14
#define STATUS_TRAVEL_EXCEEDED 15
#define STATUS_INVALID_JOG_COMMAND 16

#define STATUS_GCODE_UNSUPPORTED_COMMAND 20
#define STATUS_GCODE_MODAL_GROUP_VIOLATION 21
#define STATUS_GCODE_UNDEFINED_FEED_RATE 22
#define STATUS_GCODE_COMMAND_VALUE_NOT_INTEGER 23
#define STATUS_GCODE_AXIS_COMMAND_CONFLICT 24
#define STATUS_GCODE_WORD_REPEATED 25
#define STATUS_GCODE_NO_AXIS_WORDS 26
#define STATUS_GCODE_INVALID_LINE_NUMBER 27
#define STATUS_GCODE_VALUE_WORD_MISSING 28
#define STATUS_GCODE_UNSUPPORTED_COORD_SYS 29
#define STATUS_GCODE_G53_INVALID_MOTION_MODE 30
#define STATUS_GCODE_AXIS_WORDS_EXIST 31
#define STATUS_GCODE_NO_AXIS_WORDS_IN_PLANE 32
#define STATUS_GCODE_INVALID_TARGET 33
#define STATUS_GCODE_ARC_RADIUS_ERROR 34
#define STATUS_GCODE_NO_OFFSETS_IN_PLANE 35
#define STATUS_GCODE_UNUSED_WORDS 36
#define STATUS_GCODE_G43_DYNAMIC_AXIS_ERROR 37
#define STATUS_GCODE_MAX_VALUE_EXCEEDED 38

// The temporal resolution of the acceleration management subsystem. A higher number gives smoother
// acceleration, particularly noticeable on machines that run at very high feedrates, but may negatively
// impact performance. The correct value for this parameter is machine dependent, so it's advised to
// set this only as high as needed. Approximate successful values can widely range from 50 to 200 or more.
// NOTE: Changing this value also changes the execution time of a segment in the step segment buffer.
// When increasing this value, this stores less overall time in the segment buffer and vice versa. Make
// certain the step segment buffer is increased/decreased to account for these changes.
#define ACCELERATION_TICKS_PER_SECOND 100

// Number of blocks Grbl executes upon startup. These blocks are stored in EEPROM, where the size
// and addresses are defined in settings.h. With the current settings, up to 2 startup blocks may
// be stored and executed in order. These startup blocks would typically be used to set the g-code
// parser state depending on user preferences.
#define N_STARTUP_LINE 2 // Integer (1-2)
#endif /* ALL_INCLUDES_H_ */