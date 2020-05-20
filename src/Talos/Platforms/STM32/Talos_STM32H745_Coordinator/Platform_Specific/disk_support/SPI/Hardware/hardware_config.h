#ifndef __SPI_HARDWARE_CONFIG_H__
#define __SPI_HARDWARE_CONFIG_H__
#include <stdint.h>
#include <stm32h7xx_hal.h>
#include <stm32_hal_legacy.h>

#ifdef __cplusplus
extern "C" {
#endif

SPI_HandleTypeDef cfg_spi_handler;

void HW_config(SPI_TypeDef* SPIxInstance);
void HW_config_gpio();
void HW_config_spi_handler(SPI_TypeDef* SPIxInstance);
uint32_t HW_config_spi_baud_prescaler(uint32_t clock_src_freq, uint32_t baudrate_mbps);

#ifdef __cplusplus
}
#endif

#endif // !__SPI_HARDWARE_CONFIG_H__