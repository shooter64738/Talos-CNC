/*
* c_core_avr_2560.cpp
*
* Created: 2/27/2019 3:47:52 PM
* Author: jeff_d
*/


#include "c_disk_avr_2560.h"

#include <avr/interrupt.h>
#include "../../../../Coordinator/coordinator_hardware_def.h"
#include "../../../../Coordinator/Processing/Main/Main_Process.h"
#include "disk_support/ff.h"

FATFS FatFs;
FRESULT FatResult;
DIR dir;
FIL file;
UINT bw = 0;

uint8_t Hardware_Abstraction_Layer::Disk::initialize()
{

	//OCR0A = F_CPU / 1024 / 1000 - 1;
	//TCCR0A = (1<<WGM01);
	//TCCR0B = 0b101;
	//TIMSK0 = (1<<OCIE0A);
	

	
	FatResult = f_mount(&FatFs,"",1);
	//Talos::Coordinator::Main_Process::host_serial.print_string("f_mount =  ");
	//Talos::Coordinator::Main_Process::host_serial.print_int32(FatResult);
	//Talos::Coordinator::Main_Process::host_serial.print_string("\r\n");

	return (uint8_t) FatResult;
}

uint8_t Hardware_Abstraction_Layer::Disk::load_configuration()
{
	FILINFO f_info;

	FatResult = f_stat ("m_setup.txt", &f_info);
	if (FatResult == FR_NO_FILE)
	{
		return 1;
	}
	
}

void Hardware_Abstraction_Layer::Disk::write(const char * filename, char * buffer, e_file_modes mode)
{
}



//-----------------------------volume
