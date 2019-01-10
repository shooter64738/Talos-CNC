/*
*  c_grbl.h - NGC_RS274 controller.
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


#ifndef __C_GRBL_H__
#define __C_GRBL_H__

#include <stddef.h>
#include <ctype.h>
#include <stdint.h>

#include "..\..\..\..\..\Common\MotionControllerInterface\c_motion_controller.h"


#define COMMAND_SET_SIZE 2
class c_grbl
{
//variables
public:
static char command_set[];
protected:
private:

//functions
public:
	//c_Grbl();
	//c_Grbl(c_Serial &ComDevice, c_Serial &_hostcomDevice);
	//~c_Grbl();
	static uint8_t Status;
	static void RequestValue(const char *data);
	static c_motion_controller::e_controller_responses LoadSettings();
	static c_motion_controller::e_controller_responses SendToController(const char*Buffer, bool mute, bool wait_for_response = true);
	static c_motion_controller::e_controller_responses Check_Response(const char * marker);
	static void Feed_Hold();
	static void Reset();
	static void Cycle_Start_Resume();
	static void Safety_Alarm();
	static void Jog_Cancel();
	static void Jog_Axis(char Axis_ID,float Value);
	static void _process_response(char query_type);
	static uint16_t _store_value(char query, char * group, char * sub_group, uint8_t value_id, float value);
	static bool IsControllerCommand(char Byte);
	static c_motion_controller::e_controller_responses Initialize();
	static void Inquiry();
	static void output_to_host();
	static c_motion_controller::e_controller_responses Connect();
	
	static void _parse_setting();
	static void _assign_value(uint16_t,float);
	static bool _feed_hold;
	static char *OK_RESP;
	static char *DONE_RESP;
	static char *ERROR_RESP;

protected:
private:
	//c_Grbl( const c_Grbl &c );
	//c_Grbl& operator=( const c_Grbl &c );


	
		

}; //c_Grbl

#endif //__GRBL_H__
