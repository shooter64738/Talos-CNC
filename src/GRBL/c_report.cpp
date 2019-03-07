/* 
* c_report.cpp
*
* Created: 3/5/2019 3:49:11 PM
* Author: jeff_d
*/


#include "c_report.h"
#include "all_includes.h"
#include "..\helpers.h"
#include "c_serial.h"
#include "c_system.h"
#include "c_gcode.h"
#include "c_print.h"
#include "c_settings.h"
#include <string.h>
#include "utils.h"

/*
 report.c - reporting and messaging methods
 Part of Grbl

 Copyright (c) 2012-2016 Sungeun K. Jeon for Gnea Research LLC

 Grbl is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Grbl is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 This file functions as the primary feedback interface for Grbl. Any outgoing data, such
 as the protocol status messages, feedback messages, and status reports, are stored here.
 For the most part, these functions primarily are called from protocol.c methods. If a
 different style feedback is desired (i.e. JSON), then a user can change these following
 methods to accomodate their needs.
 */
#include <stdint.h>
#include "c_report.h"
//#include "grbl.h"
//#include "print.h"
//#include "planner.h"
//#include "gcode.h"
//#include "coolant_control.h"
//#include "spindle_control.h"

// Internal report utilities to reduce flash with repetitive tasks turned into functions.
void c_report::report_util_setting_prefix(uint8_t n)
{
    c_serial::serial_write('$');
    c_print::print_uint8_base10(n);
    c_serial::serial_write('=');
}

void c_report::report_util_line_feed()
{
    c_print::print_string_P(PSTR("\r\n"));
}

void c_report::report_util_feedback_line_feed()
{
    c_serial::serial_write(']');
    report_util_line_feed();
}

void c_report::report_util_gcode_modes_G()
{
    c_print::print_string_P(PSTR(" G"));
}

void c_report::report_util_gcode_modes_M()
{
    c_print::print_string_P(PSTR(" M"));
}

// static void report_util_comment_line_feed() { serial_write(')'); report_util_line_feed(); }
void c_report::report_util_axis_values(float *axis_value)
{
    uint8_t idx;
    for (idx = 0; idx < N_AXIS; idx++)
    {
        c_print::print_float_coord_value(axis_value[idx]);
        if (idx < (N_AXIS - 1))
        {
            c_serial::serial_write(',');
        }
    }
}

/*
static void report_util_setting_string(uint8_t n)
{
    serial_write(' ');
    serial_write('(');
    switch (n)
    {
        case 0:
            printPgmString(PSTR("stp pulse"));
            break;
        case 1:
            printPgmString(PSTR("idl delay"));
            break;
        case 2:
            printPgmString(PSTR("stp inv"));
            break;
        case 3:
            printPgmString(PSTR("dir inv"));
            break;
        case 4:
            printPgmString(PSTR("stp en inv"));
            break;
        case 5:
            printPgmString(PSTR("lim inv"));
            break;
        case 6:
            printPgmString(PSTR("prb inv"));
            break;
        case 10:
            printPgmString(PSTR("rpt"));
            break;
        case 11:
            printPgmString(PSTR("jnc dev"));
            break;
        case 12:
            printPgmString(PSTR("arc tol"));
            break;
        case 13:
            printPgmString(PSTR("rpt inch"));
            break;
        case 20:
            printPgmString(PSTR("sft lim"));
            break;
        case 21:
            printPgmString(PSTR("hrd lim"));
            break;
        case 22:
            printPgmString(PSTR("hm cyc"));
            break;
        case 23:
            printPgmString(PSTR("hm dir inv"));
            break;
        case 24:
            printPgmString(PSTR("hm feed"));
            break;
        case 25:
            printPgmString(PSTR("hm seek"));
            break;
        case 26:
            printPgmString(PSTR("hm delay"));
            break;
        case 27:
            printPgmString(PSTR("hm pulloff"));
            break;
        case 30:
            printPgmString(PSTR("rpm max"));
            break;
        case 31:
            printPgmString(PSTR("rpm min"));
            break;
        case 32:
            printPgmString(PSTR("laser"));
            break;
        default:
            n -= AXIS_SETTINGS_START_VAL;
            uint8_t idx = 0;
            while (n >= AXIS_SETTINGS_INCREMENT)
            {
                n -= AXIS_SETTINGS_INCREMENT;
                idx++;
            }
            serial_write(n + 'x');
            switch (idx)
            {
                case 0:
                    printPgmString(PSTR(":stp/mm"));
                    break;
                case 1:
                    printPgmString(PSTR(":mm/min"));
                    break;
                case 2:
                    printPgmString(PSTR(":mm/s^2"));
                    break;
                case 3:
                    printPgmString(PSTR(":mm max"));
                    break;
            }
            break;
    }
    report_util_comment_line_feed();
}
*/

void c_report::report_util_uint8_setting(uint8_t n, int val)
{
    report_util_setting_prefix(n);
    c_print::print_uint8_base10(val);
    report_util_line_feed(); // report_util_setting_string(n);
}
void c_report::report_util_float_setting(uint8_t n, float val, uint8_t n_decimal)
{
    report_util_setting_prefix(n);
    c_print::print_float(val, n_decimal);
    report_util_line_feed(); // report_util_setting_string(n);
}

// Handles the primary confirmation protocol response for streaming interfaces and human-feedback.
// For every incoming line, this method responds with an 'ok' for a successful command or an
// 'error:'  to indicate some error event with the line or some critical system error during
// operation. Errors events can originate from the g-code parser, settings module, or asynchronously
// from a critical error, such as a triggered hard limit. Interface should always monitor for these
// responses.
void c_report::report_status_message(uint8_t status_code)
{
    switch (status_code)
    {
        case STATUS_OK: // STATUS_OK
            c_print::print_string_P(PSTR("ok\r\n"));
            break;
        default:
            c_print::print_string_P(PSTR("error:"));
            c_print::print_uint8_base10(status_code);
            report_util_line_feed();
    }
}

// Prints alarm messages.
void c_report::report_alarm_message(uint8_t alarm_code)
{
    c_print::print_string_P(PSTR("ALARM:"));
    c_print::print_uint8_base10(alarm_code);
    report_util_line_feed();
    utils::delay_ms(500); // Force delay to ensure message clears serial write buffer.
}

// Prints feedback messages. This serves as a centralized method to provide additional
// user feedback for things that are not of the status/alarm message protocol. These are
// messages such as setup warnings, switch toggling, and how to exit alarms.
// NOTE: For interfaces, messages are always placed within brackets. And if silent mode
// is installed, the message number codes are less than zero.
void c_report::report_feedback_message(uint8_t message_code)
{
    c_print::print_string_P(PSTR("[MSG:"));
    switch (message_code)
    {
        case MESSAGE_CRITICAL_EVENT:
            c_print::print_string_P(PSTR("Reset to continue"));
            break;
        case MESSAGE_ALARM_LOCK:
            c_print::print_string_P(PSTR("'$H'|'$X' to unlock"));
            break;
        case MESSAGE_ALARM_UNLOCK:
            c_print::print_string_P(PSTR("Caution: Unlocked"));
            break;
        case MESSAGE_ENABLED:
            c_print::print_string_P(PSTR("Enabled"));
            break;
        case MESSAGE_DISABLED:
            c_print::print_string_P(PSTR("Disabled"));
            break;
        case MESSAGE_SAFETY_DOOR_AJAR:
            c_print::print_string_P(PSTR("Check Door"));
            break;
        case MESSAGE_CHECK_LIMITS:
           c_print::print_string_P(PSTR("Check Limits"));
            break;
        case MESSAGE_PROGRAM_END:
            c_print::print_string_P(PSTR("Pgm End"));
            break;
        case MESSAGE_RESTORE_DEFAULTS:
            c_print::print_string_P(PSTR("Restoring defaults"));
            break;
        case MESSAGE_SPINDLE_RESTORE:
            c_print::print_string_P(PSTR("Restoring spindle"));
            break;
        case MESSAGE_SLEEP_MODE:
            c_print::print_string_P(PSTR("Sleeping"));
            break;
    }
    c_report::report_util_feedback_line_feed();
}

// Welcome message
void c_report::report_init_message()
{
    c_print::print_string_P(PSTR("\r\nGrbl " GRBL_VERSION " ['$' for help]\r\n"));
}

// Grbl help message
void c_report::report_grbl_help()
{
    c_print::print_string_P(PSTR("[HLP:$$ $# $G $I $N $x=val $Nx=line $J=line $SLP $C $X $H ~ ! ? ctrl-x]\r\n"));
}

// Grbl global settings print out.
// NOTE: The numbering scheme here must correlate to storing in settings.c
void c_report::report_grbl_settings()
{
    // Print Grbl settings.
    report_util_uint8_setting(0,  c_settings::settings.pulse_microseconds);
    report_util_uint8_setting(1,  c_settings::settings.stepper_idle_lock_time);
    report_util_uint8_setting(2,  c_settings::settings.step_invert_mask);
    report_util_uint8_setting(3,  c_settings::settings.dir_invert_mask);
    report_util_uint8_setting(4,  bit_istrue(c_settings::settings.flags, BITFLAG_INVERT_ST_ENABLE));
    report_util_uint8_setting(5,  bit_istrue(c_settings::settings.flags, BITFLAG_INVERT_LIMIT_PINS));
    report_util_uint8_setting(6,  bit_istrue(c_settings::settings.flags, BITFLAG_INVERT_PROBE_PIN));
    report_util_uint8_setting(10, c_settings::settings.status_report_mask);
    report_util_float_setting(11, c_settings::settings.junction_deviation, N_DECIMAL_SETTINGVALUE);
    report_util_float_setting(12, c_settings::settings.arc_tolerance, N_DECIMAL_SETTINGVALUE);
    report_util_uint8_setting(13, bit_istrue(c_settings::settings.flags, BITFLAG_REPORT_INCHES));
    report_util_uint8_setting(20, bit_istrue(c_settings::settings.flags, BITFLAG_SOFT_LIMIT_ENABLE));
    report_util_uint8_setting(21, bit_istrue(c_settings::settings.flags, BITFLAG_HARD_LIMIT_ENABLE));
    report_util_uint8_setting(22, bit_istrue(c_settings::settings.flags, BITFLAG_HOMING_ENABLE));
    report_util_uint8_setting(23, c_settings::settings.homing_dir_mask);
    report_util_float_setting(24, c_settings::settings.homing_feed_rate, N_DECIMAL_SETTINGVALUE);
    report_util_float_setting(25, c_settings::settings.homing_seek_rate, N_DECIMAL_SETTINGVALUE);
    report_util_uint8_setting(26, c_settings::settings.homing_debounce_delay);
    report_util_float_setting(27, c_settings::settings.homing_pulloff, N_DECIMAL_SETTINGVALUE);
    report_util_float_setting(30, c_settings::settings.rpm_max, N_DECIMAL_RPMVALUE);
    report_util_float_setting(31, c_settings::settings.rpm_min, N_DECIMAL_RPMVALUE);
    report_util_uint8_setting(32, bit_istrue(c_settings::settings.flags, BITFLAG_LASER_MODE));

    // Print axis settings
    uint8_t idx, set_idx;
    uint8_t val = AXIS_SETTINGS_START_VAL;
    for (set_idx = 0; set_idx < AXIS_N_SETTINGS; set_idx++)
    {
        for (idx = 0; idx < N_AXIS; idx++)
            switch (set_idx)
            {
                case 0:
                    report_util_float_setting(val + idx, c_settings::settings.steps_per_mm[idx], N_DECIMAL_SETTINGVALUE);
                    break;
                case 1:
                    report_util_float_setting(val + idx, c_settings::settings.max_rate[idx], N_DECIMAL_SETTINGVALUE);
                    break;
                case 2:
                    report_util_float_setting(val + idx, c_settings::settings.acceleration[idx] / (60 * 60), N_DECIMAL_SETTINGVALUE);
                    break;
                case 3:
                    report_util_float_setting(val + idx, -c_settings::settings.max_travel[idx], N_DECIMAL_SETTINGVALUE);
                    break;
            }
        val += AXIS_SETTINGS_INCREMENT;
    }
}

// Prints current probe parameters. Upon a probe command, these parameters are updated upon a
// successful probe or upon a failed probe with the G38.3 without errors command (if supported).
// These values are retained until Grbl is power-cycled, whereby they will be re-zeroed.
void c_report::report_probe_parameters()
{
    // Report in terms of machine position.
    c_print::print_string_P(PSTR("[PRB:"));
    float print_position[N_AXIS];
    c_system::system_convert_array_steps_to_mpos(print_position, c_system::sys_probe_position);
    report_util_axis_values(print_position);
    c_serial::serial_write(':');
    c_print::print_uint8_base10(c_system::sys.probe_succeeded);
    c_report::report_util_feedback_line_feed();
}

// Prints Grbl NGC parameters (coordinate offsets, probing)
void c_report::report_ngc_parameters()
{
    float coord_data[N_AXIS];
    uint8_t coord_select;
    for (coord_select = 0; coord_select <= SETTING_INDEX_NCOORD; coord_select++)
    {
        if (!(c_settings::settings_read_coord_data(coord_select, coord_data)))
        {
            c_report::report_status_message(STATUS_SETTING_READ_FAIL);
            return;
        }
        c_print::print_string_P(PSTR("[G"));
        switch (coord_select)
        {
            case 6:
                c_print::print_string_P(PSTR("28"));
                break;
            case 7:
               c_print:: print_string_P(PSTR("30"));
                break;
            default:
                c_print::print_uint8_base10(coord_select + 54);
                break; // G54-G59
        }
        c_serial::serial_write(':');
        c_report::report_util_axis_values(coord_data);
        c_report::report_util_feedback_line_feed();
    }
    c_print::print_string_P(PSTR("[G92:")); // Print G92,G92.1 which are not persistent in memory
    c_report::report_util_axis_values(c_gcode::gc_state.coord_offset);
    c_report::report_util_feedback_line_feed();
    c_print::print_string_P(PSTR("[TLO:")); // Print tool length offset value
   c_print:: print_float_coord_value(c_gcode::gc_state.tool_length_offset);
    c_report::report_util_feedback_line_feed();
    c_report::report_probe_parameters(); // Print probe parameters. Not persistent in memory.
}

// Print current gcode parser mode state
void c_report::report_gcode_modes()
{
    c_print::print_string_P(PSTR("[GC:G"));
    if (c_gcode::gc_state.modal.motion >= MOTION_MODE_PROBE_TOWARD)
    {
        c_print::print_string_P(PSTR("38."));
        c_print::print_uint8_base10(c_gcode::gc_state.modal.motion - (MOTION_MODE_PROBE_TOWARD - 2));
    }
    else
    {
        c_print::print_uint8_base10(c_gcode::gc_state.modal.motion);
    }

    report_util_gcode_modes_G();
    c_print::print_uint8_base10(c_gcode::gc_state.modal.coord_select + 54);

    report_util_gcode_modes_G();
    c_print::print_uint8_base10(c_gcode::gc_state.modal.plane_select + 17);

    report_util_gcode_modes_G();
    c_print::print_uint8_base10(21 - c_gcode::gc_state.modal.units);

    report_util_gcode_modes_G();
    c_print::print_uint8_base10(c_gcode::gc_state.modal.distance + 90);

    report_util_gcode_modes_G();
    c_print::print_uint8_base10(94 - c_gcode::gc_state.modal.feed_rate);

    if (c_gcode::gc_state.modal.program_flow)
    {
        report_util_gcode_modes_M();
        switch (c_gcode::gc_state.modal.program_flow)
        {
            case PROGRAM_FLOW_PAUSED:
                c_serial::serial_write('0');
                break;
                // case PROGRAM_FLOW_OPTIONAL_STOP : serial_write('1'); break; // M1 is ignored and not supported.
            case PROGRAM_FLOW_COMPLETED_M2:
            case PROGRAM_FLOW_COMPLETED_M30:
                c_print::print_uint8_base10(c_gcode::gc_state.modal.program_flow);
                break;
        }
    }

    report_util_gcode_modes_M();
    switch (c_gcode::gc_state.modal.spindle)
    {
        case SPINDLE_ENABLE_CW:
            c_serial::serial_write('3');
            break;
        case SPINDLE_ENABLE_CCW:
            c_serial::serial_write('4');
            break;
        case SPINDLE_DISABLE:
            c_serial::serial_write('5');
            break;
    }

    report_util_gcode_modes_M();
    if (c_gcode::gc_state.modal.coolant)
    {
        // Note: Multiple coolant states may be active at the same time.
        if (c_gcode::gc_state.modal.coolant & PL_COND_FLAG_COOLANT_MIST)  c_serial::serial_write('7');
        if (c_gcode::gc_state.modal.coolant & PL_COND_FLAG_COOLANT_FLOOD) c_serial::serial_write('8');
    }
    else
        c_serial::serial_write('9');

    c_print::print_string_P(PSTR(" T"));
    c_print::print_uint8_base10(c_gcode::gc_state.tool);

    c_print::print_string_P(PSTR(" F"));
    c_print::print_float_rate_value(c_gcode::gc_state.feed_rate);

    c_print::print_string_P(PSTR(" S"));
    c_print::print_float(c_gcode::gc_state.spindle_speed, N_DECIMAL_RPMVALUE);

    c_report::report_util_feedback_line_feed();
}

// Prints specified startup line
void c_report::report_startup_line(uint8_t n, char *line)
{
    c_print::print_string_P(PSTR("$N"));
    c_print::print_uint8_base10(n);
    c_serial::serial_write('=');
    c_print::print_string(line);
    report_util_line_feed();
}

void c_report::report_execute_startup_message(char *line, uint8_t status_code)
{
    c_serial::serial_write('>');
    c_print::print_string(line);
    c_serial::serial_write(':');
    report_status_message(status_code);
}

// Prints build info line
void c_report::report_build_info(char *line)
{
    c_print::print_string_P(PSTR("[VER:" GRBL_VERSION "." GRBL_VERSION_BUILD ":"));
    c_print::print_string(line);
    report_util_feedback_line_feed();
    c_print::print_string_P(PSTR("[OPT:")); // Generate compile-time build option list
    c_serial::serial_write('V');
    c_serial::serial_write('N');
    c_serial::serial_write('M');
#ifdef COREXY
    c_serial::serial_write('C');
#endif
#ifdef PARKING_ENABLE
    c_serial::serial_write('P');
#endif
#ifdef HOMING_FORCE_SET_ORIGIN
    c_serial::serial_write('Z');
#endif
#ifdef HOMING_SINGLE_AXIS_COMMANDS
    c_serial::serial_write('H');
#endif
#ifdef LIMITS_TWO_SWITCHES_ON_AXES
    c_serial::serial_write('L');
#endif
#ifdef ALLOW_FEED_OVERRIDE_DURING_PROBE_CYCLES
    c_serial::serial_write('A');
#endif
#ifndef ENABLE_RESTORE_EEPROM_WIPE_ALL // NOTE: Shown when disabled.
    c_serial::serial_write('*');
#endif
#ifndef ENABLE_RESTORE_EEPROM_DEFAULT_SETTINGS // NOTE: Shown when disabled.
    c_serial::serial_write('$');
#endif
#ifndef ENABLE_RESTORE_EEPROM_CLEAR_PARAMETERS // NOTE: Shown when disabled.
    c_serial::serial_write('#');
#endif
#ifndef ENABLE_BUILD_INFO_WRITE_COMMAND // NOTE: Shown when disabled.
    c_serial::serial_write('I');
#endif
#ifndef FORCE_BUFFER_SYNC_DURING_EEPROM_WRITE // NOTE: Shown when disabled.
    c_serial::serial_write('E');
#endif
#ifndef FORCE_BUFFER_SYNC_DURING_WCO_CHANGE // NOTE: Shown when disabled.
    c_serial::serial_write('W');
#endif
    // NOTE: Compiled values, like override increments/max/min values, may be added at some point later.
    // These will likely have a comma delimiter to separate them.

    report_util_feedback_line_feed();
}

// Prints the character string line Grbl has received from the user, which has been pre-parsed,
// and has been sent into protocol_execute_line() routine to be executed by Grbl.
void c_report::report_echo_line_received(char *line)
{
    c_print::print_string_P(PSTR("[echo: "));
    c_print::print_string(line);
    c_report::report_util_feedback_line_feed();
}

// Prints real-time data. This function grabs a real-time snapshot of the stepper subprogram
// and the actual location of the CNC machine. Users may change the following function to their
// specific needs, but the desired real-time data report must be as short as possible. This is
// requires as it minimizes the computational overhead and allows grbl to keep running smoothly,
// especially during g-code programs with fast, short line segments and high frequency reports (5-20Hz).
void c_report::report_realtime_status()
{
    uint8_t idx;
    int32_t current_position[N_AXIS]; // Copy current state of the system position variable
    memcpy(current_position, c_system::sys_position, sizeof(c_system::sys_position));
    float print_position[N_AXIS];
    c_system::system_convert_array_steps_to_mpos(print_position, current_position);

    // Report current machine state and sub-states
    c_serial::serial_write('<');
    switch (c_system::sys.state)
    {
        case STATE_IDLE:
            c_print::print_string_P(PSTR("Idle"));
            break;
        case STATE_CYCLE:
            c_print::print_string_P(PSTR("Run"));
            break;
        case STATE_HOLD:
            if (!(c_system::sys.suspend & SUSPEND_JOG_CANCEL))
            {
                c_print::print_string_P(PSTR("Hold:"));
                if (c_system::sys.suspend & SUSPEND_HOLD_COMPLETE)
                {
                    c_serial::serial_write('0');
                } // Ready to resume
                else
                {
                    c_serial::serial_write('1');
                } // Actively holding
                break;
            } // Continues to print jog state during jog cancel.
        /* no break */
        case STATE_JOG:
            c_print::print_string_P(PSTR("Jog"));
            break;
        case STATE_HOMING:
            c_print::print_string_P(PSTR("Home"));
            break;
        case STATE_ALARM:
            c_print::print_string_P(PSTR("Alarm"));
            break;
        case STATE_CHECK_MODE:
            c_print::print_string_P(PSTR("Check"));
            break;
        case STATE_SAFETY_DOOR:
            c_print::print_string_P(PSTR("Door:"));
            if (c_system::sys.suspend & SUSPEND_INITIATE_RESTORE)
                c_serial::serial_write('3'); // Restoring
            else
            {
                if (c_system::sys.suspend & SUSPEND_RETRACT_COMPLETE)
                {
                    if (c_system::sys.suspend & SUSPEND_SAFETY_DOOR_AJAR)
                        c_serial::serial_write('1'); // Door ajar
                    else
                        c_serial::serial_write('0'); // Door closed and ready to resume
                }
                else
                    c_serial::serial_write('2'); // Retracting
            }
            break;
        case STATE_SLEEP:
            c_print::print_string_P(PSTR("Sleep"));
            break;
    }

    float wco[N_AXIS];
    if (bit_isfalse(c_settings::settings.status_report_mask,BITFLAG_RT_STATUS_POSITION_TYPE) || (c_system::sys.report_wco_counter == 0))
    {
        for (idx = 0; idx < N_AXIS; idx++)
        {
            // Apply work coordinate offsets and tool length offset to current position.
            wco[idx] = c_gcode::gc_state.coord_system[idx] + c_gcode::gc_state.coord_offset[idx];
            if (idx == TOOL_LENGTH_OFFSET_AXIS)
                wco[idx] += c_gcode::gc_state.tool_length_offset;
            if (bit_isfalse(c_settings::settings.status_report_mask, BITFLAG_RT_STATUS_POSITION_TYPE))
                print_position[idx] -= wco[idx];
        }
    }

    // Report machine position
    if (bit_istrue(c_settings::settings.status_report_mask, BITFLAG_RT_STATUS_POSITION_TYPE))
        c_print::print_string_P(PSTR("|MPos:"));
    else
        c_print::print_string_P(PSTR("|WPos:"));
    report_util_axis_values(print_position);

    // Returns planner and serial read buffer states.
#ifdef REPORT_FIELD_BUFFER_STATE
    if (bit_istrue(settings.status_report_mask, BITFLAG_RT_STATUS_BUFFER_STATE))
    {
        c_print::print_string_P(PSTR("|Bf:"));
        c_print::print_uint8_base10(plan_get_block_buffer_available());
        c_serial::serial_write(',');
        c_print::print_uint8_base10(serial_get_rx_buffer_available());
    }
#endif

#ifdef REPORT_FIELD_LINE_NUMBERS
    // Report current line number
    plan_block_t *cur_block = plan_get_current_block();
    if (cur_block != NULL)
    {
        uint32_t ln = cur_block->line_number;
        if (ln > 0)
        {
            c_print::print_string_P(PSTR("|Ln:"));
            c_print::print_int32(ln);
        }
    }
#endif

    // Report realtime feed speed
#ifdef REPORT_FIELD_CURRENT_FEED_SPEED
    c_print::print_string_P(PSTR("|FS:"));
    c_print::print_float_rate_value(st_get_realtime_rate());
    c_serial::serial_write(',');
    c_print::print_float(sys.spindle_speed, N_DECIMAL_RPMVALUE);
#endif

#ifdef REPORT_FIELD_PIN_STATE
    uint8_t lim_pin_state = limits_get_state();
    uint8_t ctrl_pin_state = system_control_get_state();
    uint8_t prb_pin_state = probe_get_state();
    if (lim_pin_state | ctrl_pin_state | prb_pin_state)
    {
        c_print::print_string_P(PSTR("|Pn:"));
        if (prb_pin_state) c_serial::serial_write('P');
        if (lim_pin_state)
        {
            if (bit_istrue(lim_pin_state, bit(X_AXIS))) c_serial::serial_write('X');
            if (bit_istrue(lim_pin_state, bit(Y_AXIS))) c_serial::serial_write('Y');
            if (bit_istrue(lim_pin_state, bit(Z_AXIS))) c_serial::serial_write('Z');
            if (bit_istrue(lim_pin_state, bit(A_AXIS))) c_serial::serial_write('A');
            if (bit_istrue(lim_pin_state, bit(B_AXIS))) c_serial::serial_write('B');
            if (bit_istrue(lim_pin_state, bit(C_AXIS))) c_serial::serial_write('C');
        }
        if (ctrl_pin_state)
        {
#ifdef ENABLE_SAFETY_DOOR_INPUT_PIN
            if (bit_istrue(ctrl_pin_state,CONTROL_PIN_INDEX_SAFETY_DOOR))  serial_write('D');
#endif
            if (bit_istrue(ctrl_pin_state, CONTROL_PIN_INDEX_RESET))       serial_write('R');
            if (bit_istrue(ctrl_pin_state, CONTROL_PIN_INDEX_FEED_HOLD))   serial_write('H');
            if (bit_istrue(ctrl_pin_state, CONTROL_PIN_INDEX_CYCLE_START)) serial_write('S');
        }
    }
#endif

#ifdef REPORT_FIELD_WORK_COORD_OFFSET
    if (sys.report_wco_counter > 0) sys.report_wco_counter--;
    else
    {
        if (sys.state & (STATE_HOMING | STATE_CYCLE | STATE_HOLD | STATE_JOG | STATE_SAFETY_DOOR))
            sys.report_wco_counter = (REPORT_WCO_REFRESH_BUSY_COUNT - 1); // Reset counter for slow refresh
        else
            sys.report_wco_counter = (REPORT_WCO_REFRESH_IDLE_COUNT - 1);
        if (sys.report_ovr_counter == 0)
            sys.report_ovr_counter = 1; // Set override on next report.
        c_print::print_string_P(PSTR("|WCO:"));
        report_util_axis_values(wco);
    }
#endif

#ifdef REPORT_FIELD_OVERRIDES
    if (sys.report_ovr_counter > 0) sys.report_ovr_counter--;
    else
    {
        if (sys.state & (STATE_HOMING | STATE_CYCLE | STATE_HOLD | STATE_JOG | STATE_SAFETY_DOOR))
            sys.report_ovr_counter = (REPORT_OVR_REFRESH_BUSY_COUNT - 1); // Reset counter for slow refresh
        else
            sys.report_ovr_counter = (REPORT_OVR_REFRESH_IDLE_COUNT - 1);

        c_print::print_string_P(PSTR("|Ov:"));
        c_print::print_uint8_base10(sys.f_override);
        serial_write(',');
        c_print::print_uint8_base10(sys.r_override);
        serial_write(',');
        c_print::print_uint8_base10(sys.spindle_speed_ovr);

        uint8_t sp_state = spindle_get_state();
        uint8_t cl_state = coolant_get_state();
        if (sp_state || cl_state)
        {
            c_print::print_string_P(PSTR("|A:"));
            if (sp_state)
            {
                // != SPINDLE_STATE_DISABLE
                if (sp_state == SPINDLE_STATE_CW)
                    serial_write('S'); // CW
                else
                    serial_write('C'); // CCW
            }
            if (cl_state & COOLANT_STATE_FLOOD) serial_write('F');
            if (cl_state & COOLANT_STATE_MIST)  serial_write('M');
        }
    }
#endif

    c_serial::serial_write('>');
    report_util_line_feed();
}

#ifdef DEBUG
void report_realtime_debug()
{

}
#endif
