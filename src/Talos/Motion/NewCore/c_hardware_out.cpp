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
					/*
					Since this port could be shared with other operations we want to copy
					what is already set and jsut modify as needed*/
					uint16_t port_value = STEPPER_DIR_PORT_DIRECT_REGISTER;
					
					/*
					Since we can have multiple things happen on a single port (step,direction,enable)
					we need to transfer the 1,2,3,4,5,6,x bits from the axis directions to their physical
					pin location on the port. This is the most straight forward way I can think of right
					now.
					*/
					BitXfer_(BitTst(*directions, MACHINE_X_AXIS), port_value, STEPPER_DIR_PIN_0);
					BitXfer_(BitTst(*directions, MACHINE_Y_AXIS), port_value, STEPPER_DIR_PIN_1);
					BitXfer_(BitTst(*directions, MACHINE_Z_AXIS), port_value, STEPPER_DIR_PIN_2);
					BitXfer_(BitTst(*directions, MACHINE_A_AXIS), port_value, STEPPER_DIR_PIN_3);
					BitXfer_(BitTst(*directions, MACHINE_B_AXIS), port_value, STEPPER_DIR_PIN_4);
					BitXfer_(BitTst(*directions, MACHINE_C_AXIS), port_value, STEPPER_DIR_PIN_5);

					//This port may NOT be different than the step or enable port, or it might be.
					STEPPER_DIR_PORT_DIRECT_REGISTER = port_value;
					/*if (_bittest(*directions,MACHINE_X_AXIS)
						hal_mtn::Stepper::step_dir_high();
					else
						hal_mtn::Stepper::step_dir_low();*/
				}

				void Hardware::Motion::brakes(uint16_t* brake_pins)
				{

				}

				void Hardware::Motion::step(uint16_t* outputs)
				{
					//directions may have changed here
					/*
					Since this port could be shared with other operations we want to copy
					what is already set and just modify as needed*/
					uint16_t port_value = STEPPER_PUL_PORT_DIRECT_REGISTER;

					/*
					Since we can have multiple things happen on a single port (step,direction,enable)
					we need to transfer the 1,2,3,4,5,6,x bits from the axis pulse to their physical
					pin location on the port. This is the most straight forward way I can think of right
					now.
					*/
					BitXfer_(BitTst(*outputs, MACHINE_X_AXIS), port_value, STEPPER_PUL_PIN_0);
					BitXfer_(BitTst(*outputs, MACHINE_Y_AXIS), port_value, STEPPER_PUL_PIN_1);
					BitXfer_(BitTst(*outputs, MACHINE_Z_AXIS), port_value, STEPPER_PUL_PIN_2);
					BitXfer_(BitTst(*outputs, MACHINE_A_AXIS), port_value, STEPPER_PUL_PIN_3);
					BitXfer_(BitTst(*outputs, MACHINE_B_AXIS), port_value, STEPPER_PUL_PIN_4);
					BitXfer_(BitTst(*outputs, MACHINE_C_AXIS), port_value, STEPPER_PUL_PIN_5);

					//This port may NOT be different than the step or enable port, or it might be.
					STEPPER_PUL_PORT_DIRECT_REGISTER = port_value;
				}

				void Hardware::Motion::un_step(uint16_t* outputs)
				{
					//directions may have changed here
					/*
					Since this port could be shared with other operations we want to copy
					what is already set and jsut modify as needed*/
					uint16_t port_value = STEPPER_PUL_PORT_DIRECT_REGISTER;

					/*
					Since we can have multiple things happen on a single port (step,direction,enable)
					we need to transfer the 1,2,3,4,5,6 bits from the axis directions to their physical
					pin location on the port. This is the most straight forward way I can think of right
					now.
					*/
					BitXfer_(0, port_value, STEPPER_PUL_PIN_0);
					BitXfer_(0, port_value, STEPPER_PUL_PIN_1);
					BitXfer_(0, port_value, STEPPER_PUL_PIN_2);
					BitXfer_(0, port_value, STEPPER_PUL_PIN_3);
					BitXfer_(0, port_value, STEPPER_PUL_PIN_4);
					BitXfer_(0, port_value, STEPPER_PUL_PIN_5);

					//This port may NOT be different than the step or enable port, or it might be.
					STEPPER_PUL_PORT_DIRECT_REGISTER = port_value;
				}

#pragma endregion
				//-------------------------------------------------------------------
			}
		}
	}
}