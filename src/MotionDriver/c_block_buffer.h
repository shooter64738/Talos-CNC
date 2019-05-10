#ifndef __C_BLOCK_ITEM_BUFFER
#define __C_BLOCK_ITEM_BUFFER
#include <stdint.h>
#include "c_block.h"

namespace Motion_Core
{
	namespace Planner
	{
		class Buffer
		{
			public:
			static const uint8_t BUFFER_SIZE = 36;

			static Block_Item *Read();
			static Block_Item *Write();
			static Block_Item *Current();
			static Block_Item *Previous();
			static Block_Item *Newest();
			static Block_Item *Get(int8_t From);
			static void Advance();
			static void Reset();
			static uint8_t IsLastItem();
			static uint8_t Available();
			static uint8_t Full();

			private:
			static Block_Item _buffer[];
			static int8_t _tail;
			static int8_t _head;
			static uint8_t _full;
		};
	};
};
#endif