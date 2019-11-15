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
#include "disk_support/c_sdcard.h"
#include "disk_support/c_sdvolume.h"
#include "disk_support/c_sdfile.h"

static c_sdcard::s_card_info card_info;

//FRESULT fr = FR_OK;
uint8_t Hardware_Abstraction_Layer::Disk::initialize()
{

	OCR0A = F_CPU / 1024 / 1000 - 1;
	TCCR0A = (1<<WGM01);
	TCCR0B = 0b101;
	TIMSK0 = (1<<OCIE0A);
	card_info = c_sdcard::initialize();
	c_sdvolume volume = c_sdvolume();
	volume.initialize(1);
	
	c_sdfile files = c_sdfile();
	uint8_t ret_code = files.openRoot(&volume);
	Talos::Coordinator::Main_Process::host_serial.print_string("root open =  ");
	Talos::Coordinator::Main_Process::host_serial.print_int32(ret_code);
	Talos::Coordinator::Main_Process::host_serial.print_string("\r\n");
	
	

//
	//
	////fr = disk_initialize(0);// & STA_NOINIT;		/* Clear STA_NOINIT */;
	//FATFS fs;
	////
	//FIL file1;
	//UINT bw;
	////
	//Talos::Coordinator::Main_Process::host_serial.print_string("mount\r\n");
	//fr = f_mount(&fs, "", 1);
	//if (fr == FR_OK)
	//{
		//Talos::Coordinator::Main_Process::host_serial.print_string("open\r\n");
		//fr = f_open(&file1, "coord.txt", FA_WRITE | FA_CREATE_ALWAYS |FA_OPEN_APPEND);
		//if (fr == FR_OK)
		//{
			//Talos::Coordinator::Main_Process::host_serial.print_string("write\r\n");
			//fr = f_write(&file1, "testing\r\n\0", 10, &bw);
		//}
	//}
	////
	////
	//Talos::Coordinator::Main_Process::host_serial.print_int32(fr);
	return (uint8_t) 1; //fr;
}


void Hardware_Abstraction_Layer::Disk::write(const char * filename, char * buffer, e_file_modes mode)
{
}



//-----------------------------volume
