/*
*  c_motion_controller.h - NGC_RS274 controller.
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

/*
This library is shared across all projects.
*/

#ifndef __C_MOTION_CONTROLLER_H__
#define __C_MOTION_CONTROLLER_H__

//#define ERR_OK 0
//#define ERR_CONTROL_CONNECT_FAIL 1
//#define ERR_CONTROL_TIMEOUT 2
//#define ERR_CONTROL_DATA_ERR 3

#include "../../Talos.h"
class c_motion_controller
{

	//variables
public:
	enum class e_controller_responses :uint8_t
	{
		NO_RESPONSE, OK, PROCEDE, CONNECT_FAIL, TIMEOUT, HALT
	};

	static void send_motion(char * line_data, bool is_motion);
	static void read_controller_data();
	static void Initialize();
	static e_controller_responses last_response;


	static bool Check_Response(const char * marker);
	static bool(*PNTR_COMMAND_CHECK_CALLBACK)(char);
	static c_motion_controller::e_controller_responses(*PNTR_RESPONSE_PENDING)(const char*);
	static void(*PNTR_INQUIRY_CALLBACK)(void);
	static void(*PNTR_FEED_HOLD)(void);
	static void(*PNTR_E_STOP)(void);
	static void(*PNTR_RESET)(void);
	static void(*PNTR_CYCLE_START)(void);
	static void(*PNTR_CYCLE_RESUME)(void);
	static void(*PNTR_JOG_CANCEL)(void);
	static void(*PNTR_JOG_AXIS)(char, float);
	static c_motion_controller::e_controller_responses(*PNTR_SEND_DATA)(const char*, bool, bool);



protected:
private:

	//functions
public:

protected:
private:
	c_motion_controller(const c_motion_controller &c);
	c_motion_controller& operator=(const c_motion_controller &c);
	c_motion_controller();
	~c_motion_controller();
}; //c_motion_controller

#endif //__C_MOTION_CONTROLLER_H__
