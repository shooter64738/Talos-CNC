/*
*  c_hal.cpp - NGC_RS274 controller.
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

#include "c_hal.h"
#include "..\..\talos.h"
#include "..\..\Coordinator\Shared\Settings\c_general.h"

#ifdef CONTROL_TYPE_SPINDLE
#include "AVR_328\spindle_avr_328.h"
#include "VIRTUAL\control_type_spindle.h"
using namespace Hardware_Abstraction_Layer;
#endif

#ifdef CONTROL_TYPE_PLASMA_THC
#include "AVR_328\control_type_thc.h"
//#include "VIRTUAL\control_type_thc.h"
#endif
#ifdef CONTROL_TYPE_COORDINATOR
#include "AVR_2560\control_type_coordinator.h"
//#include "VIRTUAL\control_type_coordinator.h"
#endif

#ifdef MSVC
//special include here, just so i can start threads and mimic encoder inputs for the spindle control
#include "..\..\Spindle\c_encoder.h"
#include <thread>
#endif // MSVC


/*
This might be confusing to some, especially if you are not familiar with function pointers.
1. The function pointers in the HAL (hardware abstraction layer) are assigned to ACTUAL functions
in the respective CPU class.
2. When a call is needed to be made into the hardware you simply use c_hal::<module>.<method name>
3. In order to do that you will need to include the proper cpu file below.
*/

//c_hal::s_driver_function_pointers c_hal::driver;
//c_hal::s_core_function_pointers c_hal::core;
//c_hal::s_comm_function_pointers c_hal::comm;
c_hal::s_lcd_function_pointers c_hal::lcd;
c_hal::s_storage_function_pointers c_hal::storage;


void c_hal::initialize()
{
	#ifdef __AVR_ATmega328P__
	//Functions called from program->hal
	//c_hal::core.PNTR_INITIALIZE = &c_cpu_AVR_328::core_initializer;
	//c_hal::core.PNTR_START_INTERRUPTS = &c_cpu_AVR_328::core_start_interrupts;
	//c_hal::core.PNTR_STOP_INTERRUPTS = &c_cpu_AVR_328::core_stop_interrupts;
	//c_hal::core.PNTR_GET_CPU_SPEED = &c_cpu_AVR_328::core_get_cpu_clock_rate;

	//c_hal::comm.PNTR_INITIALIZE = &c_cpu_AVR_328::serial_initializer;
	//c_hal::comm.PNTR_SERIAL_TX = &c_cpu_AVR_328::serial_send;
	//c_hal::comm.PNTR_SERIAL_RX_BUFFER = c_cpu_AVR_328::rxBuffer;
	//Depending on the control_type defined in talos.h, this will call a different initializer.
	
	//control_type::initialize();
	
	
	#endif

	#ifdef __AVR_ATmega2560__

	//c_hal::core.PNTR_BASE = &cpu_AVR_2560::initialize;
	c_hal::core.PNTR_INITIALIZE = &c_cpu_AVR_2560::core_initializer;
	c_hal::core.PNTR_START_INTERRUPTS = &c_cpu_AVR_2560::core_start_interrupts;
	c_hal::core.PNTR_STOP_INTERRUPTS = &c_cpu_AVR_2560::core_stop_interrupts;
	c_hal::core.PNTR_GET_CPU_SPEED = &c_cpu_AVR_2560::core_get_cpu_clock_rate;

	//map the function pointers in c_hal, to the functions in c_driver for the board
	//c_hal::driver.PNTR_INITIALIZE = &c_cpu_AVR_2560::driver_timer_initializer;
	//c_hal::driver.PNTR_ENABLE = &c_cpu_AVR_2560::driver_timer_activate;
	//c_hal::driver.PNTR_DISABLE = &c_cpu_AVR_2560::driver_timer_deactivate;
	//c_hal::driver.PNTR_DRIVE = &c_cpu_AVR_2560::driver_drive;
	//c_hal::driver.PNTR_SET_PRESCALER = &c_cpu_AVR_2560::driver_set_prescaler;
	//c_hal::driver.PNTR_SET_TIMER_RATE = &c_cpu_AVR_2560::driver_dset_timer_rate;
	//c_hal::driver.PNTR_CONFIGURE_STEPPER = &c_cpu_AVR_2560::driver_configure_step_data;

	c_hal::comm.PNTR_INITIALIZE = &c_cpu_AVR_2560::serial_initializer;
	c_hal::comm.PNTR_SERIAL_TX = &c_cpu_AVR_2560::serial_send;
	c_hal::comm.PNTR_SERIAL_RX_BUFFER = c_cpu_AVR_2560::rxBuffer;

	//c_hal::feedback.PNTR_INITIALIZE = &c_cpu_AVR_2560::feedback_initializer;
	//c_hal::feedback.PNTR_IS_DIRTY = &c_cpu_AVR_2560::feedback_dirty;
	//c_hal::feedback.PNTR_POSITION_DATA = c_cpu_AVR_2560::Axis_Positions;
	//These are default function mappings, but will/can be reconfigured depending on machine type.
	//c_cpu_AVR_2560::PNTR_INTERNAL_PCINT2 = &c_cpu_AVR_2560::feedback_pulse_isr;
	//c_cpu_AVR_2560::PNTR_INTERNAL_PCINT0 = &c_cpu_AVR_2560::feedback_direction_isr;

	c_hal::lcd.PNTR_UPDATE_AXIS_DISPLAY = c_cpu_AVR_2560::lcd_update_axis;
	c_hal::lcd.PNTR_INITIALIZE = c_cpu_AVR_2560::lcd_initializer;

	c_hal::storage.PNTR_GET_BYTE = &c_cpu_AVR_2560::eeprom_get_byte;
	c_hal::storage.PNTR_GET_WORD = &c_cpu_AVR_2560::eeprom_get_word;
	c_hal::storage.PNTR_GET_DWORD = &c_cpu_AVR_2560::eeprom_get_dword;
	c_hal::storage.PNTR_GET_FLOAT = &c_cpu_AVR_2560::eeprom_get_float;

	control_type::initialize();
	#endif

	#ifdef __SAM3X8E__

	//c_hal::core.PNTR_BASE = &c_cpu_ARM_SAM3X8E::initialize;
	//c_hal::core.PNTR_INITIALIZE = &c_cpu_ARM_SAM3X8E::core_initializer;
	//c_hal::core.PNTR_START_INTERRUPTS = &c_cpu_ARM_SAM3X8E::core_start_interrupts;
	//c_hal::core.PNTR_STOP_INTERRUPTS = &c_cpu_ARM_SAM3X8E::core_stop_interrupts;
	//c_hal::core.PNTR_GET_CPU_SPEED = &c_cpu_ARM_SAM3X8E::core_get_cpu_clock_rate;
	//
	////map the function pointers in c_hal, to the functions in c_driver for the board
	//c_hal::driver.PNTR_INITIALIZE = &c_cpu_ARM_SAM3X8E::driver_timer_initializer;
	//c_hal::driver.PNTR_ENABLE = &c_cpu_ARM_SAM3X8E::driver_timer_activate;
	//c_hal::driver.PNTR_DISABLE = &c_cpu_ARM_SAM3X8E::driver_timer_deactivate;
	//c_hal::driver.PNTR_DRIVE = &c_cpu_ARM_SAM3X8E::driver_drive;

	c_hal::comm.PNTR_INITIALIZE = &c_cpu_ARM_SAM3X8E::serial_initializer;
	c_hal::comm.PNTR_SERIAL_TX = &c_cpu_ARM_SAM3X8E::serial_send;
	c_hal::comm.PNTR_SERIAL_RX_BUFFER = c_cpu_ARM_SAM3X8E::rxBuffer;

	c_hal::feedback.PNTR_INITIALIZE = &c_cpu_ARM_SAM3X8E::feedback_initializer;
	//c_hal::feedback.PNTR_IS_DIRTY = &c_cpu_ARM_SAM3X8E::feedback_dirty;
	c_hal::feedback.PNTR_POSITION_DATA = c_cpu_ARM_SAM3X8E::Axis_Positions;
	#endif

	#ifdef MSVC

	//c_hal::core.PNTR_BASE = &cpu_AVR_2560::initialize;
	//c_hal::core.PNTR_INITIALIZE = &c_cpu_VIRTUAL::core_initializer;
	//c_hal::core.PNTR_START_INTERRUPTS = &c_cpu_VIRTUAL::core_start_interrupts;
	//c_hal::core.PNTR_STOP_INTERRUPTS = &c_cpu_VIRTUAL::core_stop_interrupts;
	//c_hal::core.PNTR_GET_CPU_SPEED = &c_cpu_VIRTUAL::core_get_cpu_clock_rate;

	////map the function pointers in c_hal, to the functions in c_driver for the board
	//c_hal::driver.PNTR_INITIALIZE = &c_cpu_VIRTUAL::driver_timer_initializer;
	//c_hal::driver.PNTR_ENABLE = &c_cpu_VIRTUAL::driver_timer_activate;
	//c_hal::driver.PNTR_DISABLE = &c_cpu_VIRTUAL::driver_timer_deactivate;
	c_hal::driver.PNTR_DRIVE = &c_cpu_VIRTUAL::driver_drive;

	//c_hal::comm.PNTR_INITIALIZE = &c_cpu_VIRTUAL::serial_initializer;
	c_hal::comm.PNTR_SERIAL_TX = &c_cpu_VIRTUAL::serial_send;
	c_hal::comm.PNTR_SERIAL_RX_BUFFER = c_cpu_VIRTUAL::rxBuffer;
	c_hal::comm.PNTR_VIRTUAL_BUFFER_WRITE = &c_cpu_VIRTUAL::add_to_buffer;

	c_hal::storage.PNTR_GET_BYTE = &c_cpu_VIRTUAL::eeprom_get_byte;

	control_type::initialize();
	//simulate timers for easier debug
	//std::thread timer1_capture(control_type::isr_threads::TIMER1_CAPT_vect);
	std::thread timer1_overflow(control_type::isr_threads::TIMER1_OVF_vect);
	//timer1_capture.detach();
	timer1_overflow.detach();
	
	#endif

}




