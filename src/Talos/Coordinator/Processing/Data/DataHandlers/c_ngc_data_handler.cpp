/*
*  c_data_events.cpp - NGC_RS274 controller.
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
#include "c_ngc_data_handler.h"

#include "../../../../NGC_RS274/_ngc_errors_interpreter.h"
//#include "../../../../Shared_Data/FrameWork/extern_events_types.h"
#include "../../../../NGC_RS274/NGC_Block_View.h"
#include "../../../../NGC_RS274/NGC_Error_Check.h"
#include "../../../../NGC_RS274/NGC_Line_Processor.h"
static uint32_t ngc_block_cache_count = 0;
s_ngc_block Talos::Coordinator::Data::Ngc::active_block;

void Talos::Coordinator::Data::Ngc::load_block_from_cache(c_ring_buffer<char> *source)
{
//	Talos::Shared::c_cache_data::txt_record.pntr_record = Talos::Shared::c_cache_data::txt_record.record;
//
//	s_ngc_block new_block;
//	e_parsing_errors return_value = e_parsing_errors::OK;
//
//	//Forward copy the previous blocks values so they will persist. This also clears whats in the block now.
//	//If the values need changed during processing it will happen in the assignor
//	NGC_RS274::Block_View::copy_persisted_data(&Talos::Shared::c_cache_data::ngc_block_record, &new_block);
//	/*
//	The __station__ value is an indexing value used to give each block a unique ID number in the collection
//	of binary converted data. It is currently an int type, but if it were converted to a float I think
//	it could also be used to locate and identify subroutines.
//	*/
//	new_block.__station__ = Talos::Shared::c_cache_data::ngc_block_record.__station__ + 1;
//
//	//Now process the gcode text line, and give us back a block of data in binary format.
//	if ((return_value = NGC_RS274::LineProcessor::_process_buffer(
//		Talos::Shared::c_cache_data::txt_record.record, &new_block, Talos::Shared::c_cache_data::txt_record.size))
//		!= e_parsing_errors::OK)
//	{
//		__raise_error(Talos::Shared::c_cache_data::txt_record.record);
//		return;
//	}
//
//	//Now that the line parsing is complete we can run an error check on the line
//
//	//Create a view of the old and new blocks. The view class is just a helper class
//	//to make the data easier to understand
//	NGC_RS274::Block_View v_new = NGC_RS274::Block_View(&new_block);
//	NGC_RS274::Block_View v_previous = NGC_RS274::Block_View(&Talos::Shared::c_cache_data::ngc_block_record);
//	if ((return_value = NGC_RS274::Error_Check::error_check(&v_new, &v_previous))
//	!= e_parsing_errors::OK)
//	{
//	
//		__raise_error(Talos::Shared::c_cache_data::txt_record.record);
//		return;
//	}
//
//
//	//Add this block to the buffer
//
//	Talos::Shared::c_cache_data::pntr_write_ngc_block_record(&new_block);
//
//	//Now move the data from the new block back to the init block. This keeps
//	//the block modal values in synch
//	NGC_RS274::Block_View::copy_persisted_data(&new_block, &Talos::Shared::c_cache_data::ngc_block_record);
//	//We dont copy station numbers so set this here.
//	Talos::Shared::c_cache_data::ngc_block_record.__station__ = new_block.__station__;
//	
//	ngc_block_cache_count++;
//	
//	//Clear the block event that was set when the line was loaded waaaaayyyy back in the dataevent handler
////	Talos::Shared::FrameWork::StartUp::CpuCluster[Talos::Shared::FrameWork::StartUp::cpu_type.Host]
////		.host_events.Data.clear((int)e_system_message::messages::e_data::NgcDataLine);
////	
//	Talos::Shared::c_cache_data::txt_record.pntr_record = NULL;
}

void  Talos::Coordinator::Data::Ngc::__raise_error(char * ngc_line)
{
	
	//Talos::Shared::FrameWork::Error::ngc_error_handler(ngc_line);
	
	__reset();
}

void Talos::Coordinator::Data::Ngc::__reset()
{
//	Talos::Shared::FrameWork::StartUp::CpuCluster[Talos::Shared::FrameWork::StartUp::cpu_type.Host]
//		.host_events.Data.clear((int)e_system_message::messages::e_data::NgcDataLine);
//	Talos::Shared::c_cache_data::ngc_block_record.__station__ = 0;


}