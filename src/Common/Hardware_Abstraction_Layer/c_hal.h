/*
*  c_hal.h - NGC_RS274 controller.
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


#ifndef __C_HAL_H__
#define __C_HAL_H__
#include "../../std_types.h"
#include "../../talos.h"
#include "../Serial/s_Buffer.h"
#ifdef __AVR_ATmega2560__
#include "AVR_2560\c_cpu_AVR_2560.h"
#endif
#ifdef MSVC
#include "Virtual/c_cpu_VIRTUAL.h"
#endif
#ifdef __SAM3X8E__
#include "ARM_SAM3X8E/c_cpu_ARM_SAM3X8E.h"
#endif

class c_hal
{
	////variables
	//public:
	//// Step and direction port invert masks.
	
	//functions
	
	public:
	static void initialize();
	//core struct refers to actions that give basic function
	typedef struct
	{
		void (*PNTR_INITIALIZE)(void);
		void (*PNTR_START_INTERRUPTS)(void);
		void (*PNTR_STOP_INTERRUPTS)(void);
		void (*PNTR_BASE)(void);		
		uint32_t (*PNTR_GET_CPU_SPEED)(void);
	}s_core_function_pointers;
	static s_core_function_pointers core;
	
	typedef struct
	{
		void (*PNTR_INITIALIZE)(uint8_t, uint32_t);
		void (*PNTR_BASE)(void);
		void (*PNTR_VIRTUAL_BUFFER_WRITE)(uint8_t, const char*);
		void (*PNTR_SERIAL_RX)(void);
		void (*PNTR_SERIAL_TX)(uint8_t , char);
		void (*PNTR_I2C_RX)(void);
		void (*PNTR_I2C_TX)(void);
		s_Buffer *PNTR_SERIAL_RX_BUFFER;
		
	}s_comm_function_pointers;
	static s_comm_function_pointers comm;	
	
	//driver struct refers to actions to run stepper or servo drivers
	typedef struct
	{
		void (*PNTR_INITIALIZE)(void);
		void (*PNTR_ENABLE)(void);
		void (*PNTR_DISABLE)(void);
		void (*PNTR_DRIVE)(void);
		void (*PNTR_RESET)(void);
		void(*PNTR_SET_PRESCALER)(uint16_t);
		void(*PNTR_SET_TIMER_RATE)(uint16_t);
		
	}s_driver_function_pointers;
	static s_driver_function_pointers driver;

	//feedback struct refers to actions to implement closed loop control
	typedef struct
	{
		void (*PNTR_INITIALIZE)(void);
		void (*PNTR_ENABLE)(void);
		void (*PNTR_DISABLE)(void);
		bool (*PNTR_IS_DIRTY)(void);
		void (*PNTR_PULSE_ISR)(void);
		int32_t *PNTR_POSITION_DATA;
		//void (*PNTR_RESET)(void);
		
	}s_feedback_function_pointers;
	static s_feedback_function_pointers feedback;
	
	//lcd struct refers to anything related to lcd display
	typedef struct
	{
		void (*PNTR_INITIALIZE)(void);
		void (*PNTR_ENABLE)(void);
		void (*PNTR_DISABLE)(void);
		void (*PNTR_UPDATE_AXIS_DISPLAY)(uint8_t,float);
	}s_lcd_function_pointers;
	static s_lcd_function_pointers lcd;
	
	public:
	protected:
	private:
	//c_hal( const c_hal &c );
	//c_hal& operator=( const c_hal &c );
	//c_hal();
	//~c_hal();

}; //c_hal

#endif //__C_HAL_H__
