#ifndef _C_FRAMEWORK_CPU_DATAHANDLER
#define _C_FRAMEWORK_CPU_DATAHANDLER

#include <stdint.h>
#include "../_e_record_types.h"
#include "../framework_configuration.h"
#include "../_s_system_record.h"
#include "../_e_system_messages.h"
#include "../../../c_ring_template.h"
#include "../../../communication_def.h"
#include "../../Settings/Motion/_s_motion_control_settings_encapsulation.h"


#define SERIAL 0
#define BUFFER_SOURCES_COUNT 1
#define MAX_TEXT_BUFFER_SIZE 256

class c_data_handler
{
public:
	e_record_types cdh_get_message_type(c_ring_buffer<char> * buffer);
	void cdh_read();
	bool cdh_is_busy = false;

private:

	union u_data_overlays
	{
		s_control_message system_control;
		s_motion_control_settings_encapsulation motion_control_settings;
		char text[256];
	};

	struct s_read_record
	{
		u_data_overlays overlays;
		uint16_t addendum_size = 0;
		uint16_t expand_record()
		{
			return __sys_typer();
		}
		e_record_types record_type;

	private:
		uint16_t __sys_critical_classify()
		{
			switch ((e_system_message::messages::e_critical)overlays.system_control.message)
			{
			case e_system_message::messages::e_critical::testcritical:
			{
				addendum_size = 0;
				break;
			}

			default:
				addendum_size = 0;
				break;

			}
			return addendum_size;
		}
		
		uint16_t __sys_data_classify()
		{
			switch ((e_system_message::messages::e_data)overlays.system_control.message)
			{
			case e_system_message::messages::e_data::MotionConfiguration:
				addendum_size = sizeof(s_motion_control_settings_encapsulation);
				break;
			case e_system_message::messages::e_data::NgcDataLine:
				addendum_size = 256;
				break;
			case e_system_message::messages::e_data::NgcDataRequest:
				addendum_size = 0;
				break;
			case e_system_message::messages::e_data::SystemRecord:
				addendum_size = sizeof(s_control_message);
				break;
			default:
				addendum_size = 0;
				break;
			}
			return addendum_size;
		}

		uint16_t __sys_informal_classify()
		{
			switch ((e_system_message::messages::e_critical)overlays.system_control.message)
			{
			case e_system_message::messages::e_critical::testcritical:
			{
				addendum_size = 0;
				break;
			}

			default:
				addendum_size = 0;
				break;
			}
			return addendum_size;
		}

		uint16_t __sys_inquiry_classify()
		{
			switch ((e_system_message::messages::e_inquiry)overlays.system_control.message)
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
				break;
			}
			return addendum_size;
		}

		uint16_t __sys_warning_classify()
		{
			switch ((e_system_message::messages::e_warning)overlays.system_control.message)
			{
			case e_system_message::messages::e_warning::testwarning:
			{
				addendum_size = 0;
				break;
			}

			default:
				addendum_size = 0;
				break;
			}
			return addendum_size;
		}

		uint16_t  __sys_typer()
		{
			switch ((e_system_message::e_status_type)overlays.system_control.type)
			{
			case e_system_message::e_status_type::Critical:
			{
				__sys_critical_classify();
				break;
			}
			case e_system_message::e_status_type::Warning:
			{
				__sys_warning_classify();
				break;
			}
			case e_system_message::e_status_type::Informal:
			{
				__sys_informal_classify();
				break;
			}
			case e_system_message::e_status_type::Data:
			{
				__sys_data_classify();
				break;
			}
			case e_system_message::e_status_type::Inquiry:
			{
				__sys_inquiry_classify();
				break;
			}

			default:
				addendum_size = 0;
				break;
			}
			return addendum_size;
		}
	};


	uint8_t __current_source;

	c_ring_buffer<char> * __active_source_buffer;

	char * __active_target_buffer;

	char __text_buffer[MAX_TEXT_BUFFER_SIZE];

	struct s_source_buffer
	{
		uint8_t index;
		//u_data_overlays rec_buffer[CPU_CONTROL_BUFFER_SIZE];
		s_read_record rec_buffer[CPU_CONTROL_BUFFER_SIZE];
		uint8_t free()
		{
			return CPU_CONTROL_BUFFER_SIZE - index;
		}
		//u_data_overlays * get()
		s_read_record * get()
		{
			return &rec_buffer[index];
		}
		s_read_record * active_record;
	};

	s_source_buffer __source_buffers[BUFFER_SOURCES_COUNT]; //a buffer for each input type (com,net,disk,spi, etc)
	
	uint16_t __read_size = 0;
	uint16_t __read_count = 0;

	/*void __expand_sys_message(s_read_record *sys_record);
	void __sys_typer(s_read_record *sys_record);

	uint16_t(*pntr_classifier)(s_read_record *sys_record);
	static uint16_t __sys_critical_classify(s_read_record *sys_record);
	static uint16_t __sys_data_classify(s_read_record *sys_record);
	static uint16_t __sys_informal_classify(s_read_record *sys_record);
	static uint16_t __sys_inquiry_classify(s_read_record *sys_record);
	static uint16_t __sys_warning_classify(s_read_record *sys_record);*/

};
#endif