#ifndef __C_KERNEL_CPU
#define __C_KERNEL_CPU

#include <stdint.h>
#include "../../../_bit_flag_control.h"
#include "../../../c_ring_template.h"
#include "../_s_system_record.h"
#include "../_e_system_messages.h"
#include "../kernel_configuration.h"
#include "../Error/kernel_error_codes_reader.h"
#include "../Error/c_kernel_error.h"
#include "../Error/_d_err_macros.h"
#include "../../Settings/Motion/_s_motion_control_settings_encapsulation.h"
//#include "c_data_handler_read.h"

#define SERIAL 0
#define BUFFER_SOURCES_COUNT 1
#define MAX_TEXT_BUFFER_SIZE 256

using Talos::Kernel::ErrorCodes::ERR_RDR;

class c_cpu
{
public:
	c_cpu();
	c_cpu(uint8_t id, volatile uint32_t* tick_timer_ms);

	uint8_t ID;
	bool initialize(uint8_t id, volatile uint32_t* tick_timer_ms);
	bool Synch(
		e_system_message::messages::e_data init_message
		, e_system_message::e_status_type init_request_type
		, uint8_t init_ready_wait_id
		, e_system_message::e_status_type init_response_type
		, bool is_master_cpus);

	bool service_events(int32_t* position, uint16_t rpm);
	bool send_message(uint8_t message
		, uint8_t type
		, uint8_t origin
		, uint8_t target
		, uint8_t state
		, uint8_t sub_state
		, int32_t* position_data);



	s_control_message sys_message;
	enum class e_event_type : uint8_t
	{
		NoState = 0,
		OnLine = 1,
		Error = 2,
		SystemRecord = 3,
		AddendumRecord = 4,
		ReBoot = 5,
		UnHealthy = 6,
	};
	s_bit_flag_controller<uint32_t> system_events;

	struct s_event_class
	{
		s_bit_flag_controller<uint32_t> Critical;
		s_bit_flag_controller<uint32_t> Warning;
		s_bit_flag_controller<uint32_t> Informal;
		s_bit_flag_controller<uint32_t> Data;
		s_bit_flag_controller<uint32_t> Inquiry;
	};

	s_event_class host_events;

	volatile char hw_data_container[RING_BUFFER_SIZE];
	c_ring_buffer <char> hw_data_buffer;

	volatile uint32_t* pntr_cycle_count_ms;
	uint32_t message_lag_cycles = 0;
	uint32_t next_cycle_check_time = 0;
	uint32_t last_time_code = 0;

	bool(*pntr_data_read_handler)(c_ring_buffer<char>* buffer, uint8_t cpu_id);
	bool(*pntr_data_write_handler)();

private:


	bool __check_health();
	bool __send_formatted_message(uint8_t init_message, uint8_t init_type);
	bool __wait_formatted_message(uint8_t init_message, uint8_t init_type);

	bool __check_data();

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
		uint16_t* addendum_crc_value = 0;
		bool has_addendum = false;

		/*bool expand_record()
		{
			if (record_type == e_record_types::Text)
			{
				return __sys_data_classify(e_system_message::messages::e_data::NgcDataLine);
			}
			else
			{
				ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(__sys_typer()
					, 0, ERR_RDR::BASE, ERR_RDR::METHOD::expand_record, ERR_RDR::METHOD::__sys_typer);
			}
			has_addendum = addendum_size > 0;
			return true;
		}*/

		//private:
		//	bool __sys_critical_classify(e_system_message::messages::e_critical message)
		//	{
		//		switch (message)
		//		{
		//		case e_system_message::messages::e_critical::testcritical:
		//		{
		//			addendum_size = 0;
		//			break;
		//		}

		//		default:
		//			ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_critical_classify, ERR_RDR::METHOD::LINE::illegal_system_critical_class_type);
		//			break;

		//		}
		//		return true;
		//	}

		//	bool __sys_data_classify(e_system_message::messages::e_data message)
		//	{
		//		switch (message)
		//		{
		//		case e_system_message::messages::e_data::MotionConfiguration:
		//			addendum_crc_value = &overlays.motion_control_settings.crc;
		//			addendum_size = sizeof(s_motion_control_settings_encapsulation);
		//			break;
		//		case e_system_message::messages::e_data::NgcDataLine:
		//			addendum_size = 256;

		//			break;
		//		case e_system_message::messages::e_data::NgcDataRequest:
		//			addendum_size = 0;
		//			break;
		//		case e_system_message::messages::e_data::SystemRecord:
		//			addendum_crc_value = &overlays.system_control.crc;
		//			addendum_size = sizeof(s_control_message);
		//			break;
		//		default:
		//			ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_data_classify, ERR_RDR::METHOD::LINE::illegal_system_data_class_type);
		//			break;
		//		}
		//		return true;
		//	}

		//	bool __sys_informal_classify(e_system_message::messages::e_informal message)
		//	{
		//		switch (message)
		//		{
		//		case e_system_message::messages::e_informal::ReadyToProcess:
		//		{
		//			addendum_size = 0;
		//			break;
		//		}

		//		default:
		//			ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_informal_classify, ERR_RDR::METHOD::LINE::illegal_system_informal_class_type);
		//			break;
		//		}
		//		return true;
		//	}

		//	bool __sys_inquiry_classify(e_system_message::messages::e_inquiry message)
		//	{
		//		switch (message)
		//		{
		//		case e_system_message::messages::e_inquiry::GCodeBlockReport:
		//			addendum_size = 0;
		//			break;
		//		case e_system_message::messages::e_inquiry::Invalid:
		//			addendum_size = 0;
		//			break;
		//		case e_system_message::messages::e_inquiry::MCodeBlockReport:
		//			addendum_size = 0;
		//			break;
		//		case e_system_message::messages::e_inquiry::WordStatusReport:
		//			addendum_size = 0;
		//			break;
		//		case e_system_message::messages::e_inquiry::MotionConfiguration:
		//			addendum_size = 0;
		//			break;
		//		case e_system_message::messages::e_inquiry::NgcDataLine:
		//			addendum_size = 0;
		//			break;
		//		case e_system_message::messages::e_inquiry::SystemRecord:
		//			addendum_size = 0;
		//			break;
		//		default:
		//			ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_inquiry_classify, ERR_RDR::METHOD::LINE::illegal_system_inquiry_class_type);
		//			break;
		//		}
		//		return true;
		//	}

		//	bool __sys_warning_classify(e_system_message::messages::e_warning message)
		//	{
		//		switch (message)
		//		{
		//		case e_system_message::messages::e_warning::testwarning:
		//		{
		//			addendum_size = 0;
		//			break;
		//		}

		//		default:
		//			ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_warning_classify, ERR_RDR::METHOD::LINE::illegal_system_warning_class_type);
		//			break;
		//		}
		//		return true;
		//	}

		//	bool  __sys_typer()
		//	{
		//		switch ((e_system_message::e_status_type)overlays.system_control.type)
		//		{
		//		case e_system_message::e_status_type::Critical:
		//		{
		//			__sys_critical_classify((e_system_message::messages::e_critical)overlays.system_control.message);
		//			break;
		//		}
		//		case e_system_message::e_status_type::Warning:
		//		{
		//			__sys_warning_classify((e_system_message::messages::e_warning)overlays.system_control.message);
		//			break;
		//		}
		//		case e_system_message::e_status_type::Informal:
		//		{
		//			__sys_informal_classify((e_system_message::messages::e_informal)overlays.system_control.message);
		//			break;
		//		}
		//		case e_system_message::e_status_type::Data:
		//		{
		//			ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(__sys_data_classify((e_system_message::messages::e_data)overlays.system_control.message)
		//				, 0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_typer, ERR_RDR::METHOD::__sys_data_classify);

		//			/*ADD_2_STK_RTN_FALSE(0, __sys_data_classify((e_system_message::messages::e_data)overlays.system_control.message)
		//				,ERR_RDR::BASE,*/
		//			break;
		//		}
		//		case e_system_message::e_status_type::Inquiry:
		//		{
		//			__sys_inquiry_classify((e_system_message::messages::e_inquiry)overlays.system_control.message);
		//			break;
		//		}

		//		default:
		//			ADD_2_STK_RTN_FALSE(0, ERR_RDR::BASE, ERR_RDR::METHOD::__sys_typer, ERR_RDR::METHOD::LINE::illegal_system_record_type);
		//			break;
		//		}
		//		return true;
		//	}

		//	void __set_entry_mode(char first_byte, char second_byte)
		//	{

		//		switch (first_byte)
		//		{
		//		case '?': //inquiry mode
		//		{
		//			__set_sub_entry_mode(second_byte);
		//			//This is an inquiry and system_events will set and handle this. We dont actually need the record data
		//			//Talos::Shared::c_cache_data::txt_record.pntr_record = NULL;
		//			break;
		//		}
		//		default:
		//		{
		//			//Talos::Shared::FrameWork::StartUp::CpuCluster[Talos::Shared::FrameWork::StartUp::cpu_type.Host].host_events.Data.set((int)e_system_message::messages::e_data::NgcDataLine);
		//			//assume its plain ngc g code data
		//		}

		//		}
		//	}

		//	void __set_sub_entry_mode(char byte)
		//	{
		//		//s_bit_flag_controller<uint32_t>* pntr_event = &Talos::Shared::FrameWork::StartUp::CpuCluster[Talos::Shared::FrameWork::StartUp::cpu_type.Host].host_events.Inquiry;
		//		switch (byte)
		//		{
		//		case 'G': //block g group status
		//			//pntr_event->set((int)e_system_message::messages::e_inquiry::GCodeBlockReport);
		//			break;
		//		case 'M': //block m group status
		//			//pntr_event->set((int)e_system_message::messages::e_inquiry::MCodeBlockReport);
		//			break;
		//		case 'W': //word value status
		//			//pntr_event->set((int)e_system_message::messages::e_inquiry::WordStatusReport);
		//			break;
		//		default:
		//			/*__raise_error(NULL, e_error_behavior::Informal, 0, e_error_group::DataHandler, e_error_process::Process
		//				, tracked_read_type, e_error_source::Serial, e_error_code::UnExpectedDataTypeForRecord);*/
		//			break;
		//		}

		//	}

	};

	bool __expand_record(s_read_record * record);
	bool __sys_critical_classify(e_system_message::messages::e_critical message, s_read_record* record);
	bool __sys_data_classify(e_system_message::messages::e_data message, s_read_record* record);
	bool __sys_informal_classify(e_system_message::messages::e_informal message, s_read_record* record);
	bool __sys_inquiry_classify(e_system_message::messages::e_inquiry message, s_read_record* record);
	bool __sys_warning_classify(e_system_message::messages::e_warning message, s_read_record* record);
	bool __sys_typer(s_read_record* record);
	bool __set_entry_mode(char first_byte, char second_byte);
	bool __set_sub_entry_mode(char byte);
	//Reader code
protected:

	uint8_t __current_source;

	c_ring_buffer<char>* __active_hw_buffer;

	char* __pntr_active_rcv_buffer;

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
		s_read_record* get()
		{
			if (free() > 0)
				return &rec_buffer[index];
			else
			{
				ADD_2_STK(0, ERR_RDR::BASE, ERR_RDR::METHOD::get, ERR_RDR::METHOD::free);
				return NULL;
			}

		}
		s_read_record* active_rcv_buffer;
	};

	s_source_buffer __rcving_buffers[BUFFER_SOURCES_COUNT]; //a buffer for each input type (com,net,disk,spi, etc)

	uint16_t __read_size = 0;
	bool __cdh_r_close_read();

public:
	bool cdh_r_get_message_type(c_ring_buffer<char>* buffer);
	bool cdh_r_read();
	bool cdh_r_is_busy = false;
	s_source_buffer* dvc_source;


	//reader code

};
//	};
//};
#endif