/*
*  NGC_Block.cpp - NGC_RS274 controller.
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


#include "NGC_Line_Processor.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>


void c_line::start(char * buffer)
{
	//set line data to all lower case
	if (!_set_buffer_to_lower(buffer))
	{/*return bad data in the buffer*/
	}

	_process_buffer(buffer);

}

uint8_t c_line::_set_buffer_to_lower(char * buffer)
{
	int count = 0;
	for (int i = 0; buffer[i]; i++)
	{
		//jsut for funsies. make sure there are no illegal characters in the buffer
		if (buffer[i] < 32 || buffer[i] > 127)
			return 0;

		if (buffer[i] != ' ' && buffer[i] != '\t')
		{
			buffer[count++] = tolower(buffer[i]);
		}
	}
	buffer[count] = '\0';
	return 0;
}

void c_line::_process_buffer(char * buffer)
{
	int read_pos, buff_len = 0;

	buff_len = strlen(buffer);
	read_pos = 0;

	//If the block skipper is the first character, ignore it. 
	//We process this line regardless and give it to the motion
	//controller which will determine if it should process it
	//depending on the block skip hardware switch
	read_pos = buffer[read_pos] == '/' ? read_pos++ : read_pos;

	//We could read the N word here. For nwo I am handling it as 
	//I do any other word.

	//O words are a little special. cant process those yet, but 
	//I am working on it. I think they can be called re-entrant
	//which is similar to how how linux cnc handles O words
	if (buffer[read_pos] == 'o')
		return;

	float word_value = 0.0;
	//get current word, and advance reader
	char current_word = 0;

	//And now without further delay... lets process this line!
	while (read_pos < buff_len)
	{
		word_value = 0.0;
		//get current word, and advance reader
		current_word = buffer[read_pos++];
		_read(buffer, &read_pos, &word_value);

		//Right here is where we have the word, and its address value.
		//If you want to update some type of block object with the word
		//data, do it right here. If there was an expression it would
		//have been evaluated and calculated already. Parameters would
		//have already been read. However O words are not currently
		//handled. I plan to expand that some time but not right now.
		//block_object_word_array[current_word] = word_value
	}

}

void c_line::_read(char * buffer, int * read_pos, float * word_value)
{
	e_value_class_types current_class = __get_value_class(buffer, read_pos, word_value);

	switch (current_class)
	{
	case c_line::e_value_class_types::UnSpecified:
		break;
	case c_line::e_value_class_types::Expression:
	{
		c_line::_read_class_expression(buffer, read_pos, word_value);
		break;
	}
	case c_line::e_value_class_types::Parameter:
		c_line::_read_class_parameter(buffer, read_pos, word_value);
		break;
		//case c_line::e_value_class_types::PositiveValue:
		//	break;
		//case c_line::e_value_class_types::NegativeValue:
		//	break;
	case c_line::e_value_class_types::Unary:
		_read_class_unary(buffer, read_pos, word_value);
		break;
	case c_line::e_value_class_types::Numeric:
	{
		c_line::_read_class_numeric(buffer, read_pos, word_value);
		break;
	}
	default:
		break;
	}
}

c_line::e_value_class_types c_line::__get_value_class(char * buffer, int * read_pos, float * word_value)
{
	char current_byte, next_byte;

	current_byte = buffer[*read_pos];

	if (current_byte == 0)
		return e_value_class_types::UnSpecified;

	next_byte = buffer[*read_pos + 1];

	if (current_byte == '[')
	{
		return e_value_class_types::Expression;
	}
	else if (current_byte == '#')
	{
		//re-read calssifier?
		return e_value_class_types::Parameter;
	}
	else if (current_byte == '+' && next_byte && !isdigit(next_byte) && next_byte != '.')
	{
		//re-read calssifier?
		//(*read_pos)++;
		//return e_class_types::PositiveValue;
		c_line::_read(buffer, read_pos, word_value);
	}
	else if (current_byte == '-' && next_byte && !isdigit(next_byte) && next_byte != '.')
	{
		//(*read_pos)++;
		//return e_class_types::NegativeValue;
		//*double_ptr = -*double_ptr;
		c_line::_read(buffer, read_pos, word_value);
	}
	else if ((current_byte >= 'a') && (current_byte <= 'z'))
		return e_value_class_types::Unary;
	else
	{
		return e_value_class_types::Numeric;
	}
}

c_line::e_parser_codes c_line::_read_class_unary(char * buffer, int * read_pos, float * read_value)
{
	e_parser_codes ret_code = e_parser_codes::Ok;
	e_unary_operator_class_types operation = e_unary_operator_class_types::NoOperation;

	__get_unary_operator_class(buffer, read_pos, &operation);
	if (buffer[*read_pos] != '[')
	{
		ret_code = e_parser_codes::UnaryMissingOpen;
	}
	_read_class_expression(buffer, read_pos, read_value);

	if (operation == e_unary_operator_class_types::Atan)
		ret_code = __read_atan(buffer, read_pos, read_value);
	else
		ret_code = __execute_unary(read_value, operation);
	return ret_code;
}

c_line::e_parser_codes c_line::_read_class_numeric(char * buffer, int * read_pos, float * read_value)
{
	char *start, *end, save;
	size_t after;

	start = buffer + *read_pos;

	after = strspn(start, "0123456789+-.");
	save = start[after];
	start[after] = 0;

	*read_value = strtod(start, &end);
	start[after] = save;

	if (end == start)
	{
		return e_parser_codes::NumericValueMissing;
	}

	*read_pos = end - buffer;
	return e_parser_codes::Ok;
}

c_line::e_parser_codes c_line::_read_class_expression(char * buffer, int * read_pos, float * read_value)
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	float values[MAX_EXPRESSION_OPS];
	e_expression_operator_class_types operators[MAX_EXPRESSION_OPS];
	int stack_index;

	if (buffer[*read_pos] != '[')
	{
		ret_code = e_parser_codes::IllegalCaller;
		return ret_code;
	}

	*read_pos = (*read_pos + 1);
	//read_real_value(line, counter, values, parameters);
	c_line::_read(buffer, read_pos, values);

	//read_operation(buffer, read_pos, operators);
	__get_expression_operator_class(buffer, read_pos, operators);

	stack_index = 1;
	for (; operators[0] != e_expression_operator_class_types::ClosingBracket;)
	{
		//read_real_value(buffer, read_pos, values + stack_index, parameters);
		c_line::_read(buffer, read_pos, values + stack_index);

		//read_operation(buffer, read_pos, operators + stack_index);
		__get_expression_operator_class(buffer, read_pos, operators + stack_index);

		if (__get_operator_precedence(operators[stack_index]) > __get_operator_precedence(operators[stack_index - 1]))
			stack_index++;
		else
		{                      /* precedence of latest operator is <= previous precedence */

			for (; __get_operator_precedence(operators[stack_index]) <= __get_operator_precedence(operators[stack_index - 1]);)
			{
				__binary_operation((values + stack_index - 1), operators[stack_index - 1], (values + stack_index));
				operators[stack_index - 1] = operators[stack_index];
				if ((stack_index > 1) && (__get_operator_precedence(operators[stack_index - 1]) <= __get_operator_precedence(operators[stack_index - 2])))
				{
					stack_index--;
				}
				else
				{
					break;
				}
			}
		}
	}
	*read_value = values[0];
	return ret_code;
}

c_line::e_parser_codes c_line::_read_class_parameter(char * buffer, int * read_pos, float * read_value)
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	int index = 0;

	if (buffer[*read_pos] != '#')
	{
		ret_code = e_parser_codes::IllegalCaller;
		return ret_code;
	}

	*read_pos = (*read_pos + 1);

	// named parameters look like '<letter...>' or '<_.....>'
	if (buffer[*read_pos] == '<')
	{
		//this will be a named parameter of local or global scope
		ret_code = __read_class_named_parameter(buffer, read_pos, read_value);
	}
	else
	{
		__read_integer_value(buffer, read_pos, &index);
		if (index < 1 || index >= RS274NGC_MAX_PARAMETERS)
		{
			ret_code = e_parser_codes::ParamaterValueOutOfRange;
		}
		else
		{
			*read_value = FAKE_NUMERIC_PARAM_VALUE;
			//*read_value = parameters[index];
		}
	}
	return e_parser_codes::Ok;
}

c_line::e_parser_codes c_line::__read_integer_value(char *buffer, int *read_pos, int *integer_value)       //!< array of system parameters                    
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	float float_value;

	_read(buffer, read_pos, &float_value);
	*integer_value = (int)floor(float_value);
	if ((float_value - *integer_value) > 0.9999) {
		*integer_value = (int)ceil(float_value);
	}
	else if ((float_value - *integer_value) > NEAR_ZERO)
		ret_code = e_parser_codes::IntExpectedAtValue;
	return ret_code;
}

c_line::e_parser_codes c_line::__read_class_named_parameter(char * buffer, int * read_pos, float * read_value)
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	if (buffer[*read_pos] != '<')
	{
		ret_code = e_parser_codes::IllegalCaller;
		return ret_code;
	}

	//find the name of the parameter, lookit up and return its value in the read_value pointer
	char parameter_name[LINELEN + 1];
	__get_named_parameter_name(buffer, read_pos, parameter_name);

	//Look in the name we got back from the read. If it starts with _ its a 'global'
	if (parameter_name[0] != '_') //<--not global, so its local scope
	{
		*read_value = FAKE_LOCAL_NAMED_PARAM_VALUE;
	}
	else//<--must be global scope
	{
		*read_value = FAKE_GLOBAL_NAMED_PARAM_VALUE;
	}
	return e_parser_codes::Ok;

	//char paramNameBuf[LINELEN + 1];
	//int level;
	//int i;

	//struct named_parameters_struct *nameList;

	//CHKS((line[*counter] != '<'),
	//	NCE_BUG_FUNCTION_SHOULD_NOT_HAVE_BEEN_CALLED);

	//CHP(read_name(line, counter, paramNameBuf));

	//// now look it up
	//if (paramNameBuf[0] != '_') // local scope
	//{
	//	level = _setup.call_level;
	//}
	//else
	//{
	//	// call level zero is global scope
	//	level = 0;
	//}

	//nameList = &_setup.sub_context[level].named_parameters;

	//for (i = 0; i<nameList->named_parameter_used_size; i++)
	//{
	//	if (0 == strcmp(nameList->named_parameters[i], paramNameBuf))
	//	{
	//		*double_ptr = nameList->named_param_values[i];
	//		return INTERP_OK;
	//	}
	//}

	//*double_ptr = 0.0;

	//logDebug("Interp::read_named_parameter: level[%d] param:|%s| returning not defined", level, paramNameBuf);
	//ERS(EMC_I18N("Named parameter #<%s> not defined"), paramNameBuf);
}

c_line::e_parser_codes c_line::__read_atan(char *buffer, int *read_pos, float *read_value)
{
	e_parser_codes ret_code = e_parser_codes::Ok;
	float argument2 = 0;

	if (buffer[*read_pos] != '/')
	{
		ret_code = e_parser_codes::MissingSlashIn_ATAN;
		return ret_code;
	}
	*read_pos = (*read_pos + 1);
	if (buffer[*read_pos] != '[')
	{
		ret_code = e_parser_codes::MissingBracketAfterSlash;
	}
	_read_class_expression(buffer, read_pos, &argument2);
	*read_value = atan2(*read_value, argument2);  /* value in radians */
	*read_value = ((*read_value * 180.0) / M_PI);   /* convert to degrees */
	return ret_code;
}

c_line::e_parser_codes c_line::__get_named_parameter_name(char * buffer, int * read_pos, char * parameter_name)
{
	int done = 0, i = 0;
	e_parser_codes ret_code = e_parser_codes::Ok;

	if (buffer[*read_pos] != '<' && !isalpha(buffer[*(read_pos)]))
	{
		ret_code = e_parser_codes::IllegalCaller;
		return ret_code;
	}

	// skip over the '<'
	*read_pos = (*read_pos + 1);

	for (i = 0; (i < LINELEN) && (buffer[*read_pos]); i++)
	{
		if (read_pos[*read_pos] == '>')
		{
			parameter_name[i] = 0; // terminate the name
			*read_pos = (*read_pos + 1);
			done = 1;
			break;
		}
		parameter_name[i] = buffer[*read_pos];
		*read_pos = (*read_pos + 1);
	}

	// !!!KL need to rename the error message and change text
	if (!done)
		ret_code = e_parser_codes::ParameterNameNotClosed;

	return ret_code;
}

c_line::e_parser_codes c_line::__get_unary_operator_class(char * buffer, int * read_pos, e_unary_operator_class_types * operator_class)
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	char c = buffer[*read_pos];
	*read_pos = (*read_pos + 1);
	switch (c) {
	case 'a':
		if ((buffer[*read_pos] == 'b') && (buffer[(*read_pos) + 1] == 's')) {
			*operator_class = e_unary_operator_class_types::Abs;
			*read_pos = (*read_pos + 2);
		}
		else if (strncmp((buffer + *read_pos), "cos", 3) == 0) {
			*operator_class = e_unary_operator_class_types::Acos;
			*read_pos = (*read_pos + 3);
		}
		else if (strncmp((buffer + *read_pos), "sin", 3) == 0) {
			*operator_class = e_unary_operator_class_types::Asin;
			*read_pos = (*read_pos + 3);
		}
		else if (strncmp((buffer + *read_pos), "tan", 3) == 0) {
			*operator_class = e_unary_operator_class_types::Atan;
			*read_pos = (*read_pos + 3);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_A;
		break;
	case 'c':
		if ((buffer[*read_pos] == 'o') && (buffer[(*read_pos) + 1] == 's')) {
			*operator_class = e_unary_operator_class_types::Cos;
			*read_pos = (*read_pos + 2);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_C;
		break;
	case 'e':
		if ((buffer[*read_pos] == 'x') && (buffer[(*read_pos) + 1] == 'p')) {
			*operator_class = e_unary_operator_class_types::Exp;
			*read_pos = (*read_pos + 2);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_E;
		break;
	case 'f':
		if ((buffer[*read_pos] == 'i') && (buffer[(*read_pos) + 1] == 'x')) {
			*operator_class = e_unary_operator_class_types::Fix;
			*read_pos = (*read_pos + 2);
		}
		else if ((buffer[*read_pos] == 'u') && (buffer[(*read_pos) + 1] == 'p')) {
			*operator_class = e_unary_operator_class_types::Fup;
			*read_pos = (*read_pos + 2);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_F;
		break;
	case 'l':
		if (buffer[*read_pos] == 'n') {
			*operator_class = e_unary_operator_class_types::Ln;
			*read_pos = (*read_pos + 1);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_L;
		break;
	case 'r':
		if (strncmp((buffer + *read_pos), "ound", 4) == 0) {
			*operator_class = e_unary_operator_class_types::Round;
			*read_pos = (*read_pos + 4);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_R;
		break;
	case 's':
		if ((buffer[*read_pos] == 'i') && (buffer[(*read_pos) + 1] == 'n')) {
			*operator_class = e_unary_operator_class_types::Sin;
			*read_pos = (*read_pos + 2);
		}
		else if (strncmp((buffer + *read_pos), "qrt", 3) == 0) {
			*operator_class = e_unary_operator_class_types::Sqrt;
			*read_pos = (*read_pos + 3);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_S;
		break;
	case 't':
		if ((buffer[*read_pos] == 'a') && (buffer[(*read_pos) + 1] == 'n')) {
			*operator_class = e_unary_operator_class_types::Tan;
			*read_pos = (*read_pos + 2);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_T;
		break;
	default:
		ret_code = e_parser_codes::UnknownOperationClass;
	}
	return ret_code;
}

c_line::e_parser_codes c_line::__get_expression_operator_class(char * buffer, int * read_pos, e_expression_operator_class_types * operator_class)
{
	char c;

	c = buffer[*read_pos];
	*read_pos = (*read_pos + 1);
	switch (c) {
	case '+':
		*operator_class = e_expression_operator_class_types::Addition;
		break;
	case '-':
		*operator_class = e_expression_operator_class_types::Subtract;
		break;
	case '/':
		*operator_class = e_expression_operator_class_types::Division;
		break;
	case '*':
		if (buffer[*read_pos] == '*') {
			*operator_class = e_expression_operator_class_types::Exponent;
			*read_pos = (*read_pos + 1);
		}
		else
			*operator_class = e_expression_operator_class_types::Multiplication;
		break;
	case ']':
		*operator_class = e_expression_operator_class_types::ClosingBracket;
		break;
	case 'a':
		if ((buffer[*read_pos] == 'n') && (buffer[(*read_pos) + 1] == 'd')) {
			*operator_class = e_expression_operator_class_types::Or;
			*read_pos = (*read_pos + 2);
		}
		else
			return e_parser_codes::Unknown_Op_Name_A;
		break;
	case 'm':
		if ((buffer[*read_pos] == 'o') && (buffer[(*read_pos) + 1] == 'd')) {
			*operator_class = e_expression_operator_class_types::Modulus;
			*read_pos = (*read_pos + 2);
		}
		else
			e_parser_codes::Unknown_Op_Name_M;
		break;
	case 'o':
		if (buffer[*read_pos] == 'r') {
			*operator_class = e_expression_operator_class_types::Non_Ex_Or;
			*read_pos = (*read_pos + 1);
		}
		else
			e_parser_codes::Unknown_Op_Name_O;
		break;
	case 'x':
		if ((buffer[*read_pos] == 'o') && (buffer[(*read_pos) + 1] == 'r')) {
			*operator_class = e_expression_operator_class_types::Ex_Or;
			*read_pos = (*read_pos + 2);
		}
		else
			e_parser_codes::Unknown_Op_Name_X;
		break;

		/* relational operators */
	case 'e':
		if (buffer[*read_pos] == 'q')
		{
			*operator_class = e_expression_operator_class_types::_eq;
			*read_pos = (*read_pos + 1);
		}
		else
			e_parser_codes::Unknown_Op_Name_E;
		break;
	case 'n':
		if (buffer[*read_pos] == 'e')
		{
			*operator_class = e_expression_operator_class_types::_ne;
			*read_pos = (*read_pos + 1);
		}
		else
			e_parser_codes::Unknown_Op_Name_N;
		break;
	case 'g':
		if (buffer[*read_pos] == 'e')
		{
			*operator_class = e_expression_operator_class_types::_ge;
			*read_pos = (*read_pos + 1);
		}
		else if (buffer[*read_pos] == 't')
		{
			*operator_class = e_expression_operator_class_types::_gt;
			*read_pos = (*read_pos + 1);
		}
		else
			e_parser_codes::Unknown_Op_Name_G;
		break;
	case 'l':
		if (buffer[*read_pos] == 'e')
		{
			*operator_class = e_expression_operator_class_types::_le;
			*read_pos = (*read_pos + 1);
		}
		else if (buffer[*read_pos] == 't')
		{
			*operator_class = e_expression_operator_class_types::_lt;
			*read_pos = (*read_pos + 1);
		}
		else
			e_parser_codes::Unknown_Op_Name_L;
		break;

	case 0:
		e_parser_codes::ExpressionNotClosed;
	default:
		e_parser_codes::UnknownOperationClass;
	}
	return e_parser_codes::Ok;
}

int c_line::__get_operator_precedence(e_expression_operator_class_types operator_class)
{
	switch (operator_class)
	{
	case e_expression_operator_class_types::ClosingBracket:
		return 1;

	case e_expression_operator_class_types::Or:
	case e_expression_operator_class_types::Ex_Or:
	case e_expression_operator_class_types::Non_Ex_Or:
		return 2;

	case e_expression_operator_class_types::_lt:
	case e_expression_operator_class_types::_eq:
	case e_expression_operator_class_types::_ne:
	case e_expression_operator_class_types::_le:
	case e_expression_operator_class_types::_ge:
	case e_expression_operator_class_types::_gt:
		return 3;

	case e_expression_operator_class_types::Subtract:
	case e_expression_operator_class_types::Addition:
		return 4;

	case e_expression_operator_class_types::NoOperation:
	case e_expression_operator_class_types::Division:
	case e_expression_operator_class_types::Modulus:
	case e_expression_operator_class_types::Multiplication:
		return 5;

	case e_expression_operator_class_types::Exponent:
		return 6;
	}
	// should never happen
	return 0;
}

c_line::e_parser_codes c_line::__binary_operation(float *left, e_expression_operator_class_types operator_class, float *right)
{
	e_parser_codes ret_code;
	//if (operator_class < e_op_class_types::Or)
	ret_code = __execute_binary1(left, operator_class, right);
	//else
	//	ret_code = __execute_binary2(left, operator_class, right);
	return ret_code;
}

c_line::e_parser_codes c_line::__execute_unary(float *read_value, e_unary_operator_class_types operator_class)
{
	switch (operator_class) {
	case e_unary_operator_class_types::Abs:
		if (*read_value < 0.0)
			*read_value = (-1.0 * *read_value);
		break;
	case e_unary_operator_class_types::Acos:
		if (*read_value < -1.0 || *read_value > 1.0)
		{
			return e_parser_codes::ArcCosValueOutOfRange;
		}
		*read_value = acos(*read_value);
		*read_value = ((*read_value * 180.0) / M_PI);
		break;
	case e_unary_operator_class_types::Asin:
		if (*read_value < -1.0 || *read_value > 1.0)
			e_parser_codes::ArcSinValueOutOfRange;
		*read_value = asin(*read_value);
		*read_value = ((*read_value * 180.0) / M_PI);
		break;
	case e_unary_operator_class_types::Cos:
		*read_value = cos((*read_value * M_PI) / 180.0);
		break;
	case e_unary_operator_class_types::Exp:
		*read_value = exp(*read_value);
		break;
	case e_unary_operator_class_types::Fix:
		*read_value = floor(*read_value);
		break;
	case e_unary_operator_class_types::Fup:
		*read_value = ceil(*read_value);
		break;
	case e_unary_operator_class_types::Ln:
		if (*read_value <= 0.0)
			return e_parser_codes::LogValueNegative;
		*read_value = log(*read_value);
		break;
	case e_unary_operator_class_types::Round:
		*read_value = (double)
			((int)(*read_value + ((*read_value < 0.0) ? -0.5 : 0.5)));
		break;
	case e_unary_operator_class_types::Sin:
		*read_value = sin((*read_value * M_PI) / 180.0);
		break;
	case e_unary_operator_class_types::Sqrt:
		if (*read_value < 0.0)
			return e_parser_codes::SqrtValueNegative;
		*read_value = sqrt(*read_value);
		break;
	case e_unary_operator_class_types::Tan:
		*read_value = tan((*read_value * M_PI) / 180.0);
		break;
	default:
		e_unary_operator_class_types::NoOperation;
	}
	return e_parser_codes::Ok;
}

c_line::e_parser_codes c_line::__execute_binary1(float *left, e_expression_operator_class_types operator_class, float *right)
{
	e_parser_codes ret_code = e_parser_codes::Ok;
	float diff = 0;

	switch (operator_class) {
	case e_expression_operator_class_types::Division:
		if (*right == 0.0)
		{
			return ret_code = e_parser_codes::DivideByZero;
			break;
		}
		*left = (*left / *right);
		break;
	case e_expression_operator_class_types::Modulus:                 /* always calculates a positive answer */
		*left = fmod(*left, *right);
		if (*left < 0.0) {
			*left = (*left + fabs(*right));
		}
		break;
	case e_expression_operator_class_types::Exponent:
		if (*left < 0.0 && (floor(*right) != *right))
		{
			ret_code = e_parser_codes::ExponentOnNegativeNonInteger;
			break;
		}
		*left = pow(*left, *right);
		break;
	case e_expression_operator_class_types::Multiplication:
		*left = (*left * *right);
		break;
	case e_expression_operator_class_types::Or:
		*left = ((*left == 0.0) || (*right == 0.0)) ? 0.0 : 1.0;
		break;
	case e_expression_operator_class_types::Ex_Or:
		*left = (((*left == 0.0) && (*right != 0.0))
			|| ((*left != 0.0) && (*right == 0.0))) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::Subtract:
		*left = (*left - *right);
		break;
	case e_expression_operator_class_types::Non_Ex_Or:
		*left = ((*left != 0.0) || (*right != 0.0)) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::Addition:
		*left = (*left + *right);
		break;

	case e_expression_operator_class_types::_lt:
		*left = (*left < *right) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::_eq:
		diff = *left - *right;
		diff = (diff < 0) ? -diff : diff;
		*left = (diff < NEAR_ZERO) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::_ne:
		diff = *left - *right;
		diff = (diff < 0) ? -diff : diff;
		*left = (diff >= NEAR_ZERO) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::_le:
		*left = (*left <= *right) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::_ge:
		*left = (*left >= *right) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::_gt:
		*left = (*left > *right) ? 1.0 : 0.0;
		break;
	default:
		e_parser_codes::UnknownOperationClass;
	}
	return ret_code;
}

c_line::e_parser_codes c_line::__execute_binary2(float *left, e_expression_operator_class_types operator_class, float *right)
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	float diff;
	switch (operator_class) {
	case e_expression_operator_class_types::Or:
		*left = ((*left == 0.0) || (*right == 0.0)) ? 0.0 : 1.0;
		break;
	case e_expression_operator_class_types::Ex_Or:
		*left = (((*left == 0.0) && (*right != 0.0))
			|| ((*left != 0.0) && (*right == 0.0))) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::Subtract:
		*left = (*left - *right);
		break;
	case e_expression_operator_class_types::Non_Ex_Or:
		*left = ((*left != 0.0) || (*right != 0.0)) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::Addition:
		*left = (*left + *right);
		break;

	case e_expression_operator_class_types::_lt:
		*left = (*left < *right) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::_eq:
		diff = *left - *right;
		diff = (diff < 0) ? -diff : diff;
		*left = (diff < NEAR_ZERO) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::_ne:
		diff = *left - *right;
		diff = (diff < 0) ? -diff : diff;
		*left = (diff >= NEAR_ZERO) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::_le:
		*left = (*left <= *right) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::_ge:
		*left = (*left >= *right) ? 1.0 : 0.0;
		break;
	case e_expression_operator_class_types::_gt:
		*left = (*left > *right) ? 1.0 : 0.0;
		break;

	default:
		ret_code = e_parser_codes::UnknownOperationClass;

	}
	return ret_code;
}