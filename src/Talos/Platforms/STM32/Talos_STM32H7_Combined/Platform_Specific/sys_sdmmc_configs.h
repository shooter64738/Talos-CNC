#ifndef _C_SYS_SDMMC1_CONFIG_
#define _C_SYS_SDMMC1_CONFIG_
#include <stm32h7xx_hal.h>
#include <stm32_hal_legacy.h>

extern SD_HandleTypeDef hsd1;

static void sd_mmc1_config(void)
{
	
	__HAL_RCC_SDMMC1_CLK_ENABLE();
	hsd1.Instance = SDMMC1;
	hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
	hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
	hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B; //SDMMC_BUS_WIDE_1B;
	hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_ENABLE;
	hsd1.Init.ClockDiv = 8;
	hsd1.Init.TranceiverPresent = SDMMC_TRANSCEIVER_NOT_PRESENT;

	//HAL_SD_Init(&hsd1);
}

#endif