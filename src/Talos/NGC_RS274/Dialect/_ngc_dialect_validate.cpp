#include "_ngc_dialect_validate.h"
#include "../../Configuration/c_configuration.h"

//char NGC_RS274::Dialect::Validate::validation_params[MAX_PARAMETER_VALIDATION_COUNT];
//int NGC_RS274::Dialect::Validate::param_count;
//
//int NGC_RS274::Dialect::Validate::_Param_Entered_Test(s_ngc_block * block, char * params, int param_count)
//{
//	for (int i = 0; i < param_count; i++)
//		if (!_Param_Entered_Test(block, (*(params + i))))
//			//parameter wasnt defined. let the caller know which one
//			return i + 1;
//
//	return 0;
//}
//
//bool NGC_RS274::Dialect::Validate::_Param_Entered_Test(s_ngc_block * block, char word)
//{
//	return (!block->word_flags.get(_get_word_index(word)));
//}
//
//int NGC_RS274::Dialect::Validate::_get_word_index(uint8_t word_value)
//{
//	return word_value - 'A';
//}

void NGC_RS274::Dialect::Validate::set_fanuc_decimal_by_parameter(uint8_t word, float * word_value, float factor)
{
	float multiplier = Talos::Configuration::Interpreter::Parameters.Settings.input_process.flags.get(e_config_bit_flags::DecimalPointInput) ? factor : 1;

	//XYZUVWABCIJKQRF - mill words
	//XYZUVWABCIJKRF - turn words
	//ABC  F  IJKL    QR  UVWXYZ - excluded
	//   DE GH    MNOP  ST - do not reformat these word values
	uint8_t turn = 0;
	if (word != 'D' && word != 'E'
		&& word != 'G' && word != 'H'
		&& word != 'M' && word != 'N'
		&& word != 'O' && word != 'P'
		&& word != 'S' && word != 'T'
		|| (word != 'Q' && turn))
	{
		*word_value *= multiplier;
	}

}
