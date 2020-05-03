/*
* c_core_avr_2560.cpp
*
* Created: 2/27/2019 3:47:52 PM
* Author: jeff_d
*/


#include "c_spi_STM32F103.h"


void Hardware_Abstraction_Layer::Spi::initialize()
{

}

void Hardware_Abstraction_Layer::Spi::init_master_interrupt()
{
	//enable spi as master here
	

}

void Hardware_Abstraction_Layer::Spi::init_master_no_interrupt()
{
		

}

// Initialize SPI Slave Device
void Hardware_Abstraction_Layer::Spi::init_slave()
{
	
}


uint8_t Hardware_Abstraction_Layer::Spi::rx_tx(uint8_t data)
{
	//read spi
	// Return received data
	return(0);
}

bool Hardware_Abstraction_Layer::Spi::has_data()
{
	
}

//A spi interrupt was here
