#include "sd_card.h"
#include "hardware_io.h"
#include "hardware_config.h"

#define SD_DUMMY_BYTE            0xFFU
#define SD_CMD_LENGTH               6U
#define SD_MAX_TRY                100U  
#define SD_BLOCK_SIZE 512U

#define SD_TOKEN_START_DATA_SINGLE_BLOCK_READ    0xFEU  /* Data token start byte, Start Single Block Read */
#define SD_TOKEN_START_DATA_MULTIPLE_BLOCK_READ  0xFEU  /* Data token start byte, Start Multiple Block Read */
#define SD_TOKEN_START_DATA_SINGLE_BLOCK_WRITE   0xFEU  /* Data token start byte, Start Single Block Write */
#define SD_TOKEN_START_DATA_MULTIPLE_BLOCK_WRITE 0xFCU  /* Data token start byte, Start Multiple Block Write */
#define SD_TOKEN_STOP_DATA_MULTIPLE_BLOCK_WRITE  0xFDU  /* Data token stop byte, Stop Multiple Block Write */

static uint32_t CardType = SDSC_NORM_CAPACITY;
static volatile DSTATUS Stat = STA_NOINIT;

int32_t SD_IO_Init(SPI_TypeDef* SPIxInstance)
{
	int32_t ret = HARDWARE_IO_OK;
	uint8_t counter = 0, tmp;
	GPIO_InitTypeDef  GPIO_InitStruct;
	//Configure the spi object. If it was already configured this wont execute
	if (HAL_SPI_GetState(&cfg_spi_handler) == HAL_SPI_STATE_RESET)
	{
		//Config gpio, and spi object
		HW_config(SPIxInstance);

		HW_IO_SET_CS_HIGH();
	}
	
	HW_IO_SET_CS_HIGH();
	//Send dummy byte 0xFF, 10 times with CS high 
		//Rise CS and MOSI for 80 clocks cycles
	tmp = SD_DUMMY_BYTE;

	do
	{
		/* Send dummy byte 0xFF */
		if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
		{
			ret = HARDWARE_IO_COMM_ERROR;
			break;
		}
		counter++;
	} while (counter <= 9U);

	//Put card in idle state
	/* SD initialized and set to SPI mode properly */
	if (SD_GoIdleState() != HARDWARE_IO_OK)
	{
		ret = HARDWARE_IO_UNKNOWN_ERROR;
	}

	return ret;
}

//void SD_IO_CSState(uint32_t Value)
//{
//	if (Value == 1U)
//	{
//		HW_IO_SET_CS_HIGH();
//	}
//	else
//	{
//		HW_IO_SET_CS_LOW();
//	}
//}
//static int32_t SD_GetCSDRegister(SD_CardSpecificData_t* Csd)
//{
//	int32_t ret = BSP_ERROR_NONE;
//	uint32_t counter;
//	uint32_t CSD_Tab[16];
//	uint32_t response;
//	uint8_t tmp;
//
//	/* Send CMD9 (CSD register) or CMD10(CSD register) and Wait for response in the R1 format (0x00 is no errors) */
//	response = SD_SendCmd(SD_CMD_SEND_CSD, 0U, 0xFFU, (uint8_t)SD_ANSWER_R1_EXPECTED);
//	if ((uint8_t)(response & 0xFFU) == (uint8_t)SD_R1_NO_ERROR)
//	{
//		if (SD_WaitData(SD_TOKEN_START_DATA_SINGLE_BLOCK_READ) == BSP_ERROR_NONE)
//		{
//			tmp = SD_DUMMY_BYTE;
//			for (counter = 0U; counter < 16U; counter++)
//			{
//				/* Store CSD register value on CSD_Tab */
//				if (BUS_SPIx_SendRecv(&tmp, (uint8_t*)& CSD_Tab[counter], 1U) != BSP_ERROR_NONE)
//				{
//					ret = BSP_ERROR_PERIPH_FAILURE;
//					break;
//				}
//			}
//
//			if (ret == BSP_ERROR_NONE)
//			{
//				/* Get CRC bytes (not really needed by us, but required by SD) */
//				if (BUS_SPIx_Send(&tmp, 1U) != BSP_ERROR_NONE)
//				{
//					ret = BSP_ERROR_PERIPH_FAILURE;
//				}
//				else if (BUS_SPIx_Send(&tmp, 1U) != BSP_ERROR_NONE)
//				{
//					ret = BSP_ERROR_PERIPH_FAILURE;
//				}
//				else
//				{
//					/*************************************************************************
//					CSD header decoding
//					*************************************************************************/
//
//					/* Byte 0 */
//					Csd->CSDStruct = (CSD_Tab[0] & 0xC0U) >> 6U;
//					Csd->Reserved1 = CSD_Tab[0] & 0x3FU;
//
//					/* Byte 1 */
//					Csd->TAAC = CSD_Tab[1];
//
//					/* Byte 2 */
//					Csd->NSAC = CSD_Tab[2];
//
//					/* Byte 3 */
//					Csd->MaxBusClkFrec = CSD_Tab[3];
//
//					/* Byte 4/5 */
//					Csd->CardComdClasses = (uint16_t)(((uint16_t)CSD_Tab[4] << 4U) | ((uint16_t)(CSD_Tab[5] & 0xF0U) >> 4U));
//					Csd->RdBlockLen = CSD_Tab[5] & 0x0FU;
//
//					/* Byte 6 */
//					Csd->PartBlockRead = (CSD_Tab[6] & 0x80U) >> 7U;
//					Csd->WrBlockMisalign = (CSD_Tab[6] & 0x40U) >> 6U;
//					Csd->RdBlockMisalign = (CSD_Tab[6] & 0x20U) >> 5U;
//					Csd->DSRImpl = (CSD_Tab[6] & 0x10U) >> 4U;
//
//					/*************************************************************************
//					CSD v1/v2 decoding
//					*************************************************************************/
//
//					if (CardType == ADAFRUIT_802_CARD_SDSC)
//					{
//						Csd->version.v1.Reserved1 = ((CSD_Tab[6] & 0x0CU) >> 2U);
//
//						Csd->version.v1.DeviceSize = ((CSD_Tab[6] & 0x03U) << 10U) | (CSD_Tab[7] << 2U) | ((CSD_Tab[8] & 0xC0U) >> 6U);
//						Csd->version.v1.MaxRdCurrentVDDMin = (CSD_Tab[8] & 0x38U) >> 3U;
//						Csd->version.v1.MaxRdCurrentVDDMax = (CSD_Tab[8] & 0x07U);
//						Csd->version.v1.MaxWrCurrentVDDMin = (CSD_Tab[9] & 0xE0U) >> 5U;
//						Csd->version.v1.MaxWrCurrentVDDMax = (CSD_Tab[9] & 0x1CU) >> 2U;
//						Csd->version.v1.DeviceSizeMul = ((CSD_Tab[9] & 0x03U) << 1U) | ((CSD_Tab[10] & 0x80U) >> 7U);
//					}
//					else
//					{
//						Csd->version.v2.Reserved1 = ((CSD_Tab[6] & 0x0FU) << 2U) | ((CSD_Tab[7] & 0xC0U) >> 6U);
//						Csd->version.v2.DeviceSize = ((CSD_Tab[7] & 0x3FU) << 16U) | (CSD_Tab[8] << 8U) | CSD_Tab[9];
//						Csd->version.v2.Reserved2 = ((CSD_Tab[10] & 0x80U) >> 8U);
//					}
//
//					Csd->EraseSingleBlockEnable = (CSD_Tab[10] & 0x40U) >> 6U;
//					Csd->EraseSectorSize = ((CSD_Tab[10] & 0x3FU) << 1U) | ((CSD_Tab[11] & 0x80U) >> 7U);
//					Csd->WrProtectGrSize = (CSD_Tab[11] & 0x7FU);
//					Csd->WrProtectGrEnable = (CSD_Tab[12] & 0x80U) >> 7U;
//					Csd->Reserved2 = (CSD_Tab[12] & 0x60U) >> 5U;
//					Csd->WrSpeedFact = (CSD_Tab[12] & 0x1CU) >> 2U;
//					Csd->MaxWrBlockLen = ((CSD_Tab[12] & 0x03U) << 2U) | ((CSD_Tab[13] & 0xC0U) >> 6U);
//					Csd->WriteBlockPartial = (CSD_Tab[13] & 0x20U) >> 5U;
//					Csd->Reserved3 = (CSD_Tab[13] & 0x1FU);
//					Csd->FileFormatGrouop = (CSD_Tab[14] & 0x80U) >> 7U;
//					Csd->CopyFlag = (CSD_Tab[14] & 0x40U) >> 6U;
//					Csd->PermWrProtect = (CSD_Tab[14] & 0x20U) >> 5U;
//					Csd->TempWrProtect = (CSD_Tab[14] & 0x10U) >> 4U;
//					Csd->FileFormat = (CSD_Tab[14] & 0x0CU) >> 2U;
//					Csd->Reserved4 = (CSD_Tab[14] & 0x03U);
//					Csd->crc = (CSD_Tab[15] & 0xFEU) >> 1U;
//					Csd->Reserved5 = (CSD_Tab[15] & 0x01U);
//
//					ret = BSP_ERROR_NONE;
//				}
//			}
//		}
//	}
//	else
//	{
//		ret = BSP_ERROR_UNKNOWN_FAILURE;
//	}
//
//	if (ret == BSP_ERROR_NONE)
//	{
//		/* Send dummy byte: 8 Clock pulses of delay */
//		SD_IO_CSState(1);
//
//		if (BUS_SPIx_Send(&tmp, 1U) != BSP_ERROR_NONE)
//		{
//			ret = BSP_ERROR_PERIPH_FAILURE;
//		}
//	}
//
//	/* Return the reponse */
//	return ret;
//}
//static int32_t SD_GetCIDRegister(SD_CardIdData_t* Cid)
//{
//	int32_t ret = BSP_ERROR_NONE;
//	uint32_t CID_Tab[16];
//	uint32_t response;
//	uint32_t counter;
//	uint8_t tmp;
//
//	/* Send CMD10 (CID register) and Wait for response in the R1 format (0x00 is no errors) */
//	response = SD_SendCmd(SD_CMD_SEND_CID, 0U, 0xFFU, (uint8_t)SD_ANSWER_R1_EXPECTED);
//	if ((uint8_t)(response & 0xFFU) == (uint8_t)SD_R1_NO_ERROR)
//	{
//		if (SD_WaitData(SD_TOKEN_START_DATA_SINGLE_BLOCK_READ) == BSP_ERROR_NONE)
//		{
//			tmp = SD_DUMMY_BYTE;
//
//			/* Store CID register value on CID_Tab */
//			for (counter = 0U; counter < 16U; counter++)
//			{
//				if (BUS_SPIx_SendRecv(&tmp, (uint8_t*)& CID_Tab[counter], 1U) != BSP_ERROR_NONE)
//				{
//					ret = BSP_ERROR_PERIPH_FAILURE;
//					break;
//				}
//			}
//
//			if (ret == BSP_ERROR_NONE)
//			{
//				/* Get CRC bytes (not really needed by us, but required by SD) */
//				if (BUS_SPIx_Send(&tmp, 1U) != BSP_ERROR_NONE)
//				{
//					ret = BSP_ERROR_PERIPH_FAILURE;
//				}
//				else if (BUS_SPIx_Send(&tmp, 1U) != BSP_ERROR_NONE)
//				{
//					ret = BSP_ERROR_PERIPH_FAILURE;
//				}
//				else
//				{
//					/* Byte 0 */
//					Cid->ManufacturerID = CID_Tab[0];
//
//					/* Byte 1 */
//					Cid->OEM_AppliID = CID_Tab[1] << 8U;
//
//					/* Byte 2 */
//					Cid->OEM_AppliID |= CID_Tab[2];
//
//					/* Byte 3 */
//					Cid->ProdName1 = CID_Tab[3] << 24U;
//
//					/* Byte 4 */
//					Cid->ProdName1 |= CID_Tab[4] << 16U;
//
//					/* Byte 5 */
//					Cid->ProdName1 |= CID_Tab[5] << 8U;
//
//					/* Byte 6 */
//					Cid->ProdName1 |= CID_Tab[6];
//
//					/* Byte 7 */
//					Cid->ProdName2 = CID_Tab[7];
//
//					/* Byte 8 */
//					Cid->ProdRev = CID_Tab[8];
//
//					/* Byte 9 */
//					Cid->ProdSN = CID_Tab[9] << 24U;
//
//					/* Byte 10 */
//					Cid->ProdSN |= CID_Tab[10] << 16U;
//
//					/* Byte 11 */
//					Cid->ProdSN |= CID_Tab[11] << 8U;
//
//					/* Byte 12 */
//					Cid->ProdSN |= CID_Tab[12];
//
//					/* Byte 13 */
//					Cid->Reserved1 |= (CID_Tab[13] & 0xF0U) >> 4U;
//					Cid->ManufactDate = (CID_Tab[13] & 0x0FU) << 8U;
//
//					/* Byte 14 */
//					Cid->ManufactDate |= CID_Tab[14];
//
//					/* Byte 15 */
//					Cid->CID_CRC = (CID_Tab[15] & 0xFEU) >> 1U;
//					Cid->Reserved2 = 1U;
//
//					ret = BSP_ERROR_NONE;
//				}
//			}
//		}
//	}
//	else
//	{
//		ret = BSP_ERROR_UNKNOWN_FAILURE;
//	}
//
//	if (ret == BSP_ERROR_NONE)
//	{
//		/* Send dummy byte: 8 Clock pulses of delay */
//		SD_IO_CSState(1);
//
//		if (BUS_SPIx_Send(&tmp, 1U) != BSP_ERROR_NONE)
//		{
//			ret = BSP_ERROR_PERIPH_FAILURE;
//		}
//	}
//
//	/* Return the reponse */
//	return ret;
//}
uint32_t SD_SendCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc, uint8_t Answer)
{
	uint8_t frame[SD_CMD_LENGTH], frameout[SD_CMD_LENGTH];
	uint32_t response = 0xFFFF;
	uint8_t tmp, response_tmp;

	/* R1 Lenght = NCS(0)+ 6 Bytes command + NCR(min1 max8) + 1 Bytes answer + NEC(0) = 15bytes */
	/* R1b identical to R1 + Busy information                                                   */
	/* R2 Lenght = NCS(0)+ 6 Bytes command + NCR(min1 max8) + 2 Bytes answer + NEC(0) = 16bytes */

	/* Prepare Frame to send */
	frame[0] = (Cmd | 0x40U);         /* Construct byte 1 */
	frame[1] = (uint8_t)(Arg >> 24U); /* Construct byte 2 */
	frame[2] = (uint8_t)(Arg >> 16U); /* Construct byte 3 */
	frame[3] = (uint8_t)(Arg >> 8U);  /* Construct byte 4 */
	frame[4] = (uint8_t)(Arg);        /* Construct byte 5 */
	frame[5] = (Crc | 0x01U);         /* Construct byte 6 */

	/* Send the command */
	HW_IO_SET_CS_LOW();
	/* Send the Cmd bytes */
	if (HW_IO_SPI_sendrecv(frame, frameout, SD_CMD_LENGTH) != HARDWARE_IO_OK)
	{
		return 0xFFFF;
	}
	tmp = SD_DUMMY_BYTE;
	switch (Answer)
	{
	case SD_ANSWER_R1_EXPECTED:
		/* Sends one byte command to the SD card and get response */
		if (SD_ReadData(&response_tmp) != HARDWARE_IO_OK)
		{
			return 0xFFFF;
		}
		response = response_tmp;
		break;
	case SD_ANSWER_R1B_EXPECTED:
		/* Sends first byte command to the SD card and get response */
		if (SD_ReadData(&response_tmp) != HARDWARE_IO_OK)
		{
			return 0xFFFF;
		}
		response = response_tmp;
		/* Sends second byte command to the SD card and get response */
		if (HW_IO_SPI_sendrecv(&tmp, &response_tmp, 1U) != HARDWARE_IO_OK)
		{
			return 0xFFFF;
		}
		response |= response_tmp;
		/* Set CS High */
		HW_IO_SET_CS_HIGH();
		SPI_IO_Delay(1);
		/* Set CS Low */
		HW_IO_SET_CS_LOW();

		/* Wait IO line return 0xFF */
		if (HW_IO_SPI_sendrecv(&tmp, &response_tmp, 1U) != HARDWARE_IO_OK)
		{
			return 0xFFFF;
		}
		while (response_tmp != 0xFFU)
		{
			if (HW_IO_SPI_sendrecv(&tmp, &response_tmp, 1U) != HARDWARE_IO_OK)
			{
				return 0xFFFF;
			}
		}
		break;
	case SD_ANSWER_R2_EXPECTED:
		/* Sends first byte command to the SD card and get response */
		if (SD_ReadData(&response_tmp) != HARDWARE_IO_OK)
		{
			return 0xFFFF;
		}
		response = response_tmp;
		/* Sends second byte command to the SD card and get response */
		if (HW_IO_SPI_sendrecv(&tmp, &response_tmp, 1U) != HARDWARE_IO_OK)
		{
			return 0xFFFF;
		}
		response |= ((uint32_t)response_tmp << 8);
		break;
	case SD_ANSWER_R3_EXPECTED:
	case SD_ANSWER_R7_EXPECTED:
		/* Sends first byte command to the SD card and get response */
		if (SD_ReadData(&response_tmp) != HARDWARE_IO_OK)
		{
			return 0xFFFF;
		}
		response = response_tmp;
		/* Sends second byte command to the SD card and get response */
		if (HW_IO_SPI_sendrecv(&tmp, &response_tmp, 1U) != HARDWARE_IO_OK)
		{
			return 0xFFFF;
		}
		/* Only first and second responses are required */
		response |= ((uint32_t)response_tmp << 8U);
		/* Sends third byte command to the SD card and get response */
		if (HW_IO_SPI_sendrecv(&tmp, &response_tmp, 1U) != HARDWARE_IO_OK)
		{
			return 0xFFFF;
		}
		/* Sends fourth byte command to the SD card and get response */
		if (HW_IO_SPI_sendrecv(&tmp, &response_tmp, 1U) != HARDWARE_IO_OK)
		{
			return 0xFFFF;
		}
		/* Sends fifth byte command to the SD card and get response */
		if (HW_IO_SPI_sendrecv(&tmp, &response_tmp, 1U) != HARDWARE_IO_OK)
		{
			return 0xFFFF;
		}

		break;
	default:
		break;
	}
	return response;
}
int32_t SD_GetDataResponse(uint8_t* DataResponse)
{
	uint8_t dataresponse, tmp, tmp1;
	*DataResponse = (uint8_t)SD_DATA_OTHER_ERROR;

	tmp = SD_DUMMY_BYTE;
	if (HW_IO_SPI_sendrecv(&tmp, &dataresponse, 1U) != HARDWARE_IO_OK)
	{
		return HARDWARE_IO_NEGOTIATION_ERROR;
	}
	/* read the busy response byte*/
	if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
	{
		return HARDWARE_IO_NEGOTIATION_ERROR;
	}
	else
	{
		/* Mask unused bits */
		switch (dataresponse & 0x1FU)
		{
		case SD_DATA_OK:
			*DataResponse = (uint8_t)SD_DATA_OK;

			/* Set CS High */
			HW_IO_SET_CS_HIGH();
			/* Set CS Low */
			HW_IO_SET_CS_LOW();
			tmp = SD_DUMMY_BYTE;

			/* Wait IO line return 0xFF */
			if (HW_IO_SPI_sendrecv(&tmp, &tmp1, 1U) != HARDWARE_IO_OK)
			{
				return HARDWARE_IO_NEGOTIATION_ERROR;
			}
			while (tmp1 != 0xFFU)
			{
				if (HW_IO_SPI_sendrecv(&tmp, &tmp1, 1U) != HARDWARE_IO_OK)
				{
					return HARDWARE_IO_NEGOTIATION_ERROR;
				}
			}
			break;
		case SD_DATA_CRC_ERROR:
			*DataResponse = (uint8_t)SD_DATA_CRC_ERROR;
			break;
		case SD_DATA_WRITE_ERROR:
			*DataResponse = (uint8_t)SD_DATA_WRITE_ERROR;
			break;
		default:
			break;
		}
	}

	/* Return response */
	return HARDWARE_IO_OK;
}
int32_t SD_GoIdleState(void)
{
	uint32_t response;
	__IO uint8_t counter = 0;
	uint8_t tmp = SD_DUMMY_BYTE;

	/* Send CMD0 (SD_CMD_GO_IDLE_STATE) to put SD in SPI mode and
	wait for In Idle State Response (R1 Format) equal to 0x01 */
	do
	{
		counter++;
		response = SD_SendCmd(SD_CMD_GO_IDLE_STATE, 0U, 0x95U, (uint8_t)SD_ANSWER_R1_EXPECTED);
		HW_IO_SET_CS_HIGH();
		if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
		{
			return HARDWARE_IO_NEGOTIATION_ERROR;
		}
		else
		{
			if (counter >= SD_MAX_TRY)
			{
				return HARDWARE_IO_UNKNOWN_ERROR;
			}
		}
	} while ((uint8_t)(response & 0xFFU) != (uint8_t)SD_R1_IN_IDLE_STATE);

	/* Send CMD8 (SD_CMD_SEND_IF_COND) to check the power supply status
	and wait until response (R7 Format) equal to 0xAA and */
	response = SD_SendCmd(SD_CMD_SEND_IF_COND, 0x1AAU, 0x87U, (uint8_t)SD_ANSWER_R7_EXPECTED);
	HW_IO_SET_CS_HIGH();
	if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
	{
		return HARDWARE_IO_NEGOTIATION_ERROR;
	}
	else if (((uint8_t)(response & 0xFFU) & (uint8_t)SD_R1_ILLEGAL_COMMAND) == (uint8_t)SD_R1_ILLEGAL_COMMAND)
	{
		/* initialise card V1 */
		do
		{
			/* initialise card V1 */
			/* Send CMD55 (SD_CMD_APP_CMD) before any ACMD command: R1 response (0x00: no errors) */
			(void)SD_SendCmd(SD_CMD_APP_CMD, 0x00U, 0xFFU, (uint8_t)SD_ANSWER_R1_EXPECTED);
			HW_IO_SET_CS_HIGH();
			if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
			{
				return HARDWARE_IO_NEGOTIATION_ERROR;
			}
			else
			{
				/* Send ACMD41 (SD_CMD_SD_APP_OP_COND) to initialize SDHC or SDXC cards: R1 response (0x00: no errors) */
				response = SD_SendCmd(SD_CMD_SD_APP_OP_COND, 0x00U, 0xFFU, (uint8_t)SD_ANSWER_R1_EXPECTED);
				HW_IO_SET_CS_HIGH();
				if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
				{
					return HARDWARE_IO_NEGOTIATION_ERROR;
				}
			}
		} while ((uint8_t)(response & 0xFFU) == (uint8_t)SD_R1_IN_IDLE_STATE);
		CardType = SDSC_NORM_CAPACITY;
	}
	else if ((uint8_t)(response & 0xFFU) == (uint8_t)SD_R1_IN_IDLE_STATE)
	{
		/* initialise card V2 */
		do {
			/* Send CMD55 (SD_CMD_APP_CMD) before any ACMD command: R1 response (0x00: no errors) */
			(void)SD_SendCmd(SD_CMD_APP_CMD, 0, 0xFFU, (uint8_t)SD_ANSWER_R1_EXPECTED);
			HW_IO_SET_CS_HIGH();
			if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
			{
				return HARDWARE_IO_NEGOTIATION_ERROR;
			}
			else
			{
				/* Send ACMD41 (SD_CMD_SD_APP_OP_COND) to initialize SDHC or SDXC cards: R1 response (0x00: no errors) */
				response = SD_SendCmd(SD_CMD_SD_APP_OP_COND, 0x40000000U, 0xFFU, (uint8_t)SD_ANSWER_R1_EXPECTED);
				HW_IO_SET_CS_HIGH();
				if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
				{
					return HARDWARE_IO_NEGOTIATION_ERROR;
				}
			}
		} while (((uint8_t)response & 0xFFU) == (uint8_t)SD_R1_IN_IDLE_STATE);

		if (((uint8_t)(response & 0xFFU) & (uint8_t)SD_R1_ILLEGAL_COMMAND) == (uint8_t)SD_R1_ILLEGAL_COMMAND)
		{
			do {
				/* Send CMD55 (SD_CMD_APP_CMD) before any ACMD command: R1 response (0x00: no errors) */
				(void)SD_SendCmd(SD_CMD_APP_CMD, 0U, 0xFFU, (uint8_t)SD_ANSWER_R1_EXPECTED);
				HW_IO_SET_CS_HIGH();
				if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
				{
					return HARDWARE_IO_NEGOTIATION_ERROR;
				}
				else
				{
					if ((uint8_t)(response & 0xFFU) != (uint8_t)SD_R1_IN_IDLE_STATE)
					{
						return HARDWARE_IO_UNKNOWN_ERROR;
					}
					/* Send ACMD41 (SD_CMD_SD_APP_OP_COND) to initialize SDHC or SDXC cards: R1 response (0x00: no errors) */
					response = SD_SendCmd(SD_CMD_SD_APP_OP_COND, 0x00U, 0xFFU, (uint8_t)SD_ANSWER_R1_EXPECTED);
					HW_IO_SET_CS_HIGH();
					if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
					{
						return HARDWARE_IO_NEGOTIATION_ERROR;
					}
				}
			} while ((uint8_t)(response & 0xFFU) == (uint8_t)SD_R1_IN_IDLE_STATE);
		}

		/* Send CMD58 (SD_CMD_READ_OCR) to initialize SDHC or SDXC cards: R3 response (0x00: no errors) */
		response = SD_SendCmd(SD_CMD_READ_OCR, 0x00U, 0xFFU, (uint8_t)SD_ANSWER_R3_EXPECTED);
		HW_IO_SET_CS_HIGH();
		if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
		{
			return HARDWARE_IO_NEGOTIATION_ERROR;
		}
		else
		{
			if ((uint8_t)(response & 0xFFU) != (uint8_t)SD_R1_NO_ERROR)
			{
				return HARDWARE_IO_UNKNOWN_ERROR;
			}
			CardType = (uint32_t)(((response >> 8U) & 0x40U) >> 6U);
		}
	}
	else
	{
		return HARDWARE_IO_BUSY_ERROR;
	}

	return HARDWARE_IO_OK;
}
int32_t SD_ReadData(uint8_t* Data)
{
	uint8_t timeout = 0x08U;
	uint8_t tmp;

	tmp = SD_DUMMY_BYTE;
	/* Check if response is got or a timeout is happen */
	do
	{
		if (HW_IO_SPI_sendrecv(&tmp, Data, 1) != HARDWARE_IO_OK)
		{
			return HARDWARE_IO_COMM_ERROR;
		}
		timeout--;

	} while ((*Data == SD_DUMMY_BYTE) && (timeout != 0U));

	if (timeout == 0U)
	{
		/* After time out */
		return HARDWARE_IO_BUSY_ERROR;
	}

	/* Right response got */
	return HARDWARE_IO_OK;
}
int32_t SD_WaitData(uint8_t Data)
{
	uint16_t timeout = 0xFFFF;
	uint8_t readvalue, tmp;

	tmp = SD_DUMMY_BYTE;

	/* Check if response is got or a timeout is happen */
	do
	{
		if (HW_IO_SPI_sendrecv(&tmp, &readvalue, 1) != HARDWARE_IO_OK)
		{
			return HARDWARE_IO_COMM_ERROR;
		}
		timeout--;
	} while ((readvalue != Data) && (timeout != 0U));

	if (timeout == 0U)
	{
		/* After time out */
		return HARDWARE_IO_BUSY_ERROR;
	}

	/* Right response got */
	return HARDWARE_IO_OK;
}

void SPI_IO_Delay(uint32_t Delay)
{
	int32_t tickstart;
	tickstart = HW_IO_SPI_gettick();
	while ((HW_IO_SPI_gettick() - tickstart) < (int32_t)Delay)
	{
	}
}

static int32_t __ReadBlocks(uint32_t Instance, uint32_t* pData, uint32_t BlockIdx, uint32_t BlocksNbr)
{

	int32_t ret = HARDWARE_IO_OK;
	uint32_t response, offset = 0, blocks_nbr = BlocksNbr;
	uint8_t tmp;

	if (Instance >= 1)
	{
		ret = HARDWARE_IO_COMM_ERROR;
	}
	else
	{
		/* Send CMD16 (SD_CMD_SET_BLOCKLEN) to set the size of the block and
		Check if the SD acknowledged the set block length command: R1 response (0x00: no errors) */
		response = SD_SendCmd(SD_CMD_SET_BLOCKLEN, SD_BLOCK_SIZE, 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED);
		HW_IO_SET_CS_HIGH();
		if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
		{
			ret = HARDWARE_IO_COMM_ERROR;
		}
		else
		{
			if ((uint8_t)(response & 0xFFU) != (uint8_t)SD_R1_NO_ERROR)
			{
				/* Send dummy byte: 8 Clock pulses of delay */
				HW_IO_SET_CS_HIGH();
				if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
				{
					ret = HARDWARE_IO_COMM_ERROR;
				}
			}

			if (ret == HARDWARE_IO_OK)
			{
				/* Data transfer */
				do
				{
					/* Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
					/* Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
					response = SD_SendCmd(SD_CMD_READ_SINGLE_BLOCK, (BlockIdx + offset) * ((CardType == SDHC_LARG_CAPACITY) ? 1U : SD_BLOCK_SIZE), 0xFFU, (uint8_t)SD_ANSWER_R1_EXPECTED);
					if ((uint8_t)(response & 0xFFU) != (uint8_t)SD_R1_NO_ERROR)
					{
						/* Send dummy byte: 8 Clock pulses of delay */
						HW_IO_SET_CS_HIGH();
						if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
						{
							ret = HARDWARE_IO_COMM_ERROR;
						}
					}

					if (ret == HARDWARE_IO_OK)
					{
						/* Now look for the data token to signify the start of the data */
						ret = SD_WaitData(SD_TOKEN_START_DATA_SINGLE_BLOCK_READ);
						if (ret == HARDWARE_IO_OK)
						{
							/* Read the SD block data : read NumByteToRead data */
							if (HW_IO_SPI_recv((uint8_t*)pData + offset, SD_BLOCK_SIZE) != HARDWARE_IO_OK)
							{
								ret = HARDWARE_IO_COMM_ERROR;
							}
							else
							{
								/* Set next read address*/
								offset += SD_BLOCK_SIZE;
								blocks_nbr--;

								/* get CRC bytes (not really needed by us, but required by SD) */
								if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
								{
									ret = HARDWARE_IO_COMM_ERROR;
								}
								else if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
								{
									ret = HARDWARE_IO_COMM_ERROR;
								}
								else
								{
									/* Send dummy byte: 8 Clock pulses of delay */
									HW_IO_SET_CS_HIGH();
									if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
									{
										ret = HARDWARE_IO_COMM_ERROR;
									}
								}
								if (ret == HARDWARE_IO_OK)
								{
									/* End the command data read cycle */
									HW_IO_SET_CS_HIGH();
									if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
									{
										ret = HARDWARE_IO_COMM_ERROR;
									}
								}
							}
						}
					}
				} while ((blocks_nbr != 0U) && (ret == HARDWARE_IO_OK));
			}
		}
	}

	/* Return BSP status */
	return ret;
}

static int32_t __WriteBlocks(uint32_t Instance, uint32_t* pData, uint32_t BlockIdx, uint32_t BlocksNbr)
{
	int32_t ret = HARDWARE_IO_OK;
	uint32_t response, offset = 0, blocks_nbr = BlocksNbr;
	uint8_t tmp, data_response;

	if (Instance >= 1)
	{
		ret = HARDWARE_IO_COMM_ERROR;
	}
	else
	{
		/* Send CMD16 (SD_CMD_SET_BLOCKLEN) to set the size of the block and
		Check if the SD acknowledged the set block length command: R1 response (0x00: no errors) */
		response = SD_SendCmd(SD_CMD_SET_BLOCKLEN, SD_BLOCK_SIZE, 0xFF, (uint8_t)SD_ANSWER_R1_EXPECTED);
		HW_IO_SET_CS_HIGH();
		tmp = SD_DUMMY_BYTE;
		if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
		{
			ret = HARDWARE_IO_COMM_ERROR;
		}
		else
		{
			if ((uint8_t)(response & 0xFFU) != (uint8_t)SD_R1_NO_ERROR)
			{
				/* Send dummy byte: 8 Clock pulses of delay */
				HW_IO_SET_CS_HIGH();
				if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
				{
					ret = HARDWARE_IO_COMM_ERROR;
				}
			}

			if (ret == HARDWARE_IO_OK)
			{
				/* Data transfer */
				do
				{
					/* Send CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) to write blocks  and
					Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
					response = SD_SendCmd(SD_CMD_WRITE_SINGLE_BLOCK, (BlockIdx + offset) * ((CardType == SDHC_LARG_CAPACITY) ? 1U : SD_BLOCK_SIZE), 0xFFU, (uint8_t)SD_ANSWER_R1_EXPECTED);
					if ((uint8_t)(response & 0xFFU) != (uint8_t)SD_R1_NO_ERROR)
					{
						/* Send dummy byte: 8 Clock pulses of delay */
						HW_IO_SET_CS_HIGH();
						if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
						{
							ret = HARDWARE_IO_COMM_ERROR;
						}
					}

					if (ret == HARDWARE_IO_OK)
					{
						/* Send dummy byte for NWR timing : one byte between CMDWRITE and TOKEN */
						if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
						{
							ret = HARDWARE_IO_COMM_ERROR;
						}
						else if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
						{
							ret = HARDWARE_IO_COMM_ERROR;
						}
						else
						{
							/* Send the data token to signify the start of the data */
							tmp = SD_TOKEN_START_DATA_SINGLE_BLOCK_WRITE;
							if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
							{
								ret = HARDWARE_IO_COMM_ERROR;
							}/* Write the block data to SD */
							else if (HW_IO_SPI_send((uint8_t*)pData + offset, SD_BLOCK_SIZE) != HARDWARE_IO_OK)
							{
								ret = HARDWARE_IO_COMM_ERROR;
							}
							else
							{
								/* Set next write address */
								offset += SD_BLOCK_SIZE;
								blocks_nbr--;

								/* get CRC bytes (not really needed by us, but required by SD) */
								tmp = SD_DUMMY_BYTE;
								if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
								{
									ret = HARDWARE_IO_COMM_ERROR;
								}
								else if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
								{
									ret = HARDWARE_IO_COMM_ERROR;
								}/* Read data response */
								else if (SD_GetDataResponse(&data_response) != HARDWARE_IO_OK)
								{
									ret = HARDWARE_IO_COMM_ERROR;
								}
								else
								{
									if (data_response != (uint8_t)SD_DATA_OK)
									{
										/* Set response value to failure */
										/* Send dummy byte: 8 Clock pulses of delay */
										HW_IO_SET_CS_HIGH();
										if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
										{
											ret = HARDWARE_IO_COMM_ERROR;
										}
									}
								}
							}
						}
						if (ret == HARDWARE_IO_OK)
						{
							HW_IO_SET_CS_HIGH();
							if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
							{
								ret = HARDWARE_IO_COMM_ERROR;
							}
						}
					}
				} while ((blocks_nbr != 0U) && (ret == HARDWARE_IO_OK));
			}
		}
	}

	/* Return BSP status */
	return ret;
}

static DSTATUS __GetCardState(BYTE Instance)
{
	int32_t ret;
	uint32_t response;
	uint8_t tmp;

	if (Instance >= 1) //SD_INSTANCES_NBR
	{
		ret = HARDWARE_IO_NEGOTIATION_ERROR;
	}
	else
	{
		/* Send CMD13 (SD_SEND_STATUS) to get SD status */
		response = SD_SendCmd(SD_CMD_SEND_STATUS, 0, 0xFF, (uint8_t)SD_ANSWER_R2_EXPECTED);
		HW_IO_SET_CS_HIGH();
		tmp = SD_DUMMY_BYTE;
		uint8_t r1er = (uint8_t)(response & 0xFFU);
		uint8_t r2er = (uint8_t)((response & 0xFF00UL) >> 8);

		if (HW_IO_SPI_send(&tmp, 1U) != HARDWARE_IO_OK)
		{
			ret = HARDWARE_IO_COMM_ERROR;
		}/* Find SD status according to card state */
		else if (((uint8_t)(response & 0xFFU) == (uint8_t)SD_R1_NO_ERROR) && ((uint8_t)((response & 0xFF00UL) >> 8) == (uint8_t)SD_R2_NO_ERROR))
		{
			ret = (int32_t)HARDWARE_IO_OK;
		}
		else
		{
			ret = (int32_t)HARDWARE_IO_BUSY_ERROR;
		}
	}
	return ret;
}

DSTATUS pntr_from_link_drvr_SD_initialize(BYTE lun)
{
	Stat = STA_NOINIT;
	Stat = __GetCardState(lun);

	return Stat;
}
DSTATUS pntr_from_link_drvr_SD_status(BYTE lun)
{
	Stat = STA_NOINIT;

	if (__GetCardState(lun) == HARDWARE_IO_OK)
	{
		Stat &= ~STA_NOINIT;
	}

	return Stat;
}

DRESULT pntr_from_link_drvr_SD_read(BYTE lun, BYTE* buff, DWORD sector, UINT count)
{
	DRESULT res = RES_ERROR;

	res = __ReadBlocks(0, (uint32_t*)buff, (uint32_t)(sector), count);
	if (res == HARDWARE_IO_OK)
	{
		/* wait until the read operation is finished */
		while (__GetCardState(0) != HARDWARE_IO_OK)
		{
		}
		res = RES_OK;
	}

	return res;
}
DRESULT pntr_from_link_drvr_SD_write(BYTE lun, const BYTE* buff, DWORD sector, UINT count)
{
	DRESULT res = RES_ERROR;

	if (__WriteBlocks(0, (uint32_t*)buff, (uint32_t)(sector), count) == HARDWARE_IO_OK)
	{
		/* wait until the Write operation is finished */
		while (__GetCardState(0) != HARDWARE_IO_OK)
		{
		}
		res = RES_OK;
	}

	return res;
}

__weak DWORD get_fattime(void)
{
	return 0;
}


