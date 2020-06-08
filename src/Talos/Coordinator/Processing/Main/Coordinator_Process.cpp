/*
* Main_Process.cpp
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
This is the main entry point for the coordinator. All things pass through here and
then move to their respective modules.

*/



#include "Coordinator_Process.h"
#include "../../../NGC_RS274/NGC_Line_Processor.h"
#include "../../../NGC_RS274/NGC_Tool.h"
#include "../../../NGC_RS274/NGC_Coordinates.h"
#include "../../../NGC_RS274/NGC_System.h"
#include "../../../Configuration/c_configuration.h"
#include "../Events/EventHandlers/c_ngc_data_events.h"
#include "../Events/EventHandlers/c_system_event_handler.h"
#include "../Data/DataHandlers/c_system_data_handler.h"
#include "../Data/DataHandlers/c_ngc_data_handler.h"
//#include "../../../Shared_Data/_s_status_record.h"
#include "../../../Shared_Data/Settings/Motion/_s_motion_control_settings_encapsulation.h"
#include "../../../Configuration/c_configuration.h"
#include "../../../talos_hardware_def.h"


#include "../../../Shared_Data/Kernel/Base/c_kernel_base.h"
#include "../State_Control/c_coordinator_state_control.h"

using namespace Talos;

volatile uint8_t safe11 = 1;
volatile uint8_t safe12 = 1;
void Coordinator::Main_Process::cord_initialize()
{
	//Hardware_Abstraction_Layer::Disk::initialize(NULL);
	Talos::Configuration::Interpreter::DefaultBlock.load_defaults();
	Talos::Coordinator::Data::Ngc::set_start_block(Talos::Configuration::Interpreter::DefaultBlock.Settings);
}

void Talos::Coordinator::Main_Process::cord_run()
{
	//Check state manager for actions that need to run.

	Kernel::CPU::service_events();
	Talos::Coordinator::Core::States::execute();

}