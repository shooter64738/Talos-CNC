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

//temporary until I get the hal tied to this.
#define HAL_SYS_TICK_TIME() 0
#define HAL_MOTION_ON() 0
#define HAL_MOTION_OFF() 0
#define HAL_SPINDLE_ON() 0
#define HAL_SPINDLE_OFF() 0
#define HAL_SPINDLE_SPEED() 0

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
					execute(e_state_class::Motion);
					execute(e_state_class::Output);

				}

				void execute(e_state_class st_class)
				{
					switch (st_class)
					{
					case e_state_class::Motion:
					{
						Motion::execute();
						Process::execute(); //<--adding a process executor to keep the buffer full
						break;
					}
					case e_state_class::Process:
					{
						Process::execute();
						
						break;
					}
					case e_state_class::Output:
					{
						Output::execute();
						Process::execute(); //<--adding a process executor to keep the buffer full
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
				

#pragma endregion
				/*--------------------------------------------------------------------------*/
#pragma region Output class state control
				

#pragma endregion
				/*--------------------------------------------------------------------------*/
			}
		}
	}
}