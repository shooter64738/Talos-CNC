/*
*  c_grbl.cpp - NGC_RS274 controller.
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


#include "c_grbl.h"
#include "..\..\..\c_processor.h"
#include "..\..\..\Events\c_motion_control_events.h"
#include "c_Grbl_Settings.h"
#include "..\..\..\..\..\Common\MotionControllerInterface\c_motion_controller.h"
#include "..\..\..\..\..\Common\MotionControllerInterface\c_motion_controller_settings.h"

char *c_grbl::OK_RESP = "ok\r\0";
char *c_grbl::DONE_RESP = "done\r\0";
char *c_grbl::ERROR_RESP = "error:\0";
char c_grbl::command_set[COMMAND_SET_SIZE]{ '?','$' };
bool c_grbl::_feed_hold = false;
c_motion_controller::e_controller_responses c_grbl::Initialize()
{
	//Set the motion controller off line
	c_motion_control_events::clear_event(Motion_Control_Events::CONTROL_ONLINE);
	//Assume we have a full 8 axis controller
	c_motion_controller_settings::axis_count_reported = 8;

	if (c_grbl::Connect() != c_motion_controller::e_controller_responses::OK) { return c_motion_controller::e_controller_responses::CONNECT_FAIL; }
	if (c_grbl::LoadSettings() != c_motion_controller::e_controller_responses::OK) { return c_motion_controller::e_controller_responses::TIMEOUT; }
	//Set the motion controller on line
	c_motion_control_events::set_event(Motion_Control_Events::CONTROL_ONLINE);
	c_motion_controller_settings::configuration_settings.controller_type = c_motion_controller_settings::e_motion_controller::GRBL;
	c_motion_controller::PNTR_INQUIRY_CALLBACK = &c_grbl::Inquiry;
	c_motion_controller::PNTR_COMMAND_CHECK_CALLBACK = &c_grbl::IsControllerCommand;
	c_motion_controller::PNTR_FEED_HOLD = &c_grbl::Feed_Hold;
	c_motion_controller::PNTR_CYCLE_RESUME = &c_grbl::Cycle_Start_Resume;
	c_motion_controller::PNTR_CYCLE_START = &c_grbl::Cycle_Start_Resume;
	c_motion_controller::PNTR_E_STOP = &c_grbl::Safety_Alarm;
	c_motion_controller::PNTR_JOG_CANCEL = &c_grbl::Jog_Cancel;
	c_motion_controller::PNTR_JOG_AXIS = &c_grbl::Jog_Axis;
	c_motion_controller::PNTR_RESPONSE_PENDING = &c_grbl::Check_Response;
	c_motion_controller::PNTR_SEND_DATA = &c_grbl::SendToController;

	return c_motion_controller::e_controller_responses::OK;
}

void c_grbl::Inquiry()
{
	while (c_processor::host_serial.HasEOL())
	{
		char Byte = c_processor::host_serial.Get();
		c_processor::controller_serial.Write(Byte);
		if (Byte == CR)
		{
			break;
		}
	}

	c_grbl::output_to_host();

}

//Blocking call that reports to the host anything sent back by the controller. 
void c_grbl::output_to_host()
{
	while (1)
	{
		if (c_processor::controller_serial.HasEOL())
		{
			if (c_grbl::Check_Response(c_grbl::OK_RESP) == c_motion_controller::e_controller_responses::OK)
				break;
			if (c_grbl::Check_Response(c_grbl::ERROR_RESP) == c_motion_controller::e_controller_responses::HALT)
				break;

			char Byte = c_processor::controller_serial.Get();
			c_processor::host_serial.Write(Byte);
		}
	}

	//print the tail of the response (the end marker)
	while (c_processor::controller_serial.HasEOL())
	{
		char Byte = c_processor::controller_serial.Get();
		c_processor::host_serial.Write(Byte);
	}
}

c_motion_controller::e_controller_responses c_grbl::Connect()
{

	//Tell the user we are looking for the controller/grbl board.
	c_processor::host_serial.print_string("Init MC (S1), ");

	c_processor::controller_serial.print_string("?\r");

	if (c_processor::controller_serial.WaitFOrEOL(500000))
	{
		return c_motion_controller::e_controller_responses::TIMEOUT;
	}

	c_grbl::_process_response('?');

	c_processor::host_serial.print_string("OK!\r");

	return c_motion_controller::e_controller_responses::OK;
}

void c_grbl::RequestValue(const char *data)
{
	c_processor::controller_serial.print_string(data);
}

c_motion_controller::e_controller_responses c_grbl::LoadSettings()
{

	//Make a request to GRBL to get its settings
	c_processor::controller_serial.print_string("$$\r");

	//Wait for EOL character
	if (c_processor::controller_serial.WaitFOrEOL(500000))
	{
		return c_motion_controller::e_controller_responses::TIMEOUT;
	}

	//Could be some unwanted data before the settings, so just skip ahead to the $ value
	while (c_processor::controller_serial.Peek() != '$')
	{
		if (c_processor::controller_serial.Peek() == '$') { break; }
		//go to the end of the line
		c_processor::controller_serial.SkipToEOL();

		//if we read the last CR in the buffer and never got a $ value, thats not good.
		if (!c_processor::controller_serial.HasEOL())
		{
			//Wait for EOL character. If we dont get a new line before we time out, its an error
			if (c_processor::controller_serial.WaitFOrEOL(500000))
			{
				return c_motion_controller::e_controller_responses::TIMEOUT;
			}
		}
	}
	_parse_setting();
	//skip to eol, so there isnt anything left in the controller buffer
	c_processor::controller_serial.SkipToEOL();

	return c_motion_controller::e_controller_responses::OK;
}

bool c_grbl::IsControllerCommand(char Byte)
{
	for (int i = 0;i < COMMAND_SET_SIZE;i++)
	{
		if (Byte == c_grbl::command_set[i])
		{
			return true;
		}
	}

	return false;
};

c_motion_controller::e_controller_responses c_grbl::SendToController(const char*Buffer, bool mute, bool wait_for_response)
{
	c_processor::controller_serial.print_string(Buffer);
	c_processor::controller_serial.Write(CR);

	c_processor::controller_serial.WaitFOrEOL(50000);

	/*
	GRBL always sends something back to its host. Usually ok, but could be error.
	We are going to return 'ok' here since thats the expected response.
	This will cause the motion controller method that called us to set the event
	flag to check for 'ok'
	*/

	if (!wait_for_response)
		return c_motion_controller::e_controller_responses::OK;

	/*
	If we arent returning immediatley, this is a blocking call and we are reporting to the host.
	This would be the case for a non motion operation such as '$$' commands
	Since this is a blocking command and is handled internally by the grbl specific code, we do
	not set the check for response flag. All that is handled within here.
	*/

	c_grbl::output_to_host();
	return c_motion_controller::e_controller_responses::NO_RESPONSE;
}

c_motion_controller::e_controller_responses c_grbl::Check_Response(const char * marker)
{
	/*
	This is a generic response check. The motion controller may respond with more than 1 message.
	For example grbl could respond with ok,err, or err:#.
	Since an error is the most important, we should check that first.
	However, since we might be checking for a specific response by the caller, if 'marker'
	is not null, we will only check for that response. If 'marker' is null we will check for
	controller specific repsonses. for grbl, that is 'err' and 'ok'

	We call back into c_motion_controller. I know it seems redundant because we worked so
	hard to call into this pointer address function, but c_motion_controller contains code
	to process a serial stream and look for a specific value and consider it a response. That
	code can be reused over and over from any of the different motion controller types. That
	is why it is in c_motion_controller. It is not specific processing code for one controller.
	*/

	//Make sure there is an eol, if theres not wedont have a response yet.
	if (c_processor::controller_serial.HasEOL())
	{
		if (marker != NULL)//<--we are looking for 1 specific response this time
		{
			//If response was not null, then whatever we were looking for was in there. 
			if (!c_motion_controller::Check_Response(marker))
				return c_motion_controller::e_controller_responses::NO_RESPONSE;
		}
		else
		{
			//Was there an err response?
			if (c_motion_controller::Check_Response(c_grbl::ERROR_RESP))
				return c_motion_controller::e_controller_responses::HALT;

			//Was there an ok response?
			if (c_motion_controller::Check_Response(c_grbl::OK_RESP))
				return c_motion_controller::e_controller_responses::OK;
		}
	}

	//TODO
		//just re wrapping this for now.
	//return c_motion_controller::Check_Response(marker);

	////This could be reading the 'ok' from the controller. If its not a modified grbl to send 'DONE' then this will need to be addressed
	//uint8_t _end_xmit = 0;
	////if (!c_processor::controller_serial.HasEOL()){return 0;}
	//char byte = 0;
	//bool marker_found = false;
	//uint8_t marker_length = strlen(marker);
	//
	//if (c_processor::controller_serial.HasEOL())
	//{
	//for (uint8_t marker_peek = 0; marker_peek < marker_length; marker_peek++)
	//{
	//byte = toupper(c_processor::controller_serial.Peek(marker_peek));
	//if (byte == 0) { return false; }
	//if (byte == toupper(marker[marker_peek])) { _end_xmit++; }
	//else { _end_xmit = 0; } //<--we had one of them but the next one wasnt what we expected. Reset.
	//
	////have we found all the characters (consecutively) that we were looking for?
	//if (_end_xmit == marker_length) { marker_found = true; break; }
	//}
	//
	//}
	//
	//if (marker_found)
	//{
	//return true;
	//}
	////If marker was not found, leave it for the next time we get called. We may be looking for a different response next time.
	//return false;
}

void c_grbl::Feed_Hold()
{
	c_grbl::_feed_hold = !c_grbl::_feed_hold; //<--simpel toggle so we track what state we put the controller in.
	c_processor::controller_serial.Write('!');
}
void c_grbl::Reset()
{
	//TODO
	//Will we need to reload position data after this? Probably a good idea.
	c_grbl::_feed_hold = false;
	c_processor::controller_serial.Write(0x18);
}
void c_grbl::Cycle_Start_Resume()
{
	c_processor::controller_serial.Write('~');
}
void c_grbl::Safety_Alarm()
{
	c_processor::controller_serial.Write(0x84);
}
void c_grbl::Jog_Cancel()
{
	c_processor::controller_serial.Write(0x85);
}
void c_grbl::Jog_Axis(char Axis_ID, float Value)
{
	c_processor::controller_serial.print_string("$J=");
	c_processor::controller_serial.Write(Axis_ID);
	c_processor::controller_serial.print_float(Value);
	c_processor::controller_serial.Write(CR);

	//Serial jog responds with 'ok' so we should wait for it.
	c_processor::controller_serial.WaitFOrEOL(50000);
	c_grbl::Check_Response(OK_RESP);
}

void c_grbl::_process_response(char query_type)
{/*
If we got here, we have spoken with the motion controller. We can clear the default number of axis now
because we are going to load the true axis control count from teh controller now.
*/
	c_motion_controller_settings::axis_count_reported = 0; //<--clear our assumed count of 8 axis

	uint8_t _end_xmit = 0;
	uint8_t _group_id = 0;
	uint8_t _sub_group_id = 0;
	uint8_t _value_id = 0;
	//Man we gotta clean this up.. its messy.. use the generic ok/err handlers
	while (!_end_xmit)
	{
		char group[5]{ 0,0,0,0,0 };//<--init group, 5 bytes, all null
		char sub_group[5]{ 0,0,0,0,0 };//<--init sub_group, 5 bytes, all null
		char value[13]{ 0,0,0,0,0,0,0,0,0,0,0,0,0 };//<--init value, 13 bytes, all null

		char *pointer = NULL;
		while (_end_xmit < 3)
		{
			//Wait until we have an eol. On first loop this should always be true because we waited in the previous method for it.
			c_processor::controller_serial.WaitFOrEOL(50000);
			char byte = c_processor::controller_serial.Get();

			//we need the 'o','k',and CR to all appear consecutively. This keeps track of that.
			if (byte == 'o') { _end_xmit++; }
			else if (byte == 'k') { _end_xmit++; }
			else if (byte == CR) { _end_xmit++; }
			else { _end_xmit = 0; } //<--we had one of them but the next one wasnt what we expected. Reset.

			if (byte == 13) { if (_end_xmit < 2) { _end_xmit = 0; } continue; }//<--in case we get a cr out of order
			//These catch the start of a group/subgroup/value
			if (byte == '<' && _group_id == 0) { pointer = group; _group_id++; continue; }//<--if '<' point to 'group' array, restart loop
			if (byte == '|' && _sub_group_id == 0) { pointer = sub_group; _value_id = 0; _sub_group_id++; continue; }//<--if '|' point to 'sub_group' array, restart loop
			if (byte == ':') { pointer = value; _value_id++; continue; }//<--if ':' point to 'value' array, restart loop

			//This catches the end of a group/subgroup/value
			if (byte == ',' || byte == '>' || byte == '|')
			{
				pointer = value;
				c_grbl::_store_value(query_type, group, sub_group, _value_id, atof(value));
				_value_id++;//<--if ':' point to 'value' array, restart loop

				if (byte == '>') { _group_id = 0; }//<--need to check for the end of a group here, restart loop
				if (byte == '|') { pointer = sub_group; _value_id = 0; _sub_group_id++; }//<--if '|' point to 'sub_group' array, restart loop
				continue;
			}


			//if we havent found a group yet, just keep swimming..
			if (!_group_id) { continue; }

			//make these all up case for consistency
			*pointer++ = toupper(byte); //<--update the byte data in the appropriate array, move the pointer forward 1 byte
			*pointer = NULL;

			//If we have exceeded the size for the group/subgroup pointer, keep setting the last byte to null
			if ((pointer == group || pointer == sub_group) && *pointer > sizeof(group))
			{
				*pointer--;
				*pointer = NULL;
			}
		}
		//if (_end_xmit) { break; }
	}
	if (!c_motion_controller_settings::loaded)
	{
		//Only set these the first time parameters are laoded.
		c_motion_controller_settings::loaded = true;
		//Talos can handle up to 8 axis right now. If the controller cannot, theres no point in trying to track all that information.
		//The reported axis count is not set to the number of axis the controller can use. All reporting will be chagned to match that.
	}
}

uint16_t c_grbl::_store_value(char query, char*group, char*sub_group, uint8_t value_id, float value)
{
	uint8_t control_state = 0;
	switch (query)
	{
	case '?':
	{
		//storing a value from a '?' query
		if (*group == *"IDLE")
			control_state = 55;
		if (*sub_group == *"MPOS")
		{
			control_state = 99;
			//these are axis positions
			if (value_id > MACHINE_AXIS_COUNT)
			{
				int i = 0;
				//controller has more axis values than we can control. this is a bad thing..
				return 0;
			}
			//Since we have the position data, we may as well tell the machien where its currently at. (c_processor position mapper)
			if (c_motion_controller_settings::position_reported != NULL)
				c_motion_controller_settings::position_reported[value_id - 1] = value;

			if (!c_motion_controller_settings::loaded)
			{
				c_motion_controller_settings::axis_count_reported++;
			}


		}
		else if (*sub_group == *"FS")
		{
			control_state = 99;
			//these are feed and spindle speeds
			//1 is for feed speed
			//2 is for spindle speed
			value_id == 1 ? c_motion_controller_settings::feed_rate = value : 0;
			value_id == 2 ? c_motion_controller_settings::spindle_speed = value : 0;
		}
		else if (*sub_group == *"OV")
		{
			control_state = 99;
			//these are feed overrides for x,y,z,
			//1 is for feed override
			//2 is for spindle
			//value_id == 1 ? c_motion_controller::feed_rate = value : 0;
			//value_id == 2 ? c_motion_controller::spindle_speed = value : 0;

		}
		break;
	}
	default:
		break;
	}
}

void c_grbl::_parse_setting()
{
	uint8_t _end_xmit = 0;
	while (!_end_xmit)
	{
		char setting[4]{ 0,0,0,0 };//<--init setting, 4 bytes, all null
		char value[13]{ 0,0,0,0,0,0,0,0,0,0,0,0,0 };//<--init value, 13 bytes, all null
		char *pointer = NULL;
		while (1)
		{
			//Wait until we have an eol. On first loop this should always be true because we waited in the previous method for EOL.
			c_processor::controller_serial.WaitFOrEOL(50000);
			char byte = c_processor::controller_serial.Get();

			if (byte == 13 && c_processor::controller_serial.Peek() == 13) { continue; }//<--in case we get back to back CR's, lets check here.
			if (byte == '$') { pointer = setting; continue; }//<--if $ point to 'setting' array, restart loop
			if (byte == '=') { pointer = value; continue; }//<--if = point to 'value' array, restart loop
			if (byte == 13) { break; } //<--if CR then break out of this loop, we are at the end of this line
			if (pointer == NULL) { _end_xmit = 1; break; }//<--If not pointer assigned, we do not know what to do, so break out.
			*pointer++ = byte; //<--update the byte data in the appropriate array, move the pointer forward 1 byte

		}
		if (_end_xmit) { break; }

		float f_value = atof(value);
		int16_t i_setting = atoi(setting);

		_assign_value(i_setting, f_value);

	}
}

void c_grbl::_assign_value(uint16_t Setting, float value)
{
	//Load setting values into a common interface. Since these could come from different controllers, grbl, tinyg, g2, smoothie, etc.
	switch (Setting)
	{
		//case c_GRBL_Settings::_step_pulse_length:
		//this->GrblConfiguration._step_pulse_length = (uint8_t)Value;
		//break;
		//case c_GRBL_Settings::_step_idle_delay:
		//break;
		//case c_GRBL_Settings::_step_port_invert:
		//break;
		//case c_GRBL_Settings::_directionport_invert:
		//break;
		//case c_GRBL_Settings::_step_enable_invert:
		//break;
		//case c_GRBL_Settings::_limit_pin_invert:
		//break;
		//case c_GRBL_Settings::_probe_pin_invert:
		//break;
		//case c_GRBL_Settings::_status_report_mask:
		//break;
		//case c_GRBL_Settings::_junction_deviation:
		//break;
		//case c_GRBL_Settings::_arc_tolerance:
		//break;
		//case c_GRBL_Settings::_report_inches_bool:
		//break;
		//case c_GRBL_Settings::_soft_limits_bool:
		//break;
		//case c_GRBL_Settings::_hard_limits_bool:
		//break;
		//case c_GRBL_Settings::_homing_cycle_bool:
		//break;
		//case c_GRBL_Settings::_homing_direction_invert_mask:
		//break;
		//case c_GRBL_Settings::_homing_feed_speed:
		//break;
		//case c_GRBL_Settings::_homing_seek_speed:
		//break;
		//case c_GRBL_Settings::_homing_debounce:
		//break;
		//case c_GRBL_Settings::_homing_pull_off:
		//break;
		//case c_GRBL_Settings::_max_spindle_speed:
		//break;
		//case c_GRBL_Settings::_min_spindle_speed:
		//break;
		//case c_GRBL_Settings::_laser_mode_boolean:
		//break;
	case c_GRBL_Settings::_x_steps_per_mm:
	{
		c_motion_controller_settings::configuration_settings.steps_per_mm[MACHINE_X_AXIS] = value;
		break;
	}
	case c_GRBL_Settings::_y_steps_per_mm:
	{
		c_motion_controller_settings::configuration_settings.steps_per_mm[MACHINE_Y_AXIS] = value;
		break;
	}
	case c_GRBL_Settings::_z_steps_per_mm:
	{
		c_motion_controller_settings::configuration_settings.steps_per_mm[MACHINE_Z_AXIS] = value;
		break;
	}
	case c_GRBL_Settings::_a_steps_per_mm:
	{
		c_motion_controller_settings::configuration_settings.steps_per_mm[MACHINE_A_AXIS] = value;
		break;
	}
	case c_GRBL_Settings::_b_steps_per_mm:
	{
		c_motion_controller_settings::configuration_settings.steps_per_mm[MACHINE_B_AXIS] = value;
		break;
	}
	case c_GRBL_Settings::_c_steps_per_mm:
	{
		c_motion_controller_settings::configuration_settings.steps_per_mm[MACHINE_C_AXIS] = value;
		break;
	}
	case c_GRBL_Settings::_u_steps_per_mm:
	{
		c_motion_controller_settings::configuration_settings.steps_per_mm[MACHINE_U_AXIS] = value;
		break;
	}
	case c_GRBL_Settings::_v_steps_per_mm:
	{
		c_motion_controller_settings::configuration_settings.steps_per_mm[MACHINE_V_AXIS] = value;
		break;
	}
	//case c_GRBL_Settings::_x_max_speed:
	//break;
	//case c_GRBL_Settings::_y_max_speed:
	//break;
	//case c_GRBL_Settings::_z_max_speed:
	//break;
	//case c_GRBL_Settings::_a_max_speed:
	//break;
	//case c_GRBL_Settings::_b_max_speed:
	//break;
	//case c_GRBL_Settings::_c_max_speed:
	//break;
	//case c_GRBL_Settings::_x_acceleration:
	//break;
	//case c_GRBL_Settings::_y_acceleration:
	//break;
	//case c_GRBL_Settings::_z_acceleration:
	//break;
	//case c_GRBL_Settings::_a_acceleration:
	//break;
	//case c_GRBL_Settings::_b_acceleration:
	//break;
	//case c_GRBL_Settings::_c_acceleration:
	//break;
	case c_GRBL_Settings::_x_max_travel:
		//this->GrblConfiguration._max_travel.x_value = Value;
		break;
	case c_GRBL_Settings::_y_max_travel:
		//this->GrblConfiguration._max_travel.y_value = Value;
		break;
	case c_GRBL_Settings::_z_max_travel:
		//this->GrblConfiguration._max_travel.z_value = Value;
		break;
	case c_GRBL_Settings::_a_max_travel:
		//this->GrblConfiguration._max_travel.a_value = Value;
		break;
	case c_GRBL_Settings::_b_max_travel:
		//this->GrblConfiguration._max_travel.b_value = Value;
		break;
	case c_GRBL_Settings::_c_max_travel:
		//this->GrblConfiguration._max_travel.c_value = Value;
		break;

	default:
		/* Your code here */
		break;
	}
}

//// default destructor
//c_Grbl::~c_Grbl()
//{
//} //~c_Grbl
