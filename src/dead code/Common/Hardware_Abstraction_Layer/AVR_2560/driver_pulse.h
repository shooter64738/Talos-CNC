/*
*  driver_pulse.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
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


#ifndef DRIVER_PULSE_H_
#define DRIVER_PULSE_H_

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


#include "../../../std_types.h"
//#include "../talos.h"


class driver_pulse //converted from stepper_t
{
	public:
	
	//static c_motion_segment *exec_block;   // Pointer to the block data for the segment being executed
	private:
	
	driver_pulse( const driver_pulse &c );
	driver_pulse& operator=( const driver_pulse &c );
	driver_pulse();
	~driver_pulse();
};



#endif /* DRIVER_PULSE_H_ */