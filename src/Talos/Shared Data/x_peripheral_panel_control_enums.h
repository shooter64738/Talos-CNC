
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
//#include <stdint.h>
//#include "../physical_machine_parameters.h"
//#include "../NGC_RS274/_ngc_defines.h"
//#include "../NGC_RS274/_ngc_arc_struct.h"
//#include "../_bit_manipulation.h"

//#include "Settings/Motion/_s_motion_tolerance_struct.h"
//#include "Settings/Motion/_s_motion_hardware.h"

//#include "_s_motion_data_block.h"
//#include "_s_encoder_struct.h"

//#include "_e_feed_modes.h"
//#include "_e_feed_types.h"
//#include "_e_record_types.h"
//#include "_e_unit_types.h"
//#include "_e_motion_state.h"
//#include "_e_motion_sub_state.h"
//#include "_e_block_state.h"

//#include <string.h>

namespace BinaryRecords
{
//#ifndef __C_CRITICAL_ERRORS
//#define __C_CRITICAL_ERRORS
//	enum class xe_critical_errors
//	{
//		Hardware_failure_force_shutdown = 0
//	};
//#endif

//#ifndef __C_BINARY_RESPONSES
//#define __C_BINARY_RESPONSES
//	enum class xe_binary_responses : uint8_t
//	{
//		Ok = 240,
//		Data_Error = 239,
//		Response_Time_Out = 238,
//		//Jog_Complete =		237,
//		Data_Rx_Wait = 236,
//		Check_Sum_Failure = 235
//	};
//#endif

//#ifndef __C_PERIPHERAL_PANEL_PROCESSING
//#define __C_PERIPHERAL_PANEL_PROCESSING
//	enum class xe_peripheral_panel_processing : uint16_t
//	{
//		Block_Skip = 1, //when on any lines beginning with the skip '/' char are not executed
//		Single_block = 2, // only one line at a time is processed.
//		Spindle_On_Off = 3, // spindle manual on or off
//		Coolant_On_Off = 4,
//		Coolant_Mist_On_Off = 5,
//		Coolant_Flood_On_Off = 6
//	};
//#endif

//#ifndef __C_PERIPHERAL_PANEL_OVERRIDE_RAPIDS
//#define __C_PERIPHERAL_PANEL_OVERRIDE_RAPIDS
//	enum class xe_peripheral_panel_override_rapids : uint8_t
//	{
//		Zero = 0,
//		_25 = 25,
//		_50 = 50,
//		_100 = 100
//	};
//#endif

	//struct s_peripheral_group_processing
	//{
	//	e_peripheral_panel_processing Toggles; //Bit values set for on/off panel options
	//};

	//struct s_peripheral_group_overrides
	//{
	//	e_peripheral_panel_override_rapids RapidFeed;
	//	uint8_t TravelFeed; //0-150
	//	uint8_t SpindleSpeed; //0-150
	//};

	/*struct s_peripheral_group_jogging
	{
		uint8_t Axis;
		float Scale;
	};*/

	/*struct s_peripheral_panel
	{
		const e_record_types record_type = e_record_types::Peripheral_Control_Setting;
		s_peripheral_group_processing Processing;
		s_peripheral_group_overrides OverRides;
		s_peripheral_group_jogging Jogging;
		uint32_t _check_sum = 0;

	};*/

	/*struct s_jog_data_block
	{
		const e_record_types record_type = e_record_types::Jog;
		float axis_value;
		uint8_t axis;
		uint32_t flag = (int)e_block_flag::block_state_normal;
		uint32_t _check_sum = 0;
	};*/

	
	/*struct s_spindle_control_settings
	{
		uint32_t _check_sum = 0;
	};*/

	
};
#endif /* RECORDS_DEF_H */