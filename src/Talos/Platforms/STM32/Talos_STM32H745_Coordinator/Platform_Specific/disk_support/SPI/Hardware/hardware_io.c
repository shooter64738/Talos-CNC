#include "hardware_io.h"
#include "hardware_config.h"



#define SPI_TIMEOUT 1000

int32_t HW_IO_SPI_send(uint8_t * pTxData, uint32_t Length)
{
	int32_t ret = HARDWARE_IO_OK;

	if (HAL_SPI_Transmit(&cfg_spi_handler, pTxData, (uint16_t)Length, SPI_TIMEOUT) != HAL_OK)
	{
		ret = HARDWARE_IO_ERROR;
	}

	return ret;
}

int32_t HW_IO_SPI_recv(uint8_t* pRxData, uint32_t Length)
{
	int32_t ret = HARDWARE_IO_OK;

	if (HAL_SPI_Receive(&cfg_spi_handler, pRxData, (uint16_t)Length, SPI_TIMEOUT) != HAL_OK)
	{
		ret = HARDWARE_IO_ERROR;
	}

	return ret;
}

/*
I found some issues using the HAL spi tx rx method. Mostly the rx was nto waiting for data before moving on
I have simply moved my own version of those same routines here local so I can change them.
*/
static HAL_StatusTypeDef __spi_waitonflaguntiltimeout(SPI_HandleTypeDef* hspi, uint32_t Flag, FlagStatus Status,
	uint32_t Tickstart, uint32_t Timeout)
{
	/* Wait until flag is set */
	while ((__HAL_SPI_GET_FLAG(hspi, Flag) ? SET : RESET) == Status)
	{
		/* Check for the Timeout */
		if ((((HAL_GetTick() - Tickstart) >= Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
		{
			return HAL_TIMEOUT;
		}
	}
	return HAL_OK;
}
static void __spi_closetransfer(SPI_HandleTypeDef* hspi)
{
	uint32_t itflag = hspi->Instance->SR;

	__HAL_SPI_CLEAR_EOTFLAG(hspi);
	__HAL_SPI_CLEAR_TXTFFLAG(hspi);

	/* Disable SPI peripheral */
	__HAL_SPI_DISABLE(hspi);

	/* Disable ITs */
	__HAL_SPI_DISABLE_IT(hspi, (SPI_IT_EOT | SPI_IT_TXP | SPI_IT_RXP | SPI_IT_DXP | SPI_IT_UDR | SPI_IT_OVR | SPI_IT_FRE | SPI_IT_MODF));

	/* Disable Tx DMA Request */
	CLEAR_BIT(hspi->Instance->CFG1, SPI_CFG1_TXDMAEN | SPI_CFG1_RXDMAEN);

	/* Report UnderRun error for non RX Only communication */
	if (hspi->State != HAL_SPI_STATE_BUSY_RX)
	{
		if ((itflag & SPI_FLAG_UDR) != 0UL)
		{
			SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_UDR);
			__HAL_SPI_CLEAR_UDRFLAG(hspi);
		}
	}

	/* Report OverRun error for non TX Only communication */
	if (hspi->State != HAL_SPI_STATE_BUSY_TX)
	{
		if ((itflag & SPI_FLAG_OVR) != 0UL)
		{
			SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_OVR);
			__HAL_SPI_CLEAR_OVRFLAG(hspi);
		}

#if (USE_SPI_CRC != 0UL)
		/* Check if CRC error occurred */
		if (hspi->Init.CRCCalculation == SPI_CRCCALCULATION_ENABLE)
		{
			if ((itflag & SPI_FLAG_CRCERR) != 0UL)
			{
				SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_CRC);
				__HAL_SPI_CLEAR_CRCERRFLAG(hspi);
			}
		}
#endif /* USE_SPI_CRC */
	}

	/* SPI Mode Fault error interrupt occurred -------------------------------*/
	if ((itflag & SPI_FLAG_MODF) != 0UL)
	{
		SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_MODF);
		__HAL_SPI_CLEAR_MODFFLAG(hspi);
	}

	/* SPI Frame error interrupt occurred ------------------------------------*/
	if ((itflag & SPI_FLAG_FRE) != 0UL)
	{
		SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FRE);
		__HAL_SPI_CLEAR_FREFLAG(hspi);
	}

	hspi->TxXferCount = (uint16_t)0UL;
	hspi->RxXferCount = (uint16_t)0UL;
}
static HAL_StatusTypeDef __spi_tx_rx(SPI_HandleTypeDef* hspi, uint8_t* pTxData, uint8_t* pRxData, uint16_t Size,
	uint32_t Timeout)
{
	HAL_SPI_StateTypeDef tmp_state;
	HAL_StatusTypeDef errorcode = HAL_OK;
#if defined(__GNUC__)
	__IO uint16_t* ptxdr_16bits = (__IO uint16_t*)(&(hspi->Instance->TXDR));
	__IO uint16_t* prxdr_16bits = (__IO uint16_t*)(&(hspi->Instance->RXDR));
#endif /* __GNUC__ */

	uint32_t tickstart;
	uint32_t tmp_mode;
	uint16_t initial_TxXferCount;
	uint16_t initial_RxXferCount;
	uint8_t has_read = 0;

	/* Check Direction parameter */
	assert_param(IS_SPI_DIRECTION_2LINES(hspi->Init.Direction));

	/* Process Locked */
	__HAL_LOCK(hspi);

	/* Init tickstart for timeout management*/
	tickstart = HAL_GetTick();

	initial_TxXferCount = Size;
	initial_RxXferCount = Size;
	tmp_state = hspi->State;
	tmp_mode = hspi->Init.Mode;

	if (!((tmp_state == HAL_SPI_STATE_READY) ||
		((tmp_mode == SPI_MODE_MASTER) && (hspi->Init.Direction == SPI_DIRECTION_2LINES) && (tmp_state == HAL_SPI_STATE_BUSY_RX))))
	{
		errorcode = HAL_BUSY;
		__HAL_UNLOCK(hspi);
		return errorcode;
	}

	if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0UL))
	{
		errorcode = HAL_ERROR;
		__HAL_UNLOCK(hspi);
		return errorcode;
	}

	/* Don't overwrite in case of HAL_SPI_STATE_BUSY_RX */
	if (hspi->State != HAL_SPI_STATE_BUSY_RX)
	{
		hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
	}

	/* Set the transaction information */
	hspi->ErrorCode = HAL_SPI_ERROR_NONE;
	hspi->pRxBuffPtr = (uint8_t*)pRxData;
	hspi->RxXferCount = Size;
	hspi->RxXferSize = Size;
	hspi->pTxBuffPtr = (uint8_t*)pTxData;
	hspi->TxXferCount = Size;
	hspi->TxXferSize = Size;

	/*Init field not used in handle to zero */
	hspi->RxISR = NULL;
	hspi->TxISR = NULL;

	/* Set the number of data at current transfer */
	MODIFY_REG(hspi->Instance->CR2, SPI_CR2_TSIZE, Size);

	__HAL_SPI_ENABLE(hspi);

	if (hspi->Init.Mode == SPI_MODE_MASTER)
	{
		/* Master transfer start */
		SET_BIT(hspi->Instance->CR1, SPI_CR1_CSTART);
	}

	/* Transmit and Receive data in 32 Bit mode */
	if (hspi->Init.DataSize > SPI_DATASIZE_16BIT)
	{
		while ((initial_TxXferCount > 0UL) || (initial_RxXferCount > 0UL))
		{
			/* Check TXP flag */
			if ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP)) && (initial_TxXferCount > 0UL))
			{
				*((__IO uint32_t*) & hspi->Instance->TXDR) = *((uint32_t*)hspi->pTxBuffPtr);
				hspi->pTxBuffPtr += sizeof(uint32_t);
				hspi->TxXferCount--;
				initial_TxXferCount = hspi->TxXferCount;
			}

			/* Check RXWNE/EOT flag */
			has_read = 0;
			while (has_read == 0 && ((HAL_GetTick() - tickstart) < Timeout))
			{
				if (((hspi->Instance->SR & (SPI_FLAG_RXWNE | SPI_FLAG_EOT)) != 0UL) && (initial_RxXferCount > 0UL))
				{
					*((uint32_t*)hspi->pRxBuffPtr) = *((__IO uint32_t*) & hspi->Instance->RXDR);
					hspi->pRxBuffPtr += sizeof(uint32_t);
					hspi->RxXferCount--;
					initial_RxXferCount = hspi->RxXferCount;
					has_read = 1;
				}
			}
		}

		/* Timeout management */
		if ((((HAL_GetTick() - tickstart) >= Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
		{
			/* Call standard close procedure with error check */
			__spi_closetransfer(hspi);

			/* Process Unlocked */
			__HAL_UNLOCK(hspi);

			SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
			hspi->State = HAL_SPI_STATE_READY;
			return HAL_ERROR;
		}
	}
	/* Transmit and Receive data in 16 Bit mode */
	else if (hspi->Init.DataSize > SPI_DATASIZE_8BIT)
	{
		while ((initial_TxXferCount > 0UL) || (initial_RxXferCount > 0UL))
		{
			/* Check TXP flag */
			if (__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP) && (initial_TxXferCount > 0UL))
			{
				if ((initial_TxXferCount > 1UL) && (hspi->Init.FifoThreshold > SPI_FIFO_THRESHOLD_01DATA))
				{
					*((__IO uint32_t*) & hspi->Instance->TXDR) = *((uint32_t*)hspi->pTxBuffPtr);
					hspi->pTxBuffPtr += sizeof(uint32_t);
					hspi->TxXferCount -= (uint16_t)2UL;
					initial_TxXferCount = hspi->TxXferCount;
				}
				else
				{
#if defined(__GNUC__)
					* ptxdr_16bits = *((uint16_t*)hspi->pTxBuffPtr);
#else
					* ((__IO uint16_t*) & hspi->Instance->TXDR) = *((uint16_t*)hspi->pTxBuffPtr);
#endif /* __GNUC__ */
					hspi->pTxBuffPtr += sizeof(uint16_t);
					hspi->TxXferCount--;
					initial_TxXferCount = hspi->TxXferCount;
				}
			}

			/* Check RXWNE/FRLVL flag */
			has_read = 0;
			while (has_read == 0 && ((HAL_GetTick() - tickstart) < Timeout))
			{
				if (((hspi->Instance->SR & (SPI_FLAG_RXWNE | SPI_FLAG_FRLVL)) != 0UL) && (initial_RxXferCount > 0UL))
				{
					if ((hspi->Instance->SR & SPI_FLAG_RXWNE) != 0UL)
					{
						*((uint32_t*)hspi->pRxBuffPtr) = *((__IO uint32_t*) & hspi->Instance->RXDR);
						hspi->pRxBuffPtr += sizeof(uint32_t);
						hspi->RxXferCount -= (uint16_t)2UL;
						initial_RxXferCount = hspi->RxXferCount;
						has_read = 1;
					}
					else
					{
#if defined(__GNUC__)
						* ((uint16_t*)hspi->pRxBuffPtr) = *prxdr_16bits;
#else
						* ((uint16_t*)hspi->pRxBuffPtr) = *((__IO uint16_t*) & hspi->Instance->RXDR);
#endif /* __GNUC__ */
						hspi->pRxBuffPtr += sizeof(uint16_t);
						hspi->RxXferCount--;
						initial_RxXferCount = hspi->RxXferCount;
						has_read = 1;
					}
				}
			}

			/* Timeout management */
			if ((((HAL_GetTick() - tickstart) >= Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
			{
				/* Call standard close procedure with error check */
				__spi_closetransfer(hspi);

				/* Process Unlocked */
				__HAL_UNLOCK(hspi);

				SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
				hspi->State = HAL_SPI_STATE_READY;
				return HAL_ERROR;
			}
		}
	}
	/* Transmit and Receive data in 8 Bit mode */
	else
	{
		while ((initial_TxXferCount > 0UL) || (initial_RxXferCount > 0UL))
		{
			/* check TXP flag */
			if ((__HAL_SPI_GET_FLAG(hspi, SPI_FLAG_TXP)) && (initial_TxXferCount > 0UL))
			{
				if ((initial_TxXferCount > 3UL) && (hspi->Init.FifoThreshold > SPI_FIFO_THRESHOLD_03DATA))
				{
					*((__IO uint32_t*) & hspi->Instance->TXDR) = *((uint32_t*)hspi->pTxBuffPtr);
					hspi->pTxBuffPtr += sizeof(uint32_t);
					hspi->TxXferCount -= (uint16_t)4UL;
					initial_TxXferCount = hspi->TxXferCount;
				}
				else if ((initial_TxXferCount > 1UL) && (hspi->Init.FifoThreshold > SPI_FIFO_THRESHOLD_01DATA))
				{
#if defined(__GNUC__)
					* ptxdr_16bits = *((uint16_t*)hspi->pTxBuffPtr);
#else
					* ((__IO uint16_t*) & hspi->Instance->TXDR) = *((uint16_t*)hspi->pTxBuffPtr);
#endif /* __GNUC__ */
					hspi->pTxBuffPtr += sizeof(uint16_t);
					hspi->TxXferCount -= (uint16_t)2UL;
					initial_TxXferCount = hspi->TxXferCount;
				}
				else
				{
					*((__IO uint8_t*) & hspi->Instance->TXDR) = *((uint8_t*)hspi->pTxBuffPtr);
					hspi->pTxBuffPtr += sizeof(uint8_t);
					hspi->TxXferCount--;
					initial_TxXferCount = hspi->TxXferCount;
				}
			}

			/* Wait until RXWNE/FRLVL flag is reset */
			has_read = 0;
			while (has_read == 0 && ((HAL_GetTick() - tickstart) < Timeout))
			{
				if (((hspi->Instance->SR & (SPI_FLAG_RXWNE | SPI_FLAG_FRLVL)) != 0UL) && (initial_RxXferCount > 0UL))
				{
					if ((hspi->Instance->SR & SPI_FLAG_RXWNE) != 0UL)
					{
						*((uint32_t*)hspi->pRxBuffPtr) = *((__IO uint32_t*) & hspi->Instance->RXDR);
						hspi->pRxBuffPtr += sizeof(uint32_t);
						hspi->RxXferCount -= (uint16_t)4UL;
						initial_RxXferCount = hspi->RxXferCount;
						has_read = 1;

					}
					else if ((hspi->Instance->SR & SPI_FLAG_FRLVL) > SPI_RX_FIFO_1PACKET)
					{
#if defined(__GNUC__)
						* ((uint16_t*)hspi->pRxBuffPtr) = *prxdr_16bits;
#else
						* ((uint16_t*)hspi->pRxBuffPtr) = *((__IO uint16_t*) & hspi->Instance->RXDR);
#endif /* __GNUC__ */
						hspi->pRxBuffPtr += sizeof(uint16_t);
						hspi->RxXferCount -= (uint16_t)2UL;
						initial_RxXferCount = hspi->RxXferCount;
						has_read = 1;

					}
					else
					{
						*((uint8_t*)hspi->pRxBuffPtr) = *((__IO uint8_t*) & hspi->Instance->RXDR);
						hspi->pRxBuffPtr += sizeof(uint8_t);
						hspi->RxXferCount--;
						initial_RxXferCount = hspi->RxXferCount;
						has_read = 1;
					}
				}
			}

			/* Timeout management */
			if ((((HAL_GetTick() - tickstart) >= Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
			{
				/* Call standard close procedure with error check */
				__spi_closetransfer(hspi);

				/* Process Unlocked */
				__HAL_UNLOCK(hspi);

				SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TIMEOUT);
				hspi->State = HAL_SPI_STATE_READY;
				return HAL_ERROR;
			}
		}
	}

	/* Wait for Tx/Rx (and CRC) data to be sent/received */
	if (__spi_waitonflaguntiltimeout(hspi, SPI_FLAG_EOT, RESET, tickstart, Timeout) != HAL_OK)
	{
		SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_FLAG);
	}

	/* Call standard close procedure with error check */
	__spi_closetransfer(hspi);

	/* Process Unlocked */
	__HAL_UNLOCK(hspi);

	hspi->State = HAL_SPI_STATE_READY;

	if (hspi->ErrorCode != HAL_SPI_ERROR_NONE)
	{
		return HAL_ERROR;
	}
	return errorcode;
}

int32_t HW_IO_SPI_sendrecv(uint8_t* pTxData, uint8_t* pRxData, uint32_t Length)
{
	int32_t ret = HARDWARE_IO_OK;
	/*if (HAL_SPI_TransmitReceive(&cfg_spi_handler, pTxData, pRxData, (uint16_t)Length, SPI_TIMEOUT) != HAL_OK)
	{
		ret = HARDWARE_IO_ERROR;
	}*/

	if (__spi_tx_rx(&cfg_spi_handler, pTxData, pRxData, (uint16_t)Length, SPI_TIMEOUT) != HAL_OK)
	{
		ret = HARDWARE_IO_ERROR;
	}
	return ret;
}



int32_t HW_IO_SPI_gettick(void)
{
	return (int32_t)HAL_GetTick();
}
