#include "c_cpu.h"
#include "../Comm/c_kernel_comm.h"
#include "../Error/c_kernel_error.h"
#include "../Error/kernel_error_codes_cpu.h"
#include "../CPU/c_kernel_cpu.h"
#include "../../../communication_def.h"
#include "../c_kernel_utils.h"

using Talos::Kernel::ErrorCodes::ERR_CPU;

uint8_t ID = 0;

bool c_cpu::initialize(uint8_t id, volatile uint32_t * tick_timer_ms)
{
	this->ID = id;
	this->pntr_cycle_count_ms = tick_timer_ms;

	if (pntr_cycle_count_ms != NULL)
		this->next_cycle_check_time = *(pntr_cycle_count_ms)+HEALTH_CHECK_TIME_MS;

	//Setup the ring buffer so it points to the storage array.
	if (!hw_data_buffer.initialize(this->hw_data_container, RING_BUFFER_SIZE))
	{
		Talos::Kernel::Error::raise_error(ERR_CPU::BASE, ERR_CPU::METHOD::initialize, ERR_CPU::METHOD::LINE::buffer_init_failed_pointer_null, id);
		return false;
	}

	return true;
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
bool c_cpu::Synch(
	e_system_message::messages::e_data init_message
	, e_system_message::e_status_type init_request_type
	, uint8_t init_ready_wait_id
	, e_system_message::e_status_type init_response_type
	, bool is_master_cpu)
{
	Talos::Kernel::Comm::pntr_string_writer(Talos::Kernel::CPU::host_id, "value start =");
	/*Talos::Kernel::Comm::USART0._int32_writer
		(Talos::Shared::c_cache_data::motion_configuration_record.hardware.steps_per_mm[0]);*/
	Talos::Kernel::Comm::pntr_string_writer(Talos::Kernel::CPU::host_id, "\r\n");

	//if master_cpu its the controller. so we need to send a 'ReadyToProcess' message to our child
	if (is_master_cpu)
	{
		uint32_t time_start = *this->pntr_cycle_count_ms;
		//Send ready message to child
		ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(
			__send_formatted_message((int)e_system_message::messages::e_informal::ReadyToProcess, (int)e_system_message::e_status_type::Informal)
			, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::synch, ERR_CPU::METHOD::__send_formatted_message);
		//Message is sent. The master now waits for a ready message from the child

		Talos::Kernel::Comm::pntr_string_writer(Talos::Kernel::CPU::host_id, "mCU host ready\r\n");

		//Wait for specified message
		ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(
			__wait_formatted_message((int)init_message, (int)init_request_type)
			, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::synch, ERR_CPU::METHOD::__wait_formatted_message);

		uint32_t time_end = *this->pntr_cycle_count_ms;
		this->message_lag_cycles = time_end - time_start;

		ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(
			__send_formatted_message((int)init_message, (int)init_response_type)
			, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::synch, ERR_CPU::METHOD::__send_formatted_message);

		Talos::Kernel::Comm::pntr_string_writer(Talos::Kernel::CPU::host_id, "mCU host specified\r\n");

	}
	else
	{
		Talos::Kernel::Comm::pntr_string_writer(Talos::Kernel::CPU::host_id, "mCU child ready\r\n");

		//Wait for ready message
		ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(
			__wait_formatted_message((int)e_system_message::messages::e_informal::ReadyToProcess, (int)e_system_message::e_status_type::Informal)
			, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::synch, ERR_CPU::METHOD::__wait_formatted_message);

		Talos::Kernel::Comm::pntr_string_writer(Talos::Kernel::CPU::host_id, "mCU child request\r\n");

		uint32_t time_start = *this->pntr_cycle_count_ms;

		//The child has gotten a ready message from the master. The child now tells the master what it wants
		ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(
			__send_formatted_message((int)init_message, (int)init_request_type)
			, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::synch, ERR_CPU::METHOD::__send_formatted_message);


		//The proper wait to inquiry is data
		ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(
			__wait_formatted_message((int)init_message, (int)init_response_type)
			, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::synch, ERR_CPU::METHOD::__wait_formatted_message);

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
	Talos::Kernel::Comm::pntr_string_writer(Talos::Kernel::CPU::host_id, "mCU synch complete!!\r\n");

	//set this active_cpu as 'on method_or_line'
	this->system_events.set((int)c_cpu::e_event_type::OnLine);

	Talos::Kernel::Comm::pntr_string_writer(Talos::Kernel::CPU::host_id, "value end =");
	/*Talos::Kernel::Comm::USART0._int32_writer(
		Talos::Shared::c_cache_data::motion_configuration_record.hardware.steps_per_mm[0]);*/
	Talos::Kernel::Comm::pntr_string_writer(Talos::Kernel::CPU::host_id, "\r\n");
	Talos::Kernel::Comm::pntr_string_writer(Talos::Kernel::CPU::host_id, "lag =");
	Talos::Kernel::Comm::pntr_int32_writer(Talos::Kernel::CPU::host_id, this->message_lag_cycles);
	Talos::Kernel::Comm::pntr_string_writer(Talos::Kernel::CPU::host_id, "\r\n");

	return true;
}

bool c_cpu::__send_formatted_message(uint8_t init_message, uint8_t init_type)
{
	ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(
		send_message((int)init_message //message id #
			, (int)init_type //data type id #
			, Talos::Kernel::CPU::host_id// Shared::FrameWork::StartUp::cpu_type.Host //origin of the message
			, this->ID //destination of the message
			, (int)e_system_message::e_status_state::motion::e_state::Idle //state
			, (int)e_system_message::e_status_state::motion::e_sub_state::OK //sub state
			, NULL //position data
		)
		, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::__send_formatted_message, ERR_CPU::METHOD::send_message);
	return true;
}

bool c_cpu::__wait_formatted_message(uint8_t init_message, uint8_t init_type)
{
	while (1)
	{
		//keep processing framework system_events
		//Talos::Shared::FrameWork::Events::Router::process();

		//see if host responded with a message, and if so does it match the message and record type we expected
		if (this->system_events.get_clr((int)c_cpu::e_event_type::SystemRecord))
		{
			Talos::Kernel::Error::framework_error.sys_message = this->sys_message.message;
			Talos::Kernel::Error::framework_error.sys_type = this->sys_message.type;
			Talos::Kernel::Error::framework_error.user_code1 = (int)init_message;
			Talos::Kernel::Error::framework_error.user_code2 = (int)init_type;


			if (this->sys_message.message == (int)init_message //(int)e_system_message::messages::e_informal::ReadyToProcess
				&& this->sys_message.type == (int)init_type //(int)e_system_message::e_status_type::Informal
				)
			{
				//Proper message has come in, so return
				return true;
			}
			else
			{
				Talos::Kernel::Comm::pntr_string_writer(Talos::Kernel::CPU::host_id, "mCU synch unexpected data\r\n");
				Talos::Kernel::Error::framework_error.behavior = e_error_behavior::Critical;
				Talos::Kernel::Error::raise_error(ERR_CPU::BASE, ERR_CPU::METHOD::__wait_formatted_message, ERR_CPU::METHOD::LINE::unexpected_data, this->ID);
				return false;
			}
		}
	}
}

bool c_cpu::service_events(int32_t * position, uint16_t rpm)
{
	//This should get called frequently from the main method_or_line. 
	ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(
		__check_data()
		, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::service_events, ERR_CPU::METHOD::__check_data);
	
	ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(
		__check_health()
		, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::service_events, ERR_CPU::METHOD::__check_health);

	

	//if NoState and OnLine is false, theres no need to process events.
	if (this->system_events.get((int)c_cpu::e_event_type::NoState)
		|| !this->system_events.get((int)c_cpu::e_event_type::OnLine))
		return true;

	//If active_cpu is in error set it offline
	if (this->system_events.get((int)c_cpu::e_event_type::Error))
		this->system_events.clear((int)c_cpu::e_event_type::OnLine);

	if (position != NULL)
		memcpy(&this->sys_message.position, position, sizeof(int32_t)*MACHINE_AXIS_COUNT);

	memcpy(&this->sys_message.rpm, &rpm, sizeof(uint16_t));

	return true;
}

bool c_cpu::__check_data()
{
	//If not conencted to any data just return
	if (this->hw_data_buffer._storage_pointer == NULL)
		return true;

	//TODO:neeed an indicator to determine when data has arrived for this cpu

	if (cdh_r_is_busy)//<--Check to see if this data is setup to be read already. If not assign a reader
	{
		ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(cdh_r_read()
			, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::__check_data, ERR_CPU::METHOD::cdh_r_read);//<--Reader is configured, just read
	}
	else
	{
		ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(cdh_r_get_message_type(&this->hw_data_buffer)
			, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::__check_data, ERR_CPU::METHOD::cdh_r_get_message_type);//<--get message type, configure reader

		ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(cdh_r_read()
			, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::__check_data, ERR_CPU::METHOD::cdh_r_read);//<--read what is already in the buffer, if its nto all there, we get it on the next loop
	}
	return true;
}

bool c_cpu::__check_health()
{
	if (this->pntr_cycle_count_ms == NULL)
		return true;
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
	return true;
}
