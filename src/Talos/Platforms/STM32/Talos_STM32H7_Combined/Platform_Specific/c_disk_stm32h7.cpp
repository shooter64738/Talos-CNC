/*
* c_core_avr_2560.cpp
*
* Created: 2/27/2019 3:47:52 PM
* Author: jeff_d
*/


#include "c_disk_stm32h7.h"


#include "../../../../NGC_RS274/_ngc_defines.h"
#include "../../../../NGC_RS274/_ngc_g_groups.h"
#include "../../../../NGC_RS274/_ngc_m_groups.h"
#include "../../../../NGC_RS274/_ngc_m_groups.h"

#include <stm32h7xx_hal.h>
#include <stm32_hal_legacy.h>

#include <string.h>

//#include "../../../../../Shared_Data/FrameWork/extern_events_types.h"
using namespace Hardware_Abstraction_Layer;

static FATFS FatFs;
static FRESULT FatResult;
static FIL _cache_file_object;
static char _cache_file_name[11] = "cache.dat\0";
static uint32_t _cache_read_position = 0;
static uint32_t _cache_write_position = 0;

static FIL _tool_file_object;
static char _tool_file_name[10] = "tool.dat\0";
static uint32_t _tool_read_position = 0;
static uint32_t _tool_write_position = 0;

static FIL _wcs_file_object;
static char _wcs_file_name[9] = "wcs.dat\0";
static uint32_t _wcs_read_position = 0;
static uint32_t _wcs_write_position = 0;

static FIL _mcs_file_object;
static char _mcs_file_name[9] = "mcs.dat\0";
static uint32_t _mcs_read_position = 0;
static uint32_t _mcs_write_position = 0;

static FIL _dflt_blk_file_object;
static char _dflt_blk_file_name[13] = "dfltblk.dat\0";


static FIL _motion_control_settings_file_object;
static char _motion_control_settings_file_name[14] = "mtnctcfg.dat\0";

s_bit_flag_controller<uint32_t> Disk::states;

//static uint32_t _wcs_read_position = 0;
//static uint32_t _wcs_write_position = 0;

static void debug_out(const char* message, void(*string_writer)(int serial_id, const char* data))
{
	if (string_writer != NULL)
	{
		string_writer(0, "\t");
		string_writer(0, message);
		string_writer(0, "\r\n");
	}
}

static bool create_file_if_not_exist(const char * filename)
{
	FILINFO f_info;
	FIL file;

	FatResult = f_stat(filename, &f_info);
	if (FatResult == FR_NO_FILE)
	{
		FatResult = f_open(&file, filename, FA_WRITE | FA_CREATE_ALWAYS);
		FatResult = f_close(&file);
		return 1;
	}
}



uint8_t Disk::initialize(void(*string_writer)(int serial_id, const char* data))
{

	((FatResult = (FRESULT)SD_IO_Init(SPI1)) != FR_OK)
		? Disk::states.set((int)e_disk_states::drive_start_error) : Disk::states.set((int)e_disk_states::drive_start_success);

	((FatResult = f_mount(&FatFs, "", 1)) != FR_OK)
		? Disk::states.set((int)e_disk_states::drive_mount_error) : Disk::states.set((int)e_disk_states::drive_mount_success);

	((FatResult = f_open(&_cache_file_object, _cache_file_name, FA_WRITE | FA_READ | FA_CREATE_ALWAYS)) != FR_OK)
		? Disk::states.set((int)e_disk_states::cache_open_error) : Disk::states.set((int)e_disk_states::cache_open_success);

	((FatResult = f_open(&_tool_file_object, _tool_file_name, FA_WRITE | FA_READ | FA_OPEN_ALWAYS)) != FR_OK)
		? Disk::states.set((int)e_disk_states::tool_open_error) : Disk::states.set((int)e_disk_states::tool_open_success);
	
	((FatResult = f_open(&_wcs_file_object, _wcs_file_name, FA_WRITE | FA_READ | FA_OPEN_ALWAYS)) != FR_OK)
		? Disk::states.set((int)e_disk_states::wcs_open_error) : Disk::states.set((int)e_disk_states::wcs_open_success);

	((FatResult = f_open(&_mcs_file_object, _mcs_file_name, FA_WRITE | FA_READ | FA_OPEN_ALWAYS)) != FR_OK)
		? Disk::states.set((int)e_disk_states::mcs_open_error) : Disk::states.set((int)e_disk_states::mcs_open_success);

	((FatResult = f_open(&_dflt_blk_file_object, _dflt_blk_file_name, FA_WRITE | FA_READ | FA_OPEN_ALWAYS)) != FR_OK)
		? Disk::states.set((int)e_disk_states::default_block_open_error) : Disk::states.set((int)e_disk_states::default_block_open_success);

	((FatResult = f_open(&_motion_control_settings_file_object, _motion_control_settings_file_name, FA_WRITE | FA_READ | FA_OPEN_ALWAYS)) != FR_OK)
		? Disk::states.set((int)e_disk_states::motion_setting_open_error) : Disk::states.set((int)e_disk_states::motion_setting_open_success);

	//Return 0 if not failures, so mask the first 15 bits. only return a non zero if ANY of the first 15 bits are true.
	return (Disk::states._flag & 0xFFFF);
}

uint8_t Hardware_Abstraction_Layer::Disk::load_configuration()
{
	FILINFO f_info;
	DIR dir;
	FIL file;
	UINT bw = 0;

	//If machien config file did not exist create it. 
	FatResult = f_stat("m_setup.cfg", &f_info);
	if (FatResult == FR_NO_FILE)
	{
		FatResult = f_open(&file, "m_setup.cfg", FA_WRITE | FA_CREATE_ALWAYS);
		FatResult = f_write(&file, "(machine)", 9, &bw);
		FatResult = f_close(&file);
		return 1;
	}

}

uint8_t Hardware_Abstraction_Layer::Disk::get_motion_control_settings(char * stream, uint16_t size)
{

	//If machien config file did not exist create it. 
	create_file_if_not_exist(_motion_control_settings_file_name);
	f_lseek(&_motion_control_settings_file_object, 0);

	uint8_t ret_code = read(_motion_control_settings_file_object, stream, e_file_modes::OpenCreate, size);
	if (ret_code) return ret_code;
	
	return 0;
}

uint8_t Hardware_Abstraction_Layer::Disk::put_motion_control_settings(char* stream, uint16_t size)
{
	f_lseek(&_motion_control_settings_file_object, 0);
	return write(_motion_control_settings_file_object, stream, e_file_modes::OpenCreate, size);
}

uint8_t Hardware_Abstraction_Layer::Disk::get_default_block(char * stream, uint16_t size)
{
	//If machien config file did not exist create it. 
	create_file_if_not_exist(_dflt_blk_file_name);
	f_lseek(&_dflt_blk_file_object, 0);

	uint8_t ret_code = read(_dflt_blk_file_object, stream, e_file_modes::OpenCreate, size);
	if (ret_code) return ret_code;

	return 0;
}

uint8_t Hardware_Abstraction_Layer::Disk::put_default_block(char* stream, uint16_t size)
{
	f_lseek(&_dflt_blk_file_object, 0);
	return write(_dflt_blk_file_object, stream, e_file_modes::OpenCreate, size);
}

uint8_t Hardware_Abstraction_Layer::Disk::put_block(s_ngc_block* write_block)
{
	const uint16_t rec_size = sizeof(s_ngc_block);
	char stream[rec_size];

	memcpy(stream, write_block, rec_size);

	if (write_block->__station__)
	{
		uint32_t position = rec_size * (write_block->__station__ - 1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_cache_file_object, position);
	}

	return write(_cache_file_object, stream, e_file_modes::OpenCreate, rec_size);

	_cache_write_position = f_tell(&_cache_file_object);
}

uint8_t Hardware_Abstraction_Layer::Disk::get_block(s_ngc_block* read_block)
{
	const uint16_t rec_size = sizeof(s_ngc_block);
	char stream[rec_size];

	//If a station number was sent with the block we need to seek
	//that block id in the cache. The offset int he cache is simple.
	if (read_block->__station__)
	{
		DWORD position = rec_size * (read_block->__station__ - 1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_cache_file_object, position);
	}

	uint8_t ret_code = read(_cache_file_object, stream, e_file_modes::OpenCreate, rec_size);
	if (ret_code) return ret_code;

	memcpy(read_block, stream, rec_size);

	_cache_read_position = f_tell(&_cache_file_object);
	return 0;
}

uint8_t Hardware_Abstraction_Layer::Disk::put_tool(s_tool_definition* write_tool)
{
	const uint16_t rec_size = sizeof(s_tool_definition);
	char stream[rec_size];

	memcpy(stream, write_tool, rec_size);

	if (write_tool->toolno)
	{
		uint32_t position = rec_size * (write_tool->toolno - 1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_tool_file_object, position);
	}

	return write(_tool_file_object, stream, e_file_modes::OpenCreate, rec_size);

	_tool_write_position = f_tell(&_tool_file_object);
}

uint8_t Hardware_Abstraction_Layer::Disk::get_tool(s_tool_definition* read_tool)
{
	const uint16_t rec_size = sizeof(s_tool_definition);
	char stream[rec_size];

	//If a station number was sent with the block we need to seek
	//that block id in the cache. The offset in the cache is simple.
	if (read_tool->toolno)
	{
		DWORD position = rec_size * (read_tool->toolno - 1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_cache_file_object, position);
	}

	uint8_t ret_code = read(_tool_file_object, stream, e_file_modes::OpenCreate, rec_size);
	if (ret_code) return ret_code;

	memcpy(read_tool, stream, rec_size);

	_tool_read_position = f_tell(&_tool_file_object);
	return 0;
}

uint8_t Hardware_Abstraction_Layer::Disk::put_wcs(s_wcs* write_wcs)
{
	const uint16_t rec_size = sizeof(s_wcs);
	char stream[rec_size];

	memcpy(stream, write_wcs, rec_size);

	if (write_wcs->wcs_id)
	{
		uint32_t position = rec_size * (write_wcs->wcs_id - 1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_wcs_file_object, position);
	}

	return write(_wcs_file_object, stream, e_file_modes::OpenCreate, rec_size);

	_wcs_write_position = f_tell(&_wcs_file_object);
}

uint8_t Hardware_Abstraction_Layer::Disk::get_wcs(s_wcs* read_wcs)
{
	const uint16_t rec_size = sizeof(s_wcs);
	char stream[rec_size];

	//If a station number was sent with the block we need to seek
	//that block id in the cache. The offset in the cache is simple.
	if (read_wcs->wcs_id)
	{
		DWORD position = rec_size * (read_wcs->wcs_id - 1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_wcs_file_object, position);
	}

	uint8_t ret_code = read(_wcs_file_object, stream, e_file_modes::OpenCreate, rec_size);
	if (ret_code) return ret_code;

	memcpy(read_wcs, stream, rec_size);

	_wcs_read_position = f_tell(&_wcs_file_object);
	return 0;
}

uint8_t Hardware_Abstraction_Layer::Disk::write(FIL file, char* buffer, e_file_modes mode, uint16_t size)
{
	UINT write_size;
	FRESULT result = FR_OK;

	result = f_write(&file, buffer, size, &write_size);
	if (result != FR_OK) return result;

	result = f_sync(&file);
	if (result != FR_OK) return result;
}

uint8_t Hardware_Abstraction_Layer::Disk::read(FIL file, char* buffer, e_file_modes mode, uint16_t size)
{
	UINT read_size = 0;
	FRESULT result = f_read(&file, buffer, size, &read_size);

}

