#include "c_segment_timer_item.h"
#include <stddef.h>

Motion_Core::Segment::Timer::Timer_Item::Timer_Item()
{
}
Motion_Core::Segment::Timer::Timer_Item::~Timer_Item()
{
}
Motion_Core::Segment::Timer::Timer_Item::Timer_Item(uint16_t steps_to_execute_in_this_segment
	, uint16_t timer_delay_value, Motion_Core::Segment::Bresenham::Bresenham_Item *st_block_index
	, uint8_t timer_prescaler, uint32_t line_number)
{
	this->timer_delay_value = timer_delay_value;
	this->line_number = line_number;
	this->steps_to_execute_in_this_segment = steps_to_execute_in_this_segment;
	this->timer_prescaler = timer_prescaler;
	this->bresenham_in_item = st_block_index;
}
uint8_t Motion_Core::Segment::Timer::Timer_Item::Calculate()
{
	
	return 0;
}

Motion_Core::Segment::Timer::Timer_Item Motion_Core::Segment::Timer::Buffer::_buffer[Motion_Core::Segment::Timer::BUFFER_SIZE];
int16_t Motion_Core::Segment::Timer::Buffer::_tail = 0;
int16_t Motion_Core::Segment::Timer::Buffer::_head = 1;

/*
Returns a pointer to the current buffer tail object for reading
*/
Motion_Core::Segment::Timer::Timer_Item *Motion_Core::Segment::Timer::Buffer::Read()
{
	if (Motion_Core::Segment::Timer::Buffer::_tail == BUFFER_SIZE)
		Motion_Core::Segment::Timer::Buffer::_tail = -1;

	if (Motion_Core::Segment::Timer::Buffer::_tail == Motion_Core::Segment::Timer::Buffer::_head)
		return NULL;

	return &_buffer[++Motion_Core::Segment::Timer::Buffer::_tail];
}

/*
Returns a pointer to the current buffer header object for writing
*/
Motion_Core::Segment::Timer::Timer_Item *Motion_Core::Segment::Timer::Buffer::Write()
{

	if (Motion_Core::Segment::Timer::Buffer::_head == BUFFER_SIZE)
		Motion_Core::Segment::Timer::Buffer::_head = 0;
	
	if (Motion_Core::Segment::Timer::Buffer::_head == Motion_Core::Segment::Timer::Buffer::_tail)
		return NULL;

	return &_buffer[Motion_Core::Segment::Timer::Buffer::_head++];
}

uint8_t Motion_Core::Segment::Timer::Buffer::Available()
{
	if (Motion_Core::Segment::Timer::Buffer::_head == Motion_Core::Segment::Timer::Buffer::_tail)
		return 0;
	return 1;
}

void Motion_Core::Segment::Timer::Buffer::Reset()
{
	Motion_Core::Segment::Timer::Buffer::_tail = 0;
	Motion_Core::Segment::Timer::Buffer::_head = 1;
}