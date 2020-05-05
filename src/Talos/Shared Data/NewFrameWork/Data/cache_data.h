#ifndef __C_CACHE_DATA__
#define __C_CACHE_DATA__

#include "../../_s_status_record.h"
#include "../../_s_ngc_line_record.h"
#include "../../Settings/Motion/_s_motion_control_settings_encapsulation.h"
#include "../../../NGC_RS274/_ngc_block_struct.h"
//#include "../extern_events_types.h"

//typedef void(*ret_pointer)(c_ring_buffer <char> * buffer);
namespace Talos
{
	namespace Shared
	{
		class c_cache_data
		{
			//variables
		public:
			static s_motion_control_settings_encapsulation motion_configuration_record;

			static s_txt_line_record txt_record;
			static s_ngc_block ngc_block_record;
			
			static uint8_t(*pntr_write_ngc_block_record)(s_ngc_block * ngc_block_record);
			static uint8_t(*pntr_read_ngc_block_record)(s_ngc_block * ngc_block_record);
			static uint32_t tic_count;
		protected:
		private:


			//functions
		public:
		protected:
		private:
		};
	};
};

#endif