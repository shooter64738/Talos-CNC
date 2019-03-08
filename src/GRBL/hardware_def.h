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
#include "../Common/Hardware_Abstraction_Layer/AVR_328/c_core_avr_328.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_328/c_serial_avr_328.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_328/c_eeprom_avr_328.h"
#endif

#ifdef __AVR_ATmega2560__
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_core_avr_2560.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_serial_avr_2560.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560_control.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560_spindle.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560_limits.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_eeprom_avr_2560.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560_probe.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560_stepper.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560_spindle.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560_probe.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560_limits.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560_limits.h"
#include "../Common/Hardware_Abstraction_Layer/AVR_2560/c_grbl_avr_2560_coolant.h"
#endif

#ifdef __SAM3X8E__
#include "../Common/Hardware_Abstraction_Layer/ARM_SAM3X8E/c_core_arm_3x8e.h"
#endif

#ifdef MSVC
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_grbl_win.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_core_win.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_serial_win.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_grbl_win_control.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_grbl_win_spindle.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_grbl_win_limits.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_eeprom_win.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_grbl_win_probe.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_grbl_win_stepper.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_grbl_win_spindle.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_grbl_win_probe.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_grbl_win_limits.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_grbl_win_limits.h"
#include "../Common/Hardware_Abstraction_Layer/Virtual/c_grbl_win_coolant.h"
#endif

#endif /* HARDWARE_DEF_H_ */