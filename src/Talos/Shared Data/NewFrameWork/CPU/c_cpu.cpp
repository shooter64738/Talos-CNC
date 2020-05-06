#include "c_cpu.h"
//#include "c_framework_start.h"
#include "../Comm/c_framework_comm.h"
#include "../Error/c_framework_error.h"
#include "../CPU/c_framework_cpu.h"
#include "../Data/c_framework_data_handler.h"
#include "../Data/s_data_packet.h"
#include "../../../communication_def.h"


uint8_t ID = 0;
#define BASE_ERROR 10
void c_cpu::initialize(uint8_t id, volatile uint32_t * tick_timer_ms)
{
	this->ID = id;
	this->pntr_cycle_count_ms = tick_timer_ms;

	if (pntr_cycle_count_ms != NULL)
		this->next_cycle_check_time = *(pntr_cycle_count_ms)+HEALTH_CHECK_TIME_MS;

	//Setup the ring buffer so it points to the storage array.
	hw_data_buffer.initialize(this->hw_data_container, RING_BUFFER_SIZE);
}

bool c_cpu::send_message(uint8_t message
	, uint8_t type
	, uint8_t origin
	, uint8_t target
	, uint8_t state
	, uint8_t sub_state
	, int32_t * position_data)
{
	return false;
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
	Talos::NewFrameWork::Comm::pntr_string_writer(Talos::NewFrameWork::CPU::host_id, "value start =");
	/*Talos::NewFrameWork::Comm::USART0._int32_writer
		(Talos::Shared::c_cache_data::motion_configuration_record.hardware.steps_per_mm[0]);*/
	Talos::NewFrameWork::Comm::pntr_string_writer(Talos::NewFrameWork::CPU::host_id, "\r\n");

	//if master_cpu its the controller. so we need to send a 'ReadyToProcess' message to our child
	if (is_master_cpu)
	{
		uint32_t time_start = *this->pntr_cycle_count_ms;
		//Send ready message to child
		__send_formatted_message((int)e_system_message::messages::e_informal::ReadyToProcess
			, (int)e_system_message::e_status_type::Informal);
		//Message is sent. The master now waits for a ready message from the child

		Talos::NewFrameWork::Comm::pntr_string_writer(Talos::NewFrameWork::CPU::host_id, "mCU host ready\r\n");

		//Wait for specified message
		__wait_formatted_message((int)init_message, (int)init_request_type);
		uint32_t time_end = *this->pntr_cycle_count_ms;
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

		Talos::NewFrameWork::Comm::pntr_string_writer(Talos::NewFrameWork::CPU::host_id, "mCU host specified\r\n");

	}
	else
	{
		Talos::NewFrameWork::Comm::pntr_string_writer(Talos::NewFrameWork::CPU::host_id, "mCU child ready\r\n");

		//Wait for ready message
		__wait_formatted_message((int)e_system_message::messages::e_informal::ReadyToProcess
			, (int)e_system_message::e_status_type::Informal);

		Talos::NewFrameWork::Comm::pntr_string_writer(Talos::NewFrameWork::CPU::host_id, "mCU child request\r\n");

		uint32_t time_start = *this->pntr_cycle_count_ms;

		//The child has gotten a ready message from the master. The child now tells the master what it wants
		__send_formatted_message((int)init_message, (int)init_request_type);

		//The proper wait to inquiry is data
		//if (init_request_type == e_system_message::e_status_type::Inquiry)
		//__wait_formatted_message((int)init_message, (int)e_system_message::e_status_type::Data);
		__wait_formatted_message((int)init_message, (int)init_response_type);
		uint32_t time_end = *this->pntr_cycle_count_ms;
		this->message_lag_cycles = time_end - time_start;
		//else
		//IF its not an inquiry wait for the specified message.
		//__wait_formatted_message((int)init_message, (int)init_request_type);
	}

	//Since we set events to handle and we processed those internally, clear all event flags here
	this->host_events.Critical._flag = 0;
	this->host_events.Data._flag = 0;
	this->host_events.Informal._flag = 0;
	this->host_events.Inquiry._flag = 0;
	this->host_events.Warning._flag = 0;
	this->system_events._flag = 0;

	//And now we should be done.
	Talos::NewFrameWork::Comm::pntr_string_writer(Talos::NewFrameWork::CPU::host_id, "mCU synch complete!!\r\n");

	//set this active_cpu as 'on line'
	this->system_events.set((int)c_cpu::e_event_type::OnLine);

	Talos::NewFrameWork::Comm::pntr_string_writer(Talos::NewFrameWork::CPU::host_id, "value end =");
	/*Talos::NewFrameWork::Comm::USART0._int32_writer(
		Talos::Shared::c_cache_data::motion_configuration_record.hardware.steps_per_mm[0]);*/
	Talos::NewFrameWork::Comm::pntr_string_writer(Talos::NewFrameWork::CPU::host_id, "\r\n");
	Talos::NewFrameWork::Comm::pntr_string_writer(Talos::NewFrameWork::CPU::host_id, "lag =");
	Talos::NewFrameWork::Comm::pntr_int32_writer(Talos::NewFrameWork::CPU::host_id, this->message_lag_cycles);
	Talos::NewFrameWork::Comm::pntr_string_writer(Talos::NewFrameWork::CPU::host_id, "\r\n");
}

#define __SEND_FORMATTED_MESSAGE 2
void c_cpu::__send_formatted_message(uint8_t init_message, uint8_t init_type)
{
	send_message((int)init_message //message id #
		, (int)init_type //data type id #
		, Talos::NewFrameWork::CPU::host_id// Shared::FrameWork::StartUp::cpu_type.Host //origin of the message
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
		//Talos::Shared::FrameWork::Events::Router::process();

		//see if host responded with a message, and if so does it match the message and record type we expected
		if (this->system_events.get_clr((int)c_cpu::e_event_type::SystemRecord))
		{
			Talos::NewFrameWork::Error::framework_error.sys_message = this->sys_message.message;
			Talos::NewFrameWork::Error::framework_error.sys_type = this->sys_message.type;
			Talos::NewFrameWork::Error::framework_error.user_code1 = (int)init_message;
			Talos::NewFrameWork::Error::framework_error.user_code2 = (int)init_type;


			if (this->sys_message.message == (int)init_message //(int)e_system_message::messages::e_informal::ReadyToProcess
				&& this->sys_message.type == (int)init_type //(int)e_system_message::e_status_type::Informal
				)
			{
				//Proper message has come in, so return
				return;
			}
			else
			{
				Talos::NewFrameWork::Comm::pntr_string_writer(Talos::NewFrameWork::CPU::host_id, "mCU synch unexpected data\r\n");
				Talos::NewFrameWork::Error::framework_error.behavior = e_error_behavior::Critical;
				Talos::NewFrameWork::Error::raise_error(BASE_ERROR, __WAIT_FORMATTED_MESSAGE, UNKNOWN_DATA, this->ID);
			}
		}
	}
}

void c_cpu::service_events(int32_t * position, uint16_t rpm)
{
	//This should get called frequently from the main line. 
	__check_data();

	__check_health();

	//if NoState and OnLine is false, theres no need to process events.
	if (this->system_events.get((int)c_cpu::e_event_type::NoState)
		|| !this->system_events.get((int)c_cpu::e_event_type::OnLine))
		return;

	//If active_cpu is in error set it offline
	if (this->system_events.get((int)c_cpu::e_event_type::Error))
		this->system_events.clear((int)c_cpu::e_event_type::OnLine);

	if (position != NULL)
		memcpy(&this->sys_message.position, position, sizeof(int32_t)*MACHINE_AXIS_COUNT);

	memcpy(&this->sys_message.rpm, &rpm, sizeof(uint16_t));
}

void c_cpu::__check_data()
{
	//If not conencted to any data just return
	if (this->hw_data_buffer._storage_pointer == NULL)
		return;
	//TODO:neeed an indicator to determine when data has arrived for this cpu

	//Check if there is data to read. If not return

	//Check to see if this data is setup to be read already. If not assign a reader

	if (cdh_is_busy)
	{
		cdh_read();
	}
	else
	{
		cdh_get_message_type(&this->hw_data_buffer);
		cdh_read();
	}
}

void c_cpu::__check_health()
{
	if (this->pntr_cycle_count_ms == NULL)
		return;
	//If the time code hasn't changed (we havent gotten a system message in X time)
	//then we will assume the active_cpu is not healthy. Perhaps lost connection or its just lagging
	if (*(this->pntr_cycle_count_ms) > this->next_cycle_check_time)
	{
		this->next_cycle_check_time = *(pntr_cycle_count_ms)+HEALTH_CHECK_TIME_MS;
		if (this->sys_message.time_code == last_time_code)
			this->system_events.set((int)c_cpu::e_event_type::UnHealthy);
		else
			this->system_events.clear((int)c_cpu::e_event_type::UnHealthy);
		last_time_code = this->sys_message.time_code;
	}

}
