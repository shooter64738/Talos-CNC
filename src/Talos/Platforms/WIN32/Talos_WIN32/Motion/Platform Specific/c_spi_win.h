/*
* c_core_avr_2560.h
*
* Created: 2/27/2019 3:47:53 PM
* Author: jeff_d
*/

#ifndef __C_SPI_WIN_H__
#define __C_SPI_WIN_H__

#include <stdint.h>

namespace Hardware_Abstraction_Layer
{
	class Spi
	{
		//variables
		public:
		protected:
		private:

		//functions
		public:
		static void initialize();
		
		static void init_master_interrupt();
		static void init_master_no_interrupt();
		static void init_slave();
		static uint8_t rx_tx(uint8_t data);
		static bool has_data();
		protected:
		private:
		

	};
};
#endif