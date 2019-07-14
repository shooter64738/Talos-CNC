/* 
* Machine_Parameters_Process.cpp
*
* Created: 7/12/2019 9:07:07 PM
* Author: Family
*/


#include "Machine_Parameters_Process.h"

//I prefer these in an array, but the avr will not hold an array large enough. I am only returning parameters I need at the moment
void Talos::Machine_Parameters_Process::set_parameter(uint16_t parameter,float value)
{
	switch (parameter)
	{
		case HIGH_SPEED_PECK_DISTANCE:
		//no array to set right now..
		//return 1;
		break;
		
	}
}

float Talos::Machine_Parameters_Process::get_parameter(uint16_t parameter)
{
	switch (parameter)
	{
		case HIGH_SPEED_PECK_DISTANCE:
		return 1;
		break;
		
	}
}
