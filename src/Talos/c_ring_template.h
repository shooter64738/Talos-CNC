#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>

template <typename TN>
class  c_ring_buffer
{
public:

	c_ring_buffer()
	{}

	c_ring_buffer(TN* pointer, uint16_t buf_size)
	{
		initialize(pointer, buf_size);
	}

	bool initialize(TN* pointer, uint16_t buf_size)
	{
		if (pointer == NULL)
			return false;

		this->_storage_pointer = pointer;
		this->_buffer_size = buf_size;
		this->reset();
		return true;
	}

	void attach(TN* pointer, uint16_t buf_size)
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
		this->_last_read = 0;
		this->_last_write = 0;
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

	TN* peek(uint16_t position)
	{
		return (this->_storage_pointer + position);
	}

	void shift(int16_t shift_size)
	{
		this->_tail += shift_size;
	}
	uint16_t count()
	{
		return _data_size;
	}
	//Return the specified type
	TN* get()
	{
		//caller should check has_data before calling this. 

		TN* data = (this->_storage_pointer + this->_tail);
		_move_tail();

		return data;
	}

	//return the item that was at the tail NOW
	TN* cur(bool* last)
	{
		*last = true;

		if (!this->has_data())
			return NULL;

		if (_my_tail(1) < this->_head)
			* last = false;

		return (this->_storage_pointer + this->_last_read);
	}

	//return the item that was at the tail BEFORE the last get
	TN* cur_tail(bool* last)
	{
		*last = true;
		if (!this->has_data())
			return NULL;

		if (_my_tail(1) < this->_head)
			* last = false;

		return (this->_storage_pointer + this->_last_read);
	}

	//return the item that was at the head BEFORE the last put
	TN* cur_head(bool* last)
	{
		//this one will be the 'last record' because it is at head position
		*last = true;
		if (!this->has_data())
			return NULL;

		return (this->_storage_pointer + this->_last_write);
	}

	TN* step_rev()
	{
		uint16_t index = 0;

		//first time
		if (_stepper < 0)
			index = _last_write + (++_stepper);
		else
			index = _last_write - (++_stepper);


		if (index < this->_tail)
		{
			_stepper = -1;
			return NULL;
		}

		if (index < 0)
		{
			index = _buffer_size - 1;
		}

		return this->_storage_pointer + index;
	}

	TN* step_fwd()
	{
		uint16_t index = 0;

		//first time
		if (_stepper < 0)
			index = _last_read + (++_stepper);
		else
			index = _last_read + (++_stepper);


		if (index > this->_head)
		{
			_stepper = -1;
			return NULL;
		}

		if (index > _buffer_size)
		{
			index = 0;
		}

		return this->_storage_pointer + index;
	}

	void step_rst()
	{
		_stepper = -1;
	}

	TN* writer_for_insert()
	{
		//caller should check has_data before calling this.

		//get the a handle for the object at the head position
		this->_newest = this->_head;
		TN* data = &this->_storage_pointer[this->_head];

		return data;
	}

	TN* writer_for_last_added()
	{

		//get the a handle for the object at the head position
		TN* data = &this->_storage_pointer[this->_newest];

		return data;
	}

	void advance()
	{
		_move_head();
	}

	//Add as the type, but copied from a byte stream
	uint8_t put(char* data)
	{
		//caller should check for full before calling this
		*(this->_storage_pointer + this->_head) = data;
		_move_head();

		return 1;
	}

	//Add as the type
	uint8_t put(TN data)
	{
		//caller should check for full before calling this
		*(this->_storage_pointer + this->_head) = data;
		_move_head();

		return 1;
	}

	void _move_tail()
	{
		_data_size--;
		this->_last_read = this->_tail;
		//since we are pulling a byte off the buffer, it can not be full
		this->_full = false;
		this->_tail++;
		//if we are at the size of the buffer, wrap back to zero
		if (this->_tail == this->_buffer_size)
		{
			this->_tail = 0;
		}
	}

	void _move_head()
	{
		_data_size++;
		this->_last_write = this->_head;

		this->_head++;
		//if we are at the size of the buffer, wrap back to zero
		if (this->_head == this->_buffer_size)
		{
			this->_head = 0;
		}
		//if head is equal to tail after a write, we are full
		if (this->_head == this->_tail)
			this->_full = true;
	}

	uint16_t _my_tail(int16_t val)
	{
		//I wouldnt recommend sending in a value larger than the buffer... 
		if (this->_tail + val >= this->_buffer_size)
		{
			return val -= (this->_tail + val) - this->_buffer_size;
		}
		return val;
	}

public:

	TN* _storage_pointer = NULL;
	bool _full = false;
	uint16_t _buffer_size = 0;
	uint16_t _head = 0;
	uint16_t _tail = 0;
	uint16_t _last_read = 0;
	uint16_t _last_write = 0;
	uint16_t _peek_stepper = 0;
	int16_t _stepper = 0;
	uint16_t _data_size = 0;
};

