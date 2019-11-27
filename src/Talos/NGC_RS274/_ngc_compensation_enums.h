#include <stdint.h>
#ifndef NGC_COMP_DEFINES_H_
#define NGC_COMP_DEFINES_H_
enum class e_compensation_states : int8_t
{
	CurrentCompensationOffNotActivating = 0,
	CurrentCompensationOffActiving = 1,
	CurrentCompensationOnActive = 2,
	CurrentCompensationOnDeactivating = 3
};

enum class e_compensation_side : int8_t
{
	NoSideSpecified = 0,
	CompensatingLeft = 1,
	CompensatingRight = -1,
};

enum class e_compensation_errors : int8_t
{
	OK = 0,
	EndMotionNotALine = 1,
	BeginMotionNotALine = 2,
	LeadInMotionSmallerThanToolRadius = 3,
	LeadOutMotionSmallerThanToolRadius = 4,
};
#endif