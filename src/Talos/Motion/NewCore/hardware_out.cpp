/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_hardware_out.h
/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_hardware_out.h"

namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Output
			{
				//-------------------------------------------------------------------
#pragma region Spindle driver
				void Hardware::Spindle::start(uint32_t speed)
				{

				}

				void Hardware::Spindle::stop()
				{

				}

				bool Hardware::Spindle::synch_wait()
				{

				}

				uint32_t Hardware::Spindle::get_speed()
				{

				}

#pragma endregion
				//-------------------------------------------------------------------
#pragma region Motion driver

				void Hardware::Motion::start()
				{
					Hardware_Abstraction_Layer::MotionCore::Stepper::wake_up();
				}

				void Hardware::Motion::stop()
				{

				}

#pragma endregion
				//-------------------------------------------------------------------
			}
		}
	}
}
