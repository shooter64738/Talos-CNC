
#include "c_framework_error.h"
#include <stddef.h>
#include "../Event/c_event_router.h"
#include "../Startup/c_framework_start.h"

//void(*Talos::Shared::FrameWork::Error::extern_pntr_error_handler)();
//void(*Talos::Shared::FrameWork::Error::extern_pntr_ngc_error_handler)(char * ngc_line);

s_framework_error Talos::Shared::FrameWork::Error::framework_error;

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
	__write_eol(); FrameWork::StartUp::string_writer("\tSrc:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.source);
	__write_eol(); FrameWork::StartUp::string_writer("\tBhr:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.behavior);
	__write_eol(); FrameWork::StartUp::string_writer("\tDsz:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.data_size);
	__write_eol(); FrameWork::StartUp::string_writer("\tGrp:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.group);
	__write_eol(); FrameWork::StartUp::string_writer("\tPrs:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.process);
	__write_eol(); FrameWork::StartUp::string_writer("\tRtp:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.__rec_type__);
	__write_eol(); FrameWork::StartUp::string_writer("\tCde:");
	FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.code);
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
		for (uint8_t i = 0; i<255; i++)
		{
			FrameWork::StartUp::int32_writer(*(Talos::Shared::FrameWork::Error::framework_error.data + i));
			FrameWork::StartUp::byte_writer(',');
		}

		__write_eol(); FrameWork::StartUp::string_writer("\tHed:");
		FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.buffer_head);
		__write_eol(); FrameWork::StartUp::string_writer("\tTal:");
		FrameWork::StartUp::int32_writer((int)Talos::Shared::FrameWork::Error::framework_error.buffer_tail);
	}
	__write_eol();
}

void Talos::Shared::FrameWork::Error::__write_eol()
{
	FrameWork::StartUp::string_writer("\r\n");
}