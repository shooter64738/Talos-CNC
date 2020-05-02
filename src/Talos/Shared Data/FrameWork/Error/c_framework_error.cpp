
#include "c_framework_error.h"
#include <stddef.h>
#include "../Event/c_event_router.h"
#include "../Startup/c_framework_start.h"

//void(*Talos::Shared::FrameWork::Error::extern_pntr_error_handler)();
//void(*Talos::Shared::FrameWork::Error::extern_pntr_ngc_error_handler)(char * ngc_line);

s_framework_error Talos::Shared::FrameWork::Error::framework_error;


void Talos::Shared::FrameWork::Error::raise_error(uint16_t base, uint16_t method, uint16_t line, uint8_t event_id)
{
	Talos::Shared::FrameWork::StartUp::CpuCluster[event_id].system_events.set((int)c_cpu::e_event_type::Error);

	Talos::Shared::FrameWork::Error::framework_error.buffer_head = ((Talos::Shared::FrameWork::Events::Router::inputs.pntr_ring_buffer + (int)event_id)->ring_buffer._head);
	Talos::Shared::FrameWork::Error::framework_error.buffer_tail = ((Talos::Shared::FrameWork::Events::Router::inputs.pntr_ring_buffer + (int)event_id)->ring_buffer._tail);
	Talos::Shared::FrameWork::Error::framework_error.origin = (int)event_id;
	Talos::Shared::FrameWork::Error::framework_error.data = ((Talos::Shared::FrameWork::Events::Router::inputs.pntr_ring_buffer + (int)event_id)->storage);
	//
	Talos::Shared::FrameWork::Error::framework_error.stack.base = base;
	Talos::Shared::FrameWork::Error::framework_error.stack.method = method;
	Talos::Shared::FrameWork::Error::framework_error.stack.line = line;

	Talos::Shared::FrameWork::Error::general_error_handler();


}


void Talos::Shared::FrameWork::Error::general_error_handler()
{
	bool must_lock = false;

	FrameWork::StartUp::string_writer("Err:");
	if (Talos::Shared::FrameWork::Error::framework_error.behavior == e_error_behavior::Critical)
	{
		//Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::SystemCritical);
		//Talos::Shared::FrameWork::Events::extern_system_events.event_manager.clear((int)s_system_events::e_event_type::SystemAllOk);
		Talos::Shared::FrameWork::StartUp::CpuCluster[0].system_events.set((int)c_cpu::e_event_type::Error);
		FrameWork::StartUp::string_writer("{Critical}");
		must_lock = true;
	}
	__print_base();
	while (must_lock) {}
}

void Talos::Shared::FrameWork::Error::ngc_error_handler(char * ngc_line)
{
	FrameWork::StartUp::string_writer("Err:{Ngc}");
	__write_eol();

	//Talos::Shared::FrameWork::Events::extern_system_events.event_manager.set((int)s_system_events::e_event_type::NgcReset);
	Talos::Shared::FrameWork::StartUp::CpuCluster[0].system_events.set((int)c_cpu::e_event_type::Error);

	__print_base();

	FrameWork::StartUp::string_writer(ngc_line);
	__write_eol(); FrameWork::StartUp::string_writer("** Ngc Reset **");
	__write_eol();
}

void Talos::Shared::FrameWork::Error::__print_base()
{
	//__write_eol(); FrameWork::StartUp::string_writer("\tSrc:");
	//FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.source);
	__write_eol(); FrameWork::StartUp::string_writer("\tBhr:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.behavior);
	//__write_eol(); FrameWork::StartUp::string_writer("\tDsz:");
	//FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.data_size);
	//__write_eol(); FrameWork::StartUp::string_writer("\tGrp:");
	//FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.group);
	//__write_eol(); FrameWork::StartUp::string_writer("\tPrs:");
	//FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.process);
	//__write_eol(); FrameWork::StartUp::string_writer("\tRtp:");
	//FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.__rec_type__);
	//__write_eol(); FrameWork::StartUp::string_writer("\tCde:");
	//FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.code);
	__write_eol(); FrameWork::StartUp::string_writer("\tSyM:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.sys_message);
	__write_eol(); FrameWork::StartUp::string_writer("\tSyT:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.sys_type);
	__write_eol(); FrameWork::StartUp::string_writer("\tUC1:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.user_code1);
	__write_eol(); FrameWork::StartUp::string_writer("\tUC2:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.user_code2);
	__write_eol(); FrameWork::StartUp::string_writer("\tOrg:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.origin);
	__write_eol(); FrameWork::StartUp::string_writer("\tStk:");
	__write_eol(); FrameWork::StartUp::string_writer("\t\tBse:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.stack.base);
	__write_eol(); FrameWork::StartUp::string_writer("\t\tMod:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.stack.method);
	__write_eol(); FrameWork::StartUp::string_writer("\t\tLin:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.stack.line);

	if (Talos::Shared::FrameWork::Error::framework_error.data != NULL)
	{
		__write_eol(); FrameWork::StartUp::string_writer("\tDta:");
		for (uint16_t i = 0; i<RING_BUFFER_SIZE; i++)
		{
			FrameWork::StartUp::int32_writer(*(Talos::Shared::FrameWork::Error::framework_error.data + i));
			FrameWork::StartUp::byte_writer(',');
		}

		__write_eol(); FrameWork::StartUp::string_writer("\tHed:");
		FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.buffer_head);
		__write_eol(); FrameWork::StartUp::string_writer("\tTal:");
		FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.buffer_tail);
		
		__write_eol(); FrameWork::StartUp::string_writer("\tRcR:");
		FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.read_crc);
		__write_eol(); FrameWork::StartUp::string_writer("\tNcR:");
		FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.new_crc);
	}
	__write_eol();
}

void Talos::Shared::FrameWork::Error::__write_eol()
{
	FrameWork::StartUp::string_writer("\r\n");
}