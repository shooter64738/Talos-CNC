#include "c_block.h"
#include <string.h>

using namespace Motion_Core::Planner;

//Motion_Core::Segment::Timer::Timer_Item::Timer_Item()
//{
//}
//Motion_Core::Segment::Timer::Timer_Item::~Timer_Item()
//{
//}
Block_Item::Block_Item()
{

}

Block_Item::~Block_Item()
{

}

void Block_Item::Reset()
{
	memset(this->steps, 0, sizeof(this->steps));
	this->step_event_count = 0;
	this->direction_bits = 0;
	this->condition = 0;
	this->line_number = 0;
	this->entry_speed_sqr = 0;
	this->max_entry_speed_sqr = 0;
	this->acceleration = 0;
	this->millimeters = 0;
	this->max_junction_speed_sqr = 0;
	this->rapid_rate = 0;
	this->programmed_rate = 0;
	this->spindle_speed = 0;
}