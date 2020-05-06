#ifndef ___C_KERNEL_DATAHANDLER_PACKET_
#define ___C_KERNEL_DATAHANDLER_PACKET_

#include "../../../c_ring_template.h"
#include "../CPU/c_cpu.h"
#include "../_s_system_record.h"

#define SYS_MESSAGE_BUFFER_SIZE 5
struct s_packet
{
	bool has_addendum = false;
	bool addendum_checked = false;
	s_bit_flag_controller<uint32_t> *addendum_event_object = NULL;
	//s_control_message *message_record;
	uint8_t addendum_event_id = 0;
	uint8_t addendum_size = 0;
	uint8_t record_number = 0;
	uint16_t bytes_count = 0;
	char * cache[2];
	uint8_t counter = 0; //byte counter (counts down)
	uint8_t event_id = 0;
	s_bit_flag_controller<uint32_t> * event_object = NULL;
	//uint8_t target;
	c_cpu *active_cpu = NULL;
	s_control_message sys_message_buffer[SYS_MESSAGE_BUFFER_SIZE];
	uint8_t buffer_index = 0;
	uint8_t last_complete = 0;
	bool ready = false;
};
#endif
