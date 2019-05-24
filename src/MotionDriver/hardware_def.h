/*
*  hardware_def.h - NGC_RS274 controller.
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



#ifndef HARDWARE_DEF_H_
#define HARDWARE_DEF_H_
#include "../Talos.h"

#ifdef __AVR_ATmega328P__
//#include "../Common/Hardware_Abstraction_Layer/AVR_328/c_core_avr_328.h"
//#include "../Common/Hardware_Abstraction_Layer/AVR_328/c_serial_avr_328.h"
//#include "../Common/Hardware_Abstraction_Layer/AVR_328/c_eeprom_avr_328.h"
//#include "../Common/Serial/c_Serial.h"
#endif

#ifdef __AVR_ATmega2560__
#define BINARY_SERIAL_CYCLE_TIMEOUT 1
#define F_CPU 16000000UL
#include "..\Common\Hardware_Abstraction_Layer\AVR_2560\c_serial_avr_2560.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_core_avr_2560.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_motion_core_avr_2560_stepper.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_eeprom_avr_2560.h"
#include "../Common/Serial/c_Serial.h"
#endif

#ifdef __SAM3X8E__
#define BINARY_SERIAL_CYCLE_TIMEOUT 1000
#define F_CPU 84000000UL
#include "../Common/Hardware_Abstraction_Layer/ARM_SAM3X8E/c_core_arm_3x8e.h"
#include "../Common/Hardware_Abstraction_Layer/ARM_SAM3X8E/c_motion_core_arm_3x8e_stepper.h"
#include "../Common/Hardware_Abstraction_Layer/ARM_SAM3X8E/c_serial_arm_3x8e.h"
#include "../Common/Serial/c_Serial.h"
#endif

#ifdef MSVC
#define xF_CPU 16000000
#include "../Common/Serial/c_Serial.h"
#include "../Common/Hardware_Abstraction_Layer\Virtual/c_core_win.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual\c_motion_core_win_stepper.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_serial_win.h"
#endif

#endif /* HARDWARE_DEF_H_ */