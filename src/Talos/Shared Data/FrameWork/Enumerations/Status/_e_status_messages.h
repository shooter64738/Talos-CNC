#include<stdint.h>

class e_status_message
{
public:
	uint8_t value;

	enum class e_critical :uint8_t
	{
		testinformal = 0

	};

	enum class e_data :uint8_t
	{
		testinformal = 0

	};

	enum class e_informal :uint8_t
	{
		testinformal = 0

	};
	enum class e_warning :uint8_t
	{
		testinformal = 0

	};
};