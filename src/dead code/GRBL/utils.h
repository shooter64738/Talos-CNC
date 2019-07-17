/*
* utils.h
*
* Created: 3/6/2019 3:30:03 PM
*  Author: jeff_d
*/


#ifndef UTILS_H_
#define UTILS_H_

//#include <util/delay.h>
#include <stdint.h>
#include <math.h>
#include "c_protocol.h"
#define MAX_INT_DIGITS 8 // Maximum number of digits in int32 (and float)
class utils
{
public:
	static uint8_t read_float(char *line, uint8_t *char_counter, float *float_ptr);

	// Non-blocking delay function used for general operation and suspend features.
	static void delay_sec(float seconds, uint8_t mode);

	// Delays variable-defined milliseconds. Compiler compatibility fix for _delay_ms().
	static void delay_ms(uint16_t ms);

	// Delays variable-defined microseconds. Compiler compatibility fix for _delay_us().
	static void delay_us(uint32_t us);

	// Computes hypotenuse, avoiding avr-gcc's bloated version and the extra error checking.
	static float hypot_f(float x, float y);

	static float convert_delta_vector_to_unit_vector(float *vector);
	static float limit_value_by_axis_maximum(float *max_value, float *unit_vec);

};



#endif /* UTILS_H_ */