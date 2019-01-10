/*
*  c_grbl_settings.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2018 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdint.h>

#ifndef C_GRBL_SETTINGS_H_
#define C_GRBL_SETTINGS_H_
class c_GRBL_Settings
{
	public:
	//static const uint8_t _step_pulse_length=0;//	Step pulse, microseconds
	//static const uint8_t _step_idle_delay=1;//	Step idle delay, milliseconds
	//static const uint8_t _step_port_invert=2; //$2=0	Step port invert, mask
	//static const uint8_t _directionport_invert=3; //$3=0	Direction port invert, mask
	//static const uint8_t _step_enable_invert=4; //$4=0	Step enable invert, boolean
	//static const uint8_t _limit_pin_invert=5; //$5=0	Limit pins invert, boolean
	//static const uint8_t _probe_pin_invert=6; //$6=0	Probe pin invert, boolean
	//static const uint8_t _status_report_mask=10; //$10=1	Status report, mask
	//static const uint8_t _junction_deviation=11; //$11=0.010	Junction deviation, mm
	//static const uint8_t _arc_tolerance=12; //$12=0.002	Arc tolerance, mm
	//static const uint8_t _report_inches_bool=13; //$13=0	Report inches, boolean
	//static const uint8_t _soft_limits_bool=20; //$20=0	Soft limits, boolean
	//static const uint8_t _hard_limits_bool=21; //$21=0	Hard limits, boolean
	//static const uint8_t _homing_cycle_bool=22; //$22=1	Homing cycle, boolean
	//static const uint8_t _homing_direction_invert_mask=23; //$23=0	Homing dir invert, mask
	//static const uint8_t _homing_feed_speed=24; //$24=25.000	Homing feed, mm/min
	//static const uint8_t _homing_seek_speed=25; //$25=500.000	Homing seek, mm/min
	//static const uint8_t _homing_debounce=26; //$26=250	Homing debounce, milliseconds
	//static const uint8_t _homing_pull_off=27; //$27=1.000	Homing pull-off, mm
	//static const uint8_t _max_spindle_speed=30; //$30=1000.	Max spindle speed, RPM
	//static const uint8_t _min_spindle_speed=31; //$31=0.	Min spindle speed, RPM
	//static const uint8_t _laser_mode_boolean=32; //$32=0	Laser mode, boolean
	static const uint8_t _x_steps_per_mm=100; //$100=250.000	X steps/mm
	static const uint8_t _y_steps_per_mm=101; //$101=250.000	Y steps/mm
	static const uint8_t _z_steps_per_mm=102; //$102=250.000	Z steps/mm
	static const uint8_t _a_steps_per_mm=103; //$103=250.000	A steps/mm
	static const uint8_t _b_steps_per_mm=104; //$104=250.000	B steps/mm
	static const uint8_t _c_steps_per_mm=105; //$105=250.000	C steps/mm
	static const uint8_t _u_steps_per_mm=106; //$105=250.000	C steps/mm
	static const uint8_t _v_steps_per_mm=107; //$105=250.000	C steps/mm
	//static const uint8_t _x_max_speed=110; //$110=500.000	X Max rate, mm/min
	//static const uint8_t _y_max_speed=111; //$111=500.000	Y Max rate, mm/min
	//static const uint8_t _z_max_speed=112; //$112=500.000	Z Max rate, mm/min
	//static const uint8_t _a_max_speed=113; //$113=500.000	A Max rate, mm/min
	//static const uint8_t _b_max_speed=114; //$114=500.000	B Max rate, mm/min
	//static const uint8_t _c_max_speed=115; //$115=500.000	C Max rate, mm/min
	//static const uint8_t _x_acceleration=120; //$120=10.000	X Acceleration, mm/sec^2
	//static const uint8_t _y_acceleration=121; //$121=10.000	Y Acceleration, mm/sec^2
	//static const uint8_t _z_acceleration=122; //$122=10.000	Z Acceleration, mm/sec^2
	//static const uint8_t _a_acceleration=123; //$123=10.000	A Acceleration, mm/sec^2
	//static const uint8_t _b_acceleration=124; //$124=10.000	B Acceleration, mm/sec^2
	//static const uint8_t _c_acceleration=125; //$125=10.000	C Acceleration, mm/sec^2
	static const uint8_t _x_max_travel=130; //$130=200.000	X Max travel, mm
	static const uint8_t _y_max_travel=131; //$131=200.000	Y Max travel, mm
	static const uint8_t _z_max_travel=132; //$132=200.000	Z Max travel, mm
	static const uint8_t _a_max_travel=133; //$133=200.000	A Max travel, mm
	static const uint8_t _b_max_travel=134; //$134=200.000	B Max travel, mm
	static const uint8_t _c_max_travel=135; //$135=200.000	C Max travel, mm
	static const uint8_t _u_max_travel=136; //$135=200.000	C Max travel, mm
	static const uint8_t _v_max_travel=137; //$135=200.000	C Max travel, mm

};

#endif /* C_GRBL_SETTINGS_H_ */
/*
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
#define X_DIRECTION_BIT   5 // MEGA2560 Digital Pin 37
#define Y_DIRECTION_BIT   4 // MEGA2560 Digital Pin 36
#define Z_DIRECTION_BIT   3 // MEGA2560 Digital Pin 35
#define A_DIRECTION_BIT   2 // MEGA2560 Digital Pin 34
#define B_DIRECTION_BIT   1 // MEGA2560 Digital Pin 33
#define C_DIRECTION_BIT   0 // MEGA2560 Digital Pin 32
#define DIRECTION_MASK ((1 << X_DIRECTION_BIT) | (1 << Y_DIRECTION_BIT) | (1 << Z_DIRECTION_BIT) | (1 << A_DIRECTION_BIT) | (1 << B_DIRECTION_BIT) | (1 << C_DIRECTION_BIT)) // All direction bits

// Define stepper driver enable/disable output pin.
#define STEPPERS_DISABLE_DDR   DDRB
#define STEPPERS_DISABLE_PORT  PORTB
#define STEPPERS_DISABLE_BIT   7 // MEGA2560 Digital Pin 13
#define STEPPERS_DISABLE_MASK (1 << STEPPERS_DISABLE_BIT)
*/