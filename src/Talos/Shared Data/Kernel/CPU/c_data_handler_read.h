#ifndef __C_KERNEL_CPU_DATAHANDLER_READ
#define __C_KERNEL_CPU_DATAHANDLER_READ

#include <stdint.h>
#include "../_e_record_types.h"
#include "../kernel_configuration.h"
#include "../_s_system_record.h"
#include "../_e_system_messages.h"
#include "../Error/kernel_error_codes_reader.h"
#include "../Error/c_kernel_error.h"
#include "../Error/_d_err_macros.h"
#include "../c_kernel_utils.h"
#include "../../Settings/Motion/_s_motion_control_settings_encapsulation.h"
#include "../../../c_ring_template.h"
#include "../../../communication_def.h"

#define SERIAL 0
#define BUFFER_SOURCES_COUNT 1
#define MAX_TEXT_BUFFER_SIZE 256

using Talos::Kernel::ErrorCodes::ERR_RDR;

class c_data_handler_read
{
protected:

	union u_data_overlays
	{
		s_control_message system_control;
		s_motion_control_settings_encapsulation motion_control_settings;
		char text[MAX_TEXT_BUFFER_SIZE];
	};

	struct s_read_record
	{
		u_data_overlays overlays;
		uint16_t addendum_size = 0;
		e_record_types record_type = e_record_types::Unknown;
		uint16_t read_count = 0;
		uint16_t *addendum_crc_value = 0;
		bool has_addendum = false;

		bool expand_record()
		{
			if (record_type == e_record_types::Text)
				return __sys_data_classify(e_system_message::messages::e_data::NgcDataLine);
			
			ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(__sys_typer()
				, 0,ERR_RDR::BASE, ERR_RDR::METHOD::expand_record, ERR_RDR::METHOD::__sys_typer);

			has_addendum = addendum_size > 0;
			return true;
		}

	private:
		bool __sys_critical_classify(e_system_message::messages::e_critical message)
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

		bool __sys_data_classify(e_system_message::messages::e_data message)
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

		bool __sys_informal_classify(e_system_message::messages::e_informal message)
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

		bool __sys_inquiry_classify(e_system_message::messages::e_inquiry message)
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

		bool __sys_warning_classify(e_system_message::messages::e_warning message)
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

		bool  __sys_typer()
		{
			switch ((e_system_message::e_status_type)overlays.system_control.type)
			{
			case e_system_message::e_status_type::Critical:
			{
				__sys_critical_classify((e_system_message::messages::e_critical)overlays.system_control.message);
				break;
			}
			case e_system_message::e_status_type::Warning:
			{
				__sys_warning_classify((e_system_message::messages::e_warning)overlays.system_control.message);
				break;
			}
			case e_system_message::e_status_type::Informal:
			{
				__sys_informal_classify((e_system_message::messages::e_informal)overlays.system_control.message);
				break;
			}
			case e_system_message::e_status_type::Data:
			{
				ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(__sys_data_classify((e_system_message::messages::e_data)overlays.system_control.message)
					, 0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_typer, ERR_RDR::METHOD::__sys_data_classify);

				/*ADD_2_STK_RTN_FALSE(0, __sys_data_classify((e_system_message::messages::e_data)overlays.system_control.message)
					,ERR_RDR::BASE,*/
				break;
			}
			case e_system_message::e_status_type::Inquiry:
			{
				__sys_inquiry_classify((e_system_message::messages::e_inquiry)overlays.system_control.message);
				break;
			}

			default:				
				ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_typer, ERR_RDR::METHOD::LINE::illegal_system_record_type);
				break;
			}
			return true;
		}
	};

	uint8_t __current_source;

	c_ring_buffer<char> * __active_source_buffer;

	char * __active_target_buffer;

	struct s_source_buffer
	{
		uint8_t index;
		//u_data_overlays rec_buffer[CPU_CONTROL_BUFFER_SIZE];
		s_read_record rec_buffer[CPU_CONTROL_BUFFER_SIZE];
		int16_t free()
		{
			return CPU_CONTROL_BUFFER_SIZE - index;
		}
		//u_data_overlays * get()
		s_read_record * get()
		{
			if (free()>0)
				return &rec_buffer[index];
			else
			{
				ADD_2_STK(0, ERR_RDR::BASE, ERR_RDR::METHOD::get, ERR_RDR::METHOD::free);
				return NULL;
			}
			
		}
		s_read_record * active_record;
	};

	s_source_buffer __source_buffers[BUFFER_SOURCES_COUNT]; //a buffer for each input type (com,net,disk,spi, etc)

	uint16_t __read_size = 0;
	bool __cdh_r_close_read();

public:
	bool cdh_r_get_message_type(c_ring_buffer<char> * buffer);
	bool cdh_r_read();
	bool cdh_r_is_busy = false;
	s_source_buffer * dvc_source;


};
#endif