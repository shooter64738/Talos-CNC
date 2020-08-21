/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/

/*

All control flags for motion are in here. If its a flag that has something to do with the motion
it is controlled by flags in here. If I find a missed flag I move it to here. I want a central
location to control all motion output from.

*/


#include "c_coordinator_state_control.h"
#include "../../../Configuration/c_configuration.h"
#include "../../../Shared_Data/Kernel/Base/c_kernel_base.h"
#include "../Data/DataHandlers/c_ngc_data_handler.h"
#include "../../../Motion/NewCore/c_ngc_to_block.h"
#include "../../../Motion/Processing/State_Control/c_motion_state_control.h"

//temporary until I get the hal tied to this.
#define HAL_SYS_TICK_TIME() 0
#define HAL_MOTION_ON() 0
#define HAL_MOTION_OFF() 0
#define HAL_SPINDLE_ON() 0
#define HAL_SPINDLE_OFF() 0
#define HAL_SPINDLE_SPEED() 0

namespace mtn_inp = Talos::Motion::Core::Input;
namespace crd_dat = Talos::Coordinator::Data;
namespace mtn_ctl_sta = Talos::Motion::Core::States;

//namespace mtn_cfg = Talos::Configuration::Motion;
//namespace mtn_out = Talos::Motion::Core::Output;

namespace Talos
{
	namespace Coordinator
	{
		namespace Core
		{
			namespace States
			{
				/*--------------------------------------------------------------------------*/
#pragma region Global/All state control wrapper

				void execute()
				{
					execute(e_state_class::Process);
					execute(e_state_class::Report);
					//cexecute(e_state_class::Motion);
					execute(e_state_class::Output);

				}

				void execute(e_state_class st_class)
				{
					switch (st_class)
					{
						//case e_state_class::Motion:
						//{
						//	Motion::execute();
						//	Process::execute(); //<--adding a process executor to keep the buffer full
						//	break;
						//}
					case e_state_class::Process:
					{
						Process::execute();

						break;
					}
					case e_state_class::Report:
					{
						Report::execute();

						break;
					}
					case e_state_class::Output:
					{
						Output::execute();
						//Process::execute(); //<--adding a process executor to keep the buffer full
						break;
					}
					default:
						break;
					}
				}

#pragma endregion
				/*--------------------------------------------------------------------------*/
#pragma region Motion class state control


#pragma endregion
				/*--------------------------------------------------------------------------*/
#pragma region Process class state control

				void Process::execute()
				{
					if (Kernel::CPU::cluster[Kernel::CPU::host_id].host_events.Data.get_clr(e_system_message::messages::e_data::NgcDataLine))
					{

						uint8_t ret = __read_ngc_line(
							Kernel::CPU::cluster[Kernel::CPU::host_id].dvc_source->active_rcv_buffer->overlays.text
							, Kernel::CPU::cluster[Kernel::CPU::host_id].dvc_source->active_rcv_buffer->read_count);
						//reset the buffer and be ready for the next line.
						Kernel::CPU::cluster[Kernel::CPU::host_id].cdh_r_reset();

						if (mtn_inp::Block::ngc_buffer._full)
						{
							//motion buffer is full and if we are going to allow serial streaming we HAVE to tell
							//the host we are full and they must stop streaming right now.
							Kernel::Report::ngc_status(e_block_process_State::ngc_buffer_full);
						}
						else
						{
							//if we are allowing serial streaming we should tell the host we have room for more data 
							Kernel::Report::ngc_status(e_block_process_State::ngc_block_accepted);
						}
					}

				}

				uint8_t Process::__read_ngc_line(char* source, uint16_t data_size)
				{
					uint8_t ret = 0;
					//the flag for ngc data in the serial buffer is set. we need to load that data
					//and run it through the interpreter.
					//Also this should be the only place where motion, kernel, and coordinator connect.
					s_ngc_block* new_block = crd_dat::Ngc::load_block_from_cache(source, data_size);

					//We just processed a block from txt, and now it is binary ngc data. We need to queue it
					//into the motion buffer. In the case of it being null, there was an error during processing
					//and we could not convert it. Check the ngc parser errors to find out why.
					if (new_block != NULL)
					{
						mtn_inp::Block::ngc_buffer.put(*new_block);
						mtn_ctl_sta::Process::states.set(mtn_ctl_sta::Process::e_states::ngc_buffer_not_empty);
						ret = 0;
					}
					else
					{
						Kernel::Report::ngc_status(e_block_process_State::ngc_block_rejected);
						ret = 1;
					}
					
					return ret;
				}

#pragma endregion
				/*--------------------------------------------------------------------------*/
#pragma region Report class state control
				void Report::execute()
				{
					if (Kernel::CPU::cluster[Kernel::CPU::host_id].host_events.Inquiry._flag != 0)
					{
						//reset the buffer and be ready for the next line.
						Kernel::CPU::cluster[Kernel::CPU::host_id].cdh_r_reset();

						if (Kernel::CPU::cluster[Kernel::CPU::host_id].host_events.Inquiry.get_clr(e_system_message::messages::e_inquiry::GCodeBlockReport))
							Kernel::Report::interpreter_status(crd_dat::Ngc::active_block, e_system_message::messages::e_inquiry::GCodeBlockReport);

						if (Kernel::CPU::cluster[Kernel::CPU::host_id].host_events.Inquiry.get_clr(e_system_message::messages::e_inquiry::MCodeBlockReport))
							Kernel::Report::interpreter_status(crd_dat::Ngc::active_block, e_system_message::messages::e_inquiry::MCodeBlockReport);

						if (Kernel::CPU::cluster[Kernel::CPU::host_id].host_events.Inquiry.get_clr(e_system_message::messages::e_inquiry::WordStatusReport))
							Kernel::Report::interpreter_status(crd_dat::Ngc::active_block, e_system_message::messages::e_inquiry::WordStatusReport);
						
						if (Kernel::CPU::cluster[Kernel::CPU::host_id].host_events.Inquiry.get_clr(e_system_message::messages::e_inquiry::InterpreterConfiguration))
							Kernel::Report::interpreter_status(crd_dat::Ngc::active_block, e_system_message::messages::e_inquiry::InterpreterConfiguration);
					}

				}

#pragma endregion
				/*--------------------------------------------------------------------------*/
#pragma region Output class state control
				void Output::execute()
				{

				}

#pragma endregion
				/*--------------------------------------------------------------------------*/
			}
		}
	}
}