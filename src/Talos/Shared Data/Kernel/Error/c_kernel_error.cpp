#include <stddef.h>
#include "c_kernel_error.h"
#include "../CPU/c_kernel_cpu.h"
#include "../Comm/c_kernel_comm.h"


namespace Talos
{
	namespace Kernel
	{

		s_kernel_error Error::framework_error;

		s_kernel_error Error::error_stack[ERROR_STACK_SIZE];
		uint8_t Error::stack_index = 0;

		//Keep in mind an error may originate on CPU 1, serial port 3, but we want to report it on CPU1 serial port 0 (or host) so we can debug easier.
		void Error::raise_error(uint16_t base, uint16_t method, uint16_t method_or_line, uint8_t event_id)
		{
			//Save one stack slot, so we can error that the stack was full if needed
			if (stack_index + 2 >= ERROR_STACK_SIZE)
			{
				//stack is full and we already added the full stack error. nothing we can do here. Sorry!
				if (stack_index + 2 == ERROR_STACK_SIZE)
					return;

				//Stack is full so put the error indicating the state is full in here.
				stack_index++;
				error_stack[stack_index].behavior = e_error_behavior::Critical;
				error_stack[stack_index].stack.base = 0;
				error_stack[stack_index].stack.method = 0;
				error_stack[stack_index].stack.method_or_line = 0;
				return;
			}

			CPU::cluster[event_id].system_events.set((int)c_cpu::e_event_type::Error);

			//Error::framework_error.buffer_head = ((Talos::Shared::FrameWork::Events::Router::inputs.pntr_ring_buffer + (int)event_id)->ring_buffer._head);
			//Error::framework_error.buffer_tail = ((Talos::Shared::FrameWork::Events::Router::inputs.pntr_ring_buffer + (int)event_id)->ring_buffer._tail);
			Error::framework_error.origin = (int)event_id;
			//Error::framework_error.data = ((Talos::Shared::FrameWork::Events::Router::inputs.pntr_ring_buffer + (int)event_id)->storage);
			Error::framework_error.stack.base = base;
			Error::framework_error.stack.method = method;
			Error::framework_error.stack.method_or_line = method_or_line;

			//copy the error to the error stack.
			memcpy(&error_stack[stack_index], &framework_error, sizeof(framework_error));

			//Dont print the error details yet. This is coming from the kernel and we cant play around time wise
			//Error::general_error_handler(CPU::host_id);
			stack_index++;
		}

		void Error::general_error_handler(int cpu_target)
		{
			Comm::pntr_string_writer(cpu_target, "Err:");
			if (Error::framework_error.behavior == e_error_behavior::Critical)
			{
				CPU::cluster[CPU::host_id].system_events.set((int)c_cpu::e_event_type::Error);
				Comm::pntr_string_writer(cpu_target, "{Critical}");
			}
			__print_base(cpu_target);

		}

		void Error::ngc_error_handler(int cpu_target, char * ngc_line)
		{
			Comm::pntr_string_writer(cpu_target, "Err:{Ngc}");
			__write_eol(cpu_target);

			CPU::cluster[CPU::host_id].system_events.set((int)c_cpu::e_event_type::Error);

			__print_base(cpu_target);

			Comm::pntr_string_writer(cpu_target, ngc_line);
			__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "** Ngc Reset **");
			__write_eol(cpu_target);
		}

		void Error::__print_base(int cpu_target)
		{
			__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\tBhr:");
			Comm::pntr_int32_writer(cpu_target, (int)Error::framework_error.behavior);
			__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\tSyM:");
			Comm::pntr_int32_writer(cpu_target, (int)Error::framework_error.sys_message);
			__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\tSyT:");
			Comm::pntr_int32_writer(cpu_target, (int)Error::framework_error.sys_type);
			__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\tUC1:");
			Comm::pntr_int32_writer(cpu_target, (int)Error::framework_error.user_code1);
			__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\tUC2:");
			Comm::pntr_int32_writer(cpu_target, (int)Error::framework_error.user_code2);
			__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\tOrg:");
			Comm::pntr_int32_writer(cpu_target, (int)Error::framework_error.origin);
			__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\tStk:");
			__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\t\tBse:");
			Comm::pntr_int32_writer(cpu_target, (int)Error::framework_error.stack.base);
			__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\t\tMod:");
			Comm::pntr_int32_writer(cpu_target, (int)Error::framework_error.stack.method);
			__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\t\tLin:");
			Comm::pntr_int32_writer(cpu_target, (int)Error::framework_error.stack.method_or_line);

			if (Error::framework_error.data != NULL)
			{
				__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\tDta:");
				for (uint16_t i = 0; i < RING_BUFFER_SIZE; i++)
				{
					Comm::pntr_int32_writer(cpu_target, *(Error::framework_error.data + i));
					Comm::pntr_byte_writer(cpu_target, ',');
				}

				__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\tHed:");
				Comm::pntr_int32_writer(cpu_target, (int)Error::framework_error.buffer_head);
				__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\tTal:");
				Comm::pntr_int32_writer(cpu_target, (int)Error::framework_error.buffer_tail);

				__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\tRcR:");
				Comm::pntr_int32_writer(cpu_target, (int)Error::framework_error.read_crc);
				__write_eol(cpu_target); Comm::pntr_string_writer(cpu_target, "\tNcR:");
				Comm::pntr_int32_writer(cpu_target, (int)Error::framework_error.new_crc);
			}
			__write_eol(cpu_target);
		}

		void Error::__write_eol(int cpu_target)
		{
			Comm::pntr_string_writer(cpu_target, "\r\n");
		}
	}
}