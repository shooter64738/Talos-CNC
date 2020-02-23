#include "c_segment_timer_item.h"
#include <stddef.h>

void Motion_Core::Segment::Timer::Timer_Item::Reset()
{
	this->bresenham_in_item = NULL;
	this->flag.reset();
	this->line_number = 0;
	this->sequence = 0;
	this->steps_to_execute_in_this_segment = 0;
	this->timer_delay_value = 0;
	this->timer_prescaler = 0;
}

Motion_Core::Segment::Timer::Timer_Item::Timer_Item()
{
}
Motion_Core::Segment::Timer::Timer_Item::~Timer_Item()
{
}

Motion_Core::Segment::Timer::Timer_Item Motion_Core::Segment::Timer::Buffer::_buffer[Motion_Core::Segment::Timer::BUFFER_SIZE];
int16_t Motion_Core::Segment::Timer::Buffer::_tail = 0;
int16_t Motion_Core::Segment::Timer::Buffer::_head = 0;
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
	
	////If tail==head, we dont have a new one to give right now.
	//if (Motion_Core::Segment::Timer::Buffer::_head == Motion_Core::Segment::Timer::Buffer::_tail)
	//return NULL;

	Motion_Core::Segment::Timer::Timer_Item *item =
	&Motion_Core::Segment::Timer::Buffer::_buffer[Motion_Core::Segment::Timer::Buffer::_tail];

	
	return item;

}

/*
Returns a pointer to the current buffer head object for writing (DOES move the head)
*/
Motion_Core::Segment::Timer::Timer_Item *Motion_Core::Segment::Timer::Buffer::Writex()
{
	if (Motion_Core::Segment::Timer::Buffer::_full == 1)
	return NULL;

	Motion_Core::Segment::Timer::Timer_Item *item =
	&Motion_Core::Segment::Timer::Buffer::_buffer[Motion_Core::Segment::Timer::Buffer::_head];
	//clear all segment execution flags. This is a ring buffer and we dont want old values in there. 
	item->flag.reset();
	Motion_Core::Segment::Timer::Buffer::_head++;
	
	//Are we wrapping?
	if (Motion_Core::Segment::Timer::Buffer::_head == Motion_Core::Segment::Timer::BUFFER_SIZE)
	Motion_Core::Segment::Timer::Buffer::_head = 0;
	
	//Have we gotten back to the tail?
	if (Motion_Core::Segment::Timer::Buffer::_head == Motion_Core::Segment::Timer::Buffer::_tail)
	{
		Motion_Core::Segment::Timer::Buffer::_full = 1;
	}
	//item->Reset(); these are reset when they are used.
	return item;
}

int16_t Motion_Core::Segment::Timer::Buffer::WriteableHead()
{
	if (Motion_Core::Segment::Timer::Buffer::_full == 1)
	return -1;

	int16_t item = Motion_Core::Segment::Timer::Buffer::_head++;
	
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