#include "c_segment.h"
#include <stddef.h>

Motion_Core::Segment::Item::Item()
{
}
Motion_Core::Segment::Item::~Item()
{
}
Motion_Core::Segment::Item::Item(uint16_t n_step,uint16_t cycles_per_tick,uint8_t st_block_index,uint8_t prescaler,uint32_t line_number)
{
	this->cycles_per_tick = cycles_per_tick;
	this->line_number = line_number;
	this->n_step = n_step;
	this->prescaler = prescaler;
	this->st_block_index_4_bresenham = st_block_index;
}


Motion_Core::Segment::Item Motion_Core::Segment::Buffer::_buffer[BUFFER_SIZE];
int16_t Motion_Core::Segment::Buffer::_tail = 0;
int16_t Motion_Core::Segment::Buffer::_head = 1;

/*
Returns a pointer to the current buffer tail object for reading
*/
Motion_Core::Segment::Item *Motion_Core::Segment::Buffer::Read()
{
	if (Motion_Core::Segment::Buffer::_tail == BUFFER_SIZE)
		Motion_Core::Segment::Buffer::_tail = -1;

	if (Motion_Core::Segment::Buffer::_tail == Motion_Core::Segment::Buffer::_head)
		return NULL;

	return &_buffer[++Motion_Core::Segment::Buffer::_tail];
}

/*
Returns a pointer to the current buffer header object for writing
*/
Motion_Core::Segment::Item *Motion_Core::Segment::Buffer::Write()
{

	if (Motion_Core::Segment::Buffer::_head == BUFFER_SIZE)
		Motion_Core::Segment::Buffer::_head = 0;
	
	if (Motion_Core::Segment::Buffer::_head == Motion_Core::Segment::Buffer::_tail)
		return NULL;

	return &_buffer[Motion_Core::Segment::Buffer::_head++];
}

void Motion_Core::Segment::Buffer::Reset()
{
	Motion_Core::Segment::Buffer::_tail = 0;
	Motion_Core::Segment::Buffer::_head = 1;
}