/*
*  c_spindle.h - NGC_RS274 controller.
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


#ifndef __C_SPINDLE_H__
#define __C_SPINDLE_H__
#include "../std_types.h"
#include "../Common/Serial/c_Serial.h"
//#include "../talos.h"
//#include "../Common/Serial/s_buffer.h"
class c_spindle_com_bus
{
	//variables
	public:
	//These variables hold the last read state of the spindle. The struct may be in the middle of updating.
	static float Rpm;
	static uint8_t Direction;
	static uint8_t State;

	static void ReadStream(const char* stream_data);
	static void WriteStream(const char* stream_data, c_Serial destination);
	static void Reset();
	union u_spindle_data
	{
		struct
		{
			uint32_t rpm; //Divide this by 1000 to get decimal
			uint8_t direction;
			uint8_t state; //
		}s_spindle_detail;
		//This serves as a byte stream that can be loaded or unloaded to/from a serial stream.
		char stream[(sizeof(uint32_t) * 1) + (sizeof(uint8_t) * 2)+ (sizeof(int16_t) * 1)];
		
	};
	static u_spindle_data Spindle_Data;
	
	union u_check_sum
	{
		struct
		{
			uint16_t Check_Sum_Value;
			uint16_t End_Of_Record;
		}s_check_sum;
		
		char stream[(sizeof(uint16_t)*2)];

	};
	static u_check_sum Check_Sum;

	protected:
	private:

	//functions
	public:
	
	protected:
	private:
	
	//c_spindle_rx( const c_spindle_rx &c );
	//c_spindle_rx& operator=( const c_spindle_rx &c );
	//c_spindle_rx();
	//~c_spindle_rx();

}; //c_spindle
#endif //__C_SPINDLE_H__
