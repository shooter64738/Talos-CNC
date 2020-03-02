#include<stdint.h>

class e_status_message
{
public:

	enum class e_critical :uint8_t
	{
		testcritical = 0

	};

	enum class e_data :uint8_t
	{
		testdata = 0

	};

	enum class e_informal :uint8_t
	{
		ReadyToProcess = 0

	};
	enum class e_warning :uint8_t
	{
		testwarning = 0

	};
};