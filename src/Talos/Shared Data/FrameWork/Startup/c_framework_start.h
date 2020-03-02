#ifndef _C_FRAMEWORK_STARTUP
#define _C_FRAMEWORK_STARTUP

#include <stdint.h>
namespace Talos
{
	namespace Shared
	{
		namespace FrameWork
		{
			class StartUp
			{
			public:
				struct s_cpu_type //these doubel as port address for the send
				{
					uint8_t Host;
					uint8_t Coordinator;
					uint8_t Motion;
					uint8_t Spindle;
					uint8_t Peripheral;
				};
				static s_cpu_type cpu_type;
			};
		};
	};
};
#endif