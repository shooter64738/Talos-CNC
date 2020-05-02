#include "c_framework_cpu.h"
#include "../Data/c_framework_system_data_handler.h"
#include "../Event/c_event_router.h"
#include "../Error/_s_framework_error.h"
#include "../Error/c_framework_error.h"
#include "../Data/cache_data.h"
#include "../Report/c_framework_report.h"

uint8_t ID = 0;
#define BASE_ERROR 10

void c_cpu::initialize(uint8_t id, uint32_t * tick_timer_ms)
{
	this->ID = id;
	this->cycle_count = tick_timer_ms;
}

/*
init_ready_wait_id - is the event id that we wait for, before we do anything
init_message - is the value that is sent to the host
init_request_type - is the message type that is sent to the host
init_response_type - is the event we should get back from the host after
sending init_message+init_request_type indicating we are communicating and processors are connected
*/

#define __CPU_SYNCH 1
void c_cpu::Synch(
e_system_message::messages::e_data init_message
, e_system_message::e_status_type init_request_type
, uint8_t init_ready_wait_id
, e_system_message::e_status_type init_response_type
, bool is_master_cpu)
{
	
	Talos::Shared::FrameWork::StartUp::string_writer("value start =");
	Talos::Shared::FrameWork::StartUp::int32_writer
	(Talos::Shared::c_cache_data::motion_configuration_record.hardware.steps_per_mm[0]);
	Talos::Shared::FrameWork::StartUp::string_writer("\r\n");
	
	
	//if master_cpu its the controller. so we need to send a 'ReadyToProcess' message to our child
	if (is_master_cpu)
	{
		uint32_t time_start = *this->cycle_count;
		//Send ready message to child
		__send_formatted_message((int)e_system_message::messages::e_informal::ReadyToProcess
		,(int)e_system_message::e_status_type::Informal);
		//Message is sent. The master now waits for a ready message from the child
		
		Talos::Shared::FrameWork::StartUp::string_writer("mCU host ready\r\n");
		
		//Wait for specified message
		__wait_formatted_message((int)init_message, (int)init_request_type);
		uint32_t time_end = *this->cycle_count;
		this->message_lag_cycles = time_end - time_start;
		
		//The proper response to inquiry is data
		//if (init_request_type == e_system_message::e_status_type::Inquiry)
		{
			//__send_formatted_message((int)init_message, (int)e_system_message::e_status_type::Data);
			__send_formatted_message((int)init_message, (int)init_response_type);
		}
		//else
		//Child has responded with the specified message we matched on. Now reply with what the child asked for
		//__send_formatted_message((int)init_message, (int)init_request_type);
		
		Talos::Shared::FrameWork::StartUp::string_writer("mCU host specified\r\n");
		
	}
	else
	{
		Talos::Shared::FrameWork::StartUp::string_writer("mCU child ready\r\n");
		
		//Wait for ready message
		__wait_formatted_message((int)e_system_message::messages::e_informal::ReadyToProcess
		,(int)e_system_message::e_status_type::Informal);
		
		Talos::Shared::FrameWork::StartUp::string_writer("mCU child request\r\n");
		
		uint32_t time_start = *this->cycle_count;
		
		//The child has gotten a ready message from the master. The child now tells the master what it wants
		__send_formatted_message((int)init_message, (int)init_request_type);
		
		//The proper wait to inquiry is data
		//if (init_request_type == e_system_message::e_status_type::Inquiry)
		//__wait_formatted_message((int)init_message, (int)e_system_message::e_status_type::Data);
		__wait_formatted_message((int)init_message, (int)init_response_type);
		uint32_t time_end = *this->cycle_count;
		this->message_lag_cycles = time_end - time_start;
		//else
		//IF its not an inquiry wait for the specified message.
		//__wait_formatted_message((int)init_message, (int)init_request_type);
	}
	
	//And now we should be done.
	Talos::Shared::FrameWork::StartUp::string_writer("mCU synch complete!!\r\n");
	
	//set this cpu as 'on line'
	this->system_events.set((int)c_cpu::e_event_type::OnLine);
	
	Talos::Shared::FrameWork::StartUp::string_writer("value end =");
	Talos::Shared::FrameWork::StartUp::int32_writer(
	Talos::Shared::c_cache_data::motion_configuration_record.hardware.steps_per_mm[0]);
	Talos::Shared::FrameWork::StartUp::string_writer("\r\n");
	
	Talos::Shared::FrameWork::StartUp::string_writer("lag =");
	Talos::Shared::FrameWork::StartUp::int32_writer(
	this->message_lag_cycles);Talos::Shared::FrameWork::StartUp::string_writer("\r\n");
	
	//Lock up for now.. this is all testing stuff
	//while(1){}
	
}

#define __SEND_FORMATTED_MESSAGE 2
void c_cpu::__send_formatted_message(uint8_t init_message, uint8_t init_type)
{
	Talos::Shared::FrameWork::Data::System::send((int)init_message //message id #
	, (int)init_type //data type id #
	, Talos::Shared::FrameWork::StartUp::cpu_type.Host //origin of the message
	, this->ID //destination of the message
	, (int)e_system_message::e_status_state::motion::e_state::Idle //state
	, (int)e_system_message::e_status_state::motion::e_sub_state::OK //sub state
	, NULL //position data
	);
}

#define __WAIT_FORMATTED_MESSAGE 3
#define UNKNOWN_DATA 1
void c_cpu::__wait_formatted_message(uint8_t init_message, uint8_t init_type)
{
	while (1)
	{
		//keep processing framework system_events
		Talos::Shared::FrameWork::Events::Router::process();

		//see if host responded with a message, and if so does it match the message and record type we expected
		if (this->system_events.get_clr((int)c_cpu::e_event_type::SystemRecord))
		{
			Talos::Shared::FrameWork::Error::framework_error.sys_message = this->sys_message.message;
			Talos::Shared::FrameWork::Error::framework_error.sys_type = this->sys_message.type;
			Talos::Shared::FrameWork::Error::framework_error.user_code1 = (int)init_message;
			Talos::Shared::FrameWork::Error::framework_error.user_code2 = (int)init_type;
			
			
			if (this->sys_message.message == (int)init_message //(int)e_system_message::messages::e_informal::ReadyToProcess
			&& this->sys_message.type == (int)init_type //(int)e_system_message::e_status_type::Informal
			)
			{
				//Proper message has come in, so return
				return;
			}
			else
			{
				Talos::Shared::FrameWork::StartUp::string_writer("mCU synch unexpected data\r\n");
				Talos::Shared::FrameWork::Error::framework_error.behavior = e_error_behavior::Critical;
				Talos::Shared::FrameWork::Error::raise_error(BASE_ERROR, __WAIT_FORMATTED_MESSAGE, UNKNOWN_DATA, this->ID);
			}
		}
	}
}

void c_cpu::service_events(int32_t * position, uint16_t rpm)
{
	//this->cycle_count++;

	//if NoState and OnLine is false, theres no need to process events.
	if (this->system_events.get((int)c_cpu::e_event_type::NoState)
	|| !this->system_events.get((int)c_cpu::e_event_type::OnLine))
	return;

	//If cpu is in error set it offline
	if (this->system_events.get((int)c_cpu::e_event_type::Error))
	this->system_events.clear((int)c_cpu::e_event_type::OnLine);

	memcpy(&this->sys_message.position, position, sizeof(position)*MACHINE_AXIS_COUNT);
	memcpy(&this->sys_message.rpm, &rpm, sizeof(uint16_t));
	
	__service_host_events();
	
}

void c_cpu::__service_host_events()
{
	/*
	Critical = 0,
	Warning = 1,
	Informal = 2,
	Data = 3,
	Inquiry = 4,
	*/
	__service_host_events_critical();
	__service_host_events_warning();
	__service_host_events_informal();
	__service_host_events_data();
	__service_host_events_inquiry();
}

void c_cpu::__service_host_events_critical()
{
	
}

void c_cpu::__service_host_events_warning()
{
	
}

void c_cpu::__service_host_events_informal()
{
	
}

void c_cpu::__service_host_events_data()
{
	
}

void c_cpu::__service_host_events_inquiry()
{
	//Any events to process
	if (this->host_events.Inquiry._flag == 0)
	return;
	
	//Let the reporter handle G/M/W report requests.
	Talos::Shared::FrameWork::Events::Report::process();
	
	//Are there any events left?
	if (this->host_events.Inquiry._flag == 0)
	return;
	
	//if (this->host_events.Inquiry.get_clr((int) e_system_message::messages::e_inquiry::GCodeBlockReport))
	//{
		//Talos::Shared::FrameWork::StartUp::string_writer("g config request\r\n");
		////__send_formatted_message((int) e_system_message::messages::e_data::MotionConfiguration, (int)e_system_message::e_status_type::Data );
	//}
}

void c_cpu::update_message_time(uint32_t read_time)
{
	Talos::Shared::FrameWork::StartUp::string_writer("lag read=");
	Talos::Shared::FrameWork::StartUp::int32_writer(read_time);
	Talos::Shared::FrameWork::StartUp::string_writer("\r\n");
	this->message_lag_cycles = read_time - *this->cycle_count;
}