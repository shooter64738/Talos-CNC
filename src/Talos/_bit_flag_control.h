#include <stdint.h>
#include "_bit_manipulation.h"


#ifndef BIT_FLAG_CONTROL_H
#define BIT_FLAG_CONTROL_H

template <typename TN>
struct s_bit_flag_controller
{
	bool get(int get_value)
	{
		return (BitGet(_flag, get_value));
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

#endif