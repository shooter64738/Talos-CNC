/*
* c_core_avr_2560.cpp
*
* Created: 2/27/2019 3:47:52 PM
* Author: jeff_d
*/


#include "c_disk_avr_2560.h"

#include <avr/interrupt.h>
#include "../../../../Motion/motion_hardware_def.h"
#include "../../../../Motion/Processing/Main/Main_Process.h"
#include "Disk Support/ff.h"
#include "../../../../NGC_RS274/_ngc_g_Groups.h"
#include "../../../../NGC_RS274/_ngc_m_Groups.h"


static FATFS FatFs;
static FRESULT FatResult;
static FIL _cache_file_object;
static char _cache_file_name[11] ="cache.dat\0";
static uint32_t _cache_read_position = 0;
static uint32_t _cache_write_position = 0;

static FIL _tool_file_object;
static char _tool_file_name[11] ="cache.dat\0";
static uint32_t _tool_read_position = 0;
static uint32_t _tool_write_position = 0;

static FIL _wcs_file_object;
static char _wcs_file_name[11] ="cache.dat\0";
static uint32_t _wcs_read_position = 0;
static uint32_t _wcs_write_position = 0;


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
	f_open(&_tool_file_object,_tool_file_name,FA_WRITE|FA_READ|FA_CREATE_NEW);
	f_open(&_wcs_file_object,_wcs_file_name,FA_WRITE|FA_READ|FA_CREATE_NEW);


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
		Talos::Motion::Main_Process::host_serial.print_string("f_open =  ");
		Talos::Motion::Main_Process::host_serial.print_int32(FatResult);
		Talos::Motion::Main_Process::host_serial.print_string("\r\n");
		FatResult = f_write(&file,"(machine)",9,&bw);
		Talos::Motion::Main_Process::host_serial.print_string("f_write =  ");
		Talos::Motion::Main_Process::host_serial.print_int32(FatResult);
		Talos::Motion::Main_Process::host_serial.print_string("\r\n");
		FatResult = f_close(&file);
		Talos::Motion::Main_Process::host_serial.print_string("f_close =  ");
		Talos::Motion::Main_Process::host_serial.print_int32(FatResult);
		Talos::Motion::Main_Process::host_serial.print_string("\r\n");
		return 1;
	}
	
}

uint8_t Hardware_Abstraction_Layer::Disk::load_motion_control_settings(s_motion_control_settings_encapsulation * motion_settings)
{
	return 0;
	motion_settings->hardware.spindle_encoder.meta_data.reg_tc0_cv1 = 99;
	motion_settings->hardware.spindle_encoder.meta_data.reg_tc0_ra0 = 88;
	motion_settings->hardware.spindle_encoder.meta_data.speed_rps = 14;
	motion_settings->hardware.spindle_encoder.meta_data.speed_rpm = 3500;

	for (uint8_t i = 0; i < MACHINE_AXIS_COUNT; i++)
	{
		motion_settings->hardware.steps_per_mm[i] = 160;
		motion_settings->hardware.acceleration[i] = (150.0 * 60 * 60);
		motion_settings->hardware.max_rate[i] = 12000;
		motion_settings->hardware.distance_per_rotation[i] = 5;
		//arbitrary for testing
		motion_settings->hardware.back_lash_comp_distance[i] = 55;
	}

	motion_settings->hardware.pulse_length = 5;
	motion_settings->hardware.spindle_encoder.wait_spindle_at_speed = 1;
	motion_settings->hardware.spindle_encoder.spindle_synch_wait_time_ms = 5;
	motion_settings->hardware.spindle_encoder.ticks_per_revolution = 400;
	motion_settings->hardware.spindle_encoder.current_rpm = 0;
	motion_settings->hardware.spindle_encoder.target_rpm = 100;
	motion_settings->hardware.spindle_encoder.variable_percent = 50;
	motion_settings->hardware.spindle_encoder.samples_per_second = 10;


	motion_settings->tolerance.arc_tolerance = 0.002;
	motion_settings->tolerance.arc_angular_correction = 12;
	return 0;
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
	const uint16_t rec_size = sizeof(s_ngc_block);
	char stream[rec_size];
	
	memcpy(stream, write_block, rec_size);

	if (write_block->__station__)
	{
		uint32_t position = rec_size * (write_block->__station__ - 1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_cache_file_object,position);
	}

	return write(_cache_file_object, stream, e_file_modes::OpenCreate, rec_size);

	_cache_write_position =f_tell(&_cache_file_object);
}

uint8_t Hardware_Abstraction_Layer::Disk::get_block(s_ngc_block * read_block)
{
	const uint16_t rec_size = sizeof(s_ngc_block);
	char stream[rec_size];

	//If a station number was sent with the block we need to seek
	//that block id in the cache. The offset int he cache is simple.
	if (read_block->__station__)
	{
		DWORD position = rec_size * (read_block->__station__ -1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_cache_file_object, position);
	}
	
	uint8_t ret_code = read(_cache_file_object, stream, e_file_modes::OpenCreate, rec_size);
	if (ret_code) return ret_code;

	memcpy(read_block, stream, rec_size);

	_cache_read_position = f_tell(&_cache_file_object);
	return 0;
}

uint8_t Hardware_Abstraction_Layer::Disk::put_tool(s_tool_definition * write_tool)
{
	const uint16_t rec_size = sizeof(s_tool_definition);
	char stream[rec_size];
	
	memcpy(stream, write_tool, rec_size);

	if (write_tool->toolno)
	{
		uint32_t position = rec_size * (write_tool->toolno - 1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_tool_file_object,position);
	}

	return write(_tool_file_object, stream, e_file_modes::OpenCreate, rec_size);

	_tool_write_position =f_tell(&_tool_file_object);
}

uint8_t Hardware_Abstraction_Layer::Disk::get_tool(s_tool_definition * read_tool)
{
	const uint16_t rec_size = sizeof(s_tool_definition);
	char stream[rec_size];

	//If a station number was sent with the block we need to seek
	//that block id in the cache. The offset in the cache is simple.
	if (read_tool->toolno)
	{
		DWORD position = rec_size * (read_tool->toolno -1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_cache_file_object, position);
	}
	
	uint8_t ret_code = read(_tool_file_object, stream, e_file_modes::OpenCreate, rec_size);
	if (ret_code) return ret_code;

	memcpy(read_tool, stream, rec_size);

	_tool_read_position = f_tell(&_tool_file_object);
	return 0;
}

uint8_t Hardware_Abstraction_Layer::Disk::put_wcs(s_wcs * write_wcs)
{
	const uint16_t rec_size = sizeof(s_wcs);
	char stream[rec_size];
	
	memcpy(stream, write_wcs, rec_size);

	if (write_wcs->wcs_id)
	{
		uint32_t position = rec_size * (write_wcs->wcs_id - 1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_wcs_file_object,position);
	}

	return write(_wcs_file_object, stream, e_file_modes::OpenCreate, rec_size);

	_wcs_write_position =f_tell(&_wcs_file_object);
}

uint8_t Hardware_Abstraction_Layer::Disk::get_wcs(s_wcs * read_wcs)
{
	const uint16_t rec_size = sizeof(s_wcs);
	char stream[rec_size];

	//If a station number was sent with the block we need to seek
	//that block id in the cache. The offset in the cache is simple.
	if (read_wcs->wcs_id)
	{
		DWORD position = rec_size * (read_wcs->wcs_id -1);
		//position should now be at the beginning point of the block requested by __station__
		f_lseek(&_wcs_file_object, position);
	}
	
	uint8_t ret_code = read(_wcs_file_object, stream, e_file_modes::OpenCreate, rec_size);
	if (ret_code) return ret_code;

	memcpy(read_wcs, stream, rec_size);

	_wcs_read_position = f_tell(&_wcs_file_object);
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

