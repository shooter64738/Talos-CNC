/*
* c_processor.cpp
*
* Created: 5/8/2019 2:18:48 PM
* Author: Family
*/


#include "c_hardware_out.h"
#include "../../talos_hardware_def.h"
namespace hal_mtn = Hardware_Abstraction_Layer::MotionCore;

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
				void Hardware::Spindle::start(__s_spindle_block spindle_block)
				{

				}

				void Hardware::Spindle::stop(__s_spindle_block spindle_block)
				{

				}

				bool Hardware::Spindle::synch_wait(__s_spindle_block spindle_block)
				{
					return false;
				}

				uint32_t Hardware::Spindle::get_speed(__s_spindle_block spindle_block)
				{

					return 0;
				}

#pragma endregion
				//-------------------------------------------------------------------
#pragma region Motion driver

				void Hardware::Motion::initialize()
				{
					//hal_mtn::Stepper::wake_up();
				}

				void Hardware::Motion::enable()
				{
					hal_mtn::Stepper::wake_up();
				}

				void Hardware::Motion::disable()
				{
					hal_mtn::Stepper::st_go_idle();
				}

				void Hardware::Motion::time_adjust(uint32_t* time)
				{
					
				}

				void Hardware::Motion::start()
				{
					//hal_mtn::Stepper::st_go_idle();
				}

				void Hardware::Motion::stop()
				{
					//hal_mtn::Stepper::st_go_idle();
				}

				void  Hardware::Motion::direction(uint16_t* directions)
				{
					//directions may have changed here
					if (*directions)
						hal_mtn::Stepper::step_dir_high();
					else
						hal_mtn::Stepper::step_dir_low();
				}

				void Hardware::Motion::brakes(uint16_t* brake_pins)
				{

				}
				void Hardware::Motion::step(uint16_t* outputs)
				{
					hal_mtn::Stepper::step_port(*outputs);
				}

#pragma endregion
				//-------------------------------------------------------------------
			}
		}
	}
}