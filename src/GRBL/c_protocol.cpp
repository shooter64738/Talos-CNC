/* 
* c_protocol.cpp
*
* Created: 3/5/2019 2:57:02 PM
* Author: jeff_d
*/


/*
 protocol.c - controls Grbl execution protocol and procedures
 Part of Grbl

 Copyright (c) 2011-2016 Sungeun K. Jeon for Gnea Research LLC
 Copyright (c) 2009-2011 Simen Svale Skogsrud

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

#include "c_protocol.h"
#include "..\helpers.h"
#include "c_system.h"
#include "c_serial.h"
#include <stddef.h>
#include "c_report.h"
#include "c_gcode.h"
#include "c_planner.h"
#include "c_settings.h"
#include "c_coolant.h"
#include "c_spindle.h"
#include "c_stepper.h"
#include "c_print.h"
#include "utils.h"
//#include "protocol.h"
//#include "grbl.h"
//#include "system.h"
//#include "serial.h"

// Define line flags. Includes comment type tracking and line overflow detection.
#define LINE_FLAG_OVERFLOW bit(0)
#define LINE_FLAG_COMMENT_PARENTHESES bit(1)
#define LINE_FLAG_COMMENT_SEMICOLON bit(2)

static char line[LINE_BUFFER_SIZE]; // Line to be executed. Zero-terminated.



/*
 GRBL PRIMARY LOOP:
 */
void c_protocol::protocol_main_loop()
{
    // Perform some machine checks to make sure everything is good to go.
#ifdef CHECK_LIMITS_AT_INIT
    if (bit_istrue(settings.flags, BITFLAG_HARD_LIMIT_ENABLE) && limits_get_state())
    {
        c_system::sys.state = STATE_ALARM; // Ensure alarm state is active.
        report_feedback_message(MESSAGE_CHECK_LIMITS);
    }
#endif
    // Check for and report alarm state after a reset, error, or an initial power up.
    // NOTE: Sleep mode disables the stepper drivers and position can't be guaranteed.
    // Re-initialize the sleep state as an ALARM mode to ensure user homes or acknowledges.
    if (c_system::sys.state & (STATE_ALARM | STATE_SLEEP))
    {
        c_report::report_feedback_message(MESSAGE_ALARM_LOCK);
        c_system::sys.state = STATE_ALARM; // Ensure alarm state is set.
    }
    else
    {
        // Check if the safety door is open.
        c_system::sys.state = STATE_IDLE;
        if (c_system::system_check_safety_door_ajar())
        {
            bit_true(c_system::sys_rt_exec_state, EXEC_SAFETY_DOOR);
            protocol_execute_realtime(); // Enter safety door mode. Should return as IDLE state.
        }
        // All systems go!
        c_system::system_execute_startup(line); // Execute startup script.
    }

    // ---------------------------------------------------------------------------------
    // Primary loop! Upon a system abort, this exits back to main() to reset the system.
    // This is also where Grbl idles while waiting for something to do.
    // ---------------------------------------------------------------------------------

    uint8_t line_flags = 0;
    uint8_t char_counter = 0;
    uint8_t c;
    for (;;)
    {

        // Process one line of incoming serial data, as the data becomes available. Performs an
        // initial filtering by removing spaces and comments and capitalizing all letters.
        while ((c = c_serial::serial_read()) != SERIAL_NO_DATA)
        {
            if ((c == '\n') || (c == '\r'))
            { // End of line reached

                protocol_execute_realtime(); // Runtime command check point.
                if (c_system::sys.abort)
                {
                    return;
                } // Bail to calling function upon system abort

                line[char_counter] = 0; // Set string termination character.
#ifdef REPORT_ECHO_LINE_RECEIVED
                report_echo_line_received(line);
#endif

                // Direct and execute one line of formatted input, and report status of execution.
                if (line_flags & LINE_FLAG_OVERFLOW)
                {
                    // Report line overflow error.
                    c_report::report_status_message(STATUS_OVERFLOW);
                }
                else if (line[0] == 0)
                {
                    // Empty or comment line. For syncing purposes.
                    c_report::report_status_message(STATUS_OK);
                }
                else if (line[0] == '$')
                {
                    // Grbl '$' system command
                    c_report::report_status_message(c_system::system_execute_line(line));
                }
                else if (c_system::sys.state & (STATE_ALARM | STATE_JOG))
                {
                    // Everything else is gcode. Block if in alarm or jog mode.
                    c_report::report_status_message(STATUS_SYSTEM_GC_LOCK);
                }
                else
                {
                    // Parse and execute g-code block.
                    c_report::report_status_message(c_gcode::gc_execute_line(line));
                }

                // Reset tracking data for next line.
                line_flags = 0;
                char_counter = 0;

            }
            else
            {

                if (line_flags)
                {
                    // Throw away all (except EOL) comment characters and overflow characters.
                    if (c == ')')
                    {
                        // End of '()' comment. Resume line allowed.
                        if (line_flags & LINE_FLAG_COMMENT_PARENTHESES)
                        {
                            line_flags &= ~(LINE_FLAG_COMMENT_PARENTHESES);
                        }
                    }
                }
                else
                {
                    if (c <= ' ')
                    {
                        // Throw away whitepace and control characters
                    }
                    else if (c == '/')
                    {
                        // Block delete NOT SUPPORTED. Ignore character.
                        // NOTE: If supported, would simply need to check the system if block delete is enabled.
                    }
                    else if (c == '(')
                    {
                        // Enable comments flag and ignore all characters until ')' or EOL.
                        // NOTE: This doesn't follow the NIST definition exactly, but is good enough for now.
                        // In the future, we could simply remove the items within the comments, but retain the
                        // comment control characters, so that the g-code parser can error-check it.
                        line_flags |= LINE_FLAG_COMMENT_PARENTHESES;
                    }
                    else if (c == ';')
                    {
                        // NOTE: ';' comment to EOL is a LinuxCNC definition. Not NIST.
                        line_flags |= LINE_FLAG_COMMENT_SEMICOLON;
                        // TODO: Install '%' feature
                        // } else if (c == '%') {
                        // Program start-end percent sign NOT SUPPORTED.
                        // NOTE: This maybe installed to tell Grbl when a program is running vs manual input,
                        // where, during a program, the system auto-cycle start will continue to execute
                        // everything until the next '%' sign. This will help fix resuming issues with certain
                        // functions that empty the planner buffer to execute its task on-time.
                    }
                    else if (char_counter >= (LINE_BUFFER_SIZE - 1))
                    {
                        // Detect line buffer overflow and set flag.
                        line_flags |= LINE_FLAG_OVERFLOW;
                    }
                    else if (c >= 'a' && c <= 'z')
                    { // Upcase lowercase
                        line[char_counter++] = c - 'a' + 'A';
                    }
                    else
                    {
                        line[char_counter++] = c;
                    }
                }

            }
        }

        // If there are no more characters in the serial read buffer to be processed and executed,
        // this indicates that g-code streaming has either filled the planner buffer or has
        // completed. In either case, auto-cycle start, if enabled, any queued moves.
        protocol_auto_cycle_start();

        protocol_execute_realtime();  // Runtime command check point.
        if (c_system::sys.abort)
        {
            return;
        } // Bail to main() program loop to reset system.

#ifdef SLEEP_ENABLE
        // Check for sleep conditions and execute auto-park, if timeout duration elapses.
        sleep_check();
#endif
    }

    return; /* Never reached */
}

// Block until all buffered steps are executed or in a cycle state. Works with feed hold
// during a synchronize call, if it should happen. Also, waits for clean cycle end.
void c_protocol::protocol_buffer_synchronize()
{
    // If system is queued, ensure cycle resumes if the auto start flag is present.
    protocol_auto_cycle_start();
    do
    {
        protocol_execute_realtime();   // Check and execute run-time commands
        if (c_system::sys.abort)
        {
            return;
        } // Check for system abort
    }
    while (c_planner::plan_get_current_block() || (c_system::sys.state == STATE_CYCLE));
}

// Auto-cycle start triggers when there is a motion ready to execute and if the main program is not
// actively parsing commands.
// NOTE: This function is called from the main loop, buffer sync, and mc_line() only and executes
// when one of these conditions exist respectively: There are no more blocks sent (i.e. streaming
// is finished, single commands), a command that needs to wait for the motions in the buffer to
// execute calls a buffer sync, or the planner buffer is full and ready to go.
void c_protocol::protocol_auto_cycle_start()
{
    if (c_planner::plan_get_current_block() != NULL)
    { // Check if there are any blocks in the buffer.
        c_system::system_set_exec_state_flag(EXEC_CYCLE_START); // If so, execute them!
    }
}

// This function is the general interface to Grbl's real-time command execution system. It is called
// from various check points in the main program, primarily where there may be a while loop waiting
// for a buffer to clear space or any point where the execution time from the last check point may
// be more than a fraction of a second. This is a way to execute realtime commands asynchronously
// (aka multitasking) with grbl's g-code parsing and planning functions. This function also serves
// as an interface for the interrupts to set the system realtime flags, where only the main program
// handles them, removing the need to define more computationally-expensive volatile variables. This
// also provides a controlled way to execute certain tasks without having two or more instances of
// the same task, such as the planner recalculating the buffer upon a feedhold or overrides.
// NOTE: The sys_rt_exec_state variable flags are set by any process, step or serial interrupts, pinouts,
// limit switches, or the main program.
void c_protocol::protocol_execute_realtime()
{
    protocol_exec_rt_system();
    if (c_system::sys.suspend)
    {
        protocol_exec_rt_suspend();
    }
}

// Executes run-time commands, when required. This function primarily operates as Grbl's state
// machine and controls the various real-time features Grbl has to offer.
// NOTE: Do not alter this unless you know exactly what you are doing!
void c_protocol::protocol_exec_rt_system()
{
    uint8_t rt_exec; // Temp variable to avoid calling volatile multiple times.
    rt_exec = c_system::sys_rt_exec_alarm; // Copy volatile sys_rt_exec_alarm.
    if (rt_exec)
    { // Enter only if any bit flag is true
        // System alarm. Everything has shutdown by something that has gone severely wrong. Report
        // the source of the error to the user. If critical, Grbl disables by entering an infinite
        // loop until system reset/abort.
        c_system::sys.state = STATE_ALARM; // Set system alarm state
        c_report::report_alarm_message(rt_exec);
        // Halt everything upon a critical event flag. Currently hard and soft limits flag this.
        if ((rt_exec == EXEC_ALARM_HARD_LIMIT) || (rt_exec == EXEC_ALARM_SOFT_LIMIT))
        {
            c_report::report_feedback_message(MESSAGE_CRITICAL_EVENT);
            c_system::system_clear_exec_state_flag(EXEC_RESET); // Disable any existing reset
            do
            {
                // Block everything, except reset and status reports, until user issues reset or power
                // cycles. Hard limits typically occur while unattended or not paying attention. Gives
                // the user and a GUI time to do what is needed before resetting, like killing the
                // incoming stream. The same could be said about soft limits. While the position is not
                // lost, continued streaming could cause a serious crash if by chance it gets executed.
            }
            while (bit_isfalse(c_system::sys_rt_exec_state, EXEC_RESET));
        }
        c_system::system_clear_exec_alarm(); // Clear alarm
    }

    rt_exec = c_system::sys_rt_exec_state; // Copy volatile sys_rt_exec_state.
    if (rt_exec)
    {

        // Execute system abort.
        if (rt_exec & EXEC_RESET)
        {
            c_system::sys.abort = true;  // Only place this is set true.
            return; // Nothing else to do but exit.
        }

        // Execute and serial print status
        if (rt_exec & EXEC_STATUS_REPORT)
        {
            c_report::report_realtime_status();
            c_system::system_clear_exec_state_flag(EXEC_STATUS_REPORT);
        }

        // NOTE: Once hold is initiated, the system immediately enters a suspend state to block all
        // main program processes until either reset or resumed. This ensures a hold completes safely.
        if (rt_exec & (EXEC_MOTION_CANCEL | EXEC_FEED_HOLD | EXEC_SAFETY_DOOR | EXEC_SLEEP))
        {

            // State check for allowable states for hold methods.
            if (!(c_system::sys.state & (STATE_ALARM | STATE_CHECK_MODE)))
            {

                // If in CYCLE or JOG states, immediately initiate a motion HOLD.
                if (c_system::sys.state & (STATE_CYCLE | STATE_JOG))
                {
                    if (!(c_system::sys.suspend & (SUSPEND_MOTION_CANCEL | SUSPEND_JOG_CANCEL)))
                    { // Block, if already holding.
                        c_stepper::st_update_plan_block_parameters(); // Notify stepper module to recompute for hold deceleration.
                        c_system::sys.step_control = STEP_CONTROL_EXECUTE_HOLD; // Initiate suspend state with active flag.
                        if (c_system::sys.state == STATE_JOG)
                        { // Jog cancelled upon any hold event, except for sleeping.
                            if (!(rt_exec & EXEC_SLEEP))
                            {
                                c_system::sys.suspend |= SUSPEND_JOG_CANCEL;
                            }
                        }
                    }
                }
                // If IDLE, Grbl is not in motion. Simply indicate suspend state and hold is complete.
                if (c_system::sys.state == STATE_IDLE)
                {
                    c_system::sys.suspend = SUSPEND_HOLD_COMPLETE;
                }

                // Execute and flag a motion cancel with deceleration and return to idle. Used primarily by probing cycle
                // to halt and cancel the remainder of the motion.
                if (rt_exec & EXEC_MOTION_CANCEL)
                {
                    // MOTION_CANCEL only occurs during a CYCLE, but a HOLD and SAFETY_DOOR may been initiated beforehand
                    // to hold the CYCLE. Motion cancel is valid for a single planner block motion only, while jog cancel
                    // will handle and clear multiple planner block motions.
                    if (!(c_system::sys.state & STATE_JOG))
                    {
                        c_system::sys.suspend |= SUSPEND_MOTION_CANCEL;
                    } // NOTE: State is STATE_CYCLE.
                }

                // Execute a feed hold with deceleration, if required. Then, suspend system.
                if (rt_exec & EXEC_FEED_HOLD)
                {
                    // Block SAFETY_DOOR, JOG, and SLEEP states from changing to HOLD state.
                    if (!(c_system::sys.state & (STATE_SAFETY_DOOR | STATE_JOG | STATE_SLEEP)))
                    {
                        c_system::sys.state = STATE_HOLD;
                    }
                }

                // Execute a safety door stop with a feed hold and disable spindle/coolant.
                // NOTE: Safety door differs from feed holds by stopping everything no matter state, disables powered
                // devices (spindle/coolant), and blocks resuming until switch is re-engaged.
                if (rt_exec & EXEC_SAFETY_DOOR)
                {
                    c_report::report_feedback_message(MESSAGE_SAFETY_DOOR_AJAR);
                    // If jogging, block safety door methods until jog cancel is complete. Just flag that it happened.
                    if (!(c_system::sys.suspend & SUSPEND_JOG_CANCEL))
                    {
                        // Check if the safety re-opened during a restore parking motion only. Ignore if
                        // already retracting, parked or in sleep state.
                        if (c_system::sys.state == STATE_SAFETY_DOOR)
                        {
                            if (c_system::sys.suspend & SUSPEND_INITIATE_RESTORE)
                            { // Actively restoring
#ifdef PARKING_ENABLE
                                // Set hold and reset appropriate control flags to restart parking sequence.
                                if (c_system::sys.step_control & STEP_CONTROL_EXECUTE_SYS_MOTION)
                                {
                                    st_update_plan_block_parameters(); // Notify stepper module to recompute for hold deceleration.
                                    c_system::sys.step_control = (STEP_CONTROL_EXECUTE_HOLD | STEP_CONTROL_EXECUTE_SYS_MOTION);
                                    c_system::sys.suspend &= ~(SUSPEND_HOLD_COMPLETE);
                                } // else NO_MOTION is active.
#endif
                                c_system::sys.suspend &= ~(SUSPEND_RETRACT_COMPLETE | SUSPEND_INITIATE_RESTORE | SUSPEND_RESTORE_COMPLETE);
                                c_system::sys.suspend |= SUSPEND_RESTART_RETRACT;
                            }
                        }
                        if (c_system::sys.state != STATE_SLEEP)
                        {
                            c_system::sys.state = STATE_SAFETY_DOOR;
                        }
                    }
                    // NOTE: This flag doesn't change when the door closes, unlike sys.state. Ensures any parking motions
                    // are executed if the door switch closes and the state returns to HOLD.
                    c_system::sys.suspend |= SUSPEND_SAFETY_DOOR_AJAR;
                }

            }

            if (rt_exec & EXEC_SLEEP)
            {
                if (c_system::sys.state == STATE_ALARM)
                {
                    c_system::sys.suspend |= (SUSPEND_RETRACT_COMPLETE | SUSPEND_HOLD_COMPLETE);
                }
                c_system::sys.state = STATE_SLEEP;
            }

            c_system::system_clear_exec_state_flag((EXEC_MOTION_CANCEL | EXEC_FEED_HOLD | EXEC_SAFETY_DOOR | EXEC_SLEEP));
        }

        // Execute a cycle start by starting the stepper interrupt to begin executing the blocks in queue.
        if (rt_exec & EXEC_CYCLE_START)
        {
            // Block if called at same time as the hold commands: feed hold, motion cancel, and safety door.
            // Ensures auto-cycle-start doesn't resume a hold without an explicit user-input.
            if (!(rt_exec & (EXEC_FEED_HOLD | EXEC_MOTION_CANCEL | EXEC_SAFETY_DOOR)))
            {
                // Resume door state when parking motion has retracted and door has been closed.
                if ((c_system::sys.state == STATE_SAFETY_DOOR) && !(c_system::sys.suspend & SUSPEND_SAFETY_DOOR_AJAR))
                {
                    if (c_system::sys.suspend & SUSPEND_RESTORE_COMPLETE)
                    {
                        c_system::sys.state = STATE_IDLE; // Set to IDLE to immediately resume the cycle.
                    }
                    else if (c_system::sys.suspend & SUSPEND_RETRACT_COMPLETE)
                    {
                        // Flag to re-energize powered components and restore original position, if disabled by SAFETY_DOOR.
                        // NOTE: For a safety door to resume, the switch must be closed, as indicated by HOLD state, and
                        // the retraction execution is complete, which implies the initial feed hold is not active. To
                        // restore normal operation, the restore procedures must be initiated by the following flag. Once,
                        // they are complete, it will call CYCLE_START automatically to resume and exit the suspend.
                        c_system::sys.suspend |= SUSPEND_INITIATE_RESTORE;
                    }
                }
                // Cycle start only when IDLE or when a hold is complete and ready to resume.
                if ((c_system::sys.state == STATE_IDLE) || ((c_system::sys.state & STATE_HOLD) && (c_system::sys.suspend & SUSPEND_HOLD_COMPLETE)))
                {
                    if (c_system::sys.state == STATE_HOLD && c_system::sys.spindle_stop_ovr)
                    {
                        c_system::sys.spindle_stop_ovr |= SPINDLE_STOP_OVR_RESTORE_CYCLE; // Set to restore in suspend routine and cycle start after.
                    }
                    else
                    {
                        // Start cycle only if queued motions exist in planner buffer and the motion is not canceled.
                        c_system::sys.step_control = STEP_CONTROL_NORMAL_OP; // Restore step control to normal operation
                        if (c_planner::plan_get_current_block() && bit_isfalse(c_system::sys.suspend, SUSPEND_MOTION_CANCEL))
                        {
                            c_system::sys.suspend = SUSPEND_DISABLE; // Break suspend state.
                            c_system::sys.state = STATE_CYCLE;
                            c_stepper::st_prep_buffer(); // Initialize step segment buffer before beginning cycle.
                            c_stepper::st_wake_up();
                        }
                        else
                        { // Otherwise, do nothing. Set and resume IDLE state.
                            c_system::sys.suspend = SUSPEND_DISABLE; // Break suspend state.
                            c_system::sys.state = STATE_IDLE;
                        }
                    }
                }
            }
            c_system::system_clear_exec_state_flag(EXEC_CYCLE_START);
        }

        if (rt_exec & EXEC_CYCLE_STOP)
        {
            // Reinitializes the cycle plan and stepper system after a feed hold for a resume. Called by
            // realtime command execution in the main program, ensuring that the planner re-plans safely.
            // NOTE: Bresenham algorithm variables are still maintained through both the planner and stepper
            // cycle reinitializations. The stepper path should continue exactly as if nothing has happened.
            // NOTE: EXEC_CYCLE_STOP is set by the stepper subsystem when a cycle or feed hold completes.
            if ((c_system::sys.state & (STATE_HOLD | STATE_SAFETY_DOOR | STATE_SLEEP)) && !(c_system::sys.soft_limit) && !(c_system::sys.suspend & SUSPEND_JOG_CANCEL))
            {
                // Hold complete. Set to indicate ready to resume.  Remain in HOLD or DOOR states until user
                // has issued a resume command or reset.
                c_planner::plan_cycle_reinitialize();
                if (c_system::sys.step_control & STEP_CONTROL_EXECUTE_HOLD)
                {
                    c_system::sys.suspend |= SUSPEND_HOLD_COMPLETE;
                }
                bit_false(c_system::sys.step_control, (STEP_CONTROL_EXECUTE_HOLD | STEP_CONTROL_EXECUTE_SYS_MOTION));
            }
            else
            {
                // Motion complete. Includes CYCLE/JOG/HOMING states and jog cancel/motion cancel/soft limit events.
                // NOTE: Motion and jog cancel both immediately return to idle after the hold completes.
                if (c_system::sys.suspend & SUSPEND_JOG_CANCEL)
                {   // For jog cancel, flush buffers and sync positions.
                    c_system::sys.step_control = STEP_CONTROL_NORMAL_OP;
                    c_planner::plan_reset();
                    c_stepper::st_reset();
                    c_gcode::gc_sync_position();
                    c_planner::plan_sync_position();
                }
                if (c_system::sys.suspend & SUSPEND_SAFETY_DOOR_AJAR)
                { // Only occurs when safety door opens during jog.
                    c_system::sys.suspend &= ~(SUSPEND_JOG_CANCEL);
                    c_system::sys.suspend |= SUSPEND_HOLD_COMPLETE;
                    c_system::sys.state = STATE_SAFETY_DOOR;
                }
                else
                {
                    c_system::sys.suspend = SUSPEND_DISABLE;
                    c_system::sys.state = STATE_IDLE;
                }
				c_print::print_string_P(PSTR("DONE\r\n"));
            }
            c_system::system_clear_exec_state_flag(EXEC_CYCLE_STOP);
        }
    }

    // Execute overrides.
    rt_exec = c_system::sys_rt_exec_motion_override; // Copy volatile sys_rt_exec_motion_override
    if (rt_exec)
    {
        c_system::system_clear_exec_motion_overrides(); // Clear all motion override flags.

        uint8_t new_f_override = c_system::sys.f_override;
        if (rt_exec & EXEC_FEED_OVR_RESET)
        {
            new_f_override = DEFAULT_FEED_OVERRIDE;
        }
        if (rt_exec & EXEC_FEED_OVR_COARSE_PLUS)
        {
            new_f_override += FEED_OVERRIDE_COARSE_INCREMENT;
        }
        if (rt_exec & EXEC_FEED_OVR_COARSE_MINUS)
        {
            new_f_override -= FEED_OVERRIDE_COARSE_INCREMENT;
        }
        if (rt_exec & EXEC_FEED_OVR_FINE_PLUS)
        {
            new_f_override += FEED_OVERRIDE_FINE_INCREMENT;
        }
        if (rt_exec & EXEC_FEED_OVR_FINE_MINUS)
        {
            new_f_override -= FEED_OVERRIDE_FINE_INCREMENT;
        }
        new_f_override = min(new_f_override, MAX_FEED_RATE_OVERRIDE);
        new_f_override = max(new_f_override, MIN_FEED_RATE_OVERRIDE);

        uint8_t new_r_override = c_system::sys.r_override;
        if (rt_exec & EXEC_RAPID_OVR_RESET)
        {
            new_r_override = DEFAULT_RAPID_OVERRIDE;
        }
        if (rt_exec & EXEC_RAPID_OVR_MEDIUM)
        {
            new_r_override = RAPID_OVERRIDE_MEDIUM;
        }
        if (rt_exec & EXEC_RAPID_OVR_LOW)
        {
            new_r_override = RAPID_OVERRIDE_LOW;
        }

        if ((new_f_override != c_system::sys.f_override) || (new_r_override != c_system::sys.r_override))
        {
            c_system::sys.f_override = new_f_override;
            c_system::sys.r_override = new_r_override;
            c_system::sys.report_ovr_counter = 0; // Set to report change immediately
            c_planner::plan_update_velocity_profile_parameters();
            c_planner::plan_cycle_reinitialize();
        }
    }

    rt_exec = c_system::sys_rt_exec_accessory_override;
    if (rt_exec)
    {
        c_system::system_clear_exec_accessory_overrides(); // Clear all accessory override flags.

        // NOTE: Unlike motion overrides, spindle overrides do not require a planner reinitialization.
        uint8_t last_s_override = c_system::sys.spindle_speed_ovr;
        if (rt_exec & EXEC_SPINDLE_OVR_RESET)
        {
            last_s_override = DEFAULT_SPINDLE_SPEED_OVERRIDE;
        }
        if (rt_exec & EXEC_SPINDLE_OVR_COARSE_PLUS)
        {
            last_s_override += SPINDLE_OVERRIDE_COARSE_INCREMENT;
        }
        if (rt_exec & EXEC_SPINDLE_OVR_COARSE_MINUS)
        {
            last_s_override -= SPINDLE_OVERRIDE_COARSE_INCREMENT;
        }
        if (rt_exec & EXEC_SPINDLE_OVR_FINE_PLUS)
        {
            last_s_override += SPINDLE_OVERRIDE_FINE_INCREMENT;
        }
        if (rt_exec & EXEC_SPINDLE_OVR_FINE_MINUS)
        {
            last_s_override -= SPINDLE_OVERRIDE_FINE_INCREMENT;
        }
        last_s_override = min(last_s_override, MAX_SPINDLE_SPEED_OVERRIDE);
        last_s_override = max(last_s_override, MIN_SPINDLE_SPEED_OVERRIDE);

        if (last_s_override != c_system::sys.spindle_speed_ovr)
        {
            bit_true(c_system::sys.step_control, STEP_CONTROL_UPDATE_SPINDLE_PWM);
            c_system::sys.spindle_speed_ovr = last_s_override;
            c_system::sys.report_ovr_counter = 0; // Set to report change immediately
        }

        if (rt_exec & EXEC_SPINDLE_OVR_STOP)
        {
            // Spindle stop override allowed only while in HOLD state.
            // NOTE: Report counters are set in spindle_set_state() when spindle stop is executed.
            if (c_system::sys.state == STATE_HOLD)
            {
                if (!(c_system::sys.spindle_stop_ovr))
                {
                    c_system::sys.spindle_stop_ovr = SPINDLE_STOP_OVR_INITIATE;
                }
                else if (c_system::sys.spindle_stop_ovr & SPINDLE_STOP_OVR_ENABLED)
                {
                    c_system::sys.spindle_stop_ovr |= SPINDLE_STOP_OVR_RESTORE;
                }
            }
        }

        // NOTE: Since coolant state always performs a planner sync whenever it changes, the current
        // run state can be determined by checking the parser state.
        if (rt_exec & (EXEC_COOLANT_FLOOD_OVR_TOGGLE | EXEC_COOLANT_MIST_OVR_TOGGLE))
        {
            if ((c_system::sys.state == STATE_IDLE) || (c_system::sys.state & (STATE_CYCLE | STATE_HOLD)))
            {
                uint8_t coolant_state = c_gcode::gc_state.modal.coolant;
                if (rt_exec & EXEC_COOLANT_MIST_OVR_TOGGLE)
                {
                    if (coolant_state & COOLANT_MIST_ENABLE)
                    {
                        bit_false(coolant_state, COOLANT_MIST_ENABLE);
                    }
                    else
                    {
                        coolant_state |= COOLANT_MIST_ENABLE;
                    }
                }
                if (rt_exec & EXEC_COOLANT_FLOOD_OVR_TOGGLE)
                {
                    if (coolant_state & COOLANT_FLOOD_ENABLE)
                    {
                        bit_false(coolant_state, COOLANT_FLOOD_ENABLE);
                    }
                    else
                    {
                        coolant_state |= COOLANT_FLOOD_ENABLE;
                    }
                }
                c_coolant::coolant_set_state(coolant_state); // Report counter set in coolant_set_state().
                c_gcode::gc_state.modal.coolant = coolant_state;
            }
        }
    }

//#ifdef DEBUG
    //if (c_system::sys_rt_exec_debug)
    //{
        //c_report::report_realtime_debug();
        //c_system::sys_rt_exec_debug = 0;
    //}
//#endif

    // Reload step segment buffer
    if (c_system::sys.state & (STATE_CYCLE | STATE_HOLD | STATE_SAFETY_DOOR | STATE_HOMING | STATE_SLEEP | STATE_JOG))
    {
        c_stepper::st_prep_buffer();
    }

}

// Handles Grbl system suspend procedures, such as feed hold, safety door, and parking motion.
// The system will enter this loop, create local variables for suspend tasks, and return to
// whatever function that invoked the suspend, such that Grbl resumes normal operation.
// This function is written in a way to promote custom parking motions. Simply use this as a
// template
void c_protocol::protocol_exec_rt_suspend()
{
#ifdef PARKING_ENABLE
    // Declare and initialize parking local variables
    float restore_target[N_AXIS];
    float parking_target[N_AXIS];
    float retract_waypoint = PARKING_PULLOUT_INCREMENT;
    c_planner::plan_line_data_t plan_data;
    c_planner::plan_line_data_t *pl_data = &plan_data;
    memset(pl_data,0,sizeof(c_planner::plan_line_data_t));
    pl_data->condition = (PL_COND_FLAG_SYSTEM_MOTION|PL_COND_FLAG_NO_FEED_OVERRIDE);
    pl_data->line_number = PARKING_MOTION_LINE_NUMBER;
#endif

    c_planner::plan_block_t *block = c_planner::plan_get_current_block();
    uint8_t restore_condition;
    float restore_spindle_speed;
    if (block == NULL)
    {
        restore_condition = (c_gcode::gc_state.modal.spindle | c_gcode::gc_state.modal.coolant);
        restore_spindle_speed = c_gcode::gc_state.spindle_speed;
    }
    else
    {
        restore_condition = block->condition;
        restore_spindle_speed = block->spindle_speed;
    }
#ifdef DISABLE_LASER_DURING_HOLD
    if (bit_istrue(settings.flags, BITFLAG_LASER_MODE))
    {
        c_system::system_set_exec_accessory_override_flag(EXEC_SPINDLE_OVR_STOP);
    }
#endif

    while (c_system::sys.suspend)
    {

        if (c_system::sys.abort)
        {
            return;
        }

        // Block until initial hold is complete and the machine has stopped motion.
        if (c_system::sys.suspend & SUSPEND_HOLD_COMPLETE)
        {

            // Parking manager. Handles de/re-energizing, switch state checks, and parking motions for
            // the safety door and sleep states.
            if (c_system::sys.state & (STATE_SAFETY_DOOR | STATE_SLEEP))
            {

                // Handles retraction motions and de-energizing.
                if (bit_isfalse(c_system::sys.suspend, SUSPEND_RETRACT_COMPLETE))
                {

                    // Ensure any prior spindle stop override is disabled at start of safety door routine.
                    c_system::sys.spindle_stop_ovr = SPINDLE_STOP_OVR_DISABLED;

#ifndef PARKING_ENABLE

                    c_spindle::spindle_set_state(SPINDLE_DISABLE, 0.0); // De-energize
                    c_coolant::coolant_set_state(COOLANT_DISABLE);     // De-energize

#else

                    // Get current position and store restore location and spindle retract waypoint.
                    c_system::system_convert_array_steps_to_mpos(parking_target,c_system::sys_position);
                    if (bit_isfalse(sys.suspend,SUSPEND_RESTART_RETRACT))
                    {
                        memcpy(restore_target,parking_target,sizeof(parking_target));
                        retract_waypoint += restore_target[PARKING_AXIS];
                        retract_waypoint = min(retract_waypoint,PARKING_TARGET);
                    }

                    // Execute slow pull-out parking retract motion. Parking requires homing enabled, the
                    // current location not exceeding the parking target location, and laser mode disabled.
                    // NOTE: State is will remain DOOR, until the de-energizing and retract is complete.
                    if ((bit_istrue(settings.flags,BITFLAG_HOMING_ENABLE)) &&
                        (parking_target[PARKING_AXIS] < PARKING_TARGET) &&
                        bit_isfalse(settings.flags,BITFLAG_LASER_MODE))
                    {

                        // Retract spindle by pullout distance. Ensure retraction motion moves away from
                        // the workpiece and waypoint motion doesn't exceed the parking target location.
                        if (parking_target[PARKING_AXIS] < retract_waypoint)
                        {
                            parking_target[PARKING_AXIS] = retract_waypoint;
                            pl_data->feed_rate = PARKING_PULLOUT_RATE;
                            pl_data->condition |= (restore_condition & PL_COND_ACCESSORY_MASK); // Retain accessory state
                            pl_data->spindle_speed = restore_spindle_speed;
                            mc_parking_motion(parking_target, pl_data);
                        }

                        // NOTE: Clear accessory state after retract and after an aborted restore motion.
                        pl_data->condition = (PL_COND_FLAG_SYSTEM_MOTION|PL_COND_FLAG_NO_FEED_OVERRIDE);
                        pl_data->spindle_speed = 0.0;
                        spindle_set_state(SPINDLE_DISABLE,0.0);// De-energize
                        coolant_set_state(COOLANT_DISABLE);// De-energize

                        // Execute fast parking retract motion to parking target location.
                        if (parking_target[PARKING_AXIS] < PARKING_TARGET)
                        {
                            parking_target[PARKING_AXIS] = PARKING_TARGET;
                            pl_data->feed_rate = PARKING_RATE;
                            mc_parking_motion(parking_target, pl_data);
                        }

                    }
                    else
                    {

                        // Parking motion not possible. Just disable the spindle and coolant.
                        // NOTE: Laser mode does not start a parking motion to ensure the laser stops immediately.
                        spindle_set_state(SPINDLE_DISABLE,0.0);// De-energize
                        coolant_set_state(COOLANT_DISABLE);// De-energize

                    }

#endif

                    c_system::sys.suspend &= ~(SUSPEND_RESTART_RETRACT);
                    c_system::sys.suspend |= SUSPEND_RETRACT_COMPLETE;

                }
                else
                {

                    if (c_system::sys.state == STATE_SLEEP)
                    {
                        c_report::report_feedback_message(MESSAGE_SLEEP_MODE);
                        // Spindle and coolant should already be stopped, but do it again just to be sure.
                        c_spindle::spindle_set_state(SPINDLE_DISABLE, 0.0); // De-energize
                        c_coolant::coolant_set_state(COOLANT_DISABLE); // De-energize
                        c_stepper::st_go_idle(); // Disable steppers
                        while (!(c_system::sys.abort))
                        {
                            protocol_exec_rt_system();
                        } // Do nothing until reset.
                        return; // Abort received. Return to re-initialize.
                    }

                    // Allows resuming from parking/safety door. Actively checks if safety door is closed and ready to resume.
                    if (c_system::sys.state == STATE_SAFETY_DOOR)
                    {
                        if (!(c_system::system_check_safety_door_ajar()))
                        {
                            c_system::sys.suspend &= ~(SUSPEND_SAFETY_DOOR_AJAR); // Reset door ajar flag to denote ready to resume.
                        }
                    }

                    // Handles parking restore and safety door resume.
                    if (c_system::sys.suspend & SUSPEND_INITIATE_RESTORE)
                    {

#ifdef PARKING_ENABLE
                        // Execute fast restore motion to the pull-out position. Parking requires homing enabled.
                        // NOTE: State is will remain DOOR, until the de-energizing and retract is complete.
                        if ((settings.flags & (BITFLAG_HOMING_ENABLE|BITFLAG_LASER_MODE)) == BITFLAG_HOMING_ENABLE)
                        {
                            // Check to ensure the motion doesn't move below pull-out position.
                            if (parking_target[PARKING_AXIS] <= PARKING_TARGET)
                            {
                                parking_target[PARKING_AXIS] = retract_waypoint;
                                pl_data->feed_rate = PARKING_RATE;
                                mc_parking_motion(parking_target, pl_data);
                            }
                        }
#endif

                        // Delayed Tasks: Restart spindle and coolant, delay to power-up, then resume cycle.
                        if (c_gcode::gc_state.modal.spindle != SPINDLE_DISABLE)
                        {
                            // Block if safety door re-opened during prior restore actions.
                            if (bit_isfalse(c_system::sys.suspend, SUSPEND_RESTART_RETRACT))
                            {
                                if (bit_istrue(c_settings::settings.flags, BITFLAG_LASER_MODE))
                                {
                                    // When in laser mode, ignore spindle spin-up delay. Set to turn on laser when cycle starts.
                                    bit_true(c_system::sys.step_control, STEP_CONTROL_UPDATE_SPINDLE_PWM);
                                }
                                else
                                {
                                    c_spindle::spindle_set_state((restore_condition & (PL_COND_FLAG_SPINDLE_CW | PL_COND_FLAG_SPINDLE_CCW)), restore_spindle_speed);
                                    utils::delay_sec(SAFETY_DOOR_SPINDLE_DELAY, DELAY_MODE_SYS_SUSPEND);
                                }
                            }
                        }
                        if (c_gcode::gc_state.modal.coolant != COOLANT_DISABLE)
                        {
                            // Block if safety door re-opened during prior restore actions.
                            if (bit_isfalse(c_system::sys.suspend, SUSPEND_RESTART_RETRACT))
                            {
                                // NOTE: Laser mode will honor this delay. An exhaust system is often controlled by this pin.
                                c_coolant::coolant_set_state((restore_condition & (PL_COND_FLAG_COOLANT_FLOOD | PL_COND_FLAG_COOLANT_FLOOD)));
                                utils::delay_sec(SAFETY_DOOR_COOLANT_DELAY, DELAY_MODE_SYS_SUSPEND);
                            }
                        }

#ifdef PARKING_ENABLE
                        // Execute slow plunge motion from pull-out position to resume position.
                        if ((c_settings::settings.flags & (BITFLAG_HOMING_ENABLE|BITFLAG_LASER_MODE)) == BITFLAG_HOMING_ENABLE)
                        {
                            // Block if safety door re-opened during prior restore actions.
                            if (bit_isfalse(sys.suspend,SUSPEND_RESTART_RETRACT))
                            {
                                // Regardless if the retract parking motion was a valid/safe motion or not, the
                                // restore parking motion should logically be valid, either by returning to the
                                // original position through valid machine space or by not moving at all.
                                pl_data->feed_rate = PARKING_PULLOUT_RATE;
                                pl_data->condition |= (restore_condition & PL_COND_ACCESSORY_MASK);// Restore accessory state
                                pl_data->spindle_speed = restore_spindle_speed;
                                mc_parking_motion(restore_target, pl_data);
                            }
                        }
#endif

                        if (bit_isfalse(c_system::sys.suspend, SUSPEND_RESTART_RETRACT))
                        {
                            c_system::sys.suspend |= SUSPEND_RESTORE_COMPLETE;
                            c_system::system_set_exec_state_flag(EXEC_CYCLE_START); // Set to resume program.
                        }
                    }

                }

            }
            else
            {

                // Feed hold manager. Controls spindle stop override states.
                // NOTE: Hold ensured as completed by condition check at the beginning of suspend routine.
                if (c_system::sys.spindle_stop_ovr)
                {
                    // Handles beginning of spindle stop
                    if (c_system::sys.spindle_stop_ovr & SPINDLE_STOP_OVR_INITIATE)
                    {
                        if (c_gcode::gc_state.modal.spindle != SPINDLE_DISABLE)
                        {
                            c_spindle::spindle_set_state(SPINDLE_DISABLE, 0.0); // De-energize
                            c_system::sys.spindle_stop_ovr = SPINDLE_STOP_OVR_ENABLED; // Set stop override state to enabled, if de-energized.
                        }
                        else
                        {
                            c_system::sys.spindle_stop_ovr = SPINDLE_STOP_OVR_DISABLED; // Clear stop override state
                        }
                        // Handles restoring of spindle state
                    }
                    else if (c_system::sys.spindle_stop_ovr & (SPINDLE_STOP_OVR_RESTORE | SPINDLE_STOP_OVR_RESTORE_CYCLE))
                    {
                        if (c_gcode::gc_state.modal.spindle != SPINDLE_DISABLE)
                        {
                            c_report::report_feedback_message(MESSAGE_SPINDLE_RESTORE);
                            if (bit_istrue(c_settings::settings.flags, BITFLAG_LASER_MODE))
                            {
                                // When in laser mode, ignore spindle spin-up delay. Set to turn on laser when cycle starts.
                                bit_true(c_system::sys.step_control, STEP_CONTROL_UPDATE_SPINDLE_PWM);
                            }
                            else
                            {
                                c_spindle::spindle_set_state((restore_condition & (PL_COND_FLAG_SPINDLE_CW | PL_COND_FLAG_SPINDLE_CCW)), restore_spindle_speed);
                            }
                        }
                        if (c_system::sys.spindle_stop_ovr & SPINDLE_STOP_OVR_RESTORE_CYCLE)
                        {
                            c_system::system_set_exec_state_flag(EXEC_CYCLE_START);  // Set to resume program.
                        }
                        c_system::sys.spindle_stop_ovr = SPINDLE_STOP_OVR_DISABLED; // Clear stop override state
                    }
                }
                else
                {
                    // Handles spindle state during hold. NOTE: Spindle speed overrides may be altered during hold state.
                    // NOTE: STEP_CONTROL_UPDATE_SPINDLE_PWM is automatically reset upon resume in step generator.
                    if (bit_istrue(c_system::sys.step_control, STEP_CONTROL_UPDATE_SPINDLE_PWM))
                    {
                        c_spindle::spindle_set_state((restore_condition & (PL_COND_FLAG_SPINDLE_CW | PL_COND_FLAG_SPINDLE_CCW)), restore_spindle_speed);
                        bit_false(c_system::sys.step_control, STEP_CONTROL_UPDATE_SPINDLE_PWM);
                    }
                }

            }
        }

//#ifdef SLEEP_ENABLE
        //// Check for sleep conditions and execute auto-park, if timeout duration elapses.
        //// Sleep is valid for both hold and door states, if the spindle or coolant are on or
        //// set to be re-enabled.
        //sleep_check();
//#endif

        protocol_exec_rt_system();

    }
}
