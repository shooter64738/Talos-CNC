#ifndef _SD_MMC_H_
#define _SD_MMC_H_

#include <stdint.h>
#include "../FatFs/fatfs.h"
#include "../../../sys_sdmmc_configs.h"

#ifdef __cplusplus
extern "C" {
#endif

	int32_t SD_IO_Init(uint8_t * nuller);

#ifdef __cplusplus
}
#endif
#endif