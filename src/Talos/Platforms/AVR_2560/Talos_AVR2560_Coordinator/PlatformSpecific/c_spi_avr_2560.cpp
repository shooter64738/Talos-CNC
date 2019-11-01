/*
* c_core_avr_2560.cpp
*
* Created: 2/27/2019 3:47:52 PM
* Author: jeff_d
*/


#include "c_spi_avr_2560.h"
#include <avr/interrupt.h>

void Hardware_Abstraction_Layer::Spi::initialize()
{

}

void Hardware_Abstraction_Layer::Spi::init_master_interrupt()
{
	// Set MOSI, SCK as Output
	DDRB=(1<<DDB5)|(1<<DDB3)|(1<<DDB2);
	
	// Enable SPI, Set as Master
	// Prescaler: Fosc/16, Enable Interrupts
	//The MOSI, SCK pins are as per ATMega8
	SPCR=(1<<SPE)|
	(1<<MSTR)|
	(1<<SPR0)|
	(1<<SPIE);
	

}

void Hardware_Abstraction_Layer::Spi::init_master_no_interrupt()
{
	// Set MOSI, SCK, SS as Output
	DDRB=(1<<DDB5)|(1<<DDB3)|(1<<DDB2);
	
	// Enable SPI, Set as Master
	// Prescaler: Fosc/16, Enable Interrupts
	//The MOSI, SCK pins are as per ATMega8
	SPCR = (1<<SPE)|
	(1<<MSTR)|
	(1<<SPR0);
	

}

// Initialize SPI Slave Device
void Hardware_Abstraction_Layer::Spi::init_slave()
{
	DDRB = (1<<DDB6);     //MISO as OUTPUT
	SPCR = (1<<SPE);   //Enable SPI
}


uint8_t Hardware_Abstraction_Layer::Spi::rx_tx(uint8_t data)
{
	// Load data into the buffer
	SPDR = data;
	
	//Wait until transmission complete
	while(!(SPSR & (1<<SPIF) ));
	
	// Return received data
	return(SPDR);
}

bool Hardware_Abstraction_Layer::Spi::has_data()
{
	
}

ISR(SPI_STC_vect)
{
	// Code to execute
	// whenever transmission/reception
	// is complete.
}
