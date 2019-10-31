
#include <stdint.h>
//#include "c_input.h"
struct s_data_pointers
{
	uint8_t *_text;
	uint8_t *_binary;
	s_Buffer * _buffer_pointer;

};

//I just learned this nifty macro trick for externs.. im trying it out to see if i like this coding style..
#ifdef __DATA_POINTERS__
s_data_pointers extern_data_pointers;

#else
extern s_data_pointers extern_data_pointers;

#endif
