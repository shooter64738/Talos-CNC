/*
* c_torch_height.cpp
*
* Created: 1/26/2019 11:28:06 AM
* Author: jeff_d
*/


#include "c_torch_height.h"
#include "..\Common\Hardware_Abstraction_Layer\c_hal.h"
uint8_t Plasma_Control::c_torch_height::move_head(Plasma_Control::e_torch_positions Target)
{
	//drive the torch height stepper
	c_hal::driver.PNTR_SET_PRESCALER(8);
	c_hal::driver.PNTR_SET_TIMER_RATE(24000);
	c_hal::driver.PNTR_DRIVE(); //<--activate the driver.

	//Stepper is now in motion. We need to read the input for the limit switch AND contact on the work piece
	uint8_t result = 0;
	while (1)
	{
		result = c_hal::io.PNTR_GET_DIGITAL(DIGITAL_INPUT_PORT);
		if (result &(1 << LOWER_LIMIT_SWITCH_INPUT_PIN)
			|| result & (1 << MATERIAL_SWITCH_INPUT_PIN))
		{
			c_hal::driver.PNTR_DISABLE();//<--turn off the stepper;
			//determine which condition we detected. If we hit the limit switch before the material, then its an error
			if (result &(1 << LOWER_LIMIT_SWITCH_INPUT_PIN))
			{
				//Move the torch back to the top
				c_hal::driver.PNTR_DRIVE(); //<--activate the driver.
				while (1)
				{
					result = c_hal::io.PNTR_GET_DIGITAL(DIGITAL_INPUT_PORT);
					if (result &(1 << UPPER_LIMIT_SWITCH_INPUT_PIN))
					{
						c_hal::driver.PNTR_DISABLE();//<--turn off the stepper;
						return Plasma_Control::Errors::MATERIAL_OUT_OF_RANGE;
					}
				}
			}
		}
		return Plasma_Control::Errors::OK;
	}
}
//// default constructor
//c_torch_height::c_torch_height()
//{
//} //c_torch_height
//
//// default destructor
//c_torch_height::~c_torch_height()
//{
//} //~c_torch_height
