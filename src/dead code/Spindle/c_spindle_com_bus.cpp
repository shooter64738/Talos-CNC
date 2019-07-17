/*
*  c_spindle.cpp - NGC_RS274 controller.
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


#include "c_spindle_com_bus.h"
#include <string.h>

float c_spindle_com_bus::Rpm;
uint8_t c_spindle_com_bus::Direction;
uint8_t c_spindle_com_bus::State;
c_spindle_com_bus::u_spindle_data c_spindle_com_bus::Spindle_Data;
c_spindle_com_bus::u_check_sum c_spindle_com_bus::Check_Sum;

/*
Convert a stream of serial data into a struct for spindle data.
A simplistic checksum is used to validate that the data we got is the data we expected.
This may need to be a more robust checksum calculation, but ultimately it has to be
extremely fast.
*/
void c_spindle_com_bus::ReadStream(const char* stream_data)
{
	//clear the checksum
	uint16_t running_check_sum = 0;

	//Reset the internal stream buffer
	c_spindle_com_bus::Reset();
	
	//convert the serial stream into the struct union, dont read last byte. Thats a check sum
	for (uint8_t i=0; i<sizeof(c_spindle_com_bus::Spindle_Data.s_spindle_detail);i++)
	{
		c_spindle_com_bus::Spindle_Data.stream[i] = *stream_data++;
		running_check_sum += c_spindle_com_bus::Spindle_Data.stream[i];
	}
	
	//Read the checksum that was sent with the data
	c_spindle_com_bus::Check_Sum.s_check_sum.Check_Sum_Value = (int16_t)*stream_data;
	*stream_data ++;*stream_data ++;
	//Read the record terminator
	c_spindle_com_bus::Check_Sum.s_check_sum.End_Of_Record = (int16_t)*stream_data;

	//We should be at the stream data checksum value. Read it and compare it to the checksum we calculated
	if (running_check_sum !=c_spindle_com_bus::Check_Sum.s_check_sum.Check_Sum_Value || c_spindle_com_bus::Check_Sum.s_check_sum.End_Of_Record!=65535)
	{
		//Bad read, some or all of the data is bad. Better luck next time!
		//Reset the stream buffer
		c_spindle_com_bus::Reset();
		return;
	}

	//Load the streamed data into our variables.
	c_spindle_com_bus::Direction = c_spindle_com_bus::Spindle_Data.s_spindle_detail.direction;
	c_spindle_com_bus::State = c_spindle_com_bus::Spindle_Data.s_spindle_detail.state;
	c_spindle_com_bus::Rpm = (float)c_spindle_com_bus::Spindle_Data.s_spindle_detail.rpm/1000.0;
	c_spindle_com_bus::Reset();
}

void c_spindle_com_bus::WriteStream(const char* stream_data, c_Serial destination)
{
	
	c_spindle_com_bus::Reset();

	for (uint8_t i = 0; i<sizeof(c_spindle_com_bus::Spindle_Data.s_spindle_detail); i++)
	{
		destination.Write(stream_data[i]);
		Check_Sum.s_check_sum.Check_Sum_Value += stream_data[i];
	}

	Check_Sum.s_check_sum.End_Of_Record = 65535;

	for (uint8_t i = 0; i<sizeof(c_spindle_com_bus::Check_Sum.s_check_sum);i++)
	{
		destination.Write(Check_Sum.stream[i]);
	}
	c_spindle_com_bus::Reset();
}

void c_spindle_com_bus::Reset()
{
	
	//clear the stream. May have garbage in it.
	memset(c_spindle_com_bus::Spindle_Data.stream,0,sizeof(c_spindle_com_bus::Spindle_Data.stream));
	memset(c_spindle_com_bus::Check_Sum.stream, 0, sizeof(c_spindle_com_bus::Check_Sum.stream));
}


//// default constructor
//c_spindle_rx::c_spindle_rx()
//{
//} //c_spindle_rx
//
//// default destructor
//c_spindle_rx::~c_spindle_rx()
//{
//} //~c_spindle_rx
