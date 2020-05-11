#include "c_system_typer.h"
#include "../Error/_d_err_macros.h"

bool c_system_read_typer::expand_record()
{
	if (this->rec_buffer[e_buffer_source_type::Serial0].record_type == e_record_types::Text)
		return __sys_data_classify(e_system_message::messages::e_data::NgcDataLine);

	ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(__sys_typer()
		, 0, ERR_RDR::BASE, ERR_RDR::METHOD::expand_record, ERR_RDR::METHOD::__sys_typer);

	this->rec_buffer[e_buffer_source_type::Serial0].has_addendum = this->rec_buffer[e_buffer_source_type::Serial0].addendum_size > 0;
	return true;
}

bool c_system_read_typer::__sys_critical_classify(e_system_message::messages::e_critical message)
{
	switch (message)
	{
	case e_system_message::messages::e_critical::testcritical:
	{
		addendum_size = 0;
		break;
	}

	default:
		ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_critical_classify, ERR_RDR::METHOD::LINE::illegal_system_critical_class_type);
		break;

	}
	return true;
}

bool c_system_read_typer::__sys_data_classify(e_system_message::messages::e_data message)
{
	switch (message)
	{
	case e_system_message::messages::e_data::MotionConfiguration:
		addendum_crc_value = &overlays.motion_control_settings.crc;
		addendum_size = sizeof(s_motion_control_settings_encapsulation);
		break;
	case e_system_message::messages::e_data::NgcDataLine:
		addendum_size = 256;
		break;
	case e_system_message::messages::e_data::NgcDataRequest:
		addendum_size = 0;
		break;
	case e_system_message::messages::e_data::SystemRecord:
		addendum_crc_value = &overlays.system_control.crc;
		addendum_size = sizeof(s_control_message);
		break;
	default:
		ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_data_classify, ERR_RDR::METHOD::LINE::illegal_system_data_class_type);
		break;
	}
	return true;
}

bool c_system_read_typer::__sys_informal_classify(e_system_message::messages::e_informal message)
{
	switch (message)
	{
	case e_system_message::messages::e_informal::ReadyToProcess:
	{
		addendum_size = 0;
		break;
	}

	default:
		ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_informal_classify, ERR_RDR::METHOD::LINE::illegal_system_informal_class_type);
		break;
	}
	return true;
}

bool c_system_read_typer::__sys_inquiry_classify(e_system_message::messages::e_inquiry message)
{
	switch (message)
	{
	case e_system_message::messages::e_inquiry::GCodeBlockReport:
		addendum_size = 0;
		break;
	case e_system_message::messages::e_inquiry::Invalid:
		addendum_size = 0;
		break;
	case e_system_message::messages::e_inquiry::MCodeBlockReport:
		addendum_size = 0;
		break;
	case e_system_message::messages::e_inquiry::WordStatusReport:
		addendum_size = 0;
		break;
	case e_system_message::messages::e_inquiry::MotionConfiguration:
		addendum_size = 0;
		break;
	case e_system_message::messages::e_inquiry::NgcDataLine:
		addendum_size = 0;
		break;
	case e_system_message::messages::e_inquiry::SystemRecord:
		addendum_size = 0;
		break;
	default:
		ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_inquiry_classify, ERR_RDR::METHOD::LINE::illegal_system_inquiry_class_type);
		break;
	}
	return true;
}

bool c_system_read_typer::__sys_warning_classify(e_system_message::messages::e_warning message)
{
	switch (message)
	{
	case e_system_message::messages::e_warning::testwarning:
	{
		addendum_size = 0;
		break;
	}

	default:
		ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_warning_classify, ERR_RDR::METHOD::LINE::illegal_system_warning_class_type);
		break;
	}
	return true;
}

bool c_system_read_typer::__sys_typer()
{
	uint8_t message = this->rec_buffer[e_buffer_source_type::Serial0].overlays.system_control.message;

	switch ((e_system_message::e_status_type)message)
	{
	case e_system_message::e_status_type::Critical:
	{
		__sys_critical_classify((e_system_message::messages::e_critical);
		break;
	}
	case e_system_message::e_status_type::Warning:
	{
		__sys_warning_classify((e_system_message::messages::e_warning)message);
		break;
	}
	case e_system_message::e_status_type::Informal:
	{
		__sys_informal_classify((e_system_message::messages::e_informal)message);
		break;
	}
	case e_system_message::e_status_type::Data:
	{
		ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(__sys_data_classify((e_system_message::messages::e_data)message)
			, 0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_typer, ERR_RDR::METHOD::__sys_data_classify);

		/*ADD_2_STK_RTN_FALSE(0, __sys_data_classify((e_system_message::messages::e_data)overlays.system_control.message)
		,ERR_RDR::BASE,*/
		break;
	}
	case e_system_message::e_status_type::Inquiry:
	{
		__sys_inquiry_classify((e_system_message::messages::e_inquiry)message);
		break;
	}

	default:
		ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_typer, ERR_RDR::METHOD::LINE::illegal_system_record_type);
		break;
	}
	return true;
}
