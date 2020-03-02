#include<stdint.h>
enum class e_origins :uint8_t
{
	Host = 0,
	Coordinator = 1,
	Motion = 2,
	Spindle = 3,
	Peripheral = 4

};