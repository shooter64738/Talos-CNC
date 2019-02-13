/* 
* c_state.cpp
*
* Created: 2/13/2019 10:59:08 AM
* Author: jeff_d
*/


#include "c_state_manager.h"

uint8_t Spindle_Controller::c_state_manager::check_driver_state()
{
//Check the current RPM
	float this_rpm = 0.0;
	uint16_t illegal_rotations = 0;
	
	this_rpm = Spindle_Controller::c_encoder::current_rpm();
	uint8_t test = Spindle_Controller::c_driver::Get_State(STATE_BIT_ENABLE) & Spindle_Controller::c_driver::Get_State(STATE_BIT_DIRECTION_CW);
	//Check for a direction change
	while (c_Parser::spindle_input.rotation_direction != Spindle_Controller::c_driver::Drive_Control.direction)
	{
		//Clear all PID values because we are changing direction
		c_PID::Clear(c_PID::spindle_terms);
		accel_output = c_PID::spindle_terms.Min_Val;
		
		/*
		There is a difference in the programmed rotation, and the current rotation.
		We want to end the state of current rotation, and start the state of
		programmed rotation
		*/
		
		//determine the state of current rotation
		switch (Spindle_Controller::c_driver::Drive_Control.direction)
		{
			case PARSER_DIRECTION_CCW: //<--drive is currently in CCW rotation (M04)
			{
				/*
				since we were in a rotating state, and now we are going to be stopping/changing direction,
				this is obviously deceleration.
				*/
				//Set state to decelerate
				Spindle_Controller::c_driver::Set_State(STATE_BIT_DECELERATE);
				
				//new direction is either stop, or cw. Either way, we must brake first
				Spindle_Controller::c_driver::Brake_Drive();
				//brake is active, and we continue in this state until rpm reaches 0.
				//If a stop is all we needed, we are essentially done. If we are going a new direction
				//however we set the direction pin, and then set directions to be =

				if ( this_rpm < 1 && c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_CW) //(M03)
				{
					//if this is a new direction, set the direction pin, and then match the directions
					//between the driver and the parser
					Spindle_Controller::c_driver::Drive_Control.direction = c_Parser::spindle_input.rotation_direction;
					
					//Set direction to forward
					Spindle_Controller::c_driver::Forward_Drive();
					//Set state to accelerate
					Spindle_Controller::c_driver::Set_State(STATE_BIT_ACCELERATE);
					//Clear decelerate bit
					Spindle_Controller::c_driver::Clear_State(STATE_BIT_DECELERATE);
				}
				//If the spindle was rotating, and then set to an off state, the brake will remain on.
			}
			break;
			case PARSER_DIRECTION_CW: //<--drive is currently in CW rotation (M03)
			{
				/*
				since we were in a rotating state, and now we are going to be stopping/changing direction,
				this is obviously deceleration.
				*/
				//Set state to decelerate
				Spindle_Controller::c_driver::Set_State(STATE_BIT_DECELERATE);
				
				//new direction is either stop, or ccw. Either way, we must brake first
				Spindle_Controller::c_driver::Brake_Drive();
				//brake is active, and we continue in this state until rpm reaches 0.
				//If a stop is all we needed, we are essentially done. If we are going a new direction
				//however we set the direction pin, and then set directions to be =

				if ( this_rpm < 1 && c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_CCW) //(M04)
				{
					//if this is a new direction, set the direction pin, and then match the directions
					//between the driver and the parser
					Spindle_Controller::c_driver::Drive_Control.direction = c_Parser::spindle_input.rotation_direction;
					
					//Set direction to reverse
					Spindle_Controller::c_driver::Reverse_Drive();
					//Set state to accelerate
					Spindle_Controller::c_driver::Set_State(STATE_BIT_ACCELERATE);
					//Clear decelerate bit
					Spindle_Controller::c_driver::Clear_State(STATE_BIT_DECELERATE);
				}
				//If the spindle was rotating, and then set to an off state, the brake will remain on.
			}
			break;
			case PARSER_DIRECTION_OFF: //<--drive is currently not rotating at all (we think) (M05)
			{
				//This one is simple. Just set the direction and enable the drive. Just in case
				//we check the rpm anyway, because we expect spindle to not be rotating.
				
				if ( this_rpm > 0)
				{
					illegal_rotations++;
					//rotation detected but we dont expect it while in an off state
					break; //<--brake out of the switch/case here. loop again
				}
				
				//new direction is either stop, cw/ccw. Either way, brake first
				Spindle_Controller::c_driver::Brake_Drive();
				//assume new rpm of zero for now
				Spindle_Controller::c_driver::Drive_Control.target_rpm = 0; //(M05) spindle stop
				Spindle_Controller::c_driver::Drive_Control.target_rpm = c_Parser::spindle_input.target_rpm;
				
				//Set the rotation directions to match
				Spindle_Controller::c_driver::Drive_Control.direction = c_Parser::spindle_input.rotation_direction;
				
				if(c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_CW) //(M03)
				{
					Spindle_Controller::c_driver::Forward_Drive();
				}

				if(c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_CCW) //(M04)
				{
					Spindle_Controller::c_driver::Reverse_Drive();
				}
				
				//since we were in an off state, and now we are going to be rotating, this is obviously acceleration
				//Set state to accelerate
				Spindle_Controller::c_driver::Set_State(STATE_BIT_ACCELERATE);
				//Clear decelerate bit
				Spindle_Controller::c_driver::Clear_State(STATE_BIT_DECELERATE);
			}
			break;
		}
		//With every loop check the RPM
		this_rpm = c_Encoder_RPM::CurrentRPM();
		
		/*
		When we do NOT expect rotations, we count the number of times we looped and detected movement.
		If this value reaches the maximum, we will throw an error. This might happen if the operator
		is moving the spindle by hand when we expect it to be off, and an M3/M4 command is trying to
		execute. It may also happen if there is electrical interference between the encoder	and the controller.
		*/
		
		if (illegal_rotations>650) //<-- processor will loop incredibly fast. 650 should be hit in about .00001 seconds
		{
			return NGC_Machine_Errors::SPINDLE_ROTATION_NOT_EXPECTED;
		}
		//Last state to check. We were rotating, and now we have commanded to stop
		if (c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_OFF)
		{
			//Since our brake is on, we can just let it come to a stop. Nothing else for us to do.
			//Set the rotation directions to match
			Spindle_Controller::c_driver::Drive_Control.direction = c_Parser::spindle_input.rotation_direction;
			break; //<--brake out of our loop
		}
	}
	
	if (Spindle_Controller::c_driver::Drive_Control.direction == PARSER_DIRECTION_OFF)
	return NGC_Machine_Errors::OK;
	
	Spindle_Controller::c_driver::Process_State(this_rpm);
	
	return NGC_Machine_Errors::OK;
}
//// default constructor
//c_state_manager::c_state_manager()
//{
//} //c_state
//
//// default destructor
//c_state_manager::~c_state_manager()
//{
//} //~c_state
