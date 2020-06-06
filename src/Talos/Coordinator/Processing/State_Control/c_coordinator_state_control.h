#ifndef __C_MOTION_CORE_CONTROLLER_H
#define __C_MOTION_CORE_CONTROLLER_H
#include <stdint.h>
#include "../../../c_ring_template.h"
#include "../../../_bit_flag_control.h"

/*

All control flags for coordination are in here. If its a flag that has something to do with the coordinator
it is controlled by flags in here. If I find a missed flag I move it to here. I want a central
location to control all coordinator processing from.

*/

namespace Talos
{
	namespace Coordinator
	{
		namespace Core
		{
			namespace States
			{
				enum class e_state_class :uint32_t
				{
					Process = 1, //<--Process handles gcode inbound buffering
					Output = 2,
				};
				extern void execute();
				extern void execute(e_state_class st_class);

				class Process
				{
					//variables
				public:
					enum class e_states :uint32_t
					{
						//ngc_data_available = 0,

					};
					static s_bit_flag_controller<e_states> states;

				protected:
				private:

					//functions
				public:
					static void execute();
					static void __read_ngc_line(char* source, uint16_t data_size);
				protected:
				private:

				};

				class Motion
				{
					//variables
				public:
					enum class e_states :uint32_t
					{
					};
					static s_bit_flag_controller<e_states> states;

				protected:
				private:
					enum class e_internal_states :uint32_t
					{

					};
					static s_bit_flag_controller<e_internal_states> __internal_states;

					//functions
				public:
					static void execute();
				protected:
				private:

				};

				class Output
				{
					//variables
				public:
					enum class e_states :uint32_t
					{

					};
					static s_bit_flag_controller<e_states> states;

				protected:
				private:

					//functions
				public:
					static void execute();
				protected:
				private:


				};
			};
		};
	};
};
#endif