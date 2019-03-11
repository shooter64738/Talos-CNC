/*
* c_grbl_win_limits.cpp
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

//#include <avr/io.h>
//#include <avr/interrupt.h>
#include "c_grbl_win_probe.h"
#include "../../../helpers.h"
#include "../../../GRBL/c_settings.h"

uint8_t Hardware_Abstraction_Layer::Grbl::Probe::Pin_Mask_Invert;
uint8_t Hardware_Abstraction_Layer::Grbl::Probe::Pin_Values;


void Hardware_Abstraction_Layer::Grbl::Probe::initialize()
{
	PROBE_DDR &= ~(PROBE_MASK); // Configure as input pins
	#ifdef DISABLE_PROBE_PIN_PULL_UP
	PROBE_PORT &= ~(PROBE_MASK); // Normal low operation. Requires external pull-down.
	#else
	PROBE_PORT |= PROBE_MASK;    // Enable internal pull-up resistors. Normal high operation.
	#endif
	probe_configure_invert_mask(false); // Initialize invert mask.
}
void Hardware_Abstraction_Layer::Grbl::Probe::probe_configure_invert_mask(uint8_t is_probe_away)
{
	Hardware_Abstraction_Layer::Grbl::Probe::Pin_Mask_Invert = 0; // Initialize as zero.
	if (bit_isfalse(c_settings::settings.flags, BITFLAG_INVERT_PROBE_PIN))
	{
		Hardware_Abstraction_Layer::Grbl::Probe::Pin_Mask_Invert ^= PROBE_MASK;
	}
	if (is_probe_away)
	{
		Hardware_Abstraction_Layer::Grbl::Probe::Pin_Mask_Invert ^= PROBE_MASK;
	}
}