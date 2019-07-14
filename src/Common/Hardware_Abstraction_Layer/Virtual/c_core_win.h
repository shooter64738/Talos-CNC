
/*
*  c_core_avr_328.h - NGC_RS274 controller.
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
#include "../../../Talos.h"
#ifdef MSVC//<--This will stop the multiple ISR definition error

#ifndef __C_CORE_WIN_H__
#define __C_CORE_WIN_H__
#include <stdint.h>
#define F_CPU 18*1000000*1000000

#define Hardware_Abstraction_Layer_Core_pgm_read_byte_near(x) ((int)x)
#define Hardware_Abstraction_Layer_Core_PSTR(x) ((const char*)x) //PSTR

namespace Hardware_Abstraction_Layer
{
	class Core
	{
		//variables
		public:
		protected:
		private:

		//functions
		public:
		static void initialize();
		static void critical_shutdown();
		static void start_interrupts();
		static void stop_interrupts();
		static void capture_status_register_SREG();
		static void restore_status_register_SREG();
		static uint32_t get_cpu_clock_rate();
		
		static void delay_ms(uint16_t delay_time);
		static void delay_us(uint16_t delay_time);

		protected:
		private:
	};
};
#endif //__C_CORE_WIN_H__
#endif
