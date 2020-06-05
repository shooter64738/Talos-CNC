/*
* Main_Process.cpp
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
*/


#include "Motion_Process.h"
#include "../../../talos_hardware_def.h"
//#include "../../../Shared_Data/Kernel/Base/c_kernel_base.h"

/*
testing
*/
#include "../../NewCore/c_ngc_to_block.h"
//#include "../../NewCore/c_block_to_segment.h"
#include "../../NewCore/c_state_control.h"

volatile uint8_t safe21 = 1;
volatile uint8_t safe22 = 1;

void Talos::Motion::Main_Process::mot_initialize()
{

	Hardware_Abstraction_Layer::MotionCore::Stepper::initialize();
}

void Talos::Motion::Main_Process::mot_run()
{
	//load the testing block
	Motion::Core::Input::Block::load_ngc_test();
	//if we set this flag, motion will begin!
	Core::States::Motion::states.set(Core::States::Motion::e_states::cycle_start);
	while(1)
	{
		Core::States::execute();
	}

	while (1) {}
}