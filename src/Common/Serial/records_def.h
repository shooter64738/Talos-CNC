
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


//#include "..\..\MotionDriver\c_motion_core.h"
#ifndef RECORDS_DEF_H
#define RECORDS_DEF_H
#define G_CODE_MULTIPLIER 100
#define N_AXIS 3
namespace BinaryRecords
{
	#define MOTION_RECORD 1
	#define SETTING_RECORD 2
	#define SER_ACK_PROCEED 100 //proceed with more instructions
	#define SER_BAD_READ_RE_TRANSMIT 101 //data is bad, re-send
	#ifndef __C_RAMP_TYPE
	#define __C_RAMP_TYPE
	enum class e_ramp_type
	{Accel, Cruise, Decel, Decel_Override};
	#endif
	
	#ifndef __C_MOTION_TYPE
	#define __C_MOTION_TYPE
	enum class e_motion_type : uint8_t
	{rapid_linear = 0,feed_linear = 1,arc_cw = 2,arc_ccw = 3};
	#endif
	
	#ifndef __C_BLOCK_FLAG
	#define __C_BLOCK_FLAG
	enum class e_block_flag : uint8_t
	{normal = 0, compensation = 1};
	#endif
	
	#ifndef __C_FEED_MODES
	#define __C_FEED_MODES
	enum class e_feed_modes : uint16_t
	{	FEED_RATE_MINUTES_PER_UNIT_MODE = 93 * G_CODE_MULTIPLIER,
		FEED_RATE_UNITS_PER_MINUTE_MODE = 94 * G_CODE_MULTIPLIER,
		FEED_RATE_UNITS_PER_ROTATION = 95 * G_CODE_MULTIPLIER,
		FEED_RATE_CONSTANT_SURFACE_SPEED = 96 * G_CODE_MULTIPLIER,
		FEED_RATE_RPM_MODE = 97 * G_CODE_MULTIPLIER
	};
	#endif
	
	struct s_motion_arc_values
	{
		float horizontal_center = 0 ;
		float vertical_center = 0;
		float Radius = 0;
	};

	struct s_motion_data_block
	{
		const uint8_t record_type = MOTION_RECORD;
		BinaryRecords::e_motion_type motion_type = BinaryRecords::e_motion_type::rapid_linear;
		float feed_rate = 0;
		BinaryRecords::e_feed_modes feed_rate_mode = BinaryRecords::e_feed_modes::FEED_RATE_UNITS_PER_MINUTE_MODE;
		//float word_values[26];
		uint32_t line_number = 0 ;
		float axis_values[N_AXIS];
		s_motion_arc_values arc_values;
		BinaryRecords::e_block_flag flag = BinaryRecords::e_block_flag::normal;
		
	};
	
	struct s_settings
	{
		float acceleration[N_AXIS];
		float max_rate[N_AXIS];
		int16_t steps_per_mm[N_AXIS];
		float junction_deviation;
		float arc_tolerance;
		uint16_t pulse_length;
		float back_lash_comp_distance[N_AXIS];
	};
	
};
#endif /* RECORDS_DEF_H */