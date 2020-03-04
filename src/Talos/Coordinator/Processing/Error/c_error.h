/*
*  c_events.h - NGC_RS274 controller.
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

#ifndef __C_ERROR_HANDLING_H__
#define __C_ERROR_HANDLING_H__

#include <stdint.h>
#include "../../../Shared Data/_s_framework_error.h"
#include "../../../c_ring_template.h"
#include "../../Serial/c_Serial.h"
#include "../../../Shared Data/FrameWork/extern_events_types.h"

namespace Talos
{
	namespace Coordinator
	{
		class Error
		{
			//variables
		public:

		protected:
		private:


			//functions
		public:
			static void general_error(e_error_behavior e_behavior
				, uint8_t data_size, e_error_group e_group, e_error_process e_process, e_record_types e_rec_type
				, e_error_source e_source, e_error_code e_code, uint8_t e_origin, e_error_stack e_stack);
			
			static void ngc_error(char * ngc_line, e_error_behavior e_behavior
				, uint8_t data_size, e_error_group e_group, e_error_process e_process, e_record_types e_rec_type
				, e_error_source e_source, e_error_code e_code, uint8_t e_origin, e_error_stack e_stack);
			
			static uint8_t initialize(c_Serial *serial);
		private:
			static void __print_base(e_error_behavior e_behavior
				, uint8_t data_size, e_error_group e_group, e_error_process e_process, e_record_types e_rec_type
				, e_error_source e_source, e_error_code e_code, uint8_t e_origin, uint16_t e_stack);

			static void __write_eol();


		};
	};
};
#endif //__C_EVENTS_H__
