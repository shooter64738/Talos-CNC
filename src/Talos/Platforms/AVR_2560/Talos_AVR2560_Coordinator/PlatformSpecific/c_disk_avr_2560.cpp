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
#include "../../../../NGC_RS274/_ngc_g_Groups.h"
#include "../../../../NGC_RS274/_ngc_m_Groups.h"


static FATFS FatFs;
static FRESULT FatResult;
static FIL _cache_file_object;
static char _cache_file_name[11] ="cache.dat\0";
static uint32_t _cache_read_position = 0;
static uint32_t _cache_write_position = 0;


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
	f_open(&_cache_file_object,_cache_file_name,FA_WRITE|FA_READ|FA_CREATE_NEW);


	return (uint8_t) FatResult;
}

uint8_t Hardware_Abstraction_Layer::Disk::load_configuration()
{
	FILINFO f_info;
	DIR dir;
	FIL file;
	UINT bw = 0;
	
	FatResult = f_stat ("m_setup.cfg", &f_info);
	if (FatResult == FR_NO_FILE)
	{
		FatResult = f_open(&file,"m_setup.cfg", FA_WRITE|FA_CREATE_ALWAYS);
		Talos::Coordinator::Main_Process::host_serial.print_string("f_open =  ");
		Talos::Coordinator::Main_Process::host_serial.print_int32(FatResult);
		Talos::Coordinator::Main_Process::host_serial.print_string("\r\n");
		FatResult = f_write(&file,"(machine)",9,&bw);
		Talos::Coordinator::Main_Process::host_serial.print_string("f_write =  ");
		Talos::Coordinator::Main_Process::host_serial.print_int32(FatResult);
		Talos::Coordinator::Main_Process::host_serial.print_string("\r\n");
		FatResult = f_close(&file);
		Talos::Coordinator::Main_Process::host_serial.print_string("f_close =  ");
		Talos::Coordinator::Main_Process::host_serial.print_int32(FatResult);
		Talos::Coordinator::Main_Process::host_serial.print_string("\r\n");
		return 1;
	}
	
}

uint8_t Hardware_Abstraction_Layer::Disk::load_initialize_block(s_ngc_block * initial_block )
{
	//default the motion state to canceled
	initial_block->g_group[NGC_RS274::Groups::G::Motion] = NGC_RS274::G_codes::MOTION_CANCELED;
	//default plane selection
	initial_block->g_group[NGC_RS274::Groups::G::PLANE_SELECTION] = NGC_RS274::G_codes::XY_PLANE_SELECTION;
	//default the machines distance mode to absolute
	initial_block->g_group[NGC_RS274::Groups::G::DISTANCE_MODE] = NGC_RS274::G_codes::ABSOLUTE_DISANCE_MODE;
	//default feed rate mode
	initial_block->g_group[NGC_RS274::Groups::G::Feed_rate_mode] = NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE;
	//default the machines units to inches
	initial_block->g_group[NGC_RS274::Groups::G::Units] = NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION;
	//default the machines cutter comp to off
	initial_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] = NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION;
	//default tool length offset
	initial_block->g_group[NGC_RS274::Groups::G::Tool_length_offset] = NGC_RS274::G_codes::CANCEL_TOOL_LENGTH_OFFSET;
	//default tool length offset
	initial_block->g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_Z;
	//default coordinate system selection
	initial_block->g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM;
	//default path control mode
	initial_block->g_group[NGC_RS274::Groups::G::PATH_CONTROL_MODE] = NGC_RS274::G_codes::PATH_CONTROL_EXACT_PATH;
	//default coordinate system type
	initial_block->g_group[NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION] = NGC_RS274::G_codes::RECTANGULAR_COORDINATE_SYSTEM;
	//default canned cycle return mode
	initial_block->g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R;
	//default spindle mode
	initial_block->g_group[NGC_RS274::Groups::M::SPINDLE] = NGC_RS274::M_codes::SPINDLE_STOP;
	//default coolant mode
	initial_block->g_group[NGC_RS274::Groups::M::COOLANT] = NGC_RS274::M_codes::COOLANT_OFF;
}

uint8_t Hardware_Abstraction_Layer::Disk::put_block(s_ngc_block * write_block)
{
	char stream[sizeof(s_ngc_block)];
	
	memcpy(stream, write_block, sizeof(s_ngc_block));

	if (write_block->__station__)
	{
		uint32_t position = sizeof(s_ngc_block) * (write_block->__station__ - 1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_cache_file_object,position);
	}

	return write(_cache_file_object, stream, e_file_modes::OpenCreate, sizeof(s_ngc_block));

	_cache_write_position =f_tell(&_cache_file_object);
}

uint8_t Hardware_Abstraction_Layer::Disk::update_block(s_ngc_block * update_block)
{

	char stream[sizeof(s_ngc_block)];
	
	memcpy(stream, update_block, sizeof(s_ngc_block));
	
	if (update_block->__station__)
	{
		uint32_t position = sizeof(s_ngc_block) * (update_block->__station__ - 1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_cache_file_object,position);
	}

	return write(_cache_file_object, stream, e_file_modes::OpenCreate, sizeof(s_ngc_block));

	_cache_write_position =f_tell(&_cache_file_object);
}

uint8_t Hardware_Abstraction_Layer::Disk::get_block(s_ngc_block * read_block)
{
	char stream[sizeof(s_ngc_block)];

	//If a station number was sent with the block we need to seek
	//that block id in the cache. The offset int he cache is simple.
	if (read_block->__station__)
	{
		DWORD position = sizeof(s_ngc_block) * (read_block->__station__ -1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_cache_file_object, position);
	}
	
	uint8_t ret_code = read(_cache_file_object, stream, e_file_modes::OpenCreate, sizeof(s_ngc_block));
	if (ret_code) return ret_code;

	memcpy(read_block, stream, sizeof(s_ngc_block));

	_cache_read_position = f_tell(&_cache_file_object);
	return 0;
}

uint8_t Hardware_Abstraction_Layer::Disk::write(FIL file, char * buffer, e_file_modes mode, uint16_t size)
{
	UINT write_size;
	FRESULT result = FR_OK;
	
	result = f_write(&file, buffer, size, &write_size);
	if (result != FR_OK) return result;
	
	result = f_sync(&file);
	if (result != FR_OK) return result;
}

uint8_t Hardware_Abstraction_Layer::Disk::read(FIL file, char * buffer, e_file_modes mode, uint16_t size)
{
	UINT read_size=0;
	FRESULT result = f_read(&file, buffer, size, &read_size);
	
}

