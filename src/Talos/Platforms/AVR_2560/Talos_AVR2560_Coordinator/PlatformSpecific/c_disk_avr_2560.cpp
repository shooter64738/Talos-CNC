/*
* c_core_avr_2560.cpp
*
* Created: 2/27/2019 3:47:52 PM
* Author: jeff_d
*/


#include "c_disk_avr_2560.h"
#include "disk_support/diskio.h"
#include <avr/interrupt.h>
#include "../../../../Coordinator/coordinator_hardware_def.h"
#include "../../../../Coordinator/Processing/Main/Main_Process.h"

FRESULT fr = FR_OK;
uint8_t Hardware_Abstraction_Layer::Disk::initialize()
{
	OCR0A = F_CPU / 1024 / 100 - 1;
	TCCR0A = (1<<WGM01);
	TCCR0B = 0b101;
	TIMSK0 = (1<<OCIE0A);
	
	//fr = disk_initialize(0);// & STA_NOINIT;		/* Clear STA_NOINIT */;
	FATFS fs;
	
	FIL file1;
	UINT bw;
	
	fr = f_mount(&fs, "", 1);
	if (fr == FR_OK)
	{
		if (fr == FR_OK)
		{
			fr = f_open(&file1, "coord.txt", FA_WRITE | FA_CREATE_ALWAYS |FA_OPEN_APPEND);
			if (fr == FR_OK)
			{
				fr = f_write(&file1, "testing\r\n\0", 10, &bw);
			}
		}
		
	}
	
	
	Talos::Coordinator::Main_Process::host_serial.print_int32(fr);
	return (uint8_t) fr;
}


void Hardware_Abstraction_Layer::Disk::write(const char * filename, char * buffer, e_file_modes mode)
{
}
