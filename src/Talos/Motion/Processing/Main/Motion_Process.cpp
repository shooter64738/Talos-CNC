/*
* Main_Process.cpp
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
*/


#include "Motion_Process.h"
#include "../../../talos_hardware_def.h"

/*
testing
*/
#include "../../NewCore/c_ngc_to_block.h"
//#include "../../NewCore/c_block_to_segment.h"
#include "../State_Control/c_motion_state_control.h"
#include "../../..//Configuration/c_configuration.h"

namespace mtn_cfg = Talos::Configuration::Motion;

void Talos::Motion::Main_Process::mot_initialize()
{
	mtn_cfg::Controller.load_defaults();

	Hardware_Abstraction_Layer::MotionCore::Stepper::initialize();
	Core::States::Motion::states.set(Core::States::Motion::e_states::auto_cycle_start);
}

void Talos::Motion::Main_Process::mot_run()
{
	//load the testing block
	//Motion::Core::Input::Block::load_ngc_test();
	//if we set this flag, motion will begin!
	
	//while(1)
	//{
		Core::States::execute();
	//}
}