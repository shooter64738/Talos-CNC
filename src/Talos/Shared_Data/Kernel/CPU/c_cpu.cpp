#include "c_cpu.h"
#include "../Comm/c_kernel_comm.h"
#include "../Error/c_kernel_error.h"
#include "../Error/kernel_error_codes_cpu.h"
#include "../Base/c_kernel_base.h"
#include "../CPU/c_kernel_cpu.h"
#include "../../../communication_def.h"
#include "../c_kernel_utils.h"
#include <ctype.h>

using Talos::Kernel::ErrorCodes::ERR_CPU;

uint8_t ID = 0;

c_cpu::c_cpu()
{

}
c_cpu::c_cpu(uint8_t id, volatile uint32_t* tick_timer_ms)
{
	this->ID = id;
}

bool c_cpu::initialize(uint8_t id, volatile uint32_t* tick_timer_ms)
{
	this->ID = id;
	this->pntr_cycle_count_ms = tick_timer_ms;

	if (pntr_cycle_count_ms != NULL)
		this->next_cycle_check_time = *(pntr_cycle_count_ms)+HEALTH_CHECK_TIME_MS;

	//The ring buffer for comms should already be configured and pointed to the correct
	//hw buffer. if it is not, comms should have been inisitialzed before 'cpu'
	hw_data_buffer = &Talos::Kernel::Comm::host_ring_buffer;

	

	return true;
}

bool c_cpu::service_events(int32_t* position, uint16_t rpm)
{
	//This should get called frequently from the main method_or_line. 
	
	__check_data();

	//__check_health();

	return true;
}

bool c_cpu::__check_data()
{
	//If not connected to any data just return
	if (this->hw_data_buffer->_storage_pointer == NULL)
		return true;

	//TODO:neeed an indicator to determine when data has arrived for this cpu
	if (!this->hw_data_buffer->has_data())
		return true;


	if (cdh_r_is_busy)//<--Check to see if this data is setup to be read already. If not assign a reader
	{
		ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(cdh_r_read()
			, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::__check_data, ERR_CPU::METHOD::cdh_r_read);//<--Reader is configured, just read
	}
	else
	{
		uint8_t ret = cdh_r_get_message_type(this->hw_data_buffer);
		if (ret == 2)
		{
			//this just means we read a throw away byte from a text record (cr or lf)
			cdh_r_is_busy = 0;
			return true;
		}
		else if (ret == 1)
		{
			ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(cdh_r_read()
				, this->ID, ERR_CPU::BASE, ERR_CPU::METHOD::__check_data, ERR_CPU::METHOD::cdh_r_read);//<--read what is already in the buffer, if its nto all there, we get it on the next loop
		}
	}
	return true;
}

bool c_cpu::__check_health()
{
	//if (this->pntr_cycle_count_ms == NULL)
	//	return true;
	////If the time code hasn't changed (we havent gotten a system message in X time)
	////then we will assume the active_cpu is not healthy. Perhaps lost connection or its just lagging
	//if (*(this->pntr_cycle_count_ms) > this->next_cycle_check_time)
	//{
	//	this->next_cycle_check_time = *(pntr_cycle_count_ms)+HEALTH_CHECK_TIME_MS;
	//	if (this->sys_message.time_code == last_time_code)
	//		this->system_events.set(c_cpu::e_event_type::UnHealthy);
	//	else
	//		this->system_events.clear(c_cpu::e_event_type::UnHealthy);
	//	last_time_code = this->sys_message.time_code;
	//}
	return true;
}


//Reader Code

uint8_t c_cpu::cdh_r_get_message_type(c_ring_buffer<char>* buffer)
{
	dvc_source = &__rcving_buffers[SERIAL];

	char peek_tail = *buffer->peek(buffer->_tail);

	//Printable data is ngc method_or_line data. We need to check cr or lf because those are
	//special method_or_line ending characters for ngc data. We will NEVER use 10 or 13 as a
	//binary record type.
	if ((peek_tail >= 32 && peek_tail <= 127) || (peek_tail == ___CR || peek_tail == ___LF))
	{
		//If CR and LF are the termination for an ngc method_or_line, we wont see those unless we jsut processed a record
		//We can throw those characters away.
		if (peek_tail == ___CR || peek_tail == ___LF)
		{
			//txt record started with Cr or LF. throw it away
			buffer->get();
			__read_size = 0;
			cdh_r_is_busy = 2;
		}
		else
		{

			cdh_r_is_busy = 1;
			__read_size = 256;
			dvc_source->active_rcv_buffer = __rcving_buffers[SERIAL].get();
			ADD_2_STK_RTN_FALSE_IF_OBJECT_NULL(dvc_source->active_rcv_buffer, 0, ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_get_message_type, ERR_RDR::METHOD::LINE::txt_buffer_over_run);
			dvc_source->active_rcv_buffer->record_type = e_record_types::Text;
		}
	}
	else if (peek_tail == (int)e_record_types::System)
	{
		cdh_r_is_busy = 1;
		__read_size = sizeof(s_control_message);
		dvc_source->active_rcv_buffer = dvc_source->get();
		ADD_2_STK_RTN_FALSE_IF_OBJECT_NULL(dvc_source->active_rcv_buffer, 0, ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_get_message_type, ERR_RDR::METHOD::LINE::sys_buffer_over_run);
		dvc_source->active_rcv_buffer->record_type = e_record_types::System;

	}
	else if (peek_tail > 127) //non-printable and above 127 is a control code
	{
		Talos::Kernel::Error::raise_error(ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_get_message_type, ERR_RDR::METHOD::LINE::record_type_called_on_invalid_data, 0);
		cdh_r_is_busy = 0;
	}
	else
	{
		buffer->get();
		Talos::Kernel::Error::raise_error(ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_get_message_type, ERR_RDR::METHOD::LINE::record_type_called_on_null_data, 0);
		cdh_r_is_busy = 0;
	}

	if (cdh_r_is_busy)
	{
		__current_source = SERIAL;
		__pntr_active_rcv_buffer = (char*)& __rcving_buffers[SERIAL].active_rcv_buffer->overlays;
		__active_hw_buffer = buffer;
	}
	else
	{
		__current_source = 0;
		__pntr_active_rcv_buffer = NULL;
		__active_hw_buffer = NULL;
	}

	return cdh_r_is_busy;
}

bool c_cpu::cdh_r_read()
{
	//read from dvc_source buffer, write to target buffer

	while (this->__active_hw_buffer->has_data())
	{
		char byte = *this->__active_hw_buffer->get();

		__read_size--;

		if (dvc_source->active_rcv_buffer->record_type == e_record_types::Text)
		{
			byte = toupper(byte);
			//see if end of data
			if (byte == ___CR || byte == ___LF)
			{
				__read_size = 0;
				//dvc_source->active_rcv_buffer->read_count--;
				byte = 0;
			}
			else if (byte < 32 || byte >126)
			{
				ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_read, ERR_RDR::METHOD::LINE::illegal_data_in_text_stream);
			}
		}

		*(this->__pntr_active_rcv_buffer + dvc_source->active_rcv_buffer->read_count) = byte;
		

		if (!__read_size)
		{
			ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(__expand_record(dvc_source->active_rcv_buffer)
				, 0, ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_read, ERR_RDR::METHOD::expand_record);

			__read_size = dvc_source->active_rcv_buffer->addendum_size;
			if (!__read_size)
			{
				cdh_r_is_busy = false;
				dvc_source->rec_buffer.overlays.motion_control_settings;
				

				__cdh_r_close_read();
				return true;
			}
		}
		dvc_source->active_rcv_buffer->read_count++;
	}
	return true;
}

void c_cpu::cdh_r_reset()
{
	cdh_r_is_busy = false;
	memset(dvc_source->active_rcv_buffer->overlays.text, 0, MAX_TEXT_BUFFER_SIZE);
	dvc_source->active_rcv_buffer->read_count = 0;
	dvc_source->active_rcv_buffer = NULL;
	dvc_source = NULL;
	__pntr_active_rcv_buffer = NULL;
	__active_hw_buffer = NULL;
	__read_size = 0;
}

bool c_cpu::__cdh_r_close_read()
{
	if (dvc_source->active_rcv_buffer->record_type == e_record_types::System)
	{
		//check system rec crc
		uint16_t system_crc = dvc_source->active_rcv_buffer->overlays.system_control.crc;

		//crcs are always calcualted with the current crc value ignored.
		uint16_t new_crc = Talos::Kernel::Base::kernel_crc::generate(this->__pntr_active_rcv_buffer, sizeof(s_control_message) - KERNEL_CRC_BYTE_SIZE);
		if (system_crc != new_crc)
		{
			ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__cdh_r_close_read, ERR_RDR::METHOD::LINE::crc_failed_system_record);
		}

		if (dvc_source->active_rcv_buffer->has_addendum)
		{
			//check addendum rec crc. it is a pointer
			uint16_t addendum_crc = *dvc_source->active_rcv_buffer->addendum_crc_value;

			//crcs are always calcualted with the current crc value ignored. This check must skip over the system rec
			uint16_t new_crc = Talos::Kernel::Base::kernel_crc::generate(
				(this->__pntr_active_rcv_buffer + sizeof(s_control_message))
				, dvc_source->active_rcv_buffer->addendum_size - KERNEL_CRC_BYTE_SIZE);
			if (addendum_crc != new_crc)
			{
				ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__cdh_r_close_read, ERR_RDR::METHOD::LINE::crc_failed_addendum_record);
			}
		}
	}
	
	return true;
}

bool c_cpu::__expand_record(s_read_record* record)
{
	if (record->record_type == e_record_types::Text)
	{
		return __sys_data_classify(e_system_message::messages::e_data::NgcDataLine, record);
	}
	else
	{
		ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(__sys_typer(record)
			, 0, ERR_RDR::BASE, ERR_RDR::METHOD::expand_record, ERR_RDR::METHOD::__sys_typer);
	}
	record->has_addendum = record->addendum_size > 0;
	return true;
}

bool c_cpu::__sys_critical_classify(e_system_message::messages::e_critical message, s_read_record* record)
{
	switch (message)
	{
	case e_system_message::messages::e_critical::testcritical:
	{
		record->addendum_size = 0;
		break;
	}

	default:
		ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_critical_classify, ERR_RDR::METHOD::LINE::illegal_system_critical_class_type);
		break;

	}
	return true;
}

bool c_cpu::__sys_data_classify(e_system_message::messages::e_data message, s_read_record* record)
{
	switch (message)
	{
	case e_system_message::messages::e_data::MotionConfiguration:
		record->addendum_crc_value = &record->overlays.motion_control_settings.crc;
		record->addendum_size = sizeof(s_motion_control_settings_encapsulation);
		break;
	case e_system_message::messages::e_data::NgcDataLine:
		//record->addendum_size = 256;
		__set_entry_mode(record->overlays.text[0], record->overlays.text[1]);
		break;
	case e_system_message::messages::e_data::NgcDataRequest:
		record->addendum_size = 0;
		break;
	case e_system_message::messages::e_data::SystemRecord:
		record->addendum_crc_value = &record->overlays.system_control.crc;
		record->addendum_size = sizeof(s_control_message);
		break;
	default:
		ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_data_classify, ERR_RDR::METHOD::LINE::illegal_system_data_class_type);
		break;
	}
	return true;
}

bool c_cpu::__sys_informal_classify(e_system_message::messages::e_informal message, s_read_record* record)
{
	switch (message)
	{
	case e_system_message::messages::e_informal::ReadyToProcess:
	{
		record->addendum_size = 0;
		break;
	}

	default:
		ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_informal_classify, ERR_RDR::METHOD::LINE::illegal_system_informal_class_type);
		break;
	}
	return true;
}

bool c_cpu::__sys_inquiry_classify(e_system_message::messages::e_inquiry message, s_read_record* record)
{
	switch (message)
	{
	case e_system_message::messages::e_inquiry::GCodeBlockReport:
		record->addendum_size = 0;
		break;
	case e_system_message::messages::e_inquiry::Invalid:
		record->addendum_size = 0;
		break;
	case e_system_message::messages::e_inquiry::MCodeBlockReport:
		record->addendum_size = 0;
		break;
	case e_system_message::messages::e_inquiry::WordStatusReport:
		record->addendum_size = 0;
		break;
	case e_system_message::messages::e_inquiry::MotionConfiguration:
		record->addendum_size = 0;
		break;
	case e_system_message::messages::e_inquiry::NgcDataLine:
		record->addendum_size = 0;
		break;
	case e_system_message::messages::e_inquiry::SystemRecord:
		record->addendum_size = 0;
		break;
	default:
		ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_inquiry_classify, ERR_RDR::METHOD::LINE::illegal_system_inquiry_class_type);
		break;
	}
	return true;
}

bool c_cpu::__sys_warning_classify(e_system_message::messages::e_warning message, s_read_record* record)
{
	switch (message)
	{
	case e_system_message::messages::e_warning::testwarning:
	{
		record->addendum_size = 0;
		break;
	}

	default:
		ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_warning_classify, ERR_RDR::METHOD::LINE::illegal_system_warning_class_type);
		break;
	}
	return true;
}

bool c_cpu::__sys_typer(s_read_record* record)
{
	switch ((e_system_message::e_status_type)record->overlays.system_control.type)
	{
	case e_system_message::e_status_type::Critical:
	{
		__sys_critical_classify(
			(e_system_message::messages::e_critical)record->overlays.system_control.message,
			record);
		break;
	}
	case e_system_message::e_status_type::Warning:
	{
		__sys_warning_classify(
			(e_system_message::messages::e_warning)record->overlays.system_control.message,
			record);
		break;
	}
	case e_system_message::e_status_type::Informal:
	{
		__sys_informal_classify((e_system_message::messages::e_informal)record->overlays.system_control.message,
			record);
		break;
	}
	case e_system_message::e_status_type::Data:
	{
		ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(
			__sys_data_classify((e_system_message::messages::e_data)record->overlays.system_control.message, record)
			, 0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_typer, ERR_RDR::METHOD::__sys_data_classify);

		/*ADD_2_STK_RTN_FALSE(0, __sys_data_classify((e_system_message::messages::e_data)overlays.system_control.message)
			,ERR_RDR::BASE,*/
		break;
	}
	case e_system_message::e_status_type::Inquiry:
	{
		__sys_inquiry_classify((e_system_message::messages::e_inquiry)record->overlays.system_control.message, record);
		break;
	}

	default:
		ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_typer, ERR_RDR::METHOD::LINE::illegal_system_record_type);
		break;
	}
	return true;
}

bool c_cpu::__set_entry_mode(char first_byte, char second_byte)
{

	switch (first_byte)
	{
	case '?': //inquiry mode
	{
		__set_sub_entry_mode(second_byte);
		//This is an inquiry and system_events will set and handle this. We dont actually need the record data
		//Talos::Shared::c_cache_data::txt_record.pntr_record = NULL;
		break;
	}
	default:
	{
		//Talos::Shared::FrameWork::StartUp::CpuCluster[Talos::Shared::FrameWork::StartUp::cpu_type.Host].host_events.Data.set(e_system_message::messages::e_data::NgcDataLine);
		//assume its plain ngc g code data
		this->host_events.Data.set(e_system_message::messages::e_data::NgcDataLine);
		
		return false;
	}

	}
	return true;
}

bool c_cpu::__set_sub_entry_mode(char byte)
{
	//s_bit_flag_controller<uint32_t>* pntr_event = &Talos::Shared::FrameWork::StartUp::CpuCluster[Talos::Shared::FrameWork::StartUp::cpu_type.Host].host_events.Inquiry;
	switch (byte)
	{
	case 'G': //block g group status
		this->host_events.Inquiry.set(e_system_message::messages::e_inquiry::GCodeBlockReport);
		//pntr_event->set((int)e_system_message::messages::e_inquiry::GCodeBlockReport);
		break;
	case 'M': //block m group status
		//pntr_event->set((int)e_system_message::messages::e_inquiry::MCodeBlockReport);
		this->host_events.Inquiry.set(e_system_message::messages::e_inquiry::MCodeBlockReport);
		break;
	case 'W': //word value status
		//pntr_event->set((int)e_system_message::messages::e_inquiry::WordStatusReport);
		this->host_events.Inquiry.set(e_system_message::messages::e_inquiry::WordStatusReport);
		break;
	case 'I': //word value status
	//pntr_event->set((int)e_system_message::messages::e_inquiry::WordStatusReport);
		this->host_events.Inquiry.set(e_system_message::messages::e_inquiry::InterpreterConfiguration);
		break;
	case 'S': //word value status
		//pntr_event->set((int)e_system_message::messages::e_inquiry::WordStatusReport);
		this->host_events.Inquiry.set(e_system_message::messages::e_inquiry::MotionConfiguration);
		break;
	default:
		/*__raise_error(NULL, e_error_behavior::Informal, 0, e_error_group::DataHandler, e_error_process::Process
			, tracked_read_type, e_error_source::Serial, e_error_code::UnExpectedDataTypeForRecord);*/
		return false;
		break;
	}
	return true;
}