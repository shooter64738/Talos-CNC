/*
* c_serial_avr_2560.cpp
*
* Created: 2/27/2019 3:54:31 PM
* Author: jeff_d
*/


#include "c_disk_arm_3x8e.h"
#include "..\..\..\..\Motion\Processing\Main\Main_Process.h"
#include "disk_support\ff.h"

Hardware_Abstraction_Layer::Disk::s_file_info Hardware_Abstraction_Layer::Disk::machine_param_file;
Hardware_Abstraction_Layer::Disk::s_file_info Hardware_Abstraction_Layer::Disk::global_named_param_file;
Hardware_Abstraction_Layer::Disk::s_file_info Hardware_Abstraction_Layer::Disk::local_named_param_file;
#define TIME_OUT_TIMER_CLOCK TC_CMR_TCCLKS_TIMER_CLOCK4

static FATFS FatFs;
static FRESULT FatResult;
static DIR dir;
static FIL file;
static UINT bw = 0;

uint8_t Hardware_Abstraction_Layer::Disk::initialize()
{
	FatResult = f_mount(&FatFs,"",1);
	Talos::Motion::Main_Process::host_serial.print_string("f_mount =  ");
	Talos::Motion::Main_Process::host_serial.print_int32(FatResult);
	Talos::Motion::Main_Process::host_serial.print_string("\r\n");
	
	//FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW;
	FatResult = f_open(&file,"wtf.txt",FA_WRITE | FA_CREATE_ALWAYS);
	Talos::Motion::Main_Process::host_serial.print_string("f_open =  ");
	Talos::Motion::Main_Process::host_serial.print_int32(FatResult);
	Talos::Motion::Main_Process::host_serial.print_string("\r\n");
	
	FatResult = f_write(&file,"hello",5,&bw);
	Talos::Motion::Main_Process::host_serial.print_string("f_write =  ");
	Talos::Motion::Main_Process::host_serial.print_int32(FatResult);
	Talos::Motion::Main_Process::host_serial.print_string("\r\n");
	f_close(&file);
	Talos::Motion::Main_Process::host_serial.print_string("f_close =  ");
	Talos::Motion::Main_Process::host_serial.print_int32(FatResult);
	Talos::Motion::Main_Process::host_serial.print_string("\r\n");
	
	return 1;

}

FRESULT Hardware_Abstraction_Layer::Disk::__open_machine_parameters()
{
	FRESULT ret_code = FRESULT::FR_OK;
	
	//char line[100]; /* Line buffer */
	//FRESULT fr;     /* FatFs return code */
	//

	ret_code = f_open(&machine_param_file._handle, machine_param_file._name, machine_param_file._mode);
	if (ret_code) return ret_code;

	///* Read every line and display it */
	//while (f_gets(line, sizeof line, &fil)) {
	//printf(line);
	//}

	/* Close the file */
	//f_close(&fil);

	//return 0;
	
	return ret_code;

}

void Hardware_Abstraction_Layer::Disk::__close_machine_parameters()
{
	/* Close the file */
	f_close(&machine_param_file._handle);
}

FRESULT Hardware_Abstraction_Layer::Disk::__open_named_parameters()
{
	FRESULT ret_code = FRESULT::FR_OK;
	ret_code = f_open(&machine_param_file._handle, machine_param_file._name, machine_param_file._mode);
	if (ret_code) return ret_code;
	return ret_code;

}

void Hardware_Abstraction_Layer::Disk::__close_named_parameters()
{
	/* Close the file */
	f_close(&machine_param_file._handle);
}
