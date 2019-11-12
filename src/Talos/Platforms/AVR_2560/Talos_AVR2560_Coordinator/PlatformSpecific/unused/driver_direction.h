/*
*  driver_direction.h - NGC_RS274 controller.
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


#ifndef DRIVER_DIRECTION_H_
#define DRIVER_DIRECTION_H_

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

class driver_direction
{

};



#endif /* DRIVER_DIRECTION_H_ */