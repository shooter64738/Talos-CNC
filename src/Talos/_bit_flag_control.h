#include <stdint.h>
#include "_bit_manipulation.h"


#ifndef BIT_FLAG_CONTROL_H
#define BIT_FLAG_CONTROL_H

template <typename TN> //Provides a centralized bit flag control system
struct s_bit_flag_controller
{
	//template <typename ET> //Provides a centralized bit flag control system
	bool get(TN get_value)
	{
		return (bool)(BitGet(_flag, (int)get_value));
	};

	//template <typename ET> //Provides a centralized bit flag control system
	bool get_clr(TN get_value)
	{
		bool ret = (BitGet(_flag, (int)get_value));
		clear(get_value);
		return ret;
	};

	//template <typename ET> //Provides a centralized bit flag control system
	void flip(TN flip_value)
	{
		BitFlp(_flag, (int)flip_value);
	};

	//template <typename ET> //Provides a centralized bit flag control system
	void set(TN set_value)
	{
		BitSet_(_flag, (int)set_value);
	};

	//template <typename ET> //Provides a centralized bit flag control system
	void set(bool bit_value, TN bit_num)
	{
		if (bit_value)
			BitSet_(_flag, (int)bit_num);
		else
			BitClr_(_flag, (int)bit_num);
	};

	//template <typename ET> //Provides a centralized bit flag control system
	void clear(TN clear_value)
	{
		BitClr_(_flag, (int)clear_value);
	};

	void reset()
	{
		_flag = 0;
	};
	//typing this as uint32, but would rather find the enums base type and type it as that.
	volatile uint32_t _flag;// = 0;//because this can be accessed by interrupts, i am making it volatile
};


#define low_bits 0x0000FFFF
#define high_bits 0xFFFF0000

template <typename TN>//Uses 1/2 the bits in the flag to determine state and compares to the other half
struct s_bit_flag_controller_ex
{
	uint8_t half_bits = 0;
	s_bit_flag_controller_ex()
	{
		half_bits = __bit_offset_size();
	}

	uint8_t __bit_offset_size()
	{
		return (sizeof(uint8_t) * 8) / 2;
	}

	/*
	Specify only the LOW bit. The high bit will be calculated.
	Returns true if both the hi and lo bits are set. Returns false otherwise.
	*/
	bool get(int get_value)
	{
		uint8_t low_bit = get_value;
		uint8_t high_bit = get_value + half_bits;

		if (get_value > half_bits)
		{
			low_bit = get_value - half_bits;
			high_bit = get_value;
		}

		//return true or false if BOTH the high and low bits are set
		return ((bool)(BitGet(_flag, low_bit)) & (bool)(BitGet(_flag, high_bit)));

	};

	/*
	Specify only the LOW bit. The high bit will be calculated.
	Returns true if the hi and lo bits are different. Returns false otherwise.
	*/
	bool get_inv(int get_value)
	{
		uint8_t low_bit = get_value;
		uint8_t high_bit = get_value + half_bits;

		if (get_value > half_bits)
		{
			low_bit = get_value - half_bits;
			high_bit = get_value;
		}

		//return true or false if the high and low bits are different
		return ((bool)(BitGet(_flag, low_bit)) != (bool)(BitGet(_flag, high_bit)));

	};

	bool get_low(int get_value)
	{
		return (bool)(BitGet(_flag & low_bits, get_value));
	};

	//Specify only the LOW bit. The high bit will be calculated.
	bool get_high(int get_value)
	{
		return (bool)(BitGet(_flag & high_bits, get_value + half_bits));
	};

	bool get_clr_low(int get_value)
	{
		bool ret = (BitGet(_flag & low_bits, get_value));
		clear_low(get_value);
		return ret;
	};

	//Specify only the LOW bit. The high bit will be calculated.
	bool get_clr_high(int get_value)
	{
		bool ret = (BitGet(_flag & high_bits, get_value + half_bits));
		clear_high(get_value);
		return ret;
	};

	void flip_low(int flip_value)
	{
		BitFlp(_flag & low_bits, flip_value);
	};

	//Specify only the LOW bit. The high bit will be calculated.
	void flip_high(int flip_value)
	{
		BitFlp(_flag & high_bits, flip_value + half_bits);
	};

	void set_low(int set_value)
	{
		BitSet_(_flag, set_value);
	};

	//Specify only the LOW bit. The high bit will be calculated.
	void set_high(int set_value)
	{
		BitSet_(_flag, set_value + half_bits);
	};

	void set_low(bool bit_value, int bit_num)
	{
		if (bit_value)
			BitSet_(_flag, bit_num);
		else
			BitClr_(_flag, bit_num);
	};

	//Specify only the LOW bit. The high bit will be calculated.
	void set_high(bool bit_value, int bit_num)
	{
		if (bit_value)
			BitSet_(_flag, bit_num + half_bits);
		else
			BitClr_(_flag, bit_num + half_bits);
	};

	void clear_low(int clear_value)
	{
		BitClr_(_flag, clear_value);
	};

	//Specify only the LOW bit. The high bit will be calculated.
	void clear_high(int clear_value)
	{
		BitClr_(_flag, clear_value + half_bits);
	};

	void reset()
	{
		_flag = 0;
	};

	volatile TN _flag = 0;//because this can be accessed by interrupts, i am making it volatile
};

#endif