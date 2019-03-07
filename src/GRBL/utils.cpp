#include "utils.h"
#include "c_system.h"
#include "all_includes.h"
/*
 * utils.cpp
 *
 * Created: 3/6/2019 3:39:20 PM
 *  Author: jeff_d
 */ 

 
 uint8_t utils::read_float(char *line, uint8_t *char_counter, float *float_ptr)
 {
	 char *ptr = line + *char_counter;
	 unsigned char c;

	 // Grab first character and increment pointer. No spaces assumed in line.
	 c = *ptr++;

	 // Capture initial positive/minus character
	 bool isnegative = false;
	 if (c == '-')
	 {
		 isnegative = true;
		 c = *ptr++;
	 }
	 else if (c == '+')
	 {
		 c = *ptr++;
	 }

	 // Extract number into fast integer. Track decimal in terms of exponent value.
	 uint32_t intval = 0;
	 int8_t exp = 0;
	 uint8_t ndigit = 0;
	 bool isdecimal = false;
	 while (1)
	 {
		 c -= '0';
		 if (c <= 9)
		 {
			 ndigit++;
			 if (ndigit <= MAX_INT_DIGITS)
			 {
				 if (isdecimal)
				 {
					 exp--;
				 }
				 intval = (((intval << 2) + intval) << 1) + c; // intval*10 + c
			 }
			 else
			 {
				 if (!(isdecimal))
				 {
					 exp++;
				 }  // Drop overflow digits
			 }
		 }
		 else if (c == (('.' - '0') & 0xff) && !(isdecimal))
		 {
			 isdecimal = true;
		 }
		 else
		 {
			 break;
		 }
		 c = *ptr++;
	 }

	 // Return if no digits have been read.
	 if (!ndigit)
	 {
		 return (false);
	 };

	 // Convert integer into floating point.
	 float fval;
	 fval = (float)intval;

	 // Apply decimal. Should perform no more than two floating point multiplications for the
	 // expected range of E0 to E-4.
	 if (fval != 0)
	 {
		 while (exp <= -2)
		 {
			 fval *= 0.01;
			 exp += 2;
		 }
		 if (exp < 0)
		 {
			 fval *= 0.1;
		 }
		 else if (exp > 0)
		 {
			 do
			 {
				 fval *= 10.0;
			 }
			 while (--exp > 0);
		 }
	 }

	 // Assign floating point value with correct sign.
	 if (isnegative)
	 {
		 *float_ptr = -fval;
	 }
	 else
	 {
		 *float_ptr = fval;
	 }

	 *char_counter = ptr - line - 1; // Set char_counter to next statement

	 return (true);
 }

 // Non-blocking delay function used for general operation and suspend features.
 void utils::delay_sec(float seconds, uint8_t mode)
 {
	 uint16_t i = ceil(1000 / DWELL_TIME_STEP * seconds);
	 while (i-- > 0)
	 {
		 if (c_system::sys.abort)
		 {
			 return;
		 }
		 if (mode == DELAY_MODE_DWELL)
		 {
			 c_protocol::protocol_execute_realtime();
		 }
		 else
		 { // DELAY_MODE_SYS_SUSPEND
			 // Execute rt_system() only to avoid nesting suspend loops.
			 c_protocol::protocol_exec_rt_system();
			 if (c_system::sys.suspend & SUSPEND_RESTART_RETRACT)
			 {
				 return;
			 } // Bail, if safety door reopens.
		 }
		 _delay_ms(DWELL_TIME_STEP); // Delay DWELL_TIME_STEP increment
	 }
 }

 // Delays variable defined milliseconds. Compiler compatibility fix for _delay_ms(),
 // which only accepts constants in future compiler releases.
 void utils::delay_ms(uint16_t ms)
 {
	 while (ms--)
	 {
		 _delay_ms(1);
	 }
 }

 // Delays variable defined microseconds. Compiler compatibility fix for _delay_us(),
 // which only accepts constants in future compiler releases. Written to perform more
 // efficiently with larger delays, as the counter adds parasitic time in each iteration.
 void utils::delay_us(uint32_t us)
 {
	 while (us)
	 {
		 if (us < 10)
		 {
			 _delay_us(1);
			 us--;
		 }
		 else if (us < 100)
		 {
			 _delay_us(10);
			 us -= 10;
		 }
		 else if (us < 1000)
		 {
			 _delay_us(100);
			 us -= 100;
		 }
		 else
		 {
			 _delay_ms(1);
			 us -= 1000;
		 }
	 }
 }

 // Simple hypotenuse computation function.
 float utils::hypot_f(float x, float y)
 {
	 return (sqrt(x * x + y * y));
 }

 float utils::convert_delta_vector_to_unit_vector(float *vector)
 {
	 uint8_t idx;
	 float magnitude = 0.0;
	 for (idx = 0; idx < N_AXIS; idx++)
	 {
		 if (vector[idx] != 0.0)
		 {
			 magnitude += vector[idx] * vector[idx];
		 }
	 }
	 magnitude = sqrt(magnitude);
	 float inv_magnitude = 1.0 / magnitude;
	 for (idx = 0; idx < N_AXIS; idx++)
	 {
		 vector[idx] *= inv_magnitude;
	 }
	 return (magnitude);
 }

 float utils::limit_value_by_axis_maximum(float *max_value, float *unit_vec)
 {
	 uint8_t idx;
	 float limit_value = SOME_LARGE_VALUE;
	 for (idx = 0; idx < N_AXIS; idx++)
	 {
		 if (unit_vec[idx] != 0)
		 {  // Avoid divide by zero.
			 limit_value = min(limit_value, fabs(max_value[idx] / unit_vec[idx]));
		 }
	 }
	 return (limit_value);
 }