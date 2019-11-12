/*
* c_serial_avr_2560.h
*
* Created: 2/27/2019 3:54:31 PM
* Author: jeff_d
*/


#ifndef __C_SPI_ARM_3X8E_H__
#define __C_SPI_ARM_3X8E_H__

#include <stdint.h>
#include <stddef.h>

#include "c_core_arm_3x8e.h"
#include "..\..\..\..\c_ring_template.h"
#define SPI_BUFFER_SIZE 256
namespace Hardware_Abstraction_Layer
{
	class SPI
	{
		//enum class e_spi_cs_behavior {
		///** CS does not rise until a new transfer is requested on different chip select. */
		//SPI_CS_KEEP_LOW = SPI_CSR_CSAAT,
		///** CS rises if there is no more data to transfer. */
		//SPI_CS_RISE_NO_TX = 0,
		///** CS is de-asserted systematically during a time DLYBCS. */
		//SPI_CS_RISE_FORCED = SPI_CSR_CSNAAT
		//};
		//
		//variables
		public:
		static c_ring_buffer<uint8_t> spi_buffer;
		protected:
		private:

		//functions
		public:
		static uint16_t initialize();
		static void initialize_general();
		static void initialize_disk();
		static void initialize2();
		static uint8_t send_byte(uint8_t * data);
		static int sd_send_byte(int data);
		static void start_as_master();
		protected:
		private:
		

	};
};
#endif //__SAM3X8E__
