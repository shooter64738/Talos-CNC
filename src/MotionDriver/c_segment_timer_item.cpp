#include "c_segment_timer_item.h"
#include <stddef.h>

Motion_Core::Segment::Timer::Timer_Item::Timer_Item()
{
}
Motion_Core::Segment::Timer::Timer_Item::~Timer_Item()
{
}

Motion_Core::Segment::Timer::Timer_Item Motion_Core::Segment::Timer::Buffer::_buffer[Motion_Core::Segment::Timer::BUFFER_SIZE];
int8_t Motion_Core::Segment::Timer::Buffer::_tail = 0;
int8_t Motion_Core::Segment::Timer::Buffer::_head = 0;
uint8_t Motion_Core::Segment::Timer::Buffer::_full = 0;

/*
Returns a pointer to the current buffer tail object for reading (DOES move the tail)
*/
Motion_Core::Segment::Timer::Timer_Item *Motion_Core::Segment::Timer::Buffer::Read()
{
	//If buffer is not full and tail==head, its empty
	if (Motion_Core::Segment::Timer::Buffer::_full == 0 && Motion_Core::Segment::Timer::Buffer::_head == Motion_Core::Segment::Timer::Buffer::_tail)
	return NULL;

	Motion_Core::Segment::Timer::Timer_Item *item = &Motion_Core::Segment::Timer::Buffer::_buffer[Motion_Core::Segment::Timer::Buffer::_tail++];

	//Are we wrapping?
	if (Motion_Core::Segment::Timer::Buffer::_tail == Motion_Core::Segment::Timer::BUFFER_SIZE)
	Motion_Core::Segment::Timer::Buffer::_tail = 0;
	//If we read an item we arent full now.
	Motion_Core::Segment::Timer::Buffer::_full = 0;

	return item;
}

void Motion_Core::Segment::Timer::Buffer::Advance()
{
	//If buffer is not full and tail==head, its empty
	if (Motion_Core::Segment::Timer::Buffer::_full == 0
	&& (Motion_Core::Segment::Timer::Buffer::_head == Motion_Core::Segment::Timer::Buffer::_tail))
	return;
	Motion_Core::Segment::Timer::Buffer::_tail++;

	//Are we wrapping?
	if (Motion_Core::Segment::Timer::Buffer::_tail == Motion_Core::Segment::Timer::BUFFER_SIZE)
	Motion_Core::Segment::Timer::Buffer::_tail = 0;
	//If we read an item we arent full now.
	Motion_Core::Segment::Timer::Buffer::_full = 0;

}

/*
Returns a pointer to the current buffer tail object for reading (DOES NOT move the tail)
*/
Motion_Core::Segment::Timer::Timer_Item *Motion_Core::Segment::Timer::Buffer::Current()
{
	
	//If buffer is not full and tail==head, its empty
	if (Motion_Core::Segment::Timer::Buffer::_full == 0
	&& (Motion_Core::Segment::Timer::Buffer::_head == Motion_Core::Segment::Timer::Buffer::_tail))
	return NULL;

	Motion_Core::Segment::Timer::Timer_Item *item =
	&Motion_Core::Segment::Timer::Buffer::_buffer[Motion_Core::Segment::Timer::Buffer::_tail];

	
	return item;

}

/*
Returns a pointer to the current buffer head object for writing (DOES move the head)
*/
Motion_Core::Segment::Timer::Timer_Item *Motion_Core::Segment::Timer::Buffer::Write()
{
	if (Motion_Core::Segment::Timer::Buffer::_full == 1)
	return NULL;

	Motion_Core::Segment::Timer::Timer_Item *item = &Motion_Core::Segment::Timer::Buffer::_buffer[Motion_Core::Segment::Timer::Buffer::_head++];
	
	//Are we wrapping?
	if (Motion_Core::Segment::Timer::Buffer::_head == Motion_Core::Segment::Timer::BUFFER_SIZE)
	Motion_Core::Segment::Timer::Buffer::_head = 0;
	
	//Have we gotten back to the tail?
	if (Motion_Core::Segment::Timer::Buffer::_head == Motion_Core::Segment::Timer::Buffer::_tail)
	{
		Motion_Core::Segment::Timer::Buffer::_full = 1;
	}
		
	return item;
}

void Motion_Core::Segment::Timer::Buffer::Reset()
{
	Motion_Core::Segment::Timer::Buffer::_tail = 0;
	Motion_Core::Segment::Timer::Buffer::_head = 0;
	Motion_Core::Segment::Timer::Buffer::_full = 0;
}