/*
*  c_motion_controlelr.cpp - NGC_RS274 controller.
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

#include "c_motion_controller.h"
#include "c_motion_controller_settings.h"
#include "..\..\Coordinator/Shared/Events/c_motion_events.h"
#include "..\..\Coordinator/Shared/Events/c_motion_control_events.h"
#include "..\../Coordinator/Shared/MotionControllerInterface/ExternalControllers/GRBL/c_Grbl.h"
#ifdef MSVC
//#include "../Hardware_Abstraction_Layer/c_hal.h"
#endif
#include "../../Coordinator/Shared/c_processor.h"
#include "../../Coordinator/Shared/Machine/c_machine.h"

c_motion_controller::e_controller_responses(*c_motion_controller::PNTR_RESPONSE_PENDING)(const char*);
c_motion_controller::e_controller_responses c_motion_controller::last_response;
void(*c_motion_controller::PNTR_INQUIRY_CALLBACK)(void);
bool(*c_motion_controller::PNTR_COMMAND_CHECK_CALLBACK)(char);
void(*c_motion_controller::PNTR_FEED_HOLD)(void);
void(*c_motion_controller::PNTR_E_STOP)(void);
void(*c_motion_controller::PNTR_RESET)(void);
void(*c_motion_controller::PNTR_CYCLE_START)(void);
void(*c_motion_controller::PNTR_CYCLE_RESUME)(void);
void(*c_motion_controller::PNTR_JOG_CANCEL)(void);
void(*c_motion_controller::PNTR_JOG_AXIS)(char, float);
c_motion_controller::e_controller_responses(*c_motion_controller::PNTR_SEND_DATA)(const char*, bool, bool);

void c_motion_controller::Initialize()
{
	/*
	If a controller is connected the axis count for the controller will be used.
	Just in case the motion controller is not at zero when the coordinator starts up, synch positions
	by pointing the machine position array to the controller reported position array. This way when
	we connect to the controller and get the position data for it, the machine will aready be pointed
	to the array we set the data in.
	*/
	c_motion_controller_settings::position_reported = c_machine::axis_position;
	//We dont know which motion controller is connected, so we are just going to take a stab at it, one at a time.

	//When we connected serial on the motion controller, it may have given us a hello message that we can use to determine its type.
	//if (c_motion_controller::Check_Response("grbl"))
	c_grbl::Initialize();

	//c_tiny_g::Initialize();
	//c_g2::Initialize();
	//c_smoothie::Initialize();
	//c_marlin::Initialize();

}

void c_motion_controller::send_motion(char * line_data, bool is_motion)
{

	//This method converts/translates a general control method and sends it to a specific control in its respective format.

	//Send program line to controller. mute response, we will pick it up as an event.
	c_motion_controller::e_controller_responses anticipated_response = c_motion_controller::e_controller_responses::NO_RESPONSE;
	if (c_motion_controller::PNTR_SEND_DATA != NULL)
	 anticipated_response = c_motion_controller::PNTR_SEND_DATA(line_data, false, false);
	switch (anticipated_response)
	{
		//Now check to see which type of response (if any) we should anticipate from the controller when a line of data is sent.
		case c_motion_controller::e_controller_responses::OK:
		{
			c_motion_control_events::set_event(Motion_Control_Events::AWAIT_OK_RESPONSE);
			//Special flavor of grbl. When a motion completes it sends the response 'done';
			if (is_motion && c_motion_controller_settings::configuration_settings.controller_type == c_motion_controller_settings::e_motion_controller::GRBL)
			{
				//This is a motion command which needs the 'done' response to know that motion is done.
				c_motion_events::set_event(Motion_Events::MOTION_IN_QUEUE);
				c_motion_control_events::set_event(Motion_Control_Events::AWAIT_DONE_RESPONSE);
			}

			//c_grbl::SendToController(line_data, 1, false);
			#ifdef MSVC
			//c_hal::comm.PNTR_VIRTUAL_BUFFER_WRITE(1, "ok\rDONE\r");//<--data from motion control
			#endif
			break;
		}

		default:
		/* Your code here */
		break;
	}
}

void c_motion_controller::read_controller_data()
{
	/*
	TODO: This may need some expansion if we are expecting more than one pending response.
	The modfied version of grbl will respond with 'ok when the motion line is processes
	and send 'done' when that motion has completed. Instead of having the CHECK_RESPONSE
	flag, we may want to expand this to multiple.

	When the motion controller connected we determined its type. We also set several function pointers
	that we can call to interface to that controller. The function pointers are used so that everywhere
	OUTSIDE motion controller logic, we call the same methods. But INSIDE each motion controller
	initialize method function pointers are assigned to various method specific to that controller.

	When we check for a response from the controller, regardless of what types of responses we get, these
	will always be normalized down to the response that the c_motion_controller class understands.
	*/

	//Do we need/expect a response from the controller?
	if (c_motion_control_events::event_flags != 0)
	{
		//Get the response from the controller
		if(c_motion_controller::PNTR_RESPONSE_PENDING != NULL)
		c_motion_controller::last_response = c_motion_controller::PNTR_RESPONSE_PENDING(NULL);

		//Do we have a response?
		if (last_response != c_motion_controller::e_controller_responses::NO_RESPONSE)
		{
			switch (last_response)
			{
				case c_motion_controller::e_controller_responses::NO_RESPONSE:
				break;
				case c_motion_controller::e_controller_responses::OK:
				{
					if (c_motion_control_events::get_event(Motion_Control_Events::AWAIT_OK_RESPONSE))
					{
						//The response we were waiting for has come in. Clear it.
						c_motion_control_events::clear_event(Motion_Control_Events::AWAIT_OK_RESPONSE);
					}
				}
				break;
				case c_motion_controller::e_controller_responses::PROCEDE:
				break;
				case c_motion_controller::e_controller_responses::CONNECT_FAIL:
				break;
				case c_motion_controller::e_controller_responses::TIMEOUT:
				break;
				case c_motion_controller::e_controller_responses::HALT:
				/*
				We dont expect errors so when these pop up, bad things must have happened. Abandon ship!
				*/
				//Clear all event flags for the motion controller
				c_motion_control_events::event_flags = 0;
				//Set an error event for the motion controller
				c_motion_control_events::set_event(Motion_Control_Events::CONTROL_ERROR);
				//Set the control offline, so no more commands will go to it.
				c_motion_control_events::clear_event(Motion_Control_Events::CONTROL_ONLINE);
				/*
				Now we can let main do whatever it wants to, but until the controller is set back to
				'online', no more command data will be sent to it.
				*/
				break;
			}


			//What was the response?
			//c_motion_controller::last_response;
		}
	}
}

bool c_motion_controller::Check_Response(const char * marker)
{
	//This could be reading the 'ok' from the controller. If its not a modified grbl to send 'DONE' then this will need to be addressed
	uint8_t _end_xmit = 0;
	//if (!c_processor::controller_serial.HasEOL()){return 0;}
	char byte = 0;
	bool marker_found = false;
	uint8_t marker_length = strlen(marker);

	if (c_processor::controller_serial.HasEOL())
	{
		for (uint8_t marker_peek = 0; marker_peek < marker_length; marker_peek++)
		{
			byte = toupper(c_processor::controller_serial.Peek(marker_peek));
			if (byte == 0) { return false; } //<-- out of data?
			if (byte == toupper(marker[marker_peek])) { _end_xmit++; }
			else { _end_xmit = 0; } //<--we had one of them but the next one wasnt what we expected. Reset.

			//have we found all the characters (consecutively) that we were looking for?
			if (_end_xmit == marker_length) { marker_found = true; break; }
		}

	}

	if (marker_found)
	{
		return true;
	}
	//If marker was not found, leave it for the next time we get called. We may be looking for a different response next time.
	return false;
}

//// default constructor
//c_motion_controller::c_motion_controller()
//{
//} //c_motion_controller
//
//// default destructor
//c_motion_controller::~c_motion_controller()
//{
//} //~c_motion_controller
