/*
*  NGC_Block.h - NGC_RS274 controller.
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


#ifndef NGC_SYSTEM_H
#define NGC_SYSTEM_H
#include "_ngc_plane_struct.h"
#include <string.h>
#include "NGC_Block_View.h"

namespace NGC_RS274
{
	class System
	{
	
		struct s_system_position
		{
		private:
			bool will_move = false;
			float distance = 0.0;
		public:
			s_plane_axis_l sys_axis;
			
			bool update(s_plane_axis *axis, bool absolute)
			{
				will_move = false;
				if (axis_update(&axis->horizontal_axis, &sys_axis.horizontal_axis, absolute)) will_move = true;
				if (axis_update(&axis->vertical_axis, &sys_axis.vertical_axis, absolute)) will_move = true;
				if (axis_update(&axis->normal_axis, &sys_axis.normal_axis, absolute)) will_move = true;

				if (axis_update(&axis->rotary_horizontal_axis, &sys_axis.rotary_horizontal_axis, absolute)) will_move = true;
				if (axis_update(&axis->rotary_vertical_axis, &sys_axis.rotary_vertical_axis, absolute)) will_move = true;
				if (axis_update(&axis->rotary_normal_axis, &sys_axis.rotary_normal_axis, absolute)) will_move = true;

				if (axis_update(&axis->inc_horizontal_axis, &sys_axis.inc_horizontal_axis, false)) will_move = true;
				if (axis_update(&axis->inc_vertical_axis, &sys_axis.inc_vertical_axis, false)) will_move = true;
				if (axis_update(&axis->inc_normal_axis, &sys_axis.inc_normal_axis, false)) will_move = true;

				if (will_move)
					Events.Notifications.Update.set((int)s_notification_events::e_event_type::SystemPositionChange);
				else
					Events.Notifications.Update.set((int)s_notification_events::e_event_type::SystemPositionHold);
				return will_move;
			}

			float axis_update(s_axis_property *axis, s_axis_property_l *system, bool absolute)
			{
				distance = 0.0;
				if (absolute)
				{
					distance = *axis->value - system->value;
					system->value = *axis->value;
				}
				else
				{
					distance = *axis->value;
					system->value += *axis->value;
				}

				system->name = axis->name;
				return distance;
			};
		};

	public:

		static s_ngc_block system_block;

		struct s_error_events
		{
			enum class e_event_type : uint8_t
			{
				NgcNonModalError = 1,
				NgcMotionError = 2,
				NgcPlaneError = 3,
				NgcFeedRateError = 4,
				NgcCutterCompensationError = 5,
				NgcPlaneRotationError = 6
			};
			s_bit_flag_controller<uint16_t> Block;
		};

		struct s_notification_events
		{
			enum class e_event_type : uint8_t
			{
				SystemPositionChange = 0,
				SystemPositionHold = 1,
			};
			s_bit_flag_controller<uint16_t> Update;
		};


		struct s_ngc_system_events
		{
			s_error_events Errors;
			s_notification_events Notifications;
		};

		
		static s_system_position Position;
		static s_ngc_system_events Events;
		static uint8_t intialize();
	private:

	};
};

#endif /* NGC_BINARY_BLOCK_H */