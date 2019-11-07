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

c_line::s_param_functions c_line::parameter_function_pointers;
int c_line::last_read_position = 0;
static int max_numeric_parameter_count = 0;

c_line::e_parser_codes c_line::start(char * buffer)
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	//set line data to all lower case
	if (!_set_buffer_to_upper(buffer))
	{/*return bad data in the buffer*/
		return e_parser_codes::BadDataInBuffer;
	}

	//If there are any line terminators, split this line so they are not parsed.
	int startlen = strlen(buffer);
	char * buffer_no_eol = strtok(buffer, "\r");
	int endlen = strlen(buffer_no_eol);

	ret_code = _process_buffer(buffer);

	/*
	You can send a string of gcode that is actually multiple 'lines' containing multip \r characters
	so as a convenience to the caller I process only the first 'line' then once finished, we set the
	start pointer to the end+1 of the previous 'line' so that the buffer is ready the next time we
	get called. If there was actually no more data a NULL character will be pointed too and the caller
	can check for a NULL before calling this again.

	If ret_code was success, we can skip forwad in the buffer, but if it was not a success we will NOT
	set the buffer pointer ahead.
	*/

	if (startlen != endlen && ret_code == e_parser_codes::Ok)
		//advance buffer to next 'line' (if NUll no data available)
		buffer = buffer + (endlen + 1);

	return ret_code;

}

uint8_t c_line::_set_buffer_to_upper(char * buffer)
{
	int count = 0;
	for (int i = 0; buffer[i]; i++)
	{
		//remove all spaces, tabs, and line feeds
		if (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\n')
		{
			//clean up any multiple carriage return sequences
			while (buffer[i + 1] == '\r') { i++; }

			buffer[count++] = toupper(buffer[i]);
		}

	}
	buffer[count] = '\0';
	return 1;
}

c_line::e_parser_codes c_line::_process_buffer(char * buffer)
{
	int read_pos, buff_len = 0;

	buff_len = strlen(buffer);
	read_pos = 0;

	/*
	Need to check parameters settings.
	Because this code system (as in all of talos) is expected to run on a lightweight
	mCU with much less ram than a pc, but we want the functionality that the pc version
	of a cnc control gives us, we need to do some special things. One in particular is
	how we handle parameter values. We cannot create an enormous array to hold thousands
	of parameters in. So I have left 3 function pointers that should be assigned to
	special functions to handle parameter calls for named local/global, and numeric
	parameters. Handle them how you wish, or ignore it entirely. If these functions
	are not set, parameters will not be available.
	*/

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
	if (buffer[read_pos] == 'O')
		return e_parser_codes::OCodesNotImplimented;

	float word_value = 0.0;
	//get current word, and advance reader
	char current_word = 0;

	//And now without further delay... lets process this line!
	while (read_pos < buff_len)
	{
		e_parser_codes ret_code = e_parser_codes::Ok;

		word_value = 0.0;
		//get current word, and advance reader
		current_word = buffer[read_pos++];
		ret_code = _read_as_class_type(buffer, &read_pos, &word_value);
		//This will be useful to the end user to determine where the gcode data had an error
		c_line::last_read_position = read_pos;

		if (ret_code != e_parser_codes::Ok)
		{
			//Return the error to the caller
			return ret_code;
		}
		//Right here is where we have the word, and its address value.
		//If you want to update some type of block object with the word
		//data, do it right here. If there was an expression it would
		//have been evaluated and calculated already. Parameters would
		//have already been read. However O words are not currently
		//handled. I plan to expand that some time but not right now.
		//block_object_word_array[current_word] = word_value

		return ret_code;
	}

}

c_line::e_parser_codes c_line::_read_as_class_type(char * buffer, int * read_pos, float * word_value)
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	e_value_class_types current_class = c_line::__get_value_class(buffer, read_pos, word_value);

	switch (current_class)
	{
	case c_line::e_value_class_types::NumericParametersNotAvailable:
	{
		ret_code = e_parser_codes::NumericParametersNotAvailable;
		break;
	}
	case c_line::e_value_class_types::NamedParametersNotAvailable:
	{
		ret_code = e_parser_codes::NoNamedParametersAvailable;
		break;
	}
	case c_line::e_value_class_types::UnSpecified:
	{
		ret_code = e_parser_codes::UnHandledValueClass;
		break;
	}
	case c_line::e_value_class_types::Expression:
	{
		ret_code = c_line::__read_class_expression(buffer, read_pos, word_value);
		break;
	}
	case c_line::e_value_class_types::Parameter:
	{
		ret_code = c_line::__read_class_parameter(buffer, read_pos, word_value);
		break;
	}
	case c_line::e_value_class_types::Unary:
	{
		ret_code = c_line::__read_class_unary(buffer, read_pos, word_value);
		break;
	}
	case c_line::e_value_class_types::Numeric:
	{
		ret_code = c_line::__read_class_numeric(buffer, read_pos, word_value);
		break;
	}
	case c_line::e_value_class_types::ClosingBracket:
	{
		//closing bracket from a unary or expression. we should be okay with this. 
		ret_code = e_parser_codes::Ok;
		break;
	}
	default:
		ret_code = e_parser_codes::UnHandledValueClass;
		break;
	}

	return ret_code;
}

c_line::e_value_class_types c_line::__get_value_class(char * buffer, int * read_pos, float * word_value)
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	char current_byte, next_byte;

	current_byte = buffer[*read_pos];

	if (current_byte == 0)
		return e_value_class_types::UnSpecified;

	next_byte = buffer[*read_pos + 1];

	if (current_byte == '[')
	{
		return e_value_class_types::Expression;
	}
	else if (current_byte == ']')
	{
		return e_value_class_types::ClosingBracket;
	}
	else if (current_byte == '#')
	{
		if (c_line::parameter_function_pointers.pntr_get_global_named_parameter != NULL
			|| c_line::parameter_function_pointers.pntr_get_local_named_parameter != NULL
			|| (c_line::parameter_function_pointers.pntr_get_numeric_parameter != NULL
				&&c_line::parameter_function_pointers.pntr_get_numeric_parameter_max != NULL))
			return e_value_class_types::Parameter;
		else
		{
			if (c_line::parameter_function_pointers.pntr_get_global_named_parameter == NULL
				|| c_line::parameter_function_pointers.pntr_get_local_named_parameter == NULL)
			{
				return e_value_class_types::NamedParametersNotAvailable;
			}
			else if (c_line::parameter_function_pointers.pntr_get_numeric_parameter == NULL
				|| c_line::parameter_function_pointers.pntr_get_numeric_parameter_max == NULL)
			{
				return e_value_class_types::NumericParametersNotAvailable;
			}
		}
	}
	else if (current_byte == '+' && next_byte && !isdigit(next_byte) && next_byte != '.')
	{
		//this is most likely a unary that was signed positive	
		(*read_pos)++;
		ret_code = c_line::_read_as_class_type(buffer, read_pos, word_value);
		if (ret_code != e_parser_codes::Ok)
			return e_value_class_types::UnSpecified;
		*word_value = +*word_value;
		return c_line::__get_value_class(buffer, read_pos, word_value);
		//return e_value_class_types::ClosingBracket;
	}
	else if (current_byte == '-' && next_byte && !isdigit(next_byte) && next_byte != '.')
	{
		//this is most likely a unary that was signed negative
		(*read_pos)++;
		ret_code = c_line::_read_as_class_type(buffer, read_pos, word_value);
		if (ret_code != e_parser_codes::Ok)
			return e_value_class_types::UnSpecified;
		*word_value = -*word_value;
		return c_line::__get_value_class(buffer, read_pos, word_value);
		//return e_value_class_types::ClosingBracket;
	}
	else if ((current_byte >= 'A') && (current_byte <= 'Z'))
		return e_value_class_types::Unary;
	else
	{
		return e_value_class_types::Numeric;
	}

}

c_line::e_parser_codes c_line::__read_class_unary(char * buffer, int * read_pos, float * read_value)
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	e_unary_operator_class_types operation = e_unary_operator_class_types::NoOperation;

	ret_code = c_line::__get_unary_operator_class(buffer, read_pos, &operation);
	if (ret_code != e_parser_codes::Ok)
		return ret_code;

	if (buffer[*read_pos] != '[')
	{
		ret_code = e_parser_codes::UnaryMissingOpen;
		return ret_code;
	}
	c_line::__read_class_expression(buffer, read_pos, read_value);

	if (operation == e_unary_operator_class_types::Atan)
		ret_code = c_line::___execute_atan(buffer, read_pos, read_value);
	else
		ret_code = c_line::___execute_unary(read_value, operation);
	return ret_code;
}

c_line::e_parser_codes c_line::__read_class_numeric(char * buffer, int * read_pos, float * read_value)
{
	char *end, restore_byte;
	size_t _size;

	//Found this in linux cnc. Pretty neat way to get a number without making a buffer to copy too

	//Count how many bytes match at the begining of the string
	_size = strspn((buffer + (*read_pos)), "0123456789+-.");
	//Copy the byte where the numbers end and hold it temporarily
	restore_byte = (buffer + (*read_pos))[_size];
	//replace the byte where the numbers end with a null
	(buffer + (*read_pos))[_size] = 0;
	//convert the string portion to a number, &end gets updated to the byte count converted
	*read_value = strtod((buffer + (*read_pos)), &end);
	//put the saved byte back so the null isnt there anymore
	(buffer + (*read_pos))[_size] = restore_byte;
	//see if end is equal to the string start point. If it is, we read nothing there
	if (end == (buffer + (*read_pos)))
	{
		return e_parser_codes::NumericValueMissing;
	}
	//update the pointer for the reader position
	*read_pos = end - buffer;
	return e_parser_codes::Ok;
}

c_line::e_parser_codes c_line::__read_class_expression(char * buffer, int * read_pos, float * read_value)
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	float values[MAX_EXPRESSION_OPS];
	memset(values, 0, sizeof(float) * MAX_EXPRESSION_OPS);
	e_expression_operator_class_types operators[MAX_EXPRESSION_OPS];
	memset(operators, 0, sizeof(e_expression_operator_class_types) * MAX_EXPRESSION_OPS);
	int stack_index;

	if (buffer[*read_pos] != '[')
	{
		ret_code = e_parser_codes::IllegalCaller;
		return ret_code;
	}

	*read_pos = (*read_pos + 1);
	//read_real_value(line, counter, values, parameters);
	ret_code = c_line::_read_as_class_type(buffer, read_pos, values);
	if (ret_code != e_parser_codes::Ok)
	{
		*read_value = values[0];
		return ret_code;
	}

	c_line::__get_expression_operator_class(buffer, read_pos, operators);

	stack_index = 1;

	while (operators[0] != e_expression_operator_class_types::ClosingBracket)
	{
		ret_code = c_line::_read_as_class_type(buffer, read_pos, values + stack_index);
		//If we dont get an ok response processing, we should be at the end of the expression
		//Otherwise its a problem. 
		if (ret_code == e_parser_codes::Ok)
		{
			c_line::__get_expression_operator_class(buffer, read_pos, operators + stack_index);

			if (c_line::__get_operator_precedence(operators[stack_index]) > c_line::__get_operator_precedence(operators[stack_index - 1]))
				stack_index++;
			else
			{
				while (c_line::__get_operator_precedence(operators[stack_index]) <= c_line::__get_operator_precedence(operators[stack_index - 1]))
				{
					c_line::___execute_binary((values + stack_index - 1), operators[stack_index - 1], (values + stack_index));
					operators[stack_index - 1] = operators[stack_index];
					if ((stack_index > 1) && (c_line::__get_operator_precedence(operators[stack_index - 1]) <= c_line::__get_operator_precedence(operators[stack_index - 2])))
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
		else
		{
			break;
		}
	}
	*read_value = values[0];
	return ret_code;
}

c_line::e_parser_codes c_line::__read_class_parameter(char * buffer, int * read_pos, float * read_value)
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
		if (c_line::parameter_function_pointers.pntr_get_global_named_parameter == NULL
			&& c_line::parameter_function_pointers.pntr_get_local_named_parameter == NULL)
		{
			ret_code = e_parser_codes::NoNamedParametersAvailable;
			return ret_code;
		}

		//this will be a named parameter of local or global scope
		ret_code = c_line::___read_class_named_parameter(buffer, read_pos, read_value);
		if (ret_code != e_parser_codes::Ok)
			return ret_code;
	}
	else
	{
		if (c_line::parameter_function_pointers.pntr_get_numeric_parameter_max == NULL)
		{
			ret_code = e_parser_codes::NumericParametersMaxNotAvailable;
		}
		else
		{
			if (!max_numeric_parameter_count)//<--if we dont have max_numeric params count, get it
				max_numeric_parameter_count = c_line::parameter_function_pointers.pntr_get_numeric_parameter_max();

			c_line::___read_integer_value(buffer, read_pos, &index);
			if (index < 1 || index >= max_numeric_parameter_count)
			{
				ret_code = e_parser_codes::ParamaterValueOutOfRange;
				*read_value = index;
			}
			else
			{
				if (c_line::parameter_function_pointers.pntr_get_numeric_parameter != NULL)
					*read_value = c_line::parameter_function_pointers.pntr_get_numeric_parameter(index);// FAKE_NUMERIC_PARAM_VALUE;
				else
					ret_code = e_parser_codes::NumericParametersNotAvailable;
				//*read_value = FAKE_NUMERIC_PARAM_VALUE;
			}
		}
	}
	return ret_code;
}

c_line::e_parser_codes c_line::___read_integer_value(char *buffer, int *read_pos, int *integer_value)
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	float float_value = 0;

	ret_code = c_line::_read_as_class_type(buffer, read_pos, &float_value);
	if (ret_code != e_parser_codes::Ok)
		return ret_code;

	*integer_value = (int)floor(float_value);
	if ((float_value - *integer_value) > NEAR_ONE)
	{
		*integer_value = (int)ceil(float_value);
	}
	else if ((float_value - *integer_value) > NEAR_ZERO)
		ret_code = e_parser_codes::IntExpectedAtValue;
	return ret_code;
}

c_line::e_parser_codes c_line::___read_class_named_parameter(char * buffer, int * read_pos, float * read_value)
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	if (buffer[*read_pos] != '<')
	{
		ret_code = e_parser_codes::IllegalCaller;
		return ret_code;
	}

	//find the name of the parameter, lookit up and return its value in the read_value pointer
	char parameter_name[MAX_NAMED_PARAM_LENGTH + 1];
	memset(parameter_name, 0, MAX_NAMED_PARAM_LENGTH);

	c_line::____get_named_parameter_name(buffer, read_pos, parameter_name);

	//Look in the name we got back from the read. If it starts with _ its a 'global'
	if (parameter_name[0] == '_')//<--global scope
	{
		if (c_line::parameter_function_pointers.pntr_get_global_named_parameter != NULL)
			*read_value = c_line::parameter_function_pointers.pntr_get_global_named_parameter(parameter_name);
		else
			ret_code = e_parser_codes::GlobalNamedParametersNotAvailable;
		//*read_value = FAKE_GLOBAL_NAMED_PARAM_VALUE;
	}
	else//<--not global, so its local scope
	{
		if (c_line::parameter_function_pointers.pntr_get_local_named_parameter != NULL)
			*read_value = c_line::parameter_function_pointers.pntr_get_local_named_parameter(parameter_name);
		else
			ret_code = e_parser_codes::LocalNamedParametersNotAvailable;
		//*read_value = FAKE_LOCAL_NAMED_PARAM_VALUE;
	}
	return ret_code;

	//Remaining code here we cannot use. The entire parameter table will not fit on an mCU which
	//is what I am targeting with this code. However I know of a way to fit a nearly infinite number
	//of parameters into this code.. 

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

c_line::e_parser_codes c_line::___execute_atan(char *buffer, int *read_pos, float *read_value)
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
	c_line::__read_class_expression(buffer, read_pos, &argument2);
	*read_value = atan2(*read_value, argument2);  /* value in radians */
	*read_value = ((*read_value * 180.0) / M_PI);   /* convert to degrees */
	return ret_code;
}

c_line::e_parser_codes c_line::____get_named_parameter_name(char * buffer, int * read_pos, char * parameter_name)
{
	bool done = false;
	e_parser_codes ret_code = e_parser_codes::Ok;

	if (buffer[*read_pos] != '<' && !isalpha(buffer[*(read_pos)]))
	{
		ret_code = e_parser_codes::IllegalCaller;
		return ret_code;
	}

	// skip over the '<'
	*read_pos = (*read_pos + 1);

	for (int i = 0; (i < MAX_NAMED_PARAM_LENGTH) && (buffer[*read_pos]); i++)
	{
		if (buffer[*read_pos] == '>')
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
	case 'A':
		if ((buffer[*read_pos] == 'B') && (buffer[(*read_pos) + 1] == 'S')) {
			*operator_class = e_unary_operator_class_types::Abs;
			*read_pos = (*read_pos + 2);
		}
		else if (strncmp((buffer + *read_pos), "COS", 3) == 0) {
			*operator_class = e_unary_operator_class_types::Acos;
			*read_pos = (*read_pos + 3);
		}
		else if (strncmp((buffer + *read_pos), "SIN", 3) == 0) {
			*operator_class = e_unary_operator_class_types::Asin;
			*read_pos = (*read_pos + 3);
		}
		else if (strncmp((buffer + *read_pos), "TAN", 3) == 0) {
			*operator_class = e_unary_operator_class_types::Atan;
			*read_pos = (*read_pos + 3);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_A;
		break;
	case 'C':
		if ((buffer[*read_pos] == 'O') && (buffer[(*read_pos) + 1] == 'C')) {
			*operator_class = e_unary_operator_class_types::Cos;
			*read_pos = (*read_pos + 2);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_C;
		break;
	case 'E':
		if ((buffer[*read_pos] == 'X') && (buffer[(*read_pos) + 1] == 'P')) {
			*operator_class = e_unary_operator_class_types::Exp;
			*read_pos = (*read_pos + 2);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_E;
		break;
	case 'F':
		if ((buffer[*read_pos] == 'I') && (buffer[(*read_pos) + 1] == 'X')) {
			*operator_class = e_unary_operator_class_types::Fix;
			*read_pos = (*read_pos + 2);
		}
		else if ((buffer[*read_pos] == 'U') && (buffer[(*read_pos) + 1] == 'P')) {
			*operator_class = e_unary_operator_class_types::Fup;
			*read_pos = (*read_pos + 2);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_F;
		break;
	case 'L':
		if (buffer[*read_pos] == 'L') {
			*operator_class = e_unary_operator_class_types::Ln;
			*read_pos = (*read_pos + 1);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_L;
		break;
	case 'R':
		if (strncmp((buffer + *read_pos), "OUND", 4) == 0) {
			*operator_class = e_unary_operator_class_types::Round;
			*read_pos = (*read_pos + 4);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_R;
		break;
	case 'S':
		if ((buffer[*read_pos] == 'I') && (buffer[(*read_pos) + 1] == 'N')) {
			*operator_class = e_unary_operator_class_types::Sin;
			*read_pos = (*read_pos + 2);
		}
		else if (strncmp((buffer + *read_pos), "QRT", 3) == 0) {
			*operator_class = e_unary_operator_class_types::Sqrt;
			*read_pos = (*read_pos + 3);
		}
		else
			ret_code = e_parser_codes::Unknown_Op_Name_S;
		break;
	case 'T':
		if ((buffer[*read_pos] == 'A') && (buffer[(*read_pos) + 1] == 'N')) {
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
	char c = 0;

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
	case 'A':
		if ((buffer[*read_pos] == 'N') && (buffer[(*read_pos) + 1] == 'D')) {
			*operator_class = e_expression_operator_class_types::And;
			*read_pos = (*read_pos + 2);
		}
		else
			return e_parser_codes::Unknown_Op_Name_A;
		break;
	case 'M':
		if ((buffer[*read_pos] == 'O') && (buffer[(*read_pos) + 1] == 'D')) {
			*operator_class = e_expression_operator_class_types::Modulus;
			*read_pos = (*read_pos + 2);
		}
		else
			e_parser_codes::Unknown_Op_Name_M;
		break;
	case 'O':
		if (buffer[*read_pos] == 'R') {
			*operator_class = e_expression_operator_class_types::Or;
			*read_pos = (*read_pos + 1);
		}
		else
			e_parser_codes::Unknown_Op_Name_O;
		break;
	case 'X':
		if ((buffer[*read_pos] == 'O') && (buffer[(*read_pos) + 1] == 'R')) {
			*operator_class = e_expression_operator_class_types::Ex_Or;
			*read_pos = (*read_pos + 2);
		}
		else
			e_parser_codes::Unknown_Op_Name_X;
		break;

		/* relational operators */
	case 'E':
		if (buffer[*read_pos] == 'Q')
		{
			*operator_class = e_expression_operator_class_types::_eq;
			*read_pos = (*read_pos + 1);
		}
		else
			e_parser_codes::Unknown_Op_Name_E;
		break;
	case 'N':
		if (buffer[*read_pos] == 'E')
		{
			*operator_class = e_expression_operator_class_types::_ne;
			*read_pos = (*read_pos + 1);
		}
		else
			e_parser_codes::Unknown_Op_Name_N;
		break;
	case 'G':
		if (buffer[*read_pos] == 'E')
		{
			*operator_class = e_expression_operator_class_types::_ge;
			*read_pos = (*read_pos + 1);
		}
		else if (buffer[*read_pos] == 'T')
		{
			*operator_class = e_expression_operator_class_types::_gt;
			*read_pos = (*read_pos + 1);
		}
		else
			e_parser_codes::Unknown_Op_Name_G;
		break;
	case 'L':
		if (buffer[*read_pos] == 'E')
		{
			*operator_class = e_expression_operator_class_types::_le;
			*read_pos = (*read_pos + 1);
		}
		else if (buffer[*read_pos] == 'T')
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

c_line::e_parser_codes c_line::___execute_unary(float *read_value, e_unary_operator_class_types operator_class)
{
	e_parser_codes ret_code = e_parser_codes::Ok;

	switch (operator_class) {
	case e_unary_operator_class_types::Abs:
	{
		if (*read_value < 0.0)
			*read_value = (-1.0 * *read_value);
		break;
	}
	case e_unary_operator_class_types::Acos:
	{
		if (*read_value < -1.0 || *read_value > 1.0)
		{
			return e_parser_codes::ArcCosValueOutOfRange;
		}
		*read_value = acos(*read_value);
		*read_value = ((*read_value * 180.0) / M_PI);
		break;
	}
	case e_unary_operator_class_types::Asin:
	{
		if (*read_value < -1.0 || *read_value > 1.0)
		{
			ret_code = e_parser_codes::ArcSinValueOutOfRange;
			break;
		}
		*read_value = asin(*read_value);
		*read_value = ((*read_value * 180.0) / M_PI);
		break;
	}
	case e_unary_operator_class_types::Cos:
	{
		*read_value = cos((*read_value * M_PI) / 180.0);
		break;
	}
	case e_unary_operator_class_types::Exp:
	{
		*read_value = exp(*read_value);
		break;
	}
	case e_unary_operator_class_types::Fix:
	{
		*read_value = floor(*read_value);
		break;
	}
	case e_unary_operator_class_types::Fup:
	{
		*read_value = ceil(*read_value);
		break;
	}
	case e_unary_operator_class_types::Ln:
	{
		if (*read_value <= 0.0)
		{
			ret_code = e_parser_codes::LogValueNegative;
			break;
		}
		*read_value = log(*read_value);
		break;
	}
	case e_unary_operator_class_types::Round:
	{
		*read_value = (float)
			((int)(*read_value + ((*read_value < 0.0) ? -0.5 : 0.5)));
		break;
	}
	case e_unary_operator_class_types::Sin:
	{
		*read_value = sin((*read_value * M_PI) / 180.0);
		break;
	}
	case e_unary_operator_class_types::Sqrt:
	{
		if (*read_value < 0.0)
		{
			ret_code = e_parser_codes::SqrtValueNegative;
			return ret_code;
		}
		*read_value = sqrt(*read_value);
		break;
	}
	case e_unary_operator_class_types::Tan:
	{
		*read_value = tan((*read_value * M_PI) / 180.0);
		break;
	}
	default:
		e_unary_operator_class_types::NoOperation;
	}
	return ret_code;
}

c_line::e_parser_codes c_line::___execute_binary(float *left, e_expression_operator_class_types operator_class, float *right)
{
	e_parser_codes ret_code = e_parser_codes::Ok;
	float diff = 0;

	switch (operator_class) {
	case e_expression_operator_class_types::Division:
		if (*right == 0.0)
		{
			ret_code = e_parser_codes::DivideByZero;
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
		ret_code = e_parser_codes::UnknownOperationClass;
	}
	return ret_code;
}

