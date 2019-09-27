/* 
* Velocity.cpp
*
* Created: 9/26/2019 9:49:51 PM
* Author: Family
*/


#include "Velocity.h"

Spindle::s_pid_terms Spindle::Velocity::velocity_terms;

void Spindle::Velocity::initialize()
{
	Spindle::Velocity::velocity_terms.Kp=0.0015;
	Spindle::Velocity::velocity_terms.Ki=0.0001;
	Spindle::Velocity::velocity_terms.Kd=0.01;
	Spindle::Velocity::velocity_terms.KITerm=0;
	Spindle::Velocity::velocity_terms.lastInput = 0;
	Spindle::Velocity::velocity_terms.NewOutPut = 0;
	Spindle::Velocity::velocity_terms.Err=0;
	Spindle::Velocity::velocity_terms.Max_Val=255;
	Spindle::Velocity::velocity_terms.Min_Val=0;
}