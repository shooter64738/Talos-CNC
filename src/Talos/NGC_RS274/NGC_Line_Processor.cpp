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


#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "_ngc_math_constants.h"
#include "NGC_Line_Processor.h"
#include "NGC_Parameters.h"
#include "NGC_Block_Assignor.h"
#include "../Configuration/c_configuration.h"
#include "Dialect/_ngc_dialect_validate.h"

int NGC_RS274::LineProcessor::last_read_position = 0;
static int max_numeric_parameter_count = 0;

uint8_t NGC_RS274::LineProcessor::initialize()
{
	
	return 0;
}

e_parsing_errors NGC_RS274::LineProcessor::_process_buffer(char * buffer, s_ngc_block * block, uint8_t buff_len)
{
	int read_pos = 0;
	
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


	//We could read the N word here. For nwo I am handling it as
	//I do any other word.

	//O words are a little special. cant process those yet, but
	//I am working on it. I think they can be called re-entrant
	//which is similar to how how linux cnc handles O words
	if (buffer[read_pos] == 'O')
		return e_parsing_errors::OCodesNotImplimented;

	float word_value = 0.0;
	//get current word, and advance reader
	char current_word = 0;
	e_parsing_errors ret_code = e_parsing_errors::OK;
	//Wonder if the stationing system can be used for subroutines as well... 

	//If this block is skipable, process it anyway, but flag it as a skippable block
	//and we can check it when the motion conrol executes it. If the skip switch is
	//on we will go ahead and skip it. If it is off, we can execute it. 
	if (buffer[read_pos] == '/')
	{
		block->block_events.set((int)e_block_event::BlockSkipOptionActive);
		read_pos++;
	}

	//Clear the dialect verification array
	//memset(NGC_RS274::Error_Check::dialect_verify, 0, sizeof(NGC_RS274::Error_Check::dialect_verify));

	//And now without further delay... lets process this line!
	while (read_pos < buff_len)
	{
		bool word_has_decimal = false;
		word_value = 0.0;
		//get current word, and advance reader
		current_word = buffer[read_pos];
		//process this data but do it on the new block
		if ((ret_code = (_read_as_word(buffer, &read_pos, current_word, &word_value, &word_has_decimal)))!= e_parsing_errors::OK) return ret_code;
		//This will be useful to the end user to determine where the gcode data had an error
		NGC_RS274::LineProcessor::last_read_position = read_pos;

		//This will assign the value to the appropriate place in the block.
		//If an error occurs because there have been multiple definitions of
		//The same word in this block, it will be returned here.
		if ((Talos::Confguration::Interpreter::Parameters.dialect == e_dialects::Fanuc_A && !word_has_decimal)
			&& (current_word != 'G' && current_word != 'M'))
		{
			block->dot_safety.set((int)current_word - 'A');

			NGC_RS274::Dialect::Validate::set_fanuc_decimal_by_parameter(current_word, &word_value,
				(block->g_group[NGC_RS274::Groups::G::Units] == NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION ?
					LEAST_INPUT_INCRIMET_MM : LEAST_INPUT_INCRIMET_INCH));

		}
		//assign the word value. return an error if there is one.
		if ((ret_code = (NGC_RS274::Block_Assignor::group_word(current_word, word_value, block)))!=e_parsing_errors::OK) return ret_code;
	}
	return ret_code;
}

e_parsing_errors NGC_RS274::LineProcessor::_read_as_word
(char * buffer, int * read_pos, char word, float * word_value, bool * has_decimal)
{
	//linux cnc/emc used a function pointer to read each 'word'. We dont have the space for a 256 element
	//array here, so instead I process any letters, and then check individual characters as needed if they
	//were not one of the letter words.
	e_parsing_errors ret_code = e_parsing_errors::OK;

	if (word >= 'A' & word <= 'Z') //letter A-Z
	{
		*read_pos += 1;
		ret_code = _read_as_class_type(buffer, read_pos, word_value, has_decimal);

	}
	else if (word == '#') //parameter flag #
	{
		ret_code = NGC_RS274::LineProcessor::__read_class_parameter(buffer, read_pos, word_value, has_decimal);
	}
	else //anything else not covered above
	{
		ret_code = _read_as_class_type(buffer, read_pos, word_value, has_decimal);
	}

	return ret_code;
}

e_parsing_errors NGC_RS274::LineProcessor::_read_as_class_type(char * buffer, int * read_pos, float * word_value, bool * has_decimal)
{
	e_parsing_errors ret_code = e_parsing_errors::OK;

	e_value_class_types current_class = NGC_RS274::LineProcessor::__get_value_class(buffer, read_pos, word_value, has_decimal);

	switch (current_class)
	{
	case NGC_RS274::LineProcessor::e_value_class_types::NumericParametersNotAvailable:
	{
		ret_code = e_parsing_errors::NumericParametersNotAvailable;
		break;
	}
	case NGC_RS274::LineProcessor::e_value_class_types::NamedParametersNotAvailable:
	{
		ret_code = e_parsing_errors::NoNamedParametersAvailable;
		break;
	}
	case NGC_RS274::LineProcessor::e_value_class_types::UnSpecified:
	{
		ret_code = e_parsing_errors::UnHandledValueClass;
		break;
	}
	case NGC_RS274::LineProcessor::e_value_class_types::Expression:
	{
		ret_code = NGC_RS274::LineProcessor::__read_class_expression(buffer, read_pos, word_value, has_decimal);
		break;
	}
	case NGC_RS274::LineProcessor::e_value_class_types::Parameter:
	{
		ret_code = NGC_RS274::LineProcessor::__read_class_parameter(buffer, read_pos, word_value, has_decimal);
		break;
	}
	case NGC_RS274::LineProcessor::e_value_class_types::NamedParameterAssign:
	{
		ret_code = NGC_RS274::LineProcessor::__read_class_parameter(buffer, read_pos, word_value, has_decimal);
		break;
	}
	case NGC_RS274::LineProcessor::e_value_class_types::NumericParameterAssign:
	{
		ret_code = NGC_RS274::LineProcessor::__read_class_parameter(buffer, read_pos, word_value, has_decimal);
		break;
	}
	case NGC_RS274::LineProcessor::e_value_class_types::Unary:
	{
		ret_code = NGC_RS274::LineProcessor::__read_class_unary(buffer, read_pos, word_value, has_decimal);
		break;
	}
	case NGC_RS274::LineProcessor::e_value_class_types::Numeric:
	{
		ret_code = NGC_RS274::LineProcessor::__read_class_numeric(buffer, read_pos, word_value, has_decimal);
		break;
	}
	case NGC_RS274::LineProcessor::e_value_class_types::ClosingBracket:
	{
		//closing bracket from a unary or expression. we should be okay with this.
		ret_code = e_parsing_errors::OK;
		break;
	}
	default:
		ret_code = e_parsing_errors::UnHandledValueClass;
		break;
	}

	return ret_code;
}

NGC_RS274::LineProcessor::e_value_class_types NGC_RS274::LineProcessor::__get_value_class(char * buffer, int * read_pos, float * word_value, bool * has_decimal)
{
	e_parsing_errors ret_code = e_parsing_errors::OK;

	char current_byte = 0, next_byte = 0, previous_byte = 0;

	current_byte = buffer[*read_pos];

	if (current_byte == 0)
		return e_value_class_types::UnSpecified;

	next_byte = buffer[*read_pos + 1];

	previous_byte = *read_pos > 0 ? buffer[*read_pos - 1] : 0;

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
		return e_value_class_types::Parameter;
	}
	else if (current_byte == '+' && next_byte && !isdigit(next_byte) && next_byte != '.')
	{
		//this is most likely a unary that was signed positive
		(*read_pos)++;
		ret_code = NGC_RS274::LineProcessor::_read_as_class_type(buffer, read_pos, word_value, has_decimal);
		if (ret_code != e_parsing_errors::OK)
			return e_value_class_types::UnSpecified;
		*word_value = +*word_value;
		return NGC_RS274::LineProcessor::__get_value_class(buffer, read_pos, word_value, has_decimal);
		//return e_value_class_types::ClosingBracket;
	}
	else if (current_byte == '-' && next_byte && !isdigit(next_byte) && next_byte != '.')
	{
		//this is most likely a unary that was signed negative
		(*read_pos)++;
		ret_code = NGC_RS274::LineProcessor::_read_as_class_type(buffer, read_pos, word_value, has_decimal);
		if (ret_code != e_parsing_errors::OK)
			return e_value_class_types::UnSpecified;
		*word_value = -*word_value;
		return NGC_RS274::LineProcessor::__get_value_class(buffer, read_pos, word_value, has_decimal);
		//return e_value_class_types::ClosingBracket;
	}
	else if ((current_byte >= 'A') && (current_byte <= 'Z'))
		return e_value_class_types::Unary;
	else
	{
		return e_value_class_types::Numeric;
	}

}

e_parsing_errors NGC_RS274::LineProcessor::__read_class_unary(char * buffer, int * read_pos, float * read_value, bool * has_decimal)
{

	e_parsing_errors ret_code = e_parsing_errors::OK;

	e_unary_operator_class_types operation = e_unary_operator_class_types::NoOperation;

	ret_code = NGC_RS274::LineProcessor::__get_unary_operator_class(buffer, read_pos, &operation);
	if (ret_code != e_parsing_errors::OK)
		return ret_code;

	if (buffer[*read_pos] != '[')
	{
		ret_code = e_parsing_errors::UnaryMissingOpen;
		return ret_code;
	}
	NGC_RS274::LineProcessor::__read_class_expression(buffer, read_pos, read_value, has_decimal);

	if (operation == e_unary_operator_class_types::Atan)
		ret_code = NGC_RS274::LineProcessor::___execute_atan(buffer, read_pos, read_value, has_decimal);
	else
		ret_code = NGC_RS274::LineProcessor::___execute_unary(read_value, operation);
	return ret_code;
}

e_parsing_errors NGC_RS274::LineProcessor::__read_class_numeric(char * buffer, int * read_pos, float * read_value, bool * has_decimal)
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
	//check to see if a . was entered
	char * dot_point = strstr((buffer + (*read_pos)), ".");
	if (dot_point != NULL)
	{
		//this may need further work to impliment dot safety
		*has_decimal = true;
	}
	//convert the string portion to a number, &end gets updated to the byte counter converted
	*read_value = strtod((buffer + (*read_pos)), &end);
	//put the saved byte back so the null isnt there anymore
	(buffer + (*read_pos))[_size] = restore_byte;
	//see if end is equal to the string start point. If it is, we read nothing there
	if (end == (buffer + (*read_pos)))
	{
		return e_parsing_errors::NumericValueMissing;
	}
	//update the pointer for the reader position
	*read_pos = end - buffer;
	return e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::LineProcessor::__read_class_expression(char * buffer, int * read_pos, float * read_value, bool * has_decimal)
{
	e_parsing_errors ret_code = e_parsing_errors::OK;

	float values[MAX_EXPRESSION_OPS];
	memset(values, 0, sizeof(float)* MAX_EXPRESSION_OPS);
	e_expression_operator_class_types operators[MAX_EXPRESSION_OPS];
	memset(operators, 0, sizeof(e_expression_operator_class_types)* MAX_EXPRESSION_OPS);
	int stack_index;

	if (buffer[*read_pos] != '[')
	{
		ret_code = e_parsing_errors::IllegalCaller;
		return ret_code;
	}

	*read_pos += 1;//(*read_pos + 1);
	//read_real_value(line, counter, values, parameters);
	ret_code = NGC_RS274::LineProcessor::_read_as_class_type(buffer, read_pos, values, has_decimal);
	if (ret_code != e_parsing_errors::OK)
	{
		*read_value = values[0];
		return ret_code;
	}

	NGC_RS274::LineProcessor::__get_expression_operator_class(buffer, read_pos, operators);

	stack_index = 1;

	while (operators[0] != e_expression_operator_class_types::ClosingBracket)
	{
		ret_code = NGC_RS274::LineProcessor::_read_as_class_type(buffer, read_pos, values + stack_index, has_decimal);
		//If we dont get an ok response processing, we should be at the end of the expression
		//Otherwise its a problem.
		if (ret_code == e_parsing_errors::OK)
		{
			NGC_RS274::LineProcessor::__get_expression_operator_class(buffer, read_pos, operators + stack_index);

			if (NGC_RS274::LineProcessor::__get_operator_precedence(operators[stack_index]) > NGC_RS274::LineProcessor::__get_operator_precedence(operators[stack_index - 1]))
				stack_index++;
			else
			{
				while (NGC_RS274::LineProcessor::__get_operator_precedence(operators[stack_index]) <= NGC_RS274::LineProcessor::__get_operator_precedence(operators[stack_index - 1]))
				{
					NGC_RS274::LineProcessor::___execute_binary((values + stack_index - 1), operators[stack_index - 1], (values + stack_index));
					operators[stack_index - 1] = operators[stack_index];
					if ((stack_index > 1) && (NGC_RS274::LineProcessor::__get_operator_precedence(operators[stack_index - 1]) <= NGC_RS274::LineProcessor::__get_operator_precedence(operators[stack_index - 2])))
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

e_parsing_errors NGC_RS274::LineProcessor::__read_class_parameter(char * buffer, int * read_pos, float * read_value, bool * has_decimal)
{
	e_parsing_errors ret_code = e_parsing_errors::OK;

	int index = 0;

	if (buffer[*read_pos] != '#')
	{
		ret_code = e_parsing_errors::IllegalCaller;
		return ret_code;
	}

	*read_pos += 1;//(*read_pos + 1);

	// named parameters look like '<letter...>' or '<_.....>'
	if (buffer[*read_pos] == '<')
	{
		//this will be a named parameter of local or global scope
		ret_code = NGC_RS274::LineProcessor::___read_class_named_parameter(buffer, read_pos, read_value, has_decimal);
		if (ret_code != e_parsing_errors::OK)
			return ret_code;

	}
	else
	{

		if (!max_numeric_parameter_count)//<--if we dont have max_numeric params counter, get it
			max_numeric_parameter_count = NGC_RS274::Parameters::__get_numeric_parameter_max();

		NGC_RS274::LineProcessor::___read_integer_value(buffer, read_pos, &index, has_decimal);
		if (buffer[*read_pos] == '=')//<--write param value
		{
			*read_pos += 1;
			ret_code = NGC_RS274::LineProcessor::_read_as_class_type(buffer, read_pos, read_value, has_decimal);
			if (ret_code != e_parsing_errors::OK)
				return ret_code;

			ret_code = e_parsing_errors::NumericParamaterUpdateUnavailable;
		}
		else//<--read param value
		{
			if (index < 1 || index >= max_numeric_parameter_count)
			{
				ret_code = e_parsing_errors::ParamaterValueOutOfRange;
				*read_value = index;
			}
			else
			{
				*read_value = NGC_RS274::Parameters::__get_numeric_parameter(index);// FAKE_NUMERIC_PARAM_VALUE;
			}
		}

	}
	return ret_code;
}

e_parsing_errors NGC_RS274::LineProcessor::___read_integer_value(char *buffer, int *read_pos, int *integer_value, bool * has_decimal)
{
	e_parsing_errors ret_code = e_parsing_errors::OK;

	float float_value = 0;

	ret_code = NGC_RS274::LineProcessor::_read_as_class_type(buffer, read_pos, &float_value, has_decimal);
	if (ret_code != e_parsing_errors::OK)
		return ret_code;

	*integer_value = (int)floor(float_value);
	if ((float_value - *integer_value) > NEAR_ONE)
	{
		*integer_value = (int)ceil(float_value);
	}
	else if ((float_value - *integer_value) > NEAR_ZERO)
		ret_code = e_parsing_errors::IntExpectedAtValue;
	return ret_code;
}

e_parsing_errors NGC_RS274::LineProcessor::___read_class_named_parameter(char * buffer, int * read_pos, float * read_value, bool * has_decimal)
{
	e_parsing_errors ret_code = e_parsing_errors::OK;

	if (buffer[*read_pos] != '<')
	{
		ret_code = e_parsing_errors::IllegalCaller;
		return ret_code;
	}

	//find the name of the parameter, lookit up and return its value in the read_value pointer
	char parameter_name[MAX_NAMED_PARAM_LENGTH + 1];
	memset(parameter_name, 0, MAX_NAMED_PARAM_LENGTH);

	NGC_RS274::LineProcessor::____get_named_parameter_name(buffer, read_pos, parameter_name);

	bool update = false;
	if (buffer[*read_pos] == '=')//<--read or write
	{
		update = true;
		*read_pos += 1;
		ret_code = NGC_RS274::LineProcessor::_read_as_class_type(buffer, read_pos, read_value, has_decimal);
		if (ret_code != e_parsing_errors::OK)
			return ret_code;
	}

	bool global = (parameter_name[0] == '_');//<--global scope

	if (update)//<--write param value
	{
		if (global)
		{
			if (!NGC_RS274::Parameters::__set_named_gobal_parameter(parameter_name, *read_value))
			{
				ret_code = e_parsing_errors::NamedParamaterUpdateFailure;
			}
		}
		else
		{
			if (!NGC_RS274::Parameters::__set_named_local_parameter(parameter_name, *read_value))
			{
				ret_code = e_parsing_errors::NamedParamaterUpdateFailure;
			}
		}
	}
	else
	{
		//Look in the name we got back from the read. If it starts with _ its a 'global'
		if (global)//<--global scope
		{
			*read_value = NGC_RS274::Parameters::__get_named_gobal_parameter(parameter_name);
		}
		else//<--not global, so its local scope
		{
			*read_value = NGC_RS274::Parameters::__get_named_local_parameter(parameter_name);
		}
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

e_parsing_errors NGC_RS274::LineProcessor::___execute_atan(char *buffer, int *read_pos, float *read_value, bool * has_decimal)
{
	e_parsing_errors ret_code = e_parsing_errors::OK;
	float argument2 = 0;

	if (buffer[*read_pos] != '/')
	{
		ret_code = e_parsing_errors::MissingSlashIn_ATAN;
		return ret_code;
	}
	*read_pos += 1;//(*read_pos + 1);
	if (buffer[*read_pos] != '[')
	{
		ret_code = e_parsing_errors::MissingBracketAfterSlash;
	}
	NGC_RS274::LineProcessor::__read_class_expression(buffer, read_pos, &argument2, has_decimal);
	*read_value = atan2(*read_value, argument2);  /* value in radians */
	*read_value = ((*read_value * 180.0) / M_PI);   /* convert to degrees */
	return ret_code;
}

e_parsing_errors NGC_RS274::LineProcessor::____get_named_parameter_name(char * buffer, int * read_pos, char * parameter_name)
{
	bool done = false;
	e_parsing_errors ret_code = e_parsing_errors::OK;

	if (buffer[*read_pos] != '<' && !isalpha(buffer[*(read_pos)]))
	{
		ret_code = e_parsing_errors::IllegalCaller;
		return ret_code;
	}

	// skip over the '<'
	*read_pos += 1;//(*read_pos + 1);

	for (int i = 0; (i < MAX_NAMED_PARAM_LENGTH) && (buffer[*read_pos]); i++)
	{
		if (buffer[*read_pos] == '>')
		{
			parameter_name[i] = 0; // terminate the name
			*read_pos += 1; //(*read_pos + 1);
			done = 1;
			break;
		}
		parameter_name[i] = buffer[*read_pos];
		*read_pos += 1;//(*read_pos + 1);
	}

	// !!!KL need to rename the error message and change text
	if (!done)
		ret_code = e_parsing_errors::ParameterNameNotClosed;

	return ret_code;
}

e_parsing_errors NGC_RS274::LineProcessor::__get_unary_operator_class(char * buffer, int * read_pos, e_unary_operator_class_types * operator_class)
{
	e_parsing_errors ret_code = e_parsing_errors::OK;

	char c = buffer[*read_pos];
	*read_pos += 1; //(*read_pos + 1);
	switch (c) {
	case 'A':
		if ((buffer[*read_pos] == 'B') && (buffer[(*read_pos) + 1] == 'S')) {
			*operator_class = e_unary_operator_class_types::Abs;
			*read_pos += 2;//(*read_pos + 2);
		}
		else if (strncmp((buffer + *read_pos), "COS", 3) == 0) {
			*operator_class = e_unary_operator_class_types::Acos;
			*read_pos += 3;//(*read_pos + 3);
		}
		else if (strncmp((buffer + *read_pos), "SIN", 3) == 0) {
			*operator_class = e_unary_operator_class_types::Asin;
			*read_pos += 3;//(*read_pos + 3);
		}
		else if (strncmp((buffer + *read_pos), "TAN", 3) == 0) {
			*operator_class = e_unary_operator_class_types::Atan;
			*read_pos += 3;//(*read_pos + 3);
		}
		else
			ret_code = e_parsing_errors::Unknown_Op_Name_A;
		break;
	case 'C':
		if ((buffer[*read_pos] == 'O') && (buffer[(*read_pos) + 1] == 'C')) {
			*operator_class = e_unary_operator_class_types::Cos;
			*read_pos += 2;//(*read_pos + 2);
		}
		else
			ret_code = e_parsing_errors::Unknown_Op_Name_C;
		break;
	case 'E':
		if ((buffer[*read_pos] == 'X') && (buffer[(*read_pos) + 1] == 'P')) {
			*operator_class = e_unary_operator_class_types::Exp;
			*read_pos += 2;//(*read_pos + 2);
		}
		else
			ret_code = e_parsing_errors::Unknown_Op_Name_E;
		break;
	case 'F':
		if ((buffer[*read_pos] == 'I') && (buffer[(*read_pos) + 1] == 'X')) {
			*operator_class = e_unary_operator_class_types::Fix;
			*read_pos += 2;//(*read_pos + 2);
		}
		else if ((buffer[*read_pos] == 'U') && (buffer[(*read_pos) + 1] == 'P')) {
			*operator_class = e_unary_operator_class_types::Fup;
			*read_pos += 2;//(*read_pos + 2);
		}
		else
			ret_code = e_parsing_errors::Unknown_Op_Name_F;
		break;
	case 'L':
		if (buffer[*read_pos] == 'L') {
			*operator_class = e_unary_operator_class_types::Ln;
			*read_pos += 1;//(*read_pos + 1);
		}
		else
			ret_code = e_parsing_errors::Unknown_Op_Name_L;
		break;
	case 'R':
		if (strncmp((buffer + *read_pos), "OUND", 4) == 0) {
			*operator_class = e_unary_operator_class_types::Round;
			*read_pos += 4;//(*read_pos + 4);
		}
		else
			ret_code = e_parsing_errors::Unknown_Op_Name_R;
		break;
	case 'S':
		if ((buffer[*read_pos] == 'I') && (buffer[(*read_pos) + 1] == 'N')) {
			*operator_class = e_unary_operator_class_types::Sin;
			*read_pos += 2;//(*read_pos + 2);
		}
		else if (strncmp((buffer + *read_pos), "QRT", 3) == 0) {
			*operator_class = e_unary_operator_class_types::Sqrt;
			*read_pos += 3;//(*read_pos + 3);
		}
		else
			ret_code = e_parsing_errors::Unknown_Op_Name_S;
		break;
	case 'T':
		if ((buffer[*read_pos] == 'A') && (buffer[(*read_pos) + 1] == 'N')) {
			*operator_class = e_unary_operator_class_types::Tan;
			*read_pos += 2;//(*read_pos + 2);
		}
		else
			ret_code = e_parsing_errors::Unknown_Op_Name_T;
		break;
	default:
		ret_code = e_parsing_errors::UnknownOperationClass;
	}
	return ret_code;
}

e_parsing_errors NGC_RS274::LineProcessor::__get_expression_operator_class(char * buffer, int * read_pos, e_expression_operator_class_types * operator_class)
{
	char c = 0;

	c = buffer[*read_pos];
	*read_pos += 1;//(*read_pos + 1);
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
			*read_pos += 1;//(*read_pos + 1);
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
			*read_pos += 2;//(*read_pos + 2);
		}
		else
			return e_parsing_errors::Unknown_Op_Name_A;
		break;
	case 'M':
		if ((buffer[*read_pos] == 'O') && (buffer[(*read_pos) + 1] == 'D')) {
			*operator_class = e_expression_operator_class_types::Modulus;
			*read_pos += 2;//(*read_pos + 2);
		}
		else
			e_parsing_errors::Unknown_Op_Name_M;
		break;
	case 'O':
		if (buffer[*read_pos] == 'R') {
			*operator_class = e_expression_operator_class_types::Or;
			*read_pos += 1;//(*read_pos + 1);
		}
		else
			e_parsing_errors::Unknown_Op_Name_O;
		break;
	case 'X':
		if ((buffer[*read_pos] == 'O') && (buffer[(*read_pos) + 1] == 'R')) {
			*operator_class = e_expression_operator_class_types::Ex_Or;
			*read_pos += 2;//(*read_pos + 2);
		}
		else
			e_parsing_errors::Unknown_Op_Name_X;
		break;

		/* relational operators */
	case 'E':
		if (buffer[*read_pos] == 'Q')
		{
			*operator_class = e_expression_operator_class_types::_eq;
			*read_pos += 1;//(*read_pos + 1);
		}
		else
			e_parsing_errors::Unknown_Op_Name_E;
		break;
	case 'N':
		if (buffer[*read_pos] == 'E')
		{
			*operator_class = e_expression_operator_class_types::_ne;
			*read_pos += 1;//(*read_pos + 1);
		}
		else
			e_parsing_errors::Unknown_Op_Name_N;
		break;
	case 'G':
		if (buffer[*read_pos] == 'E')
		{
			*operator_class = e_expression_operator_class_types::_ge;
			*read_pos += 1;//(*read_pos + 1);
		}
		else if (buffer[*read_pos] == 'T')
		{
			*operator_class = e_expression_operator_class_types::_gt;
			*read_pos += 1;//(*read_pos + 1);
		}
		else
			e_parsing_errors::Unknown_Op_Name_G;
		break;
	case 'L':
		if (buffer[*read_pos] == 'E')
		{
			*operator_class = e_expression_operator_class_types::_le;
			*read_pos += 1;//(*read_pos + 1);
		}
		else if (buffer[*read_pos] == 'T')
		{
			*operator_class = e_expression_operator_class_types::_lt;
			*read_pos += 1;//(*read_pos + 1);
		}
		else
			e_parsing_errors::Unknown_Op_Name_L;
		break;

	case 0:
		e_parsing_errors::ExpressionNotClosed;
		break;
	default:
		e_parsing_errors::UnknownOperationClass;
	}
	return e_parsing_errors::OK;
}

int NGC_RS274::LineProcessor::__get_operator_precedence(e_expression_operator_class_types operator_class)
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

e_parsing_errors NGC_RS274::LineProcessor::___execute_unary(float *read_value, e_unary_operator_class_types operator_class)
{
	e_parsing_errors ret_code = e_parsing_errors::OK;

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
			return e_parsing_errors::ArcCosValueOutOfRange;
		}
		*read_value = acos(*read_value);
		*read_value = ((*read_value * 180.0) / M_PI);
		break;
	}
	case e_unary_operator_class_types::Asin:
	{
		if (*read_value < -1.0 || *read_value > 1.0)
		{
			ret_code = e_parsing_errors::ArcSinValueOutOfRange;
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
			ret_code = e_parsing_errors::LogValueNegative;
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
			ret_code = e_parsing_errors::SqrtValueNegative;
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

e_parsing_errors NGC_RS274::LineProcessor::___execute_binary(float *left, e_expression_operator_class_types operator_class, float *right)
{
	e_parsing_errors ret_code = e_parsing_errors::OK;
	float diff = 0;

	switch (operator_class) {
	case e_expression_operator_class_types::Division:
		if (*right == 0.0)
		{
			ret_code = e_parsing_errors::DivideByZero;
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
			ret_code = e_parsing_errors::ExponentOnNegativeNonInteger;
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
		ret_code = e_parsing_errors::UnknownOperationClass;
	}
	return ret_code;
}