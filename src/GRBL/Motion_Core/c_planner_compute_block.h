
#include <stdint.h>
#include <stddef.h>

namespace Motion_Core
{
	namespace Planner
	{
		static const uint8_t BUFFER_SIZE = 20;

		class Block_Item
		{
		public:
			

		public:
			Block_Item();
			~Block_Item();
		};
		
		class Buffer
		{
			public:
				static Block_Item *Read();
			static Motion_Core::Planner::Block_Item *Write();
			static void Reset();

			private:
				static Block_Item _buffer[];
			static int16_t _tail;
			static int16_t _head;


		};
	};
};

