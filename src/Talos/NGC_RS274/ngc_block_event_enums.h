#include <stdint.h>
enum class e_block_event : uint8_t
{
	Motion = 0,
	Cutter_radius_compensation = 1,
	Tool_length_offset = 2,
	Feed_rate_mode = 3,
	Feed_rate = 4,
	Spindle_rate = 5,
	Spindle_mode = 6,
	Tool_id = 7,
	Non_modal = 8,
	Units = 9,
	Coolant = 10,
	Tool_Change_Request = 11,
	Block_Set_To_Execute = 12,
	Block_Set_To_Held = 13,
	Canned_Cycle_Active = 14,
	PLANE_SELECTION = 15,
	DISTANCE_MODE = 16,
	RETURN_MODE_CANNED_CYCLE = 17,
	COORDINATE_SYSTEM_SELECTION = 18,
	PATH_CONTROL_MODE = 19,
	RECTANGLAR_POLAR_COORDS_SELECTION = 20,
	STOPPING = 21,
	OVERRIDE = 25,
	USER_DEFINED = 26

};