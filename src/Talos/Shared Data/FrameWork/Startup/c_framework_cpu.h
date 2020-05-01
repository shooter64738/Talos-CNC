#ifndef _C_FRAMEWORK_CPU
#define _C_FRAMEWORK_CPU

#include <stdint.h>
#include "../../../_bit_flag_control.h"
#include "../../_s_status_record.h"

class c_cpu
{
	public:
	uint8_t ID;
	void initialize(uint8_t id);
	void Synch(
	e_system_message::messages::e_data init_message
	, e_system_message::e_status_type init_request_type
	, uint8_t init_ready_wait_id
	, e_system_message::e_status_type init_response_type
	, bool is_master_cpus);

	void service_events(int32_t * position, uint16_t rpm);

	s_control_message sys_message;
	enum class e_event_type : uint8_t
	{
		NoState = 0,
		OnLine = 1,
		Error = 2,
		SystemRecord = 3,
		AddendumRecord = 4,
	};
	s_bit_flag_controller<uint32_t> system_events;

	//s_bit_flag_controller<uint32_t> host_events;

	struct s_event_class
	{
		s_bit_flag_controller<uint32_t> Critical;
		s_bit_flag_controller<uint32_t> Warning;
		s_bit_flag_controller<uint32_t> Informal;
		s_bit_flag_controller<uint32_t> Data;
		s_bit_flag_controller<uint32_t> Inquiry;
	};

	s_event_class host_events;
	
	private:
	void __send_formatted_message(uint8_t init_message, uint8_t init_type);
	void __wait_formatted_message(uint8_t init_message, uint8_t init_type);
	
};
#endif