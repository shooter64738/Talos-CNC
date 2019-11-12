/*
* Main_Process.cpp
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
*/


#include "Main_Process.h"

c_Serial Talos::Motion::Main_Process::host_serial;

void Talos::Motion::Main_Process::initialize()
{
	uint16_t ret_code = 0;
	
	Hardware_Abstraction_Layer::Core::initialize();//<--start interrupts on hardware
	Talos::Motion::Main_Process::host_serial = c_Serial(0, 115200); //<--Connect to host
	host_serial.print_string("Motion Core Boot...\r\n");
	
	//host_serial.print_string("SPI init...");
	//Talos::Motion::Main_Process::report(ret_code =Hardware_Abstraction_Layer::SPI::initialize());
	
	
	host_serial.print_string("Disk init...");
	Talos::Motion::Main_Process::report(ret_code = Hardware_Abstraction_Layer::Disk::initialize());
		
	uint8_t spi_data;
	uint8_t new_data = 0;
	spi_data = 'a';
	host_serial.print_string("sending\r\n");
	new_data = Hardware_Abstraction_Layer::SPI::send_byte(&spi_data);
	
	host_serial.print_string("spi_read");
	host_serial.print_int32(new_data);
	host_serial.print_string("\r\nchar:");
	host_serial.Write(new_data);
	host_serial.print_string("\r\n");
	while(1)
	{
		if (Hardware_Abstraction_Layer::SPI::spi_buffer.has_data())
		{
			host_serial.print_string("have data!!\r\n");
			host_serial.print_string("\r\nbuffer char:");
			host_serial.Write(Hardware_Abstraction_Layer::SPI::spi_buffer.get());
			host_serial.print_string("\r\n");
		}
	}
}

void Talos::Motion::Main_Process::run()
{
	
}

void Talos::Motion::Main_Process::report(uint16_t ret_code)
{
	if (ret_code)
	{
		host_serial.print_string("Error code: ");
		host_serial.print_int32(ret_code);
		host_serial.print_string("\r\n");
		host_serial.print_string("System halted");
		while(1)
		{
			if (Hardware_Abstraction_Layer::Disk::time_out_ticks > 0)
			{
				host_serial.print_int32(Hardware_Abstraction_Layer::Disk::time_out_ticks);
				Hardware_Abstraction_Layer::Disk::time_out_ticks = 0;
			}
		}
	}
	else
	{
		host_serial.print_string("Ok\r\n");
	}
}