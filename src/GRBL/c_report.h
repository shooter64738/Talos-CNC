/*
* c_report.h
*
* Created: 3/5/2019 3:49:11 PM
* Author: jeff_d
*/


#ifndef __C_REPORT_H__
#define __C_REPORT_H__

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

// Define Grbl alarm codes. Valid values (1-255). 0 is reserved.
#define ALARM_HARD_LIMIT_ERROR      EXEC_ALARM_HARD_LIMIT
#define ALARM_SOFT_LIMIT_ERROR      EXEC_ALARM_SOFT_LIMIT
#define ALARM_ABORT_CYCLE           EXEC_ALARM_ABORT_CYCLE
#define ALARM_PROBE_FAIL_INITIAL    EXEC_ALARM_PROBE_FAIL_INITIAL
#define ALARM_PROBE_FAIL_CONTACT    EXEC_ALARM_PROBE_FAIL_CONTACT
#define ALARM_HOMING_FAIL_RESET     EXEC_ALARM_HOMING_FAIL_RESET
#define ALARM_HOMING_FAIL_DOOR      EXEC_ALARM_HOMING_FAIL_DOOR
#define ALARM_HOMING_FAIL_PULLOFF   EXEC_ALARM_HOMING_FAIL_PULLOFF
#define ALARM_HOMING_FAIL_APPROACH  EXEC_ALARM_HOMING_FAIL_APPROACH

// Define Grbl feedback message codes. Valid values (0-255).
#define MESSAGE_CRITICAL_EVENT 1
#define MESSAGE_ALARM_LOCK 2
#define MESSAGE_ALARM_UNLOCK 3
#define MESSAGE_ENABLED 4
#define MESSAGE_DISABLED 5
#define MESSAGE_SAFETY_DOOR_AJAR 6
#define MESSAGE_CHECK_LIMITS 7
#define MESSAGE_PROGRAM_END 8
#define MESSAGE_RESTORE_DEFAULTS 9
#define MESSAGE_SPINDLE_RESTORE 10
#define MESSAGE_SLEEP_MODE 11
#include <stdint.h>
class c_report
{
	//variables
	public:

	static void report_util_setting_prefix(uint8_t n);
	static void report_util_line_feed();
	static void report_util_feedback_line_feed();
	static void report_util_gcode_modes_G();
	static void report_util_gcode_modes_M();
	static void report_util_axis_values(float *axis_value);
	static void report_util_uint8_setting(uint8_t n, int val);
	static void report_util_float_setting(uint8_t n, float val, uint8_t n_decimal);
	// Prints system status messages.
	static void report_status_message(uint8_t status_code);

	// Prints system alarm messages.
	static void report_alarm_message(uint8_t alarm_code);

	// Prints miscellaneous feedback messages.
	static void report_feedback_message(uint8_t message_code);

	// Prints welcome message
	static void report_init_message();

	// Prints Grbl help and current global settings
	static void report_grbl_help();

	// Prints Grbl global settings
	static void report_grbl_settings();

	// Prints an echo of the pre-parsed line received right before execution.
	static void report_echo_line_received(char *line);

	// Prints realtime status report
	static void report_realtime_status();

	// Prints recorded probe position
	static void report_probe_parameters();

	// Prints Grbl NGC parameters (coordinate offsets, probe)
	static void report_ngc_parameters();

	// Prints current g-code parser mode state
	static void report_gcode_modes();

	// Prints startup line when requested and executed.
	static void report_startup_line(uint8_t n, char *line);
	static void report_execute_startup_message(char *line, uint8_t status_code);

	// Prints build info and user info
	static void report_build_info(char *line);

	#ifdef DEBUG
	static void report_realtime_debug();
	#endif

	protected:
	private:

	//functions
	public:
	
	protected:
	private:

}; //c_report

#endif //__C_REPORT_H__
