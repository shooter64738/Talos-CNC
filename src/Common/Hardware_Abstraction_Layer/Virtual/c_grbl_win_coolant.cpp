/*
* c_grbl_win_limits.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

//#include <avr/io.h>
//#include <avr/interrupt.h>
#include "c_grbl_win_coolant.h"
#include "../../../helpers.h"


void Hardware_Abstraction_Layer::Grbl::Coolant::initialize()
{
	COOLANT_FLOOD_DDR |= (1 << COOLANT_FLOOD_BIT); // Configure as output pin.
	COOLANT_MIST_DDR |= (1 << COOLANT_MIST_BIT); // Configure as output pin.
	Hardware_Abstraction_Layer::Grbl::Coolant::stop();
}

void Hardware_Abstraction_Layer::Grbl::Coolant::stop()
{
#ifdef INVERT_COOLANT_FLOOD_PIN
	COOLANT_FLOOD_PORT |= (1 << COOLANT_FLOOD_BIT);
#else
	COOLANT_FLOOD_PORT &= ~(1 << COOLANT_FLOOD_BIT);
#endif
#ifdef INVERT_COOLANT_MIST_PIN
	COOLANT_MIST_PORT |= (1 << COOLANT_MIST_BIT);
#else
	COOLANT_MIST_PORT &= ~(1 << COOLANT_MIST_BIT);
#endif
}