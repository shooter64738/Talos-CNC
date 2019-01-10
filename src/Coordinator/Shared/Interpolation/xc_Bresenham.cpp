/*
*  c_Bresham.h - NGC_RS274 controller.
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


#include "c_Bresenham.h"
#include "..\Settings\c_settings.h"

// default constructor
c_Bresenham::c_Bresenham()
{
} //c_bresenham

// default destructor
c_Bresenham::~c_Bresenham()
{
} //~c_bresenham

/*
Initialize should be called before this instance gets used to configure it for the block it is being used for.
'targets' should be an array of int32_t type with MACHINE_AXIS_COUNT elements in it. The targets are copied
to an internal array because it is assume that 'targets' are coming from ngc block data which will be destroyed
after being pushed into interpolation.
*/

void c_Bresenham::initialize(int32_t * targets)
{

	faulted = false;
	this->control_axis_index = 0;
	this->step_bits = 0;
	this->direction_bits = 0;

	for (uint8_t axis_id = 0; axis_id < MACHINE_AXIS_COUNT; axis_id++)
	{
		this->positions[axis_id] = 0;
		this->incriment_sign[axis_id] = 0;
		this->accumulators[axis_id] = 0;
		this->incriment_sign[axis_id] = (targets[axis_id] < 0 ? -1 : (targets[axis_id] > 0 ? 1 : 0));
		this->incriment_sign[axis_id] < 0 ? this->direction_bits = BitSet(this->direction_bits, axis_id) : this->direction_bits = BitClr(this->direction_bits, axis_id);
		this->targets[axis_id] = abs((int32_t)targets[axis_id]);
		//save a few bytes here by only finding the axis id that is the highest so it can be used to pull the appropriate
		//target value.
		this->control_axis_index = this->targets[axis_id] > this->targets[this->control_axis_index] ? axis_id : this->control_axis_index;
	}

	//now that we know what the control axis id is, loop again and preconfigure the lead error factors
	for (uint8_t axis_id = 0; axis_id < MACHINE_AXIS_COUNT; axis_id++)
	{
		//pre-calculate these so its faster in the algorithm.
		this->lead_factor_errors[axis_id] = ((float)this->targets[axis_id] / (float)this->targets[this->control_axis_index]);
	}
}

//This 'steps' through the algorithm. (could be called by hardware to generate stepper/servo pulses)
void c_Bresenham::step()
{
	int32_t steps[8];
	this->step_bits = 0;
	for (uint8_t axis_id = 0; axis_id < MACHINE_AXIS_COUNT; axis_id++)
	{

		steps[axis_id] = positions[axis_id] >> 3;

		this->accumulators[axis_id] += this->targets[axis_id];
		if (this->accumulators[axis_id] >= this->targets[this->control_axis_index])
		{
			this->accumulators[axis_id] -= this->targets[this->control_axis_index];
			//positions[axis_id] += this->incriment_sign[axis_id];
			positions[axis_id] ++;
			this->step_bits = BitSet(this->step_bits, axis_id);
		}
	}
}

/*
During interpolation, a stepper motor can lose a step. When that occurs the end user may not be aware of it.
If feedback is used, and its configured for closed loop, this algorithm will 'predict' where the subordinate
axis positions should be based on a factor of difference in the target end points. If the predicted lead
calculation results in a value that does not match the current position, then the interpolation is not in
synchronization. This will alert the user to that condition, and may cause the motion to stop. What happens
when an axis fault is detected is up to the user and how they want to handle such a condition. Generally
this condition will cause a hard stop of the machine and it will need to be powered down and reset to home.
NOTE:This will only work properly with a linear motion, not an arc. calculate_arc is specifically for arcs.
*/

/*
'current' should be an array of int32_t type with MACHINE_AXIS_COUNT elements in it. The values of current
would ideally be values from encoders or resolvers indicating the position of each axis in steps or ticks.
If using encoders with a CPR rate different than the steppers motion rate (800 steps per rev steppers/drivers
and 100 pulse per rev encoders) you cannot work directly off of the tick count. The pulse and encoder tick
counts must be converted to a distance first. If simple positional feedback is all that is desired, this can
be considered a loop back and will always match the position of the Bresenham steps.
*/
uint16_t c_Bresenham::calculate_line(int32_t * currents)
{
	int32_t predicted_lead = 0;

	//Loop through all axis values
	for (uint8_t axis_id = 0; axis_id < MACHINE_AXIS_COUNT; axis_id++)
	{
		//If a target value was set on the axis check its lead error
		if (this->targets[axis_id] != 0)
		{
			/*
			As noted above, if using stepper and encoders with different count resolutions, convert
			them to unit distances here and then compare.
			ie:
			predicted_lead = convert_encoder_to_unit(current[axis_id])
			 - convert_step-to_unit(((convert_encoder_to_unit(current[this->control_axis_index]) * this->lead_factor_errors[axis_id]) * this->incriment_sign[axis_id]));
			*/

			//predicted lead is where the axis should be if it stays in perfect interpolation through the motion.
			predicted_lead = currents[axis_id] - ((currents[this->control_axis_index] * this->lead_factor_errors[axis_id]))* this->incriment_sign[axis_id];

			if (predicted_lead != 0)
			{
				//Its expensive to convert steps to units, so only do it if we are not in perfect step.
				if (abs((float)((float)predicted_lead / c_motion_controller::configuration_settings.steps_per_mm[axis_id]) > c_settings::machine.interpolation_error_distance))
				{
					faulted = true;
					return (AXIS_FAULT_BASE_ERROR + axis_id);
				}
			}
		}
	}
	return NGC_Machine_Errors::OK;
}