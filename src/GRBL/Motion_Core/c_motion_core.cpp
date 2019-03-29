#include "c_motion_core.h"

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