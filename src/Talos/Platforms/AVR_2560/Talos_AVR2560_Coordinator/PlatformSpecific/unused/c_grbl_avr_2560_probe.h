/*
* c_grbl_avr_2560_limits.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/
#include "../../../Talos.h"
#ifdef __AVR_ATmega2560__
#include <avr/io.h>

#ifndef __C_GRBLAVR_2560_PROBE_H__
#define __C_GRBLAVR_2560_PROBE_H__

// Define probe switch input pin.
#define PROBE_DDR       DDRK
#define PROBE_PIN       PINK
#define PROBE_PORT      PORTK
#define PROBE_BIT       7  // MEGA2560 Analog Pin 15
#define PROBE_MASK      (1 << PROBE_BIT)
#define BITFLAG_INVERT_PROBE_PIN   bit(7)

namespace Hardware_Abstraction_Layer
{
	namespace Grbl
	{
		class Probe
		{
			//variables
			public:
			static uint8_t Pin_Mask_Invert;
			static uint8_t Pin_Values;
			static const uint8_t Pin_Mask = PROBE_MASK;
			protected:
			private:

			//functions
			public:
			static void initialize();
			static void probe_configure_invert_mask(uint8_t is_probe_away);
			protected:
			private:

		};
	};
};
#endif
#endif