/*
* c_grbl_avr_2560_limits.h
*
* Created: 3/7/2019 10:22:11 AM
* Author: jeff_d
*/
#include "../../../Talos.h"
#ifdef MSVC

#ifndef __C_GRBL_WIN_COOLANT_H__
#define __C_GRBL_WIN_COOLANT_H__

// Define flood and mist coolant enable output pins.
uint8_t COOLANT_FLOOD_DDR = 0;
uint8_t COOLANT_FLOOD_PORT = 0;
#define COOLANT_FLOOD_BIT   5 // MEGA2560 Digital Pin 8
uint8_t COOLANT_MIST_DDR = 0;
uint8_t COOLANT_MIST_PORT = 0;
#define COOLANT_MIST_BIT    6 // MEGA2560 Digital Pin 9


namespace Hardware_Abstraction_Layer
{
	namespace Grbl
	{
		class Coolant
		{
			//variables
		public:
		protected:
		private:

			//functions
		public:
			static void initialize();
			static void stop();
		protected:
		private:

		}; 
	};
};
#endif
#endif