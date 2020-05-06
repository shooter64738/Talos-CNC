#include "c_data_handler_write.h"
#include "../Base/c_kernel_base.h"
#include "../Error/kernel_error_codes_writer.h"

using Talos::Kernel::ErrorCodes::ERR_WTR;

e_record_types c_data_handler_write::cdh_r_get_message_type(c_ring_buffer<char> * buffer)
{
	dvc_source = &__source_buffers[SERIAL];

	char peek_tail = buffer->peek(buffer->_tail);

	//Printable data is ngc line data. We need to check cr or lf because those are
	//special line ending characters for ngc data. We will NEVER use 10 or 13 as a
	//binary record type.
	if ((peek_tail >= 32 && peek_tail <= 127) || (peek_tail == CR || peek_tail == LF))
	{
		//If CR and LF are the termination for an ngc line, we wont see those unless we jsut processed a record
		//We can throw those characters away.
		if (peek_tail == CR || peek_tail == LF)
		{
			//txt record started with Cr or LF. throw it away
			buffer->get();
			__read_size = 0;
			return e_record_types::Empty;
			
		}

		cdh_r_is_busy = true;
		__read_size = 256;
		dvc_source->active_record = __source_buffers[0].get();
		dvc_source->active_record->record_type = e_record_types::Text;
	}
	else if (peek_tail == (int)e_record_types::System)
	{
		cdh_r_is_busy = true;
		__read_size = sizeof(s_control_message);
		dvc_source->active_record = dvc_source->get();
		dvc_source->active_record->record_type = e_record_types::System;
	}
	else if (peek_tail > 127) //non-printable and above 127 is a control code
	{
		return e_record_types::Unknown;
	}
	__current_source = SERIAL;
	__active_target_buffer = (char*)&__source_buffers[0].active_record->overlays;
	__active_source_buffer = buffer;
	return __source_buffers[SERIAL].active_record->record_type;
}

void c_data_handler_write::cdh_r_read()
{
	//read from dvc_source buffer, write to target buffer

	while (this->__active_source_buffer->has_data())
	{
		char byte = this->__active_source_buffer->get();
		*(this->__active_target_buffer + dvc_source->active_record->read_count) = byte;
		__read_size--; dvc_source->active_record->read_count++;

		if ((dvc_source->active_record->record_type == e_record_types::Text) && (byte == CR || byte == LF))
		{
			__read_size = 0;
			dvc_source->active_record->read_count--;
		}


		if (!__read_size)
		{
			__read_size = dvc_source->active_record->expand_record();

			if (!__read_size)
			{
				cdh_r_is_busy = false;
				dvc_source->rec_buffer[0].overlays.motion_control_settings;
				dvc_source->index++;

				__cdh_r_close_read();

				
			}
		}
	}
}

void c_data_handler_write::__cdh_r_close_read()
{
	if (dvc_source->active_record->record_type == e_record_types::System)
	{
		//check system rec crc
		uint16_t system_crc = dvc_source->active_record->overlays.system_control.crc;
		
		//crcs are always calcualted with the current crc value ignored.
		uint16_t new_crc = Talos::Kernel::Base::CRC::generate(this->__active_target_buffer, sizeof(s_control_message) - CRC_BYTE_SIZE);
		if (system_crc != new_crc)
		{
			Talos::Kernel::Error::raise_error(ERR_WTR::BASE, ERR_WTR::METHOD::__cdh_r_close_read, ERR_WTR::METHOD::LINE::crc_failed_system_record, 0);
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
				Talos::Kernel::Error::raise_error(ERR_WTR::BASE, ERR_WTR::METHOD::__cdh_r_close_read, ERR_WTR::METHOD::LINE::crc_failed_addendum_record, 0);
			}
		}
	}
	cdh_r_is_busy = false;
	dvc_source->active_record = NULL;
	dvc_source = NULL;
	__active_target_buffer = NULL;
	__active_source_buffer = NULL;
	__read_size = 0;
}
