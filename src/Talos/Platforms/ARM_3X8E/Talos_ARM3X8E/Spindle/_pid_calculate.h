/*
* _pid_calculate.h
*
* Created: 9/26/2019 9:51:50 PM
*  Author: Family
*/


#ifndef _PID_CALCULATE_H_
#define _PID_CALCULATE_H_
#include <stdint.h>

namespace Spindle
{
	struct s_pid_terms
	{
		float Kp;
		float Ki;
		float Kd;
		
		float KITerm;
		float lastInput;
		float NewOutPut;
		float Err;
		int32_t Max_Val;
		int32_t Min_Val;
		
		float calculate(float current, float target)
		{
			Err = target - current;
			KITerm+= (Ki * Err);
			if(KITerm > Max_Val) KITerm= Max_Val;
			else if(KITerm < Min_Val) KITerm= Min_Val;
			
			double dInput = (current - target);
			
			/*Compute PID Output*/
			NewOutPut = Kp * Err + KITerm- Kd * dInput;
			
			if(NewOutPut > Max_Val) NewOutPut = Max_Val;
			else if(NewOutPut < Min_Val) NewOutPut = Min_Val;
			
			return NewOutPut;
		}
	};
};

#endif /* _PID_CALCULATE_H_ */