#include "c_data_handler_read.h"
#include "../Base/c_kernel_base.h"
#include "../Error/_d_err_macros.h"
#include <ctype.h>

c_data_handler_read::c_data_handler_read(c_cpu cpu)
{
	this->__local_cpu = cpu;
}

bool c_data_handler_read::cdh_r_get_message_type(c_ring_buffer<char>* buffer)
{
	dvc_source = &__rcving_buffers[SERIAL];

	char peek_tail = buffer->peek(buffer->_tail);

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
			cdh_r_is_busy = false;
		}

		cdh_r_is_busy = true;
		__read_size = 256;
		dvc_source->active_rcv_buffer = __rcving_buffers[SERIAL].get();
		ADD_2_STK_RTN_FALSE_IF_OBJECT_NULL(dvc_source->active_rcv_buffer, 0, ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_get_message_type, ERR_RDR::METHOD::LINE::txt_buffer_over_run);
		dvc_source->active_rcv_buffer->record_type = e_record_types::Text;
	}
	else if (peek_tail == (int)e_record_types::System)
	{
		cdh_r_is_busy = true;
		__read_size = sizeof(s_control_message);
		dvc_source->active_rcv_buffer = dvc_source->get();
		ADD_2_STK_RTN_FALSE_IF_OBJECT_NULL(dvc_source->active_rcv_buffer, 0, ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_get_message_type, ERR_RDR::METHOD::LINE::sys_buffer_over_run);
		dvc_source->active_rcv_buffer->record_type = e_record_types::System;

	}
	else if (peek_tail > 127) //non-printable and above 127 is a control code
	{
		Talos::Kernel::Error::raise_error(ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_get_message_type, ERR_RDR::METHOD::LINE::record_type_called_on_invalid_data, 0);
		cdh_r_is_busy = false;
	}
	else
	{
		Talos::Kernel::Error::raise_error(ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_get_message_type, ERR_RDR::METHOD::LINE::record_type_called_on_null_data, 0);
		cdh_r_is_busy = false;
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

bool c_data_handler_read::cdh_r_read()
{
	//read from dvc_source buffer, write to target buffer

	while (this->__active_hw_buffer->has_data())
	{
		char byte = this->__active_hw_buffer->get();

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
		dvc_source->active_rcv_buffer->read_count++;

		if (!__read_size)
		{
			ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(dvc_source->active_rcv_buffer->expand_record()
				, 0, ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_read, ERR_RDR::METHOD::expand_record);

			__read_size = dvc_source->active_rcv_buffer->addendum_size;
			if (!__read_size)
			{
				cdh_r_is_busy = false;
				dvc_source->rec_buffer[0].overlays.motion_control_settings;
				dvc_source->index++;

				__cdh_r_close_read();
				return;
			}
		}
	}
	return true;
}

bool c_data_handler_read::__cdh_r_close_read()
{
	if (dvc_source->active_rcv_buffer->record_type == e_record_types::System)
	{
		//check system rec crc
		uint16_t system_crc = dvc_source->active_rcv_buffer->overlays.system_control.crc;

		//crcs are always calcualted with the current crc value ignored.
		uint16_t new_crc = Talos::Kernel::Base::CRC::generate(this->__pntr_active_rcv_buffer, sizeof(s_control_message) - CRC_BYTE_SIZE);
		if (system_crc != new_crc)
		{
			ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__cdh_r_close_read, ERR_RDR::METHOD::LINE::crc_failed_system_record);
		}

		if (dvc_source->active_rcv_buffer->has_addendum)
		{
			//check addendum rec crc. it is a pointer
			uint16_t addendum_crc = *dvc_source->active_rcv_buffer->addendum_crc_value;

			//crcs are always calcualted with the current crc value ignored. This check must skip over the system rec
			uint16_t new_crc = Talos::Kernel::Base::CRC::generate(
				(this->__pntr_active_rcv_buffer + sizeof(s_control_message))
				, dvc_source->active_rcv_buffer->addendum_size - CRC_BYTE_SIZE);
			if (addendum_crc != new_crc)
			{
				ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__cdh_r_close_read, ERR_RDR::METHOD::LINE::crc_failed_addendum_record);
			}
		}
	}
	cdh_r_is_busy = false;
	dvc_source->active_rcv_buffer = NULL;
	dvc_source = NULL;
	__pntr_active_rcv_buffer = NULL;
	__active_hw_buffer = NULL;
	__read_size = 0;
	return true;
}
