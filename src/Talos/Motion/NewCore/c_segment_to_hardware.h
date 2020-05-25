#ifndef __C_MOTION_CORE_OUTPUT_SEGMENT_H
#define __C_MOTION_CORE_OUTPUT_SEGMENT_H
#include <stdint.h>

#include "support_items/s_segment_timer_common.h"
#include "support_items/s_motion_block.h"

namespace Talos
{
	namespace Motion
	{
		namespace Core
		{
			namespace Output
			{
				class Segment
				{
					//variables
				public:
					static __s_motion_block* active_block;
				protected:
				private:
					static s_common_segment_items previous_flags;

					//functions
				public:
					static void init_new_motion();
					static void run();
				protected:
				private:

				};

			};
		};
	};
};
#endif