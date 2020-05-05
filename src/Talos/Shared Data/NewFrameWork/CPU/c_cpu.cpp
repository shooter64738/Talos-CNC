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
	//This should get called frequently from teh main line. 
	__check_data();

	__check_health();

	//if NoState and OnLine is false, theres no need to process events.
	if (this->system_events.get((int)c_cpu::e_event_type::NoState)
		|| !this->system_events.get((int)c_cpu::e_event_type::OnLine))
		return;

	//If active_cpu is in error set it offline
	if (this->system_events.get((int)c_cpu::e_event_type::Error))
		this->system_events.clear((int)c_cpu::e_event_type::OnLine);

	memcpy(&this->sys_message.position, position, sizeof(int32_t)*MACHINE_AXIS_COUNT);
	memcpy(&this->sys_message.rpm, &rpm, sizeof(uint16_t));
}

void c_cpu::__check_data()
{
	//Is there a handler assigned for this data class already? If a handler is assigned
	//keep using it until all the data is consumed. Otherwise assign a new handler.
	if (this->pntr_data_read_handler == NULL)
		this->__assign_handler();

	if (this->pntr_data_read_handler != NULL)
		this->pntr_data_read_handler(&this->hw_data_buffer, this->ID);

	//Need to check the data handlers buffer to see if there are buffered records to pull. 
	/*
	//Copy the record from the buffer into into the sys_message
	memcpy(&this_packet->active_cpu->sys_message, &this_packet->sys_message_buffer[this_packet->buffer_index - 1], s_control_message::__size__);
	this_packet->active_cpu->sys_message.rx_from = (int)this_packet->event_id;
	*/
	uint8_t oldest_ready_packet = Talos::NewFrameWork::DataHandler::Binary::get_fifo(this->ID);
	if (oldest_ready_packet >0)
		memcpy(&(this->sys_message)
			, &Talos::NewFrameWork::DataHandler::Binary::read_packet[this->ID].sys_message_buffer[oldest_ready_packet-1], s_control_message::__size__);
}

#define __ASSIGN_HANDLER_IN 3
#define UNDETERMINED_SERIAL_TYPE 1
void c_cpu::__assign_handler()
{

	//Tail is always assumed to be at the 'start' of data
	//event id for serial is the port the data came from or is going to. We can use that to access the buffer array pointer.
	char peek_tail = this->hw_data_buffer.peek(this->hw_data_buffer._tail);

	//Printable data is ngc line data. We need to check cr or lf because those are
	//special line ending characters for ngc data. We will NEVER use 10 or 13 as a
	//binary record type.
	if ((peek_tail >= 32 && peek_tail <= 127) || (peek_tail == CR || peek_tail == LF))
	{
		//If CR and LF are the termination for an ngc line, we wont see those unless we jsut processed a record
		//We can throw those characters away.
		if (peek_tail == CR || peek_tail == LF)
		{
			this->hw_data_buffer.get();

			return;
		}

		//Talos::Shared::FrameWork::Data::Txt::route_read((int)event_id, &event_object->event_manager);
		//c_new_serial_event_handler::pntr_data_read_handler = Talos::Shared::FrameWork::Data::Txt::reader;
		//Talos::Shared::FrameWork::Data::Txt::pntr_read_release = c_new_serial_event_handler::read_data_handler_releaser;

	}
	else if (peek_tail > 0 && peek_tail < 32) //non-printable and below 32 is a binary record
	{

		if (peek_tail == (int)e_record_types::System)
		{
			Talos::NewFrameWork::DataHandler::Binary::route_read(this, &this->system_events, (int)e_record_types::System);
			this->pntr_data_read_handler = Talos::NewFrameWork::DataHandler::Binary::read;
		}

		//Assign a release call back function. The handler knows nothing about serial system_events
		//and we want to keep it that way.

	}
	else if (peek_tail > 127) //non-printable and above 127 is a control code
	{
		//UDR0='C';
		//Assign a specific handler for this data type
		//c_new_serial_event_handler::pntr_data_read_handler = Txt::assign_handler(event_object, event_id, (e_record_types)peek_tail);

		//Assign a release call back function. The handler knows nothing about serial system_events
		//and we want to keep it that way.
		//Txt::pntr_read_data_handler_release = c_new_serial_event_handler::read_data_handler_releaser;
	}
	else //we dont know what kind of data it is
	{

		//Talos::Shared::FrameWork::Error::framework_error.user_code1 = peek_tail;
		//since there is data here and we do not know what kind it is, we cannot determine which assigner it needs.
		//i feel like this is probably a critical error.
		//__raise_error(BASE_ERROR, __ASSIGN_HANDLER_IN, UNDETERMINED_SERIAL_TYPE, (int)event_id);
	}
	return;
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
