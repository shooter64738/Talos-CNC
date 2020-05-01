#include "c_framework_cpu.h"
#include "../Data/c_framework_system_data_handler.h"
#include "../Event/c_event_router.h"

uint8_t ID = 0;

void c_cpu::initialize(uint8_t id)
{
	this->ID = id;
}

/*
init_ready_wait_id - is the event id that we wait for, before we do anything
init_message - is the value that is sent to the host
init_type - is the message type that is sent to the host
init_ready_id - is the event we should get back from the host after
	sending init_message+init_type indicating we are communicating and processors are connected
*/


void c_cpu::Synch(
	e_system_message::messages::e_data init_message
	, e_system_message::e_status_type init_type
	, uint8_t init_ready_wait_id
	, uint8_t init_ready_id
	, bool is_master_cpu)
{
	//if host is the coordinator its the controller. so we need to send a 'ReadyToProcess' message to our child
	if (is_master_cpu)
	{
		Talos::Shared::FrameWork::StartUp::string_writer("mCU synch host\r\n");
		Talos::Shared::FrameWork::Data::System::send((int)e_system_message::messages::e_informal::ReadyToProcess //message id # (ReadyToProcess)
			, (int)e_system_message::e_status_type::Informal //data type id # (Informal)
			, Talos::Shared::FrameWork::StartUp::cpu_type.Host //origin of the message
			, this->ID //destination of the message
			, (int)e_system_message::e_status_state::motion::e_state::Idle //state
			, (int)e_system_message::e_status_state::motion::e_sub_state::OK //sub state
			, NULL //position data
		);

		//Message is sent
	}

	//host is used genericlly here. It is any cpu that controls 'this' one. 

	/*
	Wait for a ready message to come in. If this is the host, its waiting on a child to RESPOND.
	If this is a child, then it is RESPONDING to the host 'ReadyToProcess' message
	*/
	Talos::Shared::FrameWork::StartUp::string_writer("mCU wait synch\r\n");
	while (1)
	{
		//keep processing framework system_events
		Talos::Shared::FrameWork::Events::Router::process();

		//see if host responded with a message, and if so does it match the message and record type we expected 
		if (this->system_events.get((int)c_cpu::e_event_type::SystemRecord))
		{
			if (this->sys_message.message == (int)e_system_message::messages::e_informal::ReadyToProcess
				&& this->sys_message.type == (int)e_system_message::e_status_type::Informal)
			{
				Talos::Shared::FrameWork::StartUp::string_writer("mCU synch request\r\n");
				//Reply with the prescribed message
				Talos::Shared::FrameWork::Data::System::send((int)init_message //message id #
					, (int)init_type //data type id #
					, Talos::Shared::FrameWork::StartUp::cpu_type.Host //origin of the message
					, this->ID //destination of the message
					, (int)e_system_message::e_status_state::motion::e_state::Idle //state
					, (int)e_system_message::e_status_state::motion::e_sub_state::OK //sub state
					, NULL //position data
				);

				Talos::Shared::FrameWork::StartUp::string_writer("mCU wait synch\r\n");
				//Hardware_Abstraction_Layer::Core::set_time_delay(5);
				while (1)
				{
					//keep processing framework system_events
					Talos::Shared::FrameWork::Events::Router::process();

					//see if host responded with a message, and if so does it match the message and record type we expected 
					if (this->system_events.get((int)c_cpu::e_event_type::SystemRecord))
					{
						//not sure yet what message I am going to respond with.... 
						if (this->sys_message.message == (int)e_system_message::messages::e_informal::ReadyToProcess
							&& this->sys_message.type == (int)init_type
							&& this->host_events.Data.get((int)init_message))
						{
							Talos::Shared::FrameWork::StartUp::string_writer("mCU synch complete\r\n");
							this->system_events.set((int)c_cpu::e_event_type::OnLine);
							//probably jsut return here but until i decide for sure, just going to loop
							while (1)
							{
							}
						}
						else
						{
							Talos::Shared::FrameWork::StartUp::string_writer("mCU synch unexpected data\r\n");
						}
					}
				}
			}
			else
			{
				Talos::Shared::FrameWork::StartUp::string_writer("mCU synch unexpected data\r\n");
			}
		}
	}
}

void c_cpu::service_events(int32_t * position, uint16_t rpm)
{
	//if NoState and OnLine is false, theres not need to process events.
	if (this->system_events.get((int)c_cpu::e_event_type::NoState)
		|| !this->system_events.get((int)c_cpu::e_event_type::OnLine))
		return;

	//If cpu is in error set it offline
	if (this->system_events.get((int)c_cpu::e_event_type::Error))
		this->system_events.clear((int)c_cpu::e_event_type::OnLine);

	memcpy(&this->sys_message.position, position, sizeof(position)*MACHINE_AXIS_COUNT);
	//memcpy(&this->sys_message.rpm, &rpm, sizeof(uint16_t));

		//SystemRecord = 3,
		//AddendumRecord = 4,
}