#ifndef __C_NGC_LINE_RECORD
#define __C_NGC_LINE_RECORD
#include <stdint.h>
struct s_ngc_line_record
{
	char record[256];
	char * pntr_record = record;
	uint8_t size;
};
#endif