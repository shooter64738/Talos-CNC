#include <math.h>

#ifndef NGC_MATH_CONSTANTS_H
#define NGC_MATH_CONSTANTS_H

#define NEAR_ONE 0.9999
#define NEAR_ZERO 0.0001

#define TWOPI 6.2831853071795865
#define RAD2DEG_CONST 57.2957795130823209
#define DEG2RAD_CONST 0.0174532925199
#define M_PI 3.14159265358979323846
#define DEGREE_TO_RADIAN(r) ((r)*M_PI/180.0)
#define RADIAN_TO_DEGREE(r) ((r)*180.0/M_PI)

static void _check_near_zero(float *value)
{
	if (value < 0)
		*value = *value > -NEAR_ZERO ? 0 : *value;
	else
		*value = *value < NEAR_ZERO ? 0 : *value;

};

static bool _check_near_one(float *value)
{
	if (*value >= NEAR_ONE)
		return true;
};

static bool _check_near_int(int *int_value, float *value)
{
	*int_value = (int)(*value + .5);
	return fabs(*int_value - *value) < NEAR_ZERO;
}

#endif