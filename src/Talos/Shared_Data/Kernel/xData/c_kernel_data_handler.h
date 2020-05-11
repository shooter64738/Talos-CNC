#ifndef ___C_KERNEL_DATAHANDLER_
#define ___C_KERNEL_DATAHANDLER_

#include "../../../c_ring_template.h"
#include "../CPU/c_cpu.h"
#include "../_s_system_record.h"

class c_data_control
{
	

	

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
			if (free() > 0)
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
};


#endif