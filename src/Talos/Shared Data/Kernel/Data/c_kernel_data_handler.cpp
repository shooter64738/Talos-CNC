
#include "c_kernel_data_handler.h"
#include "../Base/c_kernel_base.h"
#include "../Error/c_kernel_error.h"
#include "../Comm/c_kernel_comm.h"

#define SYS_CONTROL_RECORD 0
#define SYS_ADDENDUM_RECORD 1


#define BASE_ERROR 200
#define SYSTEM_RECORD_POINTER_NULL 1
#define SYSTEM_CRC_FAILED 1
#define ADDENDUM_CRC_FAILED 2
#define UNKNOWN_RECORD_TYPE 3
#define UNHANDLED_RECORD_TYPE 4
#define UNCLASSED_MESSAGE_TYPE 5
#define UNKNOWN_MESSAGE_TYPE 6
#define CPU_SYSTEM_RECORD_LOCKED_FOR_WRITE 7
#define CPU_SYSTEM_RECORD_LOCKED_FOR_READ 8
#define OUT_BUFFER_FULL 9
//
//namespace Talos
//{
//	namespace Kernel
//	{
//		namespace DataHandler
//		{
//			c_cpu Binary::host_cpu;
//
//			s_packet Binary::read_packet[CPU_CLUSTER_COUNT];
//			
//			void Binary::f_initialize(c_cpu host_cpu)
//			{
//				Binary::host_cpu = host_cpu;
//			}
//
//
//			void Binary::__print_diag_rx(char * data, uint16_t size, uint16_t byte_count)
//			{
//				Comm::pntr_string_writer(Binary::host_cpu.ID, "rx");
//
//				for (int i = 0; i < size; i++)
//				{
//					Comm::pntr_int32_writer(Binary::host_cpu.ID, *(data + i));
//					Comm::pntr_byte_writer(Binary::host_cpu.ID, ',');
//				}
//
//				Comm::pntr_string_writer(Binary::host_cpu.ID, "=");
//				Comm::pntr_int32_writer(Binary::host_cpu.ID, byte_count);
//				Comm::pntr_string_writer(Binary::host_cpu.ID, "\r\n");
//			}
//			
//		}
//	}
//}