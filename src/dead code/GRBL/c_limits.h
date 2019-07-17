/* 
* c_limits.h
*
* Created: 3/6/2019 12:38:40 PM
* Author: jeff_d
*/


#ifndef __C_LIMITS_H__
#define __C_LIMITS_H__
#include <stdint.h>

class c_limits
{
//variables
public:

protected:
private:

//functions
public:
// Initialize the limits module
static void limits_init();
// Disables hard limits.
static void limits_disable();
// Returns limit state as a bit-wise uint8 variable.
static uint8_t limits_get_state(uint8_t port_values);
static void limits_limit_pin_event_default(uint8_t port_values);
// Perform one portion of the homing cycle based on the input settings.
static void limits_go_home(uint8_t cycle_mask);
// Check for soft limit violations
static void limits_soft_check(float *target);
protected:
private:

}; //c_limits

#endif //__C_LIMITS_H__
