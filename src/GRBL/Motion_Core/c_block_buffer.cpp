#include "c_block_buffer.h"
#include <stddef.h>


using namespace Motion_Core::Planner;

Block_Item Buffer::_buffer[BUFFER_SIZE];
int8_t Buffer::_tail = 0;
int8_t Buffer::_head = 0;
uint8_t Buffer::_full = 0;

/*
Returns a pointer to the current buffer tail object for reading
*/
Block_Item *Buffer::Read()
{
	//If buffer is not full and tail==head, its empty
	if (Buffer::_full == 0 && Buffer::_head == Buffer::_tail)
		return NULL;

	Block_Item *item = &Buffer::_buffer[Buffer::_tail++];

	//Are we wrapping?
	if (Buffer::_tail == BUFFER_SIZE)
		Buffer::_tail = 0;
	//If we read an item we arent full now.
	Buffer::_full = 0;

	return item;
}

void Buffer::Advance()
{
	//If buffer is not full and tail==head, its empty
	if (Buffer::_full == 0 && Buffer::_head == Buffer::_tail)
		return;
	Buffer::_tail++;

}

/*
Returns a pointer to the current buffer tail object for reading (DOES NOT move the tail)
*/
Block_Item *Buffer::Current()
{
	//If buffer is not full and tail==head, its empty
	if (Buffer::_full == 0 && Buffer::_head == Buffer::_tail)
		return NULL;

	return &Buffer::_buffer[Buffer::_tail];
}

/*
Returns a pointer to the current buffer head object for writing
*/
Block_Item *Buffer::Write()
{
	if (Buffer::_full == 1)
		return NULL;
	uint8_t Station = Buffer::_head;
	Block_Item *item = &Buffer::_buffer[Buffer::_head++];

	//Are we wrapping?
	if (Buffer::_head == BUFFER_SIZE)
		Buffer::_head = 0;

	//Have we gotten back to the tail?
	if (Buffer::_head == Buffer::_tail)
		Buffer::_full = 1;

	item->Reset();
	item->Station = Station;
	return item;
}

/*
Returns a pointer to the item most recenly added
*/
Block_Item *Buffer::Newest()
{
	uint8_t index = Buffer::_head - 1;
	if (index < 0)
		index = BUFFER_SIZE - 1;

	return &Buffer::_buffer[index];
}

/*
Returns a flag indicating if the buffer is empty
*/
uint8_t Buffer::Available()
{
	if (Buffer::_full == 0 && Buffer::_head == Buffer::_tail)
		return 0; //<--buffer empty

	return 1;
}

/*
Returns a flag indicating if the buffer is full
*/
uint8_t Buffer::Full()
{
	return Buffer::_full;
}


/*
Returns a pointer to the item most recenly added
*/
Block_Item *Buffer::Get(uint8_t From)
{
	if (From < 0)
		From = BUFFER_SIZE - 1;
	else if (From >= BUFFER_SIZE)
		From = 0;

	return &Buffer::_buffer[From];
}

uint8_t Buffer::IsLastItem()
{
	int8_t Index = Buffer::_head - 1;
	if (Index < 0)
		Index = BUFFER_SIZE - 1;
	if (Buffer::_tail == Index)
		return 1;

	return 0;
}

void Buffer::Reset()
{
	Buffer::_tail = 0;
	Buffer::_head = 0;
	Buffer::_full = 0;
}