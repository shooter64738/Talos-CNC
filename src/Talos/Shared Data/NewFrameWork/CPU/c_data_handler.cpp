#include "c_data_handler.h"



e_record_types c_data_handler::cdh_get_message_type(c_ring_buffer<char> * buffer)
{
	int test = sizeof(u_data_overlays);

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

		cdh_is_busy = true;
		__read_size = 256;
		__source_buffers[SERIAL].active_record = __source_buffers[0].get();
		__source_buffers[SERIAL].active_record->record_type = e_record_types::Text;
	}
	else if (peek_tail == (int)e_record_types::System)
	{
		cdh_is_busy = true;
		__read_size = sizeof(s_control_message);
		__source_buffers[SERIAL].active_record = __source_buffers[SERIAL].get();
		__source_buffers[SERIAL].active_record->record_type = e_record_types::System;
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

void c_data_handler::cdh_read()
{
	//read from source buffer, write to target buffer

	while (this->__active_source_buffer->has_data())
	{
		char byte = this->__active_source_buffer->get();
		*(this->__active_target_buffer + __read_count) = byte;
		__read_size--; __read_count++;

		if ((__source_buffers[SERIAL].active_record->record_type == e_record_types::Text) && (byte == CR || byte == LF))
		{
			__read_size = 0;
			__read_count--;
		}


		if (!__read_size)
		{
			__read_size = __source_buffers[(int)__current_source].active_record->expand_record();

			if (!__read_size)
			{
				cdh_is_busy = false;
				__source_buffers[(int)__current_source].rec_buffer[0].overlays.motion_control_settings;
				__source_buffers[(int)__current_source].index++;
			}


			cdh_is_busy = false;
			__source_buffers[SERIAL].active_record = NULL;
			__active_target_buffer = NULL;
			__active_source_buffer = NULL:
			__read_type = e_record_types::System;
			__read_size = sizeof(s_control_message);
		}
	}
}

/*
void c_data_handler::__expand_sys_message(s_read_record *sys_record)
{

	c_data_handler::pntr_classifier = NULL;
	__sys_typer(sys_record);

	if (c_data_handler::pntr_classifier != NULL)
		__read_size = c_data_handler::pntr_classifier(sys_record);

}

void  c_data_handler::__sys_typer(s_read_record *sys_record)
{
	switch ((e_system_message::e_status_type)sys_record->overlays.system_control.type)
	{
	case e_system_message::e_status_type::Critical:
	{
		c_data_handler::pntr_classifier = __sys_critical_classify;
		break;
	}
	case e_system_message::e_status_type::Warning:
	{
		c_data_handler::pntr_classifier = __sys_warning_classify;
		break;
	}
	case e_system_message::e_status_type::Informal:
	{
		c_data_handler::pntr_classifier = __sys_informal_classify;
		break;
	}
	case e_system_message::e_status_type::Data:
	{
		c_data_handler::pntr_classifier = __sys_data_classify;
		break;
	}
	case e_system_message::e_status_type::Inquiry:
	{
		c_data_handler::pntr_classifier = __sys_inquiry_classify;
		break;
	}

	default:
		break;
	}
}

uint16_t  c_data_handler::__sys_critical_classify(s_read_record *sys_record)
{
	switch ((e_system_message::messages::e_critical)sys_record->overlays.system_control.type)
	{
	case e_system_message::messages::e_critical::testcritical:
	{
		break;
	}

	default:
		break;
	}
	return 0;
}

uint16_t  c_data_handler::__sys_data_classify(s_read_record *sys_record)
{
	switch ((e_system_message::messages::e_data)sys_record->overlays.system_control.type)
	{
	case e_system_message::messages::e_data::MotionConfiguration:
		sys_record->can_publish = true;
		return sizeof(s_motion_control_settings_encapsulation);
		break;

	default:
		break;
	}
	return 0;
}

uint16_t  c_data_handler::__sys_informal_classify(s_read_record *sys_record)
{
	switch ((e_system_message::messages::e_critical)sys_record->overlays.system_control.type)
	{
	case e_system_message::messages::e_critical::testcritical:
	{
		break;
	}

	default:
		break;
	}
	return 0;
}

uint16_t  c_data_handler::__sys_inquiry_classify(s_read_record *sys_record)
{
	switch ((e_system_message::messages::e_inquiry)sys_record->overlays.system_control.type)
	{
	case e_system_message::messages::e_inquiry::GCodeBlockReport:
		break;
	case e_system_message::messages::e_inquiry::Invalid:
		break;
	case e_system_message::messages::e_inquiry::MCodeBlockReport:
		break;
	case e_system_message::messages::e_inquiry::WordStatusReport:
		break;
	case e_system_message::messages::e_inquiry::MotionConfiguration:
		break;
	case e_system_message::messages::e_inquiry::NgcDataLine:
		break;
	case e_system_message::messages::e_inquiry::SystemRecord:
		break;
	default:
		break;
	}
	return 0;
}

uint16_t  c_data_handler::__sys_warning_classify(s_read_record *sys_record)
{
	switch ((e_system_message::messages::e_warning)sys_record->overlays.system_control.type)
	{
	case e_system_message::messages::e_warning::testwarning:
	{
		break;
	}

	default:
		break;
	}
	return 0;
}
*/