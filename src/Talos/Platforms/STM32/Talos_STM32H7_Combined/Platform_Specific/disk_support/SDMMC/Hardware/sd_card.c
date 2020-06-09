#include "sd_card.h"

extern SD_HandleTypeDef hsd1;

int32_t SD_IO_Init(uint8_t * nuller)
{
	MX_FATFS_Init();
	FRESULT res = FR_OK;
	res = (FRESULT)HAL_SD_Init(&hsd1);
}
