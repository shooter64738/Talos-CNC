/*
*  c_events.cpp - NGC_RS274 controller.
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

#include "c_report_events.h"
#include "../../../../Shared Data/FrameWork/Data/cache_data.h"
#include "../../../Record Defines/e_all_event_data_types.h"
#include <math.h>

static c_Serial *rpt_serial;

uint8_t Talos::Motion::Events::Report::initialize(c_Serial *serial)
{
	rpt_serial = serial;
	return 0;
}

void Talos::Motion::Events::Report::process()
{

	if (!Talos::Shared::FrameWork::Events::extern_data_events.inquire.any())
		return;
}

void Talos::Motion::Events::Report::__write_header(s_ngc_block block)
{

}

void Talos::Motion::Events::Report::____group(uint8_t count, uint16_t * pointer, char group_name)
{
}

void Talos::Motion::Events::Report::____word(uint8_t count, float * pointer)
{
}
