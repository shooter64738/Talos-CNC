#include "c_planner_compute_block.h"
#include "c_segment_arbitrator.h"
#include "..\c_planner.h"
#include "c_segment_timer_bresenham.h"
#include "..\c_system.h"

Motion_Core::Planner::Block_Item::Block_Item()
{

}

Motion_Core::Planner::Block_Item::~Block_Item()
{

}

Motion_Core::Planner::Block_Item Motion_Core::Planner::Buffer::_buffer[Motion_Core::Planner::BUFFER_SIZE];
int16_t Motion_Core::Planner::Buffer::_tail = 0;
int16_t Motion_Core::Planner::Buffer::_head = 1;

/*
Returns a pointer to the current buffer tail object for reading
*/
Motion_Core::Planner::Block_Item *Motion_Core::Planner::Buffer::Read()
{
	if (Motion_Core::Planner::Buffer::_tail == BUFFER_SIZE)
		Motion_Core::Planner::Buffer::_tail = -1;

	if (Motion_Core::Planner::Buffer::_tail == Motion_Core::Planner::Buffer::_head)
		return NULL;

	return &_buffer[++Motion_Core::Planner::Buffer::_tail];
}

/*
Returns a pointer to the current buffer header object for writing
*/
Motion_Core::Planner::Block_Item *Motion_Core::Planner::Buffer::Write()
{

	if (Motion_Core::Planner::Buffer::_head == BUFFER_SIZE)
		Motion_Core::Planner::Buffer::_head = 0;

	if (Motion_Core::Planner::Buffer::_head == Motion_Core::Planner::Buffer::_tail)
		return NULL;

	return &_buffer[Motion_Core::Planner::Buffer::_head++];
}

void Motion_Core::Planner::Buffer::Reset()
{
	Motion_Core::Planner::Buffer::_tail = 0;
	Motion_Core::Planner::Buffer::_head = 1;
}

