/*
*  c_driver.cpp - NGC_RS274 controller.
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


#include "c_driver.h"
#include "..\Common\Hardware_Abstraction_Layer\c_hal.h"
#include "c_encoder.h"
#define MINIMUM_OUTPUT 100
#define MAXIMUM_OUTPUT 255

Spindle_Controller::c_driver::s_drive_control Spindle_Controller::c_driver::Drive_Control;
uint8_t Spindle_Controller::c_driver::current_output = 0;
uint8_t Spindle_Controller::c_driver::accel_output = 0;

void Spindle_Controller::c_driver::initialize()
{
	c_hal::driver.PNTR_DRIVE_ANALOG!=NULL?c_hal::driver.PNTR_DRIVE_ANALOG(MINIMUM_OUTPUT):void();
	Spindle_Controller::c_driver::Drive_Control.State = 0;
	Spindle_Controller::c_driver::Drive_Control.accel_time = 15;
	Spindle_Controller::c_driver::Enable_Drive();
	Spindle_Controller::c_driver::Brake_Drive();
}

void Spindle_Controller::c_driver::Enable_Drive()
{
	//call into the hal and enable the driver
	c_hal::driver.PNTR_ENABLE != NULL ? c_hal::driver.PNTR_ENABLE() : void();

	Spindle_Controller::c_driver::Set_State(STATE_BIT_ENABLE); //<--set the enable bit to 1
	Spindle_Controller::c_driver::Clear_State(STATE_BIT_DISABLE); //<--set the disable bit to 0

	/*
	braking bit is not cleared here. The drive can be enabled, and the brake be on
	but until the brake is turned off the PWM output will hold at zero.
	*/
}

void Spindle_Controller::c_driver::Disable_Drive()
{
	//call into the hal and disable the driver
	c_hal::driver.PNTR_DISABLE != NULL ? c_hal::driver.PNTR_DISABLE() : void();
	Spindle_Controller::c_driver::Set_State(STATE_BIT_DISABLE); //<--set the disable bit to 1
	Spindle_Controller::c_driver::Clear_State(STATE_BIT_ENABLE); //<--set the enable bit to 0
}

void Spindle_Controller::c_driver::Brake_Drive()
{
	/*
	braking may be accomplished by connecting motor output leads together via a relay/resistor combo.
	There should be no output to the driver during braking.
	WARNING HERE... If you do not use a braking resistor and employ a relay only for motor braking, the motor
	will come to a hault IMMEDIATLEY! This can result in broken mounts, twisted shafts and all kinds of bad things.
	*/

	//disable drive
	Spindle_Controller::c_driver::Disable_Drive();
	//call into hal and set the brake on the driver
	c_hal::driver.PNTR_BRAKE != NULL ? c_hal::driver.PNTR_BRAKE() : void();
	
	Spindle_Controller::c_driver::Set_State(STATE_BIT_BRAKE);//<--set braking bit
	Spindle_Controller::c_driver::current_output = MINIMUM_OUTPUT;
}

void Spindle_Controller::c_driver::Release_Drive()
{
	//set brake pin low
	c_hal::driver.PNTR_RELEASE != NULL ? c_hal::driver.PNTR_RELEASE() : void();
	Spindle_Controller::c_driver::Clear_State(STATE_BIT_BRAKE);//<--clear braking bit
}

void Spindle_Controller::c_driver::Reverse_Drive()
{
	c_hal::driver.PNTR_REVERSE != NULL ? c_hal::driver.PNTR_REVERSE() : void();
	Spindle_Controller::c_driver::Set_State(STATE_BIT_DIRECTION_CCW);//<--set CCW direction bit
	Spindle_Controller::c_driver::Clear_State(STATE_BIT_DIRECTION_CW);//<--clear CW direction bit
}

void Spindle_Controller::c_driver::Forward_Drive()
{
	c_hal::driver.PNTR_FORWARD != NULL ? c_hal::driver.PNTR_FORWARD() : void();
	Spindle_Controller::c_driver::Set_State(STATE_BIT_DIRECTION_CW);//<--set CW direction bit
	Spindle_Controller::c_driver::Clear_State(STATE_BIT_DIRECTION_CCW); //<--clear CCW direction bit
}

void Spindle_Controller::c_driver::Set_State(uint8_t State_Bit_Flag)
{
	Spindle_Controller::c_driver::Drive_Control.State = (BitSet(Spindle_Controller::c_driver::Drive_Control.State, (State_Bit_Flag)));
}

uint8_t Spindle_Controller::c_driver::Get_State(uint8_t State_Bit_Flag)
{
	return (BitTst(Spindle_Controller::c_driver::Drive_Control.State, (State_Bit_Flag)));
}

void Spindle_Controller::c_driver::Clear_State(uint8_t State_Bit_Flag)
{
	Spindle_Controller::c_driver::Drive_Control.State = BitClr(Spindle_Controller::c_driver::Drive_Control.State, (State_Bit_Flag));
}
//
//uint8_t Spindle_Controller::c_driver::Check_State()
//{
////Check the current RPM
//float this_rpm = 0.0;
//uint16_t illegal_rotations = 0;
//
//this_rpm = Spindle_Controller::c_encoder::current_rpm();
//
////Check for a direction change
//while (c_Parser::spindle_input.rotation_direction != Spindle_Controller::c_driver::Drive_Control.direction)
//{
////Clear all PID values because we are changing direction
//c_PID::Clear(c_PID::spindle_terms);
//accel_output = c_PID::spindle_terms.Min_Val;
//
///*
//There is a difference in the programmed rotation, and the current rotation.
//We want to end the state of current rotation, and start the state of
//programmed rotation
//*/
//
////determine the state of current rotation
//switch (Spindle_Controller::c_driver::Drive_Control.direction)
//{
//case PARSER_DIRECTION_CCW: //<--drive is currently in CCW rotation (M04)
//{
///*
//since we were in a rotating state, and now we are going to be stopping/changing direction,
//this is obviously deceleration.
//*/
////Set state to decelerate
//Spindle_Controller::c_driver::Set_State(STATE_BIT_DECELERATE);
//
////new direction is either stop, or cw. Either way, we must brake first
//Spindle_Controller::c_driver::Brake_Drive();
////brake is active, and we continue in this state until rpm reaches 0.
////If a stop is all we needed, we are essentially done. If we are going a new direction
////however we set the direction pin, and then set directions to be =
//
//if ( this_rpm < 1 && c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_CW) //(M03)
//{
////if this is a new direction, set the direction pin, and then match the directions
////between the driver and the parser
//Spindle_Controller::c_driver::Drive_Control.direction = c_Parser::spindle_input.rotation_direction;
//
////Set direction to forward
//Spindle_Controller::c_driver::Forward_Drive();
////Set state to accelerate
//Spindle_Controller::c_driver::Set_State(STATE_BIT_ACCELERATE);
////Clear decelerate bit
//Spindle_Controller::c_driver::Clear_State(STATE_BIT_DECELERATE);
//}
////If the spindle was rotating, and then set to an off state, the brake will remain on.
//}
//break;
//case PARSER_DIRECTION_CW: //<--drive is currently in CW rotation (M03)
//{
///*
//since we were in a rotating state, and now we are going to be stopping/changing direction,
//this is obviously deceleration.
//*/
////Set state to decelerate
//Spindle_Controller::c_driver::Set_State(STATE_BIT_DECELERATE);
//
////new direction is either stop, or ccw. Either way, we must brake first
//Spindle_Controller::c_driver::Brake_Drive();
////brake is active, and we continue in this state until rpm reaches 0.
////If a stop is all we needed, we are essentially done. If we are going a new direction
////however we set the direction pin, and then set directions to be =
//
//if ( this_rpm < 1 && c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_CCW) //(M04)
//{
////if this is a new direction, set the direction pin, and then match the directions
////between the driver and the parser
//Spindle_Controller::c_driver::Drive_Control.direction = c_Parser::spindle_input.rotation_direction;
//
////Set direction to reverse
//Spindle_Controller::c_driver::Reverse_Drive();
////Set state to accelerate
//Spindle_Controller::c_driver::Set_State(STATE_BIT_ACCELERATE);
////Clear decelerate bit
//Spindle_Controller::c_driver::Clear_State(STATE_BIT_DECELERATE);
//}
////If the spindle was rotating, and then set to an off state, the brake will remain on.
//}
//break;
//case PARSER_DIRECTION_OFF: //<--drive is currently not rotating at all (we think) (M05)
//{
////This one is simple. Just set the direction and enable the drive. Just in case
////we check the rpm anyway, because we expect spindle to not be rotating.
//
//if ( this_rpm > 0)
//{
//illegal_rotations++;
////rotation detected but we dont expect it while in an off state
//break; //<--brake out of the switch/case here. loop again
//}
//
////new direction is either stop, cw/ccw. Either way, brake first
//Spindle_Controller::c_driver::Brake_Drive();
////assume new rpm of zero for now
//Spindle_Controller::c_driver::Drive_Control.target_rpm = 0; //(M05) spindle stop
//Spindle_Controller::c_driver::Drive_Control.target_rpm = c_Parser::spindle_input.target_rpm;
//
////Set the rotation directions to match
//Spindle_Controller::c_driver::Drive_Control.direction = c_Parser::spindle_input.rotation_direction;
//
//if(c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_CW) //(M03)
//{
//Spindle_Controller::c_driver::Forward_Drive();
//}
//
//if(c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_CCW) //(M04)
//{
//Spindle_Controller::c_driver::Reverse_Drive();
//}
//
////since we were in an off state, and now we are going to be rotating, this is obviously acceleration
////Set state to accelerate
//Spindle_Controller::c_driver::Set_State(STATE_BIT_ACCELERATE);
////Clear decelerate bit
//Spindle_Controller::c_driver::Clear_State(STATE_BIT_DECELERATE);
//}
//break;
//}
////With every loop check the RPM
//this_rpm = c_Encoder_RPM::CurrentRPM();
//
///*
//When we do NOT expect rotations, we count the number of times we looped and detected movement.
//If this value reaches the maximum, we will throw an error. This might happen if the operator
//is moving the spindle by hand when we expect it to be off, and an M3/M4 command is trying to
//execute. It may also happen if there is electrical interference between the encoder	and the controller.
//*/
//
//if (illegal_rotations>650) //<-- processor will loop incredibly fast. 650 should be hit in about .00001 seconds
//{
//return NGC_Machine_Errors::SPINDLE_ROTATION_NOT_EXPECTED;
//}
////Last state to check. We were rotating, and now we have commanded to stop
//if (c_Parser::spindle_input.rotation_direction == PARSER_DIRECTION_OFF)
//{
////Since our brake is on, we can just let it come to a stop. Nothing else for us to do.
////Set the rotation directions to match
//Spindle_Controller::c_driver::Drive_Control.direction = c_Parser::spindle_input.rotation_direction;
//break; //<--brake out of our loop
//}
//}
//
//if (Spindle_Controller::c_driver::Drive_Control.direction == PARSER_DIRECTION_OFF)
//return NGC_Machine_Errors::OK;
//
//Spindle_Controller::c_driver::Process_State(this_rpm);
//
//return NGC_Machine_Errors::OK;
//}
//
//void Spindle_Controller::c_driver::Process_State(float current_rpm)
//{
///*
//If we get to here, the spindle is expected to be on. Keep calculating the PID loop.
//any input from the host that requires a direction change, stop, rpm adjustment, feed rate
//change will cause the PID to update automatically.
//*/
//
//
////If we don't call release_drive, the pwm output will be ignored.
//if (Spindle_Controller::c_driver::Get_State(STATE_BIT_BRAKE)) Spindle_Controller::c_driver::Release_Drive();
//
////Set the drive to enabled if it was disabled.
//if (Spindle_Controller::c_driver::Get_State(STATE_BIT_DISABLE)) Spindle_Controller::c_driver::Enable_Drive();
//
//
//if (Spindle_Controller::c_driver::Get_State(STATE_BIT_ACCELERATE))
//{
//if (accel_output< c_PID::spindle_terms.Min_Val)
//{
//accel_output = c_PID::spindle_terms.Min_Val;
//}
//Spindle_Controller::c_driver::Set_Acceleration(current_rpm);
//}
//else if (Spindle_Controller::c_driver::Get_State(STATE_BIT_DECELERATE))
//{
//Spindle_Controller::c_driver::Set_Decelerate(current_rpm);
//}
//else
//{
////save current accel value in case we increase rpm from here.
//accel_output = Spindle_Controller::c_driver::current_output;
////If we are above X rpm, switch to average rpm. It is more stable
//if (current_rpm>500)
//{
//current_rpm = c_Encoder_RPM::CurrentRPM();
//}
//Spindle_Controller::c_driver::current_output = c_PID::Calculate(current_rpm,Spindle_Controller::c_driver::Drive_Control.target_rpm,c_PID::spindle_terms);
//}
//
//OCR0A = Spindle_Controller::c_driver::current_output;
//}
//
//void Spindle_Controller::c_driver::Set_Acceleration(float current_rpm)
//{
//if (c_Time_Keeper::millis()>accel_millis)
//{
////accel_output grows fast at start, and slows as speed is reached
//
//accel_output += (1-(current_rpm/Spindle_Controller::c_driver::Drive_Control.target_rpm));
////let the pid calculate so that when we switch from accel to run, the PID is already in synch
//c_PID::Calculate(current_rpm,Spindle_Controller::c_driver::Drive_Control.target_rpm,c_PID::spindle_terms);
//
////We have either hit the rpm, or accel has wrapped.
//if ( current_rpm >= Spindle_Controller::c_driver::Drive_Control.target_rpm || accel_output < Spindle_Controller::c_driver::current_output)
//{
//Spindle_Controller::c_driver::Clear_State(STATE_BIT_ACCELERATE);
//accel_output = Spindle_Controller::c_driver::current_output;
//}
//Spindle_Controller::c_driver::current_output = accel_output;
//accel_millis = c_Time_Keeper::millis()+Spindle_Controller::c_driver::Drive_Control.accel_time;
//}
//}
//
//void Spindle_Controller::c_driver::Set_Decelerate(float current_rpm)
//{
//static uint32_t last_millis = 0;
//
//if (c_Time_Keeper::millis()>last_millis)
//{
//Spindle_Controller::c_driver::current_output = c_PID::Calculate(current_rpm,Spindle_Controller::c_driver::Drive_Control.target_rpm,c_PID::spindle_terms);
//last_millis = c_Time_Keeper::millis();
//}
//}