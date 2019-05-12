#include "c_motion_core.h"
#include "../helpers.h"

BinaryRecords::s_motion_control_settings Motion_Core::Settings::_Settings;

//int16_t Motion_Core::Settings.steps_per_mm[N_AXIS]{160};
//float Motion_Core::Settings.acceleration[N_AXIS]{ (100.0 * 60 * 60)};
//float Motion_Core::Settings.max_rate [N_AXIS]{ 5000};
//float Motion_Core::Settings.junction_deviation = 0.01;
//float Motion_Core::Settings.arc_tolerance = 0.002;
//uint16_t Motion_Core::Settings.pulse_length = 10;
//float Motion_Core::Settings.back_lash_comp_distance[N_AXIS]{0.0};

void Motion_Core::initialize()
{
	for (uint8_t i = 0; i < N_AXIS; i++)
	{
		Motion_Core::Settings::_Settings.steps_per_mm[i] = 160;
		Motion_Core::Settings::_Settings.acceleration[i] = (100.0 * 60 * 60);
		Motion_Core::Settings::_Settings.max_rate[i] = 8000;
		//arbitrary for testing
		Motion_Core::Settings::_Settings.back_lash_comp_distance[i] = 55;
	}
	
	Motion_Core::Settings::_Settings.pulse_length = 4;

}

uint8_t Motion_Core::get_direction_pin_mask(uint8_t axis_idx)
{
	return ((1 << axis_idx));
	//if (axis_idx == Y_AXIS) return ((1 << Y_DIRECTION_BIT));
	//if (axis_idx == Z_AXIS) return ((1 << Z_DIRECTION_BIT));
	//if (axis_idx == A_AXIS) return ((1 << A_DIRECTION_BIT));
	//if (axis_idx == B_AXIS) return ((1 << B_DIRECTION_BIT));
	//if (axis_idx == C_AXIS) return ((1 << C_DIRECTION_BIT));
	
}

float Motion_Core::convert_delta_vector_to_unit_vector(float *vector)
{
	uint8_t idx;
	float magnitude = 0.0;
	for (idx = 0; idx < N_AXIS; idx++)
	{
		if (vector[idx] != 0.0)
		{
			magnitude += vector[idx] * vector[idx];
		}
	}
	magnitude = sqrt(magnitude);
	float inv_magnitude = 1.0 / magnitude;
	for (idx = 0; idx < N_AXIS; idx++)
	{
		vector[idx] *= inv_magnitude;
	}
	return (magnitude);
}

float Motion_Core::limit_value_by_axis_maximum(float *max_value, float *unit_vec)
{
	uint8_t idx;
	float limit_value = SOME_LARGE_VALUE;
	for (idx = 0; idx < N_AXIS; idx++)
	{
		if (unit_vec[idx] != 0)
		{  // Avoid divide by zero.
			limit_value = min(limit_value, fabs(max_value[idx] / unit_vec[idx]));
		}
	}
	return (limit_value);
}