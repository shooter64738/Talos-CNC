
/*
* c_grbl_avr_2560_limits.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/

#include "../../../Talos.h"
#ifdef MSVC
#ifndef __C_GRBL_AVR_2560_CONTROL_H__
#define __C_GRBL_AVR_2560_CONTROL_H__

#define CONTROL_DDR       DDRK
#define CONTROL_PIN       PINK
#define CONTROL_PORT      PORTK
#define CONTROL_RESET_BIT         0  // MEGA2560 Analog Pin 8
#define CONTROL_FEED_HOLD_BIT     1  // MEGA2560 Analog Pin 9
#define CONTROL_CYCLE_START_BIT   2  // MEGA2560 Analog Pin 10
#define CONTROL_SAFETY_DOOR_BIT   3  // MEGA2560 Analog Pin 11
#define CONTROL_INT       PCIE2  // Pin change interrupt enable pin
#define CONTROL_INT_vect  PCINT2_vect
#define CONTROL_PCMSK     PCMSK2 // Pin change interrupt register
#define CONTROL_MASK      ((1 << CONTROL_RESET_BIT) | (1 << CONTROL_FEED_HOLD_BIT) | (1 << CONTROL_CYCLE_START_BIT) | (1 << CONTROL_SAFETY_DOOR_BIT) )


#define CMD_RESET 0x80
#define CMD_STATUS_REPORT 0x81
#define CMD_CYCLE_START 0x82
#define CMD_FEED_HOLD 0x83
#define CMD_SAFETY_DOOR 0x84
#define CMD_JOG_CANCEL  0x85
#define CMD_DEBUG_REPORT 0x86 // Only when DEBUG enabled, sends debug report in '{}' braces.
#define CMD_FEED_OVR_RESET 0x90         // Restores feed override value to 100%.
#define CMD_FEED_OVR_COARSE_PLUS 0x91
#define CMD_FEED_OVR_COARSE_MINUS 0x92
#define CMD_FEED_OVR_FINE_PLUS  0x93
#define CMD_FEED_OVR_FINE_MINUS  0x94
#define CMD_RAPID_OVR_RESET 0x95        // Restores rapid override value to 100%.
#define CMD_RAPID_OVR_MEDIUM 0x96
#define CMD_RAPID_OVR_LOW 0x97
#define CMD_SPINDLE_OVR_RESET 0x99      // Restores spindle override value to 100%.
#define CMD_SPINDLE_OVR_COARSE_PLUS 0x9A
#define CMD_SPINDLE_OVR_COARSE_MINUS 0x9B
#define CMD_SPINDLE_OVR_FINE_PLUS 0x9C
#define CMD_SPINDLE_OVR_FINE_MINUS 0x9D
#define CMD_SPINDLE_OVR_STOP 0x9E
#define CMD_COOLANT_FLOOD_OVR_TOGGLE 0xA0
#define CMD_COOLANT_MIST_OVR_TOGGLE 0xA1


#define BITFLAG_HARD_LIMIT_ENABLE  bit(3)

namespace Hardware_Abstraction_Layer
{
	namespace Grbl
	{
		class Control
		{
			//variables
			public:
			static uint8_t Pin_Values;
			static const uint8_t Pin_Mask = CONTROL_MASK;
			
			protected:
			private:

			//functions
			public:
			static void initialize();
			static void _configure_control_input_pins();
			protected:
			private:

		}; 
	};
};
#endif 
#endif