#include "c_segment_timer_bresenham.h"
#include "c_segment_timer_item.h"

Motion_Core::Segment::Bresenham::Bresenham_Item Motion_Core::Segment::Bresenham::Buffer::_buffer[Motion_Core::Segment::Timer::BUFFER_SIZE -1];
int16_t Motion_Core::Segment::Bresenham::Buffer::_tail = 0;
int16_t Motion_Core::Segment::Bresenham::Buffer::_head = 1;


Motion_Core::Segment::Bresenham::Bresenham_Item::Bresenham_Item()
{
}
Motion_Core::Segment::Bresenham::Bresenham_Item::~Bresenham_Item()
{
}
//Motion_Core::Segment::Bresenham_Item::Bresenham_Item()
//{
//}


/*
Returns a pointer to the current buffer tail object for reading
*/
Motion_Core::Segment::Bresenham::Bresenham_Item *Motion_Core::Segment::Bresenham::Buffer::Read()
{
	if (Motion_Core::Segment::Bresenham::Buffer::_tail == Motion_Core::Segment::Timer::BUFFER_SIZE - 1)
		Motion_Core::Segment::Bresenham::Buffer::_tail = -1;

	if (Motion_Core::Segment::Bresenham::Buffer::_tail == Motion_Core::Segment::Bresenham::Buffer::_head)
		return NULL;

	return &Motion_Core::Segment::Bresenham::Buffer::_buffer[++Motion_Core::Segment::Bresenham::Buffer::_tail];
}

/*
Returns a pointer to the current buffer header object for writing
*/
Motion_Core::Segment::Bresenham::Bresenham_Item *Motion_Core::Segment::Bresenham::Buffer::Write()
{

	if (Motion_Core::Segment::Bresenham::Buffer::_head == Motion_Core::Segment::Timer::BUFFER_SIZE)
		Motion_Core::Segment::Bresenham::Buffer::_head = 0;

	if (Motion_Core::Segment::Bresenham::Buffer::_head == Motion_Core::Segment::Bresenham::Buffer::_tail)
		return NULL;

	return &_buffer[Motion_Core::Segment::Bresenham::Buffer::_head++];
}

void Motion_Core::Segment::Bresenham::Buffer::Reset()
{
	Motion_Core::Segment::Bresenham::Buffer::_tail = 0;
	Motion_Core::Segment::Bresenham::Buffer::_head = 1;
}