/* 
* c_coolant.h
*
* Created: 3/6/2019 10:08:49 AM
* Author: jeff_d
*/


#ifndef __C_COOLANT_H__
#define __C_COOLANT_H__

#include <stdint.h>
#include <stdbool.h>
#include "..\helpers.h"
#include "hardware_def.h"

#define COOLANT_NO_SYNC     false
#define COOLANT_FORCE_SYNC  true

//#define COOLANT_STATE_DISABLE   0  // Must be zero
//#define COOLANT_STATE_FLOOD     bit(0)
//#define COOLANT_STATE_MIST      bit(1)

class c_coolant
{
//variables
public:
protected:
private:

//functions
public:
	// Initializes coolant control pins.
	static void coolant_init();

	// Returns current coolant output state. Overrides may alter it from programmed state.
	static uint8_t coolant_get_state();

	// Immediately disables coolant pins.
	static void coolant_stop();

	// Sets the coolant pins according to state specified.
	static void coolant_set_state(uint8_t mode);

	// G-code parser entry-point for setting coolant states. Checks for and executes additional conditions.
	static void coolant_sync(uint8_t mode);
protected:
private:

}; //c_coolant

#endif //__C_COOLANT_H__
