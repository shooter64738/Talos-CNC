#ifndef _C_MOTION_SYSTEM_STRUCT
#define _C_MOTION_SYSTEM_STRUCT

#include <stdint.h>

enum class e_machine_type
{
	mill = 1,
	lathe = 2,
	laser = 3,
	plasma = 4,
};

//PACK(
struct s_motion_system_settings
{
	e_machine_type machine_type_class;
	
};
//);
#endif