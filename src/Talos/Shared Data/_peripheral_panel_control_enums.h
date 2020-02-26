
/*
*  hardware_def.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef RECORDS_DEF_H
#define RECORDS_DEF_H
#include <stdint.h>
#include "../physical_machine_parameters.h"
#include "../NGC_RS274/_ngc_defines.h"
#include "../NGC_RS274/_ngc_arc_struct.h"
#include "../_bit_manipulation.h"
#include "_e_feed_modes.h"
#include "_e_feed_types.h"
#include "_e_record_types.h"
#include "_s_motion_data_block.h"
#include "Settings/Motion/_s_motion_tolerance_struct.h"
#include "_e_unit_types.h"
#include <string.h>

namespace BinaryRecords
{
#ifndef __C_CRITICAL_ERRORS
#define __C_CRITICAL_ERRORS
	enum class e_critical_errors
	{
		Hardware_failure_force_shutdown = 0
	};
#endif

#ifndef __C_RAMP_TYPE
#define __C_RAMP_TYPE
	enum class e_ramp_type
	{
		Accel, Cruise, Decel, Decel_Override
	};
#endif



#ifndef __C_BLOCK_FLAG
#define __C_BLOCK_FLAG
	enum class e_block_flag : uint8_t
	{
		block_state_normal = 0,
		block_state_skip_sys_pos_update = 1,
		motion_state_accelerating = 2,
		motion_state_decelerating = 3,
		motion_state_cruising = 4,
		motion_state_stopping = 5,
	};
#endif

//#ifndef __C_UNIT_TYPES
//#define __C_UNIT_TYPES
//	enum class e_unit_types : uint8_t
//	{
//		MM = 1,
//		INCHES = 2
//	};
//#endif

#ifndef __C_SYSTEM_STATE_RECORD_TYPES
#define __C_SYSTEM_STATE_RECORD_TYPES
	enum class e_system_state_record_types : uint8_t
	{
		Motion_Active = 1, //Motion states 1-9
		Motion_Complete = 2,
		Motion_Idle = 3,
		Motion_Jogging = 4,
		Motion_Cancel = 5,
		Motion_Discarded = 6,
		Spindle_Stopped = 10, //spindle states 10-19
		Spindle_Running = 11,
		System_Error = 99
	};
#endif

#ifndef __C_SYSTEM_SUB_STATE_RECORD_TYPES
#define __C_SYSTEM_SUB_STATE_RECORD_TYPES
	enum class e_system_sub_state_record_types : uint8_t
	{
		Block_Complete = 1, //The block has completed. Block sequence # is returned in the num_message
		Block_Starting = 2, //Block is starting execution
		Block_Queuing = 3, //The block has been placed in the motion queue
		Block_Holding = 4, //Block was executing, but feed hold was instructed
		Block_Resuming = 5, //Feed hold released, block resuming
		Block_Reserved2 = 6, //Reserved
		Block_Reserved3 = 7, //Reserved
		Block_Reserved4 = 8, //Reserved
		Block_Reserved5 = 9, //Reserved
		Jog_Complete = 30,
		Jog_Running = 31,
		Jog_Failed = 32,
		Error_Axis_Drive_Fault_X = 90, //Closed loop driver error
		Error_Axis_Drive_Fault_Y = 91, //Closed loop driver error
		Error_Axis_Drive_Fault_Z = 92, //Closed loop driver error
		Error_Axis_Drive_Fault_A = 93, //Closed loop driver error
		Error_Axis_Drive_Fault_B = 94, //Closed loop driver error
		Error_Axis_Drive_Fault_C = 95, //Closed loop driver error
		Error_Axis_Drive_Fault_U = 96, //Closed loop driver error
		Error_Axis_Drive_Fault_V = 97, //Closed loop driver error
		Error_Axis_Drive_Reserved = 98, //Closed loop driver error
		Error_Setting_Max_Rate_Exceeded = 100, //Setting error for max rate
		Error_Spindle_Synch_Failed = 110 //Spindle failed to synch before timeout

	};

#endif

#ifndef __C_BINARY_RESPONSES
#define __C_BINARY_RESPONSES
	enum class e_binary_responses : uint8_t
	{
		Ok = 240,
		Data_Error = 239,
		Response_Time_Out = 238,
		//Jog_Complete =		237,
		Data_Rx_Wait = 236,
		Check_Sum_Failure = 235
	};
#endif

#ifndef __C_PERIPHERAL_PANEL_PROCESSING
#define __C_PERIPHERAL_PANEL_PROCESSING
	enum class e_peripheral_panel_processing : uint16_t
	{
		Block_Skip = 1, //when on any lines beginning with the skip '/' char are not executed
		Single_block = 2, // only one line at a time is processed.
		Spindle_On_Off = 3, // spindle manual on or off
		Coolant_On_Off = 4,
		Coolant_Mist_On_Off = 5,
		Coolant_Flood_On_Off = 6
	};
#endif

#ifndef __C_PERIPHERAL_PANEL_OVERRIDE_RAPIDS
#define __C_PERIPHERAL_PANEL_OVERRIDE_RAPIDS
	enum class e_peripheral_panel_override_rapids : uint8_t
	{
		Zero = 0,
		_25 = 25,
		_50 = 50,
		_100 = 100
	};
#endif

	struct s_peripheral_group_processing
	{
		e_peripheral_panel_processing Toggles; //Bit values set for on/off panel options
	};

	struct s_peripheral_group_overrides
	{
		e_peripheral_panel_override_rapids RapidFeed;
		uint8_t TravelFeed; //0-150
		uint8_t SpindleSpeed; //0-150
	};

	struct s_peripheral_group_jogging
	{
		uint8_t Axis;
		float Scale;
	};

	struct s_peripheral_panel
	{
		const e_record_types record_type = e_record_types::Peripheral_Control_Setting;
		s_peripheral_group_processing Processing;
		s_peripheral_group_overrides OverRides;
		s_peripheral_group_jogging Jogging;
		uint32_t _check_sum = 0;

	};

	struct s_jog_data_block
	{
		const e_record_types record_type = e_record_types::Jog;
		float axis_value;
		uint8_t axis;
		uint32_t flag = (int)BinaryRecords::e_block_flag::block_state_normal;
		uint32_t _check_sum = 0;
	};

	struct s_encoder_meta
	{
		int32_t reg_tc0_cv1 = 0;
		int32_t reg_tc0_ra0 = 0;
		float speed_rps = 0;
		float speed_rpm = 0;
	};
	struct s_encoders
	{
		s_encoder_meta meta_data;
		uint32_t feedrate_delay;
		uint16_t samples_per_second; //How frequently are we going to check the encoder signal
		uint16_t ticks_per_revolution;//How many ticks are in a rev
		int32_t current_rpm;//what is the current spindle rpm
		int32_t target_rpm;//what is the target spindle rpm
		uint8_t variable_percent;//how far from the target can it be?
		uint8_t wait_spindle_at_speed;//do we wait for the spindle to get to speed before we start interpolation? 1 = yes, 0 = no
		uint8_t lock_to_axis;//if spindle lock_to_axis is set ('X','Y','Z','A','B','C','U','V','W') moving that axis will cause the spindle to rotate
		int32_t spindle_synch_wait_time_ms;//when we are waiting for spindle at speed, how many milliseconds do we wait for synch to occur.

		//function to determine if we are near enough to the rpm to start interpolation
		uint8_t near(uint32_t current_rpm, uint32_t target_rpm, uint8_t variable_percent)
		{
			if (current_rpm == 0 || target_rpm == 0)
			{
				return 0;
			}
			//5% = 0.05
			//1000rpm * .05 = 50
			uint8_t target_percent = (float)target_rpm * (float)variable_percent / 100.0;
			//return 1 if the current RPM is within the % range of the target. otherwise 0
			return (current_rpm >= (target_rpm - target_percent)) && (current_rpm <= (target_rpm + target_percent));
		};
	};

	struct s_hardware
	{
		s_encoders spindle_encoder;
		float back_lash_comp_distance[MACHINE_AXIS_COUNT];//53
		float distance_per_rotation[MACHINE_AXIS_COUNT];//53
		float interpolation_error_distance;//57
		float acceleration[MACHINE_AXIS_COUNT]; //13
		float max_rate[MACHINE_AXIS_COUNT];//25
		uint16_t steps_per_mm[MACHINE_AXIS_COUNT];//31
		uint16_t pulse_length = 0;//49
	};

	struct s_spindle_control_settings
	{
		uint32_t _check_sum = 0;
	};

	struct s_status_message
	{
		const e_record_types record_type = e_record_types::Status;
		BinaryRecords::e_system_state_record_types system_state;
		BinaryRecords::e_system_sub_state_record_types system_sub_state;
		float position[MACHINE_AXIS_COUNT];
		float num_message = 0.0;
		char * chr_message;
		uint32_t _check_sum = 0;
	};
};
#endif /* RECORDS_DEF_H */