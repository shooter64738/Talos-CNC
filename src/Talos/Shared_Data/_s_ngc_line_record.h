#ifndef __C_NGC_LINE_RECORD
#define __C_NGC_LINE_RECORD
#include <stdint.h>
#include <stddef.h>
struct s_txt_line_record
{
	char record[256];
	char * pntr_record = NULL;
	uint8_t size;
	uint8_t target;
};
#endif