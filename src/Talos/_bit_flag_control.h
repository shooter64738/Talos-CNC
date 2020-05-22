#include <stdint.h>
#include "_bit_manipulation.h"


#ifndef BIT_FLAG_CONTROL_H
#define BIT_FLAG_CONTROL_H

template <typename TN> //Provides a centralized bit flag control system
struct s_bit_flag_controller
{
	bool get(int get_value)
	{
		return (bool)(BitGet(_flag, get_value));
	};

	bool get_clr(int get_value)
	{
		bool ret = (BitGet(_flag, get_value));
		clear(get_value);
		return ret;
	};

	void flip(int flip_value)
	{
		BitFlp(_flag, flip_value);
	};

	void set(int set_value)
	{
		BitSet_(_flag, set_value);
	};

	void set(bool bit_value, int bit_num)
	{
		if (bit_value)
			BitSet_(_flag, bit_num);
		else
			BitClr_(_flag, bit_num);
	};

	void clear(int clear_value)
	{
		BitClr_(_flag, clear_value);
	};

	void reset()
	{
		_flag = 0;
	};

	volatile TN _flag;//because this can be accessed by interrupts, i am making it volatile
};
#define low_bits 0x0000FFFF
#define high_bits 0xFFFF0000
#define bit_offset_size 15
template <typename TN>//Uses 1/2 the bits in the flag to determine state and compares to the other half
struct s_bit_flag_controller_ex
{
	bool get(int get_value)
	{
		uint8_t low_bit = 0;
		uint8_t high_bit = 0;

		if (get_value > sizeof(TN) / 2)
		{
			low_bit = get_value - (sizeof(TN) / 2);
			high_bit = get_value;
		}

		//return true or false if BOTH the high and low bits are set
		return ((bool)(BitGet(_flag, low_bit)) & (bool)(BitGet(_flag, high_bit)));

	};

	bool get_low(int get_value)
	{
		return (bool)(BitGet(_flag & low_bits, get_value));
	};

	bool get_high(int get_value)
	{
		return (bool)(BitGet(_flag & high_bits, get_value+ bit_offset_size));
	};

	bool get_clr_low(int get_value)
	{
		bool ret = (BitGet(_flag & low_bits, get_value));
		clear_low(get_value);
		return ret;
	};
	
	bool get_clr_high(int get_value)
	{
		bool ret = (BitGet(_flag & high_bits, get_value + bit_offset_size));
		clear_high(get_value);
		return ret;
	};

	void flip_low(int flip_value)
	{
		BitFlp(_flag & low_bits, flip_value);
	};

	void flip_high(int flip_value)
	{
		BitFlp(_flag & high_bits, flip_value + bit_offset_size);
	};

	void set_low(int set_value)
	{
		BitSet_(_flag, set_value);
	};

	void set_high(int set_value)
	{
		BitSet_(_flag, set_value + bit_offset_size);
	};

	void set_low(bool bit_value, int bit_num)
	{
		if (bit_value)
			BitSet_(_flag, bit_num);
		else
			BitClr_(_flag, bit_num);
	};

	void set_high(bool bit_value, int bit_num)
	{
		if (bit_value)
			BitSet_(_flag, bit_num+bit_offset_size);
		else
			BitClr_(_flag, bit_num + bit_offset_size);
	};

	void clear_low(int clear_value)
	{
		BitClr_(_flag, clear_value);
	};

	void clear_high(int clear_value)
	{
		BitClr_(_flag, clear_value+bit_offset_size);
	};

	void reset()
	{
		_flag = 0;
	};

	volatile TN _flag;//because this can be accessed by interrupts, i am making it volatile
};

#endif