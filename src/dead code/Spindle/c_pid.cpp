/* 
* c_pid.cpp
*
* Created: 2/22/2019 5:28:22 PM
* Author: Family
*/


#include "c_pid.h"
#include "c_settings.h"

Spindle_Controller::c_pid::s_pid_terms Spindle_Controller::c_pid::servo_terms;
Spindle_Controller::c_pid::s_pid_terms Spindle_Controller::c_pid::spindle_terms;

void Spindle_Controller::c_pid::Initialize()
{
	//Spindle_Controller::c_pid::servo_terms.Kp=0.009;
	//Spindle_Controller::c_pid::servo_terms.Ki=0.00001;
	//Spindle_Controller::c_pid::servo_terms.Kd=-0.0;
	Spindle_Controller::c_pid::servo_terms.KITerm=0;
	Spindle_Controller::c_pid::servo_terms.lastInput = 0;
	Spindle_Controller::c_pid::servo_terms.NewOutPut = 0;
	Spindle_Controller::c_pid::servo_terms.Err=0;
	Spindle_Controller::c_pid::servo_terms.Max_Val=255;
	Spindle_Controller::c_pid::servo_terms.Min_Val=100;
	
	//c_PID::spindle_terms.Kp=0.05;
	//c_PID::spindle_terms.Ki=0.0011;
	//c_PID::spindle_terms.Kd=0.0001;
	//Spindle_Controller::c_pid::spindle_terms.Kp=.005;
	//Spindle_Controller::c_pid::spindle_terms.Ki=0.005;
	//Spindle_Controller::c_pid::spindle_terms.Kd=0.001;
	Spindle_Controller::c_pid::spindle_terms.KITerm=0;
	Spindle_Controller::c_pid::spindle_terms.lastInput = 0;
	Spindle_Controller::c_pid::spindle_terms.NewOutPut = 0;
	Spindle_Controller::c_pid::spindle_terms.Err=0;
	Spindle_Controller::c_pid::spindle_terms.Max_Val=255;
	Spindle_Controller::c_pid::spindle_terms.Min_Val=100;
	
	//c_Spindle_Drive::current_output = MINIMUM_OUTPUT;
	
}

void Spindle_Controller::c_pid::Clear(Spindle_Controller::c_pid::s_pid_terms &terms)
{
	terms.KITerm=0;
	terms.lastInput = 0;
	terms.NewOutPut = terms.Min_Val;
	terms.Err=0;
}

uint8_t Spindle_Controller::c_pid::Calculate(int32_t current, int32_t target, s_pid_terms &terms)
{
	
	double error = target - current;
	terms.KITerm+= (Spindle_Controller::c_settings::serializer.values.pI * error);
	if(terms.KITerm > terms.Max_Val) terms.KITerm= terms.Max_Val;
	else if(terms.KITerm < terms.Min_Val) terms.KITerm= terms.Min_Val;
	
	double dInput = (current - target);
	
	/*Compute PID Output*/
	terms.NewOutPut = Spindle_Controller::c_settings::serializer.values.pP * error + terms.KITerm - Spindle_Controller::c_settings::serializer.values.pD * dInput;
	
	if(terms.NewOutPut > terms.Max_Val) terms.NewOutPut = terms.Max_Val;
	else if(terms.NewOutPut < terms.Min_Val) terms.NewOutPut = terms.Min_Val;
	
	return terms.NewOutPut;
	
	//double error = target - current;
	//terms.KITerm += error;       //this is the integral part which eliminates the steady state error
	//if(terms.KITerm > terms.Max_Val)  //here we limit the integral part
	//terms.KITerm = terms.Max_Val;
	//if(terms.KITerm < terms.Min_Val)
	//terms.KITerm = terms.Min_Val;
	//
	////this can also be written like this
	//double output = terms.Kp*error + terms.Kd*(error - terms.Err) + terms.Ki*terms.KITerm;
	
	
	terms.Err = target - current;    //update the error
	//return output;
}
uint8_t Spindle_Controller::c_pid::Calculate(float current, float target, s_pid_terms &terms)
{
	double error = target - current;
	terms.KITerm+= (Spindle_Controller::c_settings::serializer.values.pI * error);
	if(terms.KITerm > terms.Max_Val) terms.KITerm= terms.Max_Val;
	else if(terms.KITerm < terms.Min_Val) terms.KITerm= terms.Min_Val;
	
	double dInput = (current - target);
	
	/*Compute PID Output*/
	terms.NewOutPut = Spindle_Controller::c_settings::serializer.values.pP 
	* error + terms.KITerm- Spindle_Controller::c_settings::serializer.values.pD * dInput;
	
	if(terms.NewOutPut > terms.Max_Val) terms.NewOutPut = terms.Max_Val;
	else if(terms.NewOutPut < terms.Min_Val) terms.NewOutPut = terms.Min_Val;
	
	return terms.NewOutPut;
	
	//double error = target - current;
	//
	////changeError = error - last_error; // derivative term
	//double changeError = error - terms.Err;
	////totalError += error; //accumalate errors to find integral term
	//terms.KITerm+= error;
	/////pidTerm = (Kp * error) + (Ki * totalError) + (Kd * changeError);//total gain
	//double output = (terms.Kp * error) + (terms.Ki * terms.KITerm) + (terms.Kd * changeError);//total gain
	////pidTerm = constrain(pidTerm, -255, 255);//constraining to appropriate value
	//if(output > terms.Max_Val) output = terms.Max_Val;
	//else if(output < terms.Min_Val) output = terms.Min_Val;
	////pidTerm_scaled = abs(pidTerm);//make sure it's a positive value
	////last_error = error;
	//terms.Err = error;
	//return output;
}
// default constructor



//// default constructor
//c_pid::c_pid()
//{
//} //c_pid
//
//// default destructor
//c_pid::~c_pid()
//{
//} //~c_pid
