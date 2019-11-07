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
#include "..\records_def.h"
#include <ctype.h>

#define MAX_EXPRESSION_OPS 7
#define NEAR_ZERO 0.0001
#define RS274NGC_MAX_PARAMETERS 5414
#define LINELEN 255
#define M_PI 3.14159265358979323846

//find FAKE_NUMERIC_PARAM_VALUE in the code to see where you need to load numeric params at.
#define FAKE_NUMERIC_PARAM_VALUE 100
#define FAKE_LOCAL_NAMED_PARAM_VALUE 200
#define FAKE_GLOBAL_NAMED_PARAM_VALUE 300


class c_line
{

public:
	enum class e_value_class_types
	{
		UnSpecified = 0,
		Expression = 1,
		Parameter = 2,
		Unary = 5,
		Numeric = 6
	};

	enum class e_parser_codes
	{
		Ok = 0,
		NumericValueMissing = 1,
		DivideByZero = 2,
		ExponentOnNegativeNonInteger = 3,
		Unknown_Op_Name_A = 4,
		Unknown_Op_Name_C = 5,
		Unknown_Op_Name_M = 6,
		Unknown_Op_Name_O = 7,
		Unknown_Op_Name_X = 8,
		Unknown_Op_Name_E = 9,
		Unknown_Op_Name_F = 10,
		Unknown_Op_Name_N = 11,
		Unknown_Op_Name_G = 12,
		Unknown_Op_Name_L = 13,
		Unknown_Op_Name_R = 14,
		Unknown_Op_Name_S = 15,
		Unknown_Op_Name_T = 16,
		ExpressionNotClosed = 17,
		UnknownOperationClass = 18,
		IllegalCaller = 19,
		ParameterNameNotClosed = 20,
		IntExpectedAtValue = 21,
		ParamaterValueOutOfRange = 22,
		UnaryMissingOpen = 23,
		MissingSlashIn_ATAN = 24,
		MissingBracketAfterSlash = 25,
		ArcCosValueOutOfRange = 26,
		ArcSinValueOutOfRange = 27,
		LogValueNegative = 28,
		SqrtValueNegative = 29

	};

	enum class e_expression_operator_class_types
	{
		NoOperation = 0,
		Division = 1,
		Modulus = 2,
		Exponent = 3,
		Multiplication = 4,
		Or = 5,
		Ex_Or = 6,
		Subtract = 7,
		Non_Ex_Or = 8,
		Addition = 9,
		ClosingBracket = 10,
		_lt = 11,
		_eq = 12,
		_ne = 13,
		_le = 14,
		_ge = 15,
		_gt = 16
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

	static void start(char * line);

private:
	static uint8_t _set_buffer_to_lower(char * buffer);
	static void _process_buffer(char * buffer);

	static void _read(char * buffer, int * read_pos, float * word_value);

	static e_parser_codes _read_class_numeric(char * buffer, int * read_pos, float * read_value);
	static e_parser_codes _read_class_expression(char * buffer, int * read_pos, float * read_value);
	static e_parser_codes _read_class_parameter(char * buffer, int * read_pos, float * read_value);
	static e_parser_codes __read_integer_value(char * line, int * counter, int * integer_value);
	static e_parser_codes __read_class_named_parameter(char * buffer, int * read_pos, float * read_value);
	static e_parser_codes __read_atan(char * line, int * counter, float * double_ptr);
	static e_value_class_types __get_value_class(char * buffer, int * read_pos, float * word_value);
	static e_parser_codes _read_class_unary(char * buffer, int * read_pos, float * read_value);
	static e_parser_codes __get_named_parameter_name(char * buffer, int * read_pos, char * parameter_name);
	static c_line::e_parser_codes __get_unary_operator_class(char * buffer, int * read_pos, e_unary_operator_class_types * operator_class);
	static e_parser_codes __get_expression_operator_class(char * buffer, int * read_pos, e_expression_operator_class_types * operator_class);
	static int __get_operator_precedence(e_expression_operator_class_types operator_class);
	static e_parser_codes __execute_binary1(float * left, e_expression_operator_class_types operator_class, float * right);
	static e_parser_codes __execute_binary2(float * left, e_expression_operator_class_types operator_class, float * right);
	static e_parser_codes __binary_operation(float *left, e_expression_operator_class_types operator_class, float *right);
	static e_parser_codes __execute_unary(float * double_ptr, e_unary_operator_class_types operation);


};

#endif