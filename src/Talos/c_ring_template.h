#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>

template <typename TN>
class  c_ring_buffer
{

public:

	void initialize(volatile TN *pointer, uint16_t buf_size)
	{
		this->_storage_pointer = pointer;
		this->_buffer_size = buf_size;
		this->reset();
	}

	void attach(TN *pointer, uint16_t buf_size)
	{

		this->_storage_pointer = pointer;
		this->_buffer_size = buf_size;
		this->_full = false;
		this->_head = buf_size - 1;
		this->_tail = 0;
		this->_newest = 0;
	}


	void reset()
	{
		this->_full = false;
		//memset(this->_storage_pointer, 0, sizeof(TN)*this->_buffer_size);

		this->_head = 0;
		this->_tail = 0;
		this->_newest = 0;
	}

	void release()
	{
		this->reset();

		this->_storage_pointer = NULL;
		this->_buffer_size = 0;
	}

	//Return true if contains data, false if not
	bool has_data()
	{
		if ((this->_head == this->_tail) && !this->_full)
			return false;
		
		return true;
	}

	TN peek()
	{
		uint16_t peek_point = this->_tail + 1;
		if (peek_point == this->_buffer_size)
		{
			peek_point = 0;
		}
		return this->_storage_pointer[peek_point];
	}

	TN peek_step()
	{
		//caller should check has_data before calling this. 

		//if head==tail and not full, buffer is empty, no data to provide
		if ((this->_head == this->_peek_stepper) && !this->_full)
			return 0;
		
		//get the byte at the peek position
		TN data = this->_storage_pointer[this->_peek_stepper++];
		//if we are at the size of the buffer, wrap back to zero
		if (this->_peek_stepper == this->_buffer_size)
		{
			this->_peek_stepper = 0;
		}
		return data;
	}

	TN peek(uint16_t position)
	{
		return this->_storage_pointer[this->_tail];
	}
	

	void shift(int16_t shift_size)
	{
		this->_tail +=shift_size;
	}

	//Return the specified type
	TN get()
	{
		//caller should check has_data before calling this. 

		//if head==tail and not full, buffer is empty, no data to provide
		//if ((this->_head == this->_tail) && !this->_full)
		//	return 0;

		//since we are pulling a byte off the buffer, it can not be full
		this->_full = false;
		//get the byte at the tail
		//TN data = this->_storage_pointer[this->_tail];
		TN data = *(this->_storage_pointer+ this->_tail);
		//clear this byte. keeps the storage buffered 'nulled'
		//_storage_pointer[_tail] = 0;
		//increment tail
		this->_tail++;
		//if we are at the size of the buffer, wrap back to zero
		if (this->_tail == this->_buffer_size)
		{
			this->_tail = 0;
		}
		return data;
	}

	TN * get_h()
	{
		//caller should check has_data before calling this. 

		//if head==tail and not full, buffer is empty, no data to provide
		//if ((this->_head == this->_tail) && !this->_full)
		//	return 0;

		//since we are pulling a byte off the buffer, it can not be full
		this->_full = false;
		//get the byte at the tail
		TN data = &this->_storage_pointer[this->_tail];
		//clear this byte. keeps the storage buffered 'nulled'
		//_storage_pointer[_tail] = 0;
		//increment tail
		this->_tail++;
		//if we are at the size of the buffer, wrap back to zero
		if (this->_tail == this->_buffer_size)
		{
			this->_tail = 0;
		}
		return data;
	}
	
	TN * writer_for_insert()
	{
		//caller should check has_data before calling this.

		//if head==tail and not full, buffer is empty, no data to provide
		//if ((this->_head == this->_tail) && !this->_full)
		//	return 0;

		//get the a handle for the object at the head position
		this->_newest = this->_head;
		TN * data = &this->_storage_pointer[this->_head];

		//if (this->_head == this->_buffer_size)
		//{
		//	this->_head = 0;
		//}
		////if head is equal to tail after a write, we are full
		//if (this->_head == this->_tail)
		//	this->_full = true;	
		
		return data;
	}
	
	TN * writer_for_last_added()
	{
		//caller should check has_data before calling this.

		//if head==tail and not full, buffer is empty, no data to provide
		//if ((this->_head == this->_tail) && !this->_full)
		//	return 0;

		//get the a handle for the object at the head position
		TN * data = &this->_storage_pointer[this->_newest];
		
		return data;
	}
	
	void advance()
	{
		//caller should check has_data before calling this.
		this->_head++;

		if (this->_head >= this->_buffer_size)
		{
			this->_head = 0;
		}
		//if head is equal to tail after a write, we are full
		if (this->_head == this->_tail)
			this->_full = true;
	}

	//Add as the type, but copied from a byte stream
	uint8_t put(char * data)
	{
		//caller should check for full before calling this

		//buffer is full cant add
		//if (this->_full)
		//	return 0;
		//convert byte stream to type,place data at head position, increment head
		//memcpy(&this->_storage_pointer[this->_head], data, sizeof(TN));
		*(this->_storage_pointer+this->_head) = data;
		this->_head++;
		//if we are at the size of the buffer, wrap back to zero
		if (this->_head == this->_buffer_size)
		{
			this->_head = 0;
		}
		//if head is equal to tail after a write, we are full
		if (this->_head == this->_tail)
			this->_full = true;

		return 1;
	}

	//Add as the type
	uint8_t put(TN data)
	{
		//caller should check for full before calling this

		//buffer is full cant add
		//if (this->_full)
		//	return 0;

		//place the byte at the head position, increment head
		this->_newest = this->_head;
		//this->_storage_pointer[this->_head++] = data;
		*(this->_storage_pointer+this->_head) = data;
		this->_head++;
		//if we are at the size of the buffer, wrap back to zero
		if (this->_head == this->_buffer_size)
		{
			this->_head = 0;
		}
		//if head is equal to tail after a write, we are full
		if (this->_head == this->_tail)
			this->_full = true;

		return 1;
	}

	

	//uint8_t(*pntr_device_write)(uint8_t port, char byte);
	//uint8_t(*pntr_device_read)(uint8_t port, char byte);

public:

	volatile TN * _storage_pointer = NULL;
	volatile bool _full = false;
	uint16_t _buffer_size = 0;
	uint16_t _head = 0;
	uint16_t _tail = 0;
	uint16_t _newest = 0;
	uint16_t _peek_stepper = 0;
};

