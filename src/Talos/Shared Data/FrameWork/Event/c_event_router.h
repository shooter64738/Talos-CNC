/*
*  c_data_events.h - NGC_RS274 controller.
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


#ifndef __C_NEW_SHARED_DATA_EVENTS_H__
#define __C_NEW_SHARED_DATA_EVENTS_H__

#include <stdint.h>
#include "../../../c_ring_template.h"
#include "../../../_bit_flag_control.h"

class c_event_router
{
	//variables
	public:
		struct ss_inbound_data
		{
			c_ring_buffer<char> * device;
			uint16_t ms_time_out = 0;
			void set_time_out(uint16_t millisecond_time_out)
			{
				ms_time_out = millisecond_time_out;
				event_manager.clear((int)e_event_type::TimeOutError);
			};

			bool check_time_out()
			{
				
				if (!ms_time_out)
				{
					//only set the timeout if events are pending. 
					if (event_manager._flag > 0)
					{
						event_manager.set((int)e_event_type::TimeOutError);
						return true;
					}
				}
				else
					ms_time_out--;
			};

			enum class e_event_type : uint8_t
			{
				MotionDataBlock = 0,
				StatusUpdate = 1,
				DiskDataArrival = 2,
				Usart0DataArrival = 3,
				TimeOutError = 31,
			};
			s_bit_flag_controller<uint32_t> event_manager;
		};

		struct ss_outbound_data
		{
			c_ring_buffer<char> * device;
			enum class e_event_type : uint8_t
			{
				MotionDataBlock = 0,
				StatusUpdate = 1,
				DiskDataArrival = 2,
				NgcBlockRequest = 3
			};
			s_bit_flag_controller<uint32_t> event_manager;
		};

		struct ss_ready_data
		{
			enum class e_event_type : uint8_t
			{
				NgcDataLine = 0,
				System = 1,
				MotionDataBlock = 2,
				NgcDataBlock = 2,
			};
			s_bit_flag_controller<uint32_t> event_manager;
			bool any()
			{
				if (event_manager._flag > 0)
					return true;
				else
					return false;
			}
			uint32_t ngc_block_cache_count = 0;
		};

		struct ss_inquiry_data
		{
			enum class e_event_type : uint8_t
			{
				ActiveBlockGGroupStatus = 0,
				ActiveBlockMGroupStatus = 1,
				ActiveBlockWordStatus = 2,
			};
			s_bit_flag_controller<uint32_t> event_manager;
			bool any()
			{
				if (event_manager._flag > 0)
					return true;
				else
					return false;
			}
			uint32_t ngc_block_cache_count = 0;
		};

		struct ss_serial
		{
			ss_inbound_data inbound;
			ss_outbound_data outbound;
			bool any()
			{
				if ((inbound.event_manager._flag + outbound.event_manager._flag) > 0)
					return true;
				else
					return false;
			}
			bool in_events()
			{
				if ((inbound.event_manager._flag) > 0)
					return true;
				else
					return false;
			}
			bool out_events()
			{
				if ((outbound.event_manager._flag) > 0)
					return true;
				else
					return false;
			}
		};

		struct ss_disk
		{
			ss_inbound_data inbound;
			ss_outbound_data outbound;
			bool any()
			{
				if ((inbound.event_manager._flag + outbound.event_manager._flag) > 0)
					return true;
				else
					return false;
			}
			bool in_events()
			{
				if ((inbound.event_manager._flag) > 0)
					return true;
				else
					return false;
			}
			bool out_events()
			{
				if ((outbound.event_manager._flag) > 0)
					return true;
				else
					return false;
			}
		};

		//struct s_data_events
		//{
			static ss_serial serial;
			static ss_disk disk;
			static ss_ready_data ready;
			static ss_inquiry_data inquire;
			static bool any()
			{
				if (serial.any() || disk.any() || ready.any() || inquire.any())
					return true;
				else
					return false;
			}
		//};

	protected:
	private:


	//functions
	public:
		//c_data_events();
		//~c_data_events();
		//c_data_events(const c_data_events &c);
		//c_data_events& operator=(const c_data_events &c);
		
		static void process();

	protected:
	private:
}; //c_serial_events
#endif //__C_DATA_EVENTS_H__
