#include <string.h>
#include "c_serial.h"
#include "c_settings.h"
#include "c_stepper.h"
#include "c_system.h"
//#include <avr/interrupt.h>
#include "c_gcode.h"
#include "c_spindle.h"
#include "c_coolant.h"
#include "c_limits.h"
#include "c_probe.h"
#include "c_planner.h"
#include "c_report.h"
#include "c_protocol.h"
//#include "..\Common\Hardware_Abstraction_Layer\AVR_2560\c_grbl_avr_2560_limits.h"
//#include "..\Common\Hardware_Abstraction_Layer\AVR_2560\c_core_avr_2560.h"
#include "hardware_def.h"
//#include "..\Common\Hardware_Abstraction_Layer\AVR_2560\c_grbl_avr_2560_spindle.h"
/*
 * AVR_2560_GRBL.cpp
 *
 * Created: 3/5/2019 2:53:22 PM
 * Author : jeff_d
 */ 


int main(void)
{
	// Initialize system upon power-up.
	c_serial::serial_init();   // Setup serial baud rate and interrupts
	c_settings::settings_init(); // Load Grbl settings from EEPROM
	c_stepper::stepper_init();  // Configure stepper pins and interrupt timers
	c_system::system_init();   // Configure pinout pins and pin-change interrupt

	memset(c_system::sys_position, 0, sizeof(c_system::sys_position)); // Clear machine position.
	//sei();
	Hardware_Abstraction_Layer::Core::initialize();
	Hardware_Abstraction_Layer::Core::start_interrupts();
	
	// Enable interrupts

	// Initialize system state.
	#ifdef FORCE_INITIALIZATION_ALARM
	// Force Grbl into an ALARM state upon a power-cycle or hard reset.
	c_system::sys.state = STATE_ALARM;
	#else
	c_system::sys.state = STATE_IDLE;
	#endif

	// Check for power-up and set system alarm if homing is enabled to force homing cycle
	// by setting Grbl's alarm state. Alarm locks out all g-code commands, including the
	// startup scripts, but allows access to settings and internal commands. Only a homing
	// cycle '$H' or kill alarm locks '$X' will disable the alarm.
	// NOTE: The startup script will run after successful completion of the homing cycle, but
	// not after disabling the alarm locks. Prevents motion startup blocks from crashing into
	// things uncontrollably. Very bad.
	#ifdef HOMING_INIT_LOCK
	if (bit_istrue(settings.flags, BITFLAG_HOMING_ENABLE))
	{
		c_system::sys.state = STATE_ALARM;
	}
	#endif

	// Grbl initialization loop upon power-up or a system abort. For the latter, all processes
	// will return to this loop to be cleanly re-initialized.
	for (;;)
	{

		// Reset system variables.
		uint8_t prior_state = c_system::sys.state;
		memset(&c_system::sys, 0, sizeof(c_system::system_t)); // Clear system struct variable.
		c_system::sys.state = prior_state;
		c_system::sys.f_override = DEFAULT_FEED_OVERRIDE;  // Set to 100%
		c_system::sys.r_override = DEFAULT_RAPID_OVERRIDE; // Set to 100%
		c_system::sys.spindle_speed_ovr = DEFAULT_SPINDLE_SPEED_OVERRIDE; // Set to 100%
		memset(c_system::sys_probe_position, 0, sizeof(c_system::sys_probe_position)); // Clear probe position.
		c_system::sys_probe_state = 0;
		c_system::sys_rt_exec_state = 0;
		c_system::sys_rt_exec_alarm = 0;
		c_system::sys_rt_exec_motion_override = 0;
		c_system::sys_rt_exec_accessory_override = 0;

		// Reset Grbl primary systems.
		c_serial::serial_reset_read_buffer(); // Clear serial read buffer
		c_gcode::gc_init(); // Set g-code parser to default state
		c_spindle::spindle_init();
		c_coolant::coolant_init();
		//c_limits::limits_init();
		Hardware_Abstraction_Layer::Grbl::Limits::initialize();
		c_probe::probe_init();
		//sleep_init();
		c_planner::plan_reset(); // Clear block buffer and planner variables
		c_stepper::st_reset(); // Clear stepper subsystem variables.

		// Sync cleared gcode and planner positions to current system position.
		c_planner::plan_sync_position();
		c_gcode::gc_sync_position();

		// Print welcome message. Indicates an initialization has occured at power-up or with a reset.
		c_report::report_init_message();

		// Start Grbl main loop. Processes program inputs and executes them.
		c_protocol::protocol_main_loop();

	}
	return 0; /* Never reached */
}

