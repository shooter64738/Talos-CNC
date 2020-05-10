/*
*  c_core_avr_328.cpp - NGC_RS274 controller.
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
#include "c_core_stm32h754.h"
using namespace Hardware_Abstraction_Layer;


#include "../../../../Coordinator/coordinator_hardware_def.h"

void Core::critical_shutdown()
{
	//This is the win core. 
}

void Core::set_time_delay(uint8_t delay)
{

}

uint8_t Core::initialize()
{
	//This is the win core. 
	return 0;
}
uint8_t Core::start_interrupts()
{
	return 0;
}
void stop_interrupts()
{
	
}
uint32_t Core::get_cpu_clock_rate()
{
	return F_CPU;
}

void Core::capture_status_register_SREG()
{
	//Hardware_Abstraction_Layer::Core::register_at_int_stop = SREG;
}

void Core::restore_status_register_SREG()
{
	//SREG = Hardware_Abstraction_Layer::Core::register_at_int_stop;
}

void Core::delay_ms(uint16_t delay_time)
{

}
void Core::delay_us(uint16_t delay_time)
{

}

