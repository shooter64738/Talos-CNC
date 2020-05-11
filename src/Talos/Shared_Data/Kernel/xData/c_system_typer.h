#include "../kernel_configuration.h"
#include "../_e_record_types.h"
#include "../_e_system_messages.h"
#include "u_data_overlay.h"


class c_system_read_typer
{
public:
	enum class e_buffer_source_type
	{
		Serial0 = 0,
	};
	
	struct s_read_record
	{
		u_data_overlays overlays;
		uint16_t addendum_size = 0;
		e_record_types record_type = e_record_types::Unknown;
		uint16_t read_count = 0;
		uint16_t *addendum_crc_value = 0;
		bool has_addendum = false;

		
	};
	s_read_record rec_buffer[CPU_CONTROL_BUFFER_SIZE];
	bool expand_record();
	

private:

	bool __sys_critical_classify(e_system_message::messages::e_critical message);
	bool __sys_data_classify(e_system_message::messages::e_data message);
	bool __sys_informal_classify(e_system_message::messages::e_informal);
	bool __sys_inquiry_classify(e_system_message::messages::e_inquiry message);
	bool __sys_warning_classify(e_system_message::messages::e_warning message);
	bool __sys_typer();
};