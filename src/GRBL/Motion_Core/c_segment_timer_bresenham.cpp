#include "c_segment_timer_bresenham.h"
#include "c_segment_timer_item.h"




Motion_Core::Segment::Bresenham::Bresenham_Item::Bresenham_Item()
{
}
Motion_Core::Segment::Bresenham::Bresenham_Item::~Bresenham_Item()
{
}


Motion_Core::Segment::Bresenham::Bresenham_Item Motion_Core::Segment::Bresenham::Buffer::_buffer[Motion_Core::Segment::Timer::BUFFER_SIZE - 1];
int8_t Motion_Core::Segment::Bresenham::Buffer::_tail = 0;
int8_t Motion_Core::Segment::Bresenham::Buffer::_head = 0;
uint8_t Motion_Core::Segment::Bresenham::Buffer::_full = 0;

/*
Returns a pointer to the current buffer tail object for reading
*/
Motion_Core::Segment::Bresenham::Bresenham_Item *Motion_Core::Segment::Bresenham::Buffer::Read()
{
	//If buffer is not full and tail==head, its empty
	if (Motion_Core::Segment::Bresenham::Buffer::_full == 0 && Motion_Core::Segment::Bresenham::Buffer::_head == Motion_Core::Segment::Bresenham::Buffer::_tail)
		return NULL;

	Motion_Core::Segment::Bresenham::Bresenham_Item *item = &Motion_Core::Segment::Bresenham::Buffer::_buffer[Motion_Core::Segment::Bresenham::Buffer::_tail++];

	//Are we wrapping?
	if (Motion_Core::Segment::Bresenham::Buffer::_tail == Motion_Core::Segment::Timer::BUFFER_SIZE - 1)
		Motion_Core::Segment::Bresenham::Buffer::_tail = 0;
	//If we read an item we arent full now.
	Motion_Core::Segment::Bresenham::Buffer::_full = 0;

	return item;
}

void Motion_Core::Segment::Bresenham::Buffer::Advance()
{
	//If buffer is not full and tail==head, its empty
	if (Motion_Core::Segment::Bresenham::Buffer::_full == 0 && Motion_Core::Segment::Bresenham::Buffer::_head == Motion_Core::Segment::Bresenham::Buffer::_tail)
		return;
	Motion_Core::Segment::Bresenham::Buffer::_tail++;

}

/*
Returns a pointer to the current buffer tail object for reading (DOES NOT move the tail)
*/
Motion_Core::Segment::Bresenham::Bresenham_Item *Motion_Core::Segment::Bresenham::Buffer::Current()
{
	//If buffer is not full and tail==head, its empty
	if (Motion_Core::Segment::Bresenham::Buffer::_full == 0 && Motion_Core::Segment::Bresenham::Buffer::_head == Motion_Core::Segment::Bresenham::Buffer::_tail)
		return NULL;

	return &Motion_Core::Segment::Bresenham::Buffer::_buffer[Motion_Core::Segment::Bresenham::Buffer::_tail];
}

/*
Returns a pointer to the current buffer head object for writing
*/
Motion_Core::Segment::Bresenham::Bresenham_Item *Motion_Core::Segment::Bresenham::Buffer::Write()
{
	if (Motion_Core::Segment::Bresenham::Buffer::_full == 1)
		return NULL;

	Motion_Core::Segment::Bresenham::Bresenham_Item *item = &Motion_Core::Segment::Bresenham::Buffer::_buffer[Motion_Core::Segment::Bresenham::Buffer::_head++];

	//Are we wrapping?
	if (Motion_Core::Segment::Bresenham::Buffer::_head == Motion_Core::Segment::Timer::BUFFER_SIZE - 1)
		Motion_Core::Segment::Bresenham::Buffer::_head = 0;

	//Have we gotten back to the tail?
	if (Motion_Core::Segment::Bresenham::Buffer::_head == Motion_Core::Segment::Bresenham::Buffer::_tail)
		Motion_Core::Segment::Bresenham::Buffer::_full = 1;

	return item;
}

void Motion_Core::Segment::Bresenham::Buffer::Reset()
{
	Motion_Core::Segment::Bresenham::Buffer::_tail = 0;
	Motion_Core::Segment::Bresenham::Buffer::_head = 0;
	Motion_Core::Segment::Bresenham::Buffer::_full = 0;
}