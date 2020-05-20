#ifndef __SPI_HARDWARE_IO_H__
#define __SPI_HARDWARE_IO_H__
#include <stm32h7xx_hal.h>
#include <stm32_hal_legacy.h>
#include <stdint.h>

#define HARDWARE_IO_OK 0
#define HARDWARE_IO_ERROR 1
#define HARDWARE_IO_COMM_ERROR 2
#define HARDWARE_IO_NEGOTIATION_ERROR 3
#define HARDWARE_IO_BUSY_ERROR 4
#define HARDWARE_IO_UNKNOWN_ERROR 5

#define HW_IO_SET_CS_LOW()       HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET)
#define HW_IO_SET_CS_HIGH()      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET)

#ifdef __cplusplus
extern "C" {
#endif
	int32_t HW_IO_SPI_send(uint8_t* pTxData, uint32_t Legnth);
	int32_t HW_IO_SPI_recv(uint8_t* pRxData, uint32_t Legnth);
	int32_t HW_IO_SPI_sendrecv(uint8_t* pTxData, uint8_t* pRxData, uint32_t Length);
	int32_t HW_IO_SPI_gettick(void);
#ifdef __cplusplus
		}
#endif
#endif // !__SPI_HARDWARE_IO_H__