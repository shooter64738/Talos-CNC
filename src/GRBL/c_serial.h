/*
* c_serial.h
*
* Created: 3/5/2019 3:18:12 PM
* Author: jeff_d
*/


#ifndef __C_SERIAL_H__
#define __C_SERIAL_H__

#include <stdint.h>
#include <stdint.h>


class c_serial
{

	#ifndef RX_BUFFER_SIZE
	#define RX_BUFFER_SIZE 255
	#endif
	#ifndef TX_BUFFER_SIZE
	#define TX_BUFFER_SIZE 255
	#endif

	#define SERIAL_NO_DATA 0xff


	//variables
	public:
	protected:
	private:

	//functions
	public:
	static void serial_init();

	// Writes one byte to the TX serial buffer. Called by main program.
	static void serial_write(uint8_t data);

	static void serial_tx_event(uint8_t data);
	// Fetches the first byte in the serial read buffer. Called by main program.
	static uint8_t serial_read();

	static void serial_rx_event(uint8_t data);
	// Reset and empty data in read buffer. Used by e-stop and reset.
	static void serial_reset_read_buffer();

	// Returns the number of bytes available in the RX serial buffer.
	static uint8_t serial_get_rx_buffer_available();

	// Returns the number of bytes used in the RX serial buffer.
	// NOTE: Deprecated. Not used unless classic status reports are enabled in config.h.
	static uint8_t serial_get_rx_buffer_count();

	// Returns the number of bytes used in the TX serial buffer.
	// NOTE: Not used except for debugging and ensuring no TX bottlenecks.
	static uint8_t serial_get_tx_buffer_count();
	protected:
	private:

}; //c_serial

#endif //__C_SERIAL_H__
