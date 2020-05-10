#include "c_data_handler_write.h"
#include <ctype.h>

bool c_data_handler_write::cdh_w_get_message_type(c_ring_buffer<char> * buffer)
{
	dvc_source = &__source_buffers[SERIAL];

	char peek_tail = buffer->peek(buffer->_tail);

	//Printable data is ngc method_or_line data. We need to check cr or lf because those are
	//special method_or_line ending characters for ngc data. We will NEVER use 10 or 13 as a
	//binary record type.
	if ((peek_tail >= 32 && peek_tail <= 127) || (peek_tail == CR || peek_tail == LF))
	{
		//If CR and LF are the termination for an ngc method_or_line, we wont see those unless we jsut processed a record
		//We can throw those characters away.
		if (peek_tail == CR || peek_tail == LF)
		{
			//txt record started with Cr or LF. throw it away
			buffer->get();
			__write_size = 0;
			cdh_w_is_busy = false;
		}

		cdh_w_is_busy = true;
		__write_size = 256;
		dvc_source->active_record = __source_buffers[0].get();
		ADD_2_STK_RTN_FALSE_IF_OBJECT_NULL(dvc_source->active_record, 0, ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_get_message_type, ERR_RDR::METHOD::LINE::txt_buffer_over_run);
		dvc_source->active_record->record_type = e_record_types::Text;
	}
	else if (peek_tail == (int)e_record_types::System)
	{
		cdh_w_is_busy = true;
		__write_size = sizeof(s_control_message);
		dvc_source->active_record = dvc_source->get();
		ADD_2_STK_RTN_FALSE_IF_OBJECT_NULL(dvc_source->active_record, 0, ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_get_message_type, ERR_RDR::METHOD::LINE::sys_buffer_over_run);
		dvc_source->active_record->record_type = e_record_types::System;

	}
	else if (peek_tail > 127) //non-printable and above 127 is a control code
	{
		Talos::Kernel::Error::raise_error(ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_get_message_type, ERR_RDR::METHOD::LINE::record_type_called_on_invalid_data, 0);
		cdh_w_is_busy = false;
	}
	else
	{
		Talos::Kernel::Error::raise_error(ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_get_message_type, ERR_RDR::METHOD::LINE::record_type_called_on_null_data, 0);
		cdh_w_is_busy = false;
	}

	if (cdh_w_is_busy)
	{
		__current_source = SERIAL;
		__active_target_buffer = (char*)&__source_buffers[0].active_record->overlays;
		__active_source_buffer = buffer;
	}
	else
	{
		__current_source = 0;
		__active_target_buffer = NULL;
		__active_source_buffer = NULL;
	}

	return cdh_w_is_busy;
}

bool c_data_handler_write::cdh_w_write()
{
	//read from dvc_source buffer, write to target buffer

	while (this->__active_source_buffer->has_data())
	{
		char byte = this->__active_source_buffer->get();
		*(this->__active_target_buffer + dvc_source->active_record->read_count) = byte;
		__write_size--; dvc_source->active_record->read_count++;

		if (dvc_source->active_record->record_type == e_record_types::Text)
		{
			byte = toupper(byte);
			if (byte < 32 || byte >126)
			{
				ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_read, ERR_RDR::METHOD::LINE::illegal_data_in_text_stream);
			}
			//see if end of data
			if (byte == CR || byte == LF)
			{
				__write_size = 0;
				dvc_source->active_record->read_count--;
			}
		}


		if (!__write_size)
		{
			ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(dvc_source->active_record->expand_record()
				, 0, ERR_RDR::BASE, ERR_RDR::METHOD::cdh_r_read, ERR_RDR::METHOD::expand_record)

				__write_size = dvc_source->active_record->addendum_size;
			if (!__write_size)
			{
				cdh_w_is_busy = false;
				dvc_source->rec_buffer[0].overlays.motion_control_settings;
				dvc_source->index++;

				__cdh_w_close_write();


			}
		}
	}
	return true;
}

bool c_data_handler_write::__cdh_w_close_write()
{
	if (dvc_source->active_record->record_type == e_record_types::System)
	{
		//check system rec crc
		uint16_t system_crc = dvc_source->active_record->overlays.system_control.crc;

		//crcs are always calcualted with the current crc value ignored.
		uint16_t new_crc = Talos::Kernel::Base::CRC::generate(this->__active_target_buffer, sizeof(s_control_message) - CRC_BYTE_SIZE);
		if (system_crc != new_crc)
		{
			ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__cdh_r_close_read, ERR_RDR::METHOD::LINE::crc_failed_system_record);
		}

		if (dvc_source->active_record->has_addendum)
		{
			//check addendum rec crc. it is a pointer
			uint16_t addendum_crc = *dvc_source->active_record->addendum_crc_value;

			//crcs are always calcualted with the current crc value ignored. This check must skip over the system rec
			uint16_t new_crc = Talos::Kernel::Base::CRC::generate(
				(this->__active_target_buffer + sizeof(s_control_message))
				, dvc_source->active_record->addendum_size - CRC_BYTE_SIZE);
			if (addendum_crc != new_crc)
			{
				ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__cdh_r_close_read, ERR_RDR::METHOD::LINE::crc_failed_addendum_record);
			}
		}
	}
	cdh_w_is_busy = false;
	dvc_source->active_record = NULL;
	dvc_source = NULL;
	__active_target_buffer = NULL;
	__active_source_buffer = NULL;
	__write_size = 0;
	return true;
}
