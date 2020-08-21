/*
*  NGC_Block.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef NGC_LINE_PROCESSOR_H
#define NGC_LINE_PROCESSOR_H

#include <stdint.h>
#include <ctype.h>

#include "../c_ring_template.h"
//#include "NGC_Block.h"
#include "NGC_Errors.h"
#include "_ngc_errors_interpreter.h"
#include "_ngc_block_struct.h"


#define MAX_EXPRESSION_OPS 7

namespace NGC_RS274
{
	class LineProcessor
	{

	public:
		static int last_read_position;
		static uint8_t initialize();
		static uint32_t  _process_buffer(char * buffer, s_ngc_block * block, uint8_t buff_len);

	private:

		enum class e_value_class_types
		{
			UnSpecified = 0,
			Expression = 1,
			Parameter = 2,
			Unary = 5,
			Numeric = 6,
			NumericParametersNotAvailable = 7,
			NamedParametersNotAvailable = 8,
			ClosingBracket = 9,
			NumericParameterAssign = 10,
			NamedParameterAssign = 11
		};
		enum class e_expression_operator_class_types
		{
			NoOperation = 0,
			Division = 1,
			Modulus = 2,
			Exponent = 3,
			Multiplication = 4,
			And = 5,
			Or = 6,
			Ex_Or = 7,
			Subtract = 8,
			Non_Ex_Or = 9,
			Addition = 10,
			ClosingBracket = 11,
			_lt = 12,
			_eq = 13,
			_ne = 14,
			_le = 15,
			_ge = 16,
			_gt = 17
		};
		enum class e_unary_operator_class_types
		{
			NoOperation = 0,
			Abs = 1,
			Acos = 2,
			Asin = 3,
			Atan = 4,
			Cos = 5,
			Exp = 6,
			Fix = 7,
			Fup = 8,
			Ln = 9,
			Round = 10,
			Sin = 11,
			Sqrt = 12,
			Tan = 13
		};
	
		static uint32_t  _read_as_word(char * buffer, int * read_pos, char word, float * word_value, bool * has_decimal);
		static uint32_t  _read_as_class_type(char * buffer, int * read_pos, float * word_value, bool * has_decimal);
		static uint32_t  __read_class_numeric(char * buffer, int * read_pos, float * read_value, bool * has_decimal);
		static uint32_t  __read_class_expression(char * buffer, int * read_pos, float * read_value, bool * has_decimal);
		static uint32_t  __read_class_parameter(char * buffer, int * read_pos, float * read_value, bool * has_decimal);
		static uint32_t  ___read_integer_value(char * line, int * counter, int * integer_value, bool * has_decimal);
		static uint32_t  ___read_class_named_parameter(char * buffer, int * read_pos, float * read_value, bool * has_decimal);
		static e_value_class_types __get_value_class(char * buffer, int * read_pos, float * word_value, bool * has_decimal);
		static uint32_t  __read_class_unary(char * buffer, int * read_pos, float * read_value, bool * has_decimal);
		static uint32_t  ____get_named_parameter_name(char * buffer, int * read_pos, char * parameter_name);
		static uint32_t  __get_unary_operator_class
		(char * buffer, int * read_pos, e_unary_operator_class_types * operator_class);
		static uint32_t  __get_expression_operator_class
		(char * buffer, int * read_pos, e_expression_operator_class_types * operator_class);
		static int __get_operator_precedence(e_expression_operator_class_types operator_class);
		static uint32_t  ___execute_binary(float *left, e_expression_operator_class_types operator_class, float *right);
		static uint32_t  ___execute_atan(char * line, int * counter, float * double_ptr, bool * has_decimal);
		static uint32_t  ___execute_unary(float * double_ptr, e_unary_operator_class_types operation);
	};
};
#endif