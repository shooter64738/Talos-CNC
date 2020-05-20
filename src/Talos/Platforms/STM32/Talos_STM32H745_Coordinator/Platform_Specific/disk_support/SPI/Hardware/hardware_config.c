
#include "hardware_config.h"

//If you get error on sd card init, and you can step through in 
//debug and it works fine, your baud rate is probably too fast.
#define SPI_BAUDRATE                   12000000//71000000 //18000000


void HW_config(SPI_TypeDef* SPIxInstance)
{
	HW_config_gpio();
	HW_config_spi_handler(SPIxInstance);
}

void  HW_config_gpio()
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	/* Enable GPIO clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/* Configure SPI SCK */
	GPIO_InitStruct.Pin = GPIO_PIN_5;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Configure SPI MOSI */
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Configure SPI MISO */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* Configure SPI SS */
	GPIO_InitStruct.Pin = GPIO_PIN_4;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void HW_config_spi_handler(SPI_TypeDef* SPIxInstance)
{
	//Configure the spi object. If it was already configured this wont execute
	if (HAL_SPI_GetState(&cfg_spi_handler) == HAL_SPI_STATE_RESET)
	{
		SPI_HandleTypeDef* p_spi = &cfg_spi_handler;
		/*** Configure the SPI peripheral ***/
		__HAL_RCC_SPI1_CLK_ENABLE();

		p_spi->Instance = SPIxInstance;
		p_spi->Init.BaudRatePrescaler = HW_config_spi_baud_prescaler(HAL_RCC_GetHCLKFreq(), SPI_BAUDRATE);
		p_spi->Init.Direction = SPI_DIRECTION_2LINES;
		p_spi->Init.CLKPhase = SPI_PHASE_2EDGE;
		p_spi->Init.CLKPolarity = SPI_POLARITY_HIGH;
		p_spi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
		p_spi->Init.CRCPolynomial = 7;
		p_spi->Init.DataSize = SPI_DATASIZE_8BIT;
		p_spi->Init.FirstBit = SPI_FIRSTBIT_MSB;
		p_spi->Init.NSS = SPI_NSS_SOFT;
		p_spi->Init.TIMode = SPI_TIMODE_DISABLE;
		p_spi->Init.Mode = SPI_MODE_MASTER;
		p_spi->Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_ENABLE;  /* Recommanded setting to avoid glitches */
		
		p_spi->Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
		p_spi->Init.CRCLength = SPI_CRC_LENGTH_8BIT;
		p_spi->Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
		p_spi->Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
		p_spi->Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
		p_spi->Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
		p_spi->Init.MasterSSIdleness = 0x00000000;
		p_spi->Init.MasterInterDataIdleness = 0x00000000;
		p_spi->Init.MasterReceiverAutoSusp = 0x00000000;
		HAL_SPI_Init(p_spi);
	}
}

uint32_t HW_config_spi_baud_prescaler(uint32_t clock_src_freq, uint32_t baudrate_mbps)
{
	uint32_t divisor = 0;
	uint32_t spi_clk = clock_src_freq;
	uint32_t presc = 0;
	static const uint32_t baudrate[] =
	{ SPI_BAUDRATEPRESCALER_2,
	  SPI_BAUDRATEPRESCALER_4,
	  SPI_BAUDRATEPRESCALER_8,
	  SPI_BAUDRATEPRESCALER_16,
	  SPI_BAUDRATEPRESCALER_32,
	  SPI_BAUDRATEPRESCALER_64,
	  SPI_BAUDRATEPRESCALER_128,
	  SPI_BAUDRATEPRESCALER_256,
	};

	while (spi_clk > baudrate_mbps)
	{
		presc = baudrate[divisor];
		if (++divisor > 7)
			break;

		spi_clk = (spi_clk >> 1);
	}

	return presc;
}