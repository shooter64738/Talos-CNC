#include<stdint.h>
	enum class e_status_type :uint8_t
	{
		Critical = 0,
		Warning = 1,
		Informal = 2,
		Data = 3
	};