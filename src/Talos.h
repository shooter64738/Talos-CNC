 /*
*  Talos.h - NGC_RS274 controller.
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


#ifndef TALOS_H_
#define TALOS_H_
//To enable compilation and debugging in Microsoft Visual C++ define MSCV
#define MSVC

/*
Setting this will determine how the HAL compiles, and what control type it will control.
In each HAL project there is an #ifdef for the control type. Within each control type
there is an additional #ifdef for the processor. This prevents compilation for any other
machine type and processor combination. More control types are planned in the future. 
To set compilation for a particular machine type, uncomment the define
*/
//ONLY ONE CONTROL TYPE CAN BE DEFINED FOR EACH COMPILE
#define CONTROL_TYPE_SPINDLE
//#define CONTROL_TYPE_LATHE_FLAT
//#define CONTROL_TYPE_LATHE_SLANT
//#define CONTROL_TYPE_MILL_RF45
//#define CONTROL_TYPE_MILL_GANTRY
//#define CONTROL_TYPE_MILL_ROUTER
//#define CONTROL_TYPE_MILL_LASER
//#define CONTROL_TYPE_PLASMA
//#define CONTROL_TYPE_EDM_WIRE
//#define CONTROL_TYPE_EDM_PLUNGE
//#define CONTROL_TYPE_STOCK_FEEDER
//#define CONTROL_TYPE_TOOL_CHANGER
//#define CONTROL_TYPE_GRINDER
//#define CONTROL_TYPE_TORCH_HEIGHT


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#define MM_CONVERSION 25.40001
//How many com ports are in use? (sets buffer array in CPU)
#define COM_PORT_COUNT 3
//How many words can a block contain
#define COUNT_OF_BLOCK_WORDS_ARRAY 26
//How many elements are in the G code group array
#define COUNT_OF_G_CODE_GROUPS_ARRAY 16
//How many elements are in the M code group array
#define COUNT_OF_M_CODE_GROUPS_ARRAY 6
//How many tool offset are in the tool table. (Note tool 0 is used when P value is sent)
#define COUNT_OF_TOOL_TABLE 10

#define G_CODE_MULTIPLIER 10
#define NGC_BUFFER_SIZE 8 //<--must be at least 2 in order for look-ahead to work MUST BE A 2'S COMPLIMENT SIZE!!!

//Look-ahead is required for cuter radius comp
#define MACHINE_AXIS_COUNT 8
#define MACHINE_X_AXIS 0
#define MACHINE_Y_AXIS 1
#define MACHINE_Z_AXIS 2
#define MACHINE_A_AXIS 3
#define MACHINE_B_AXIS 4
#define MACHINE_C_AXIS 5
#define MACHINE_U_AXIS 6
#define MACHINE_V_AXIS 7

#define CR '\r' //<--default line ending value

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

//#define bit_get(p,m) ((p) & (m))
//#define bit_set(p,m) ((p) |= (m))
//#define bit_clear(p,m) ((p) &= ~(m))
//#define bit_flip(p,m) ((p) ^= (m))
//#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
//#define BIT(x) (1 << (x))
//#define LONGBIT(x) ((unsigned long)0x00000001 << (x))


#include "common/NGC_RS274/NGC_G_Groups.h"
#include "common/NGC_RS274/NGC_M_Groups.h"
#include "common/NGC_RS274/NGC_G_Codes.h"
#include "common/NGC_RS274/NGC_M_Codes.h"
#include "common/NGC_RS274/NGC_Errors.h"
#include "common/NGC_RS274/NGC_Parameters.h"
//#include "Serial/c_Serial.h"
#include "helpers.h"
#include "numeric_converters.h"

/*
These defines are used in the motion driver project. Only modify these for
 the motion driver, NOT the coordinator or spindle driver
*/

#define MOTION_DRIVE_BUFFER_SIZE 20

// Speed ramp states
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3
#endif /* TALOS_H_*/
