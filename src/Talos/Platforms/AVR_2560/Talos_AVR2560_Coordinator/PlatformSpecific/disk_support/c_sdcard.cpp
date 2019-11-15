/*
* c_sdcard.cpp
*
* Created: 11/14/2019 1:11:38 PM
* Author: jeff_d
*/


#include "c_sdcard.h"
#include "..\..\..\..\..\Coordinator\Processing\Main\Main_Process.h"

static uint8_t status = 0;
//static c_sdcard::e_card_types card_type = 0;
//static c_sdcard::e_card_errors errorCode_ = 0;
volatile uint16_t t0 = 0;;

static uint32_t block_;
static uint8_t inBlock_;
static uint16_t offset_;
static uint8_t partialBlockRead_;

c_sdcard::s_card_info c_sdcard::card_info;

c_sdcard::s_card_info c_sdcard::initialize()
{
	c_sdcard::e_card_errors ret_code = e_card_errors::SD_CARD_OK;

	__spi_init__();
	ret_code = __card_init__();
	if ((int)ret_code)
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("error!\r\n");
	}
	else
	{
		Talos::Coordinator::Main_Process::host_serial.print_string("success! card type = ");
		Talos::Coordinator::Main_Process::host_serial.print_int32((int)card_info.type);
		Talos::Coordinator::Main_Process::host_serial.print_string("\r\n");
	}
	
	CS_HIGH();

	return card_info;
}

uint8_t c_sdcard::__spi_init__()
{
	//uint8_t sckRateID = 250000;
	//uint8_t chipSelectPin_ = 53;
	
	uint8_t clockDiv = 7;
	uint8_t dataMode = 0;
	SPI_DIR |= CS;//set chip select pin to output
	SPI_PORT |= CS; //make output high

	SPCR |= (1<<MSTR);
	SPCR |= (1<<SPE);

	SPI_DIR |= SCK; //set clock to output
	SPI_DIR |= MOSI; //set mosi to output
	//SPI_DIR &= MISO; //set miso to input

	// Pack into the SPISettings class
	uint8_t bitOrder = MSBFIRST;
	//settings = SPISettings(250000, MSBFIRST, SPI_MODE0);

	SPCR = (1<<SPE) | (1<<MSTR) | ((bitOrder == LSBFIRST) ? (1<<DORD) : 0) |(dataMode & SPI_MODE_MASK) | ((clockDiv >> 1) & SPI_CLOCK_MASK);
	SPSR = clockDiv & SPI_2XCLOCK_MASK;
	
	CS_HIGH();

	return 1;
}

c_sdcard::e_card_errors c_sdcard::__card_init__()
{
	uint32_t arg = 0;
	//SDCARD_SPI.beginTransaction(settings);
	for (uint8_t i = 0; i < 10; i++) __spi_snd__(0XFF);

	//CS_LOW();

	// command to go idle in SPI mode
	t0 = SD_INIT_TIMEOUT;
	while ((status = __cardCommand__(CMD0, 0)) != R1_IDLE_STATE)
	{
		if (t0==0)
		{
			card_info.error = e_card_errors::SD_CARD_ERROR_CMD0;
			//goto fail;
			return card_info.error;
		}
	}
	// check SD version
	if ((__cardCommand__(CMD8, 0x1AA) & R1_ILLEGAL_COMMAND))
	{
		card_info.type = e_card_types::SD1;
	}
	else
	{
		// only need last byte of r7 response
		for (uint8_t i = 0; i < 4; i++) status = __spi_rec__();
		if (status != 0XAA)
		{
			card_info.error = e_card_errors::SD_CARD_ERROR_CMD8;
			//goto fail;
			return card_info.error;
		}
		card_info.type = e_card_types::SD2;
	}
	// initialize card and send host supports SDHC if SD2
	arg = card_info.type == e_card_types::SD2 ? 0X40000000 : 0;

	t0 = SD_INIT_TIMEOUT;
	while ((status = cardAcmd(ACMD41, arg)) != R1_READY_STATE) {
		// check for timeout
		if (!t0)
		{
			card_info.error = e_card_errors::SD_CARD_ERROR_ACMD41;
			//goto fail;
			return card_info.error;
		}
	}
	// if SD2 read OCR register to check for SDHC card
	if (card_info.type == e_card_types::SD2)
	{
		if (__cardCommand__(CMD58, 0))
		{
			card_info.error = e_card_errors::SD_CARD_ERROR_CMD58;
			//goto fail;
			return card_info.error;
		}
		if ((__spi_rec__() & 0XC0) == 0XC0) card_info.type = e_card_types::SDHC;
		// discard rest of ocr - contains allowed voltage range
		for (uint8_t i = 0; i < 3; i++) __spi_rec__();
	}
	
	card_info.error == e_card_errors::SD_CARD_OK;

	return card_info.error;
}

uint8_t c_sdcard::__spi_begin__()
{
	// set pin modes
	SPI_DIR |= CS;//set chip select pin to output
	//pinMode(chipSelectPin_, OUTPUT);
	SPI_PORT |= CS; //make output high
	//digitalWrite(chipSelectPin_, HIGH);

	SPCR |= (1<<MSTR);
	SPCR |= (1<<SPE);

	SPI_DIR |= SCK; //set clock to output
	SPI_DIR |= MOSI; //set mosi to output
	//SPI_DIR &= MISO; //set miso to input
}

uint8_t c_sdcard::__spi_rec__()
{
	return _____spi_transfer__(0xFF);
}

uint8_t c_sdcard::__spi_snd__(uint8_t data)
{
	return _____spi_transfer__(data);
}

uint8_t c_sdcard::_____spi_transfer__(uint8_t data)
{
	SPDR = data;
	/*
	* The following NOP introduces a small delay that can prevent the wait
	* loop from iterating when running at the maximum speed. This gives
	* about 10% more speed, even if it seems counter-intuitive. At lower
	* speeds it is unnoticed.
	*/
	asm volatile("nop");
	while (!(SPSR & (1<<SPIF))) ; // wait
	return SPDR;
}

uint8_t c_sdcard::cardAcmd(uint8_t cmd, uint32_t arg)
{
	__cardCommand__(CMD55, 0);
	return __cardCommand__(cmd, arg);
}

uint8_t c_sdcard::__cardCommand__(uint8_t cmd, uint32_t arg)
{
	// end read if in partialBlockRead mode
	//readEnd();
	uint8_t status = 0;
	// select card
	CS_LOW();

	//Talos::Coordinator::Main_Process::host_serial.print_string("wait start\r\n");
	// wait up to 300 ms if busy
	__busy_wait__(300);
	//Talos::Coordinator::Main_Process::host_serial.print_string("wait end\r\n");
	// send command
	__spi_snd__(cmd | 0x40);
	//Talos::Coordinator::Main_Process::host_serial.print_string("sent 1\r\n");

	// send argument
	for (int8_t s = 24; s >= 0; s -= 8) __spi_snd__(arg >> s);

	//Talos::Coordinator::Main_Process::host_serial.print_string("args\r\n");

	// send CRC
	uint8_t crc = 0XFF;
	if (cmd == CMD0) crc = 0X95;  // correct crc for CMD0 with arg 0
	if (cmd == CMD8) crc = 0X87;  // correct crc for CMD8 with arg 0X1AA
	__spi_snd__(crc);
	//Talos::Coordinator::Main_Process::host_serial.print_string("sent 2\r\n");

	// wait for response
	for (uint8_t i = 0; ((status = __spi_rec__()) & 0X80) && i != 0XFF; i++);

	//Talos::Coordinator::Main_Process::host_serial.print_string("rx = ");
	//Talos::Coordinator::Main_Process::host_serial.print_int32(status);
	//Talos::Coordinator::Main_Process::host_serial.print_string("\r\n");
	return status;
}

uint8_t c_sdcard::__busy_wait__(uint16_t timeout_millis)
{
	uint16_t old_t0 = t0;
	uint16_t t0 = timeout_millis;
	do {
		if (__spi_rec__() == 0XFF) return true;
	}
	while (!t0);
	t0 = old_t0;
	return false;
}

uint8_t c_sdcard::__readBlock__(uint32_t block, uint8_t* dst)
{
	return __readData__(block, 0, 512, dst);
}

uint8_t c_sdcard::__readData__(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst)
{
	if (count == 0) return true;
	if ((count + offset) > 512) {
		{	CS_HIGH();return false;}
	}
	if (!inBlock_ || block != block_ || offset < offset_) {
		block_ = block;
		// use address if not SDHC card
		if (card_info.type!= e_card_types::SDHC) block <<= 9;
		if (__cardCommand__(CMD17, block))
		{
			card_info.error = e_card_errors::SD_CARD_ERROR_CMD17;
			CS_HIGH();
			return false;
		}
		if (!__waitStartBlock__())
		{
			CS_HIGH();
			return false;
		}
		offset_ = 0;
		inBlock_ = 1;
	}

	//#ifdef OPTIMIZE_HARDWARE_SPI
	// start first spi transfer
	SPDR = 0XFF;

	// skip data before offset
	for (;offset_ < offset; offset_++) {
		while (!(SPSR & (1 << SPIF)))
		;
		SPDR = 0XFF;
	}
	// transfer data
	uint16_t n = count - 1;
	for (uint16_t i = 0; i < n; i++) {
		while (!(SPSR & (1 << SPIF)))
		;
		dst[i] = SPDR;
		SPDR = 0XFF;
	}
	// wait for last byte
	while (!(SPSR & (1 << SPIF)))
	;
	dst[n] = SPDR;

	//#else  // OPTIMIZE_HARDWARE_SPI
	//
	//// skip data before offset
	//for (;offset_ < offset; offset_++) {
	//__spi_rec__();
	//}
	//// transfer data
	//for (uint16_t i = 0; i < count; i++) {
	//dst[i] = __spi_rec__();
	//}
	//#endif  // OPTIMIZE_HARDWARE_SPI

	offset_ += count;
	if (!partialBlockRead_ || offset_ >= 512) {
		// read rest of data, checksum and set chip select high
		readEnd();
	}
	return true;

	CS_HIGH();
	return false;
}

void c_sdcard::readEnd(void) {
	if (inBlock_)
	{
		// skip data and crc
		//#ifdef OPTIMIZE_HARDWARE_SPI
		// optimize skip for hardware
		SPDR = 0XFF;
		while (offset_++ < 513) {
			while (!(SPSR & (1 << SPIF)))
			;
			SPDR = 0XFF;
		}
		// wait for last crc byte
		while (!(SPSR & (1 << SPIF)))
		;
		//#else  // OPTIMIZE_HARDWARE_SPI
		//while (offset_++ < 514) __spi_rec__();
		//#endif  // OPTIMIZE_HARDWARE_SPI
		CS_HIGH();
		inBlock_ = 0;
	}
}

uint8_t c_sdcard::__waitStartBlock__(void)
{
	uint16_t t0 = SD_READ_TIMEOUT;
	while ((status = __spi_rec__()) == 0XFF)
	{
		if (t0 > SD_READ_TIMEOUT)
		{
			card_info.error = e_card_errors::SD_CARD_ERROR_READ_TIMEOUT;
			goto fail;
		}
	}
	if (status != DATA_START_BLOCK)
	{
		card_info.error = e_card_errors::SD_CARD_ERROR_READ;
		goto fail;
	}
	return true;

	fail:
	CS_HIGH();
	return false;
}

uint8_t c_sdcard::__writeBlock__(uint32_t blockNumber, const uint8_t* src)
{
	#if SD_PROTECT_BLOCK_ZERO
	// don't allow write to first block
	if (blockNumber == 0) {
		card_info.error = e_card_errors::SD_CARD_ERROR_WRITE_BLOCK_ZERO;
		goto fail;
	}
	#endif  // SD_PROTECT_BLOCK_ZERO

	// use address if not SDHC card
	if (card_info.type != e_card_types::SDHC) blockNumber <<= 9;
	if (__cardCommand__(CMD24, blockNumber))
	{
		card_info.error = e_card_errors::SD_CARD_ERROR_CMD24;
		goto fail;
	}
	if (!__writeData__(DATA_START_BLOCK, src)) goto fail;

	// wait for flash programming to complete
	if (!__busy_wait__ (SD_WRITE_TIMEOUT))
	{
		card_info.error = e_card_errors::SD_CARD_ERROR_WRITE_TIMEOUT;
		goto fail;
	}
	// response is r2 so get and check two bytes for nonzero
	if (__cardCommand__(CMD13, 0) || __spi_rec__())
	{
		card_info.error = e_card_errors::SD_CARD_ERROR_WRITE_PROGRAMMING;
		goto fail;
	}
	CS_HIGH();
	return true;

	fail:
	CS_HIGH();
	return false;
}

uint8_t c_sdcard::__writeData__(const uint8_t* src) {
	// wait for previous write to finish
	if (!__busy_wait__(SD_WRITE_TIMEOUT))
	{
		card_info.error = e_card_errors::SD_CARD_ERROR_WRITE_MULTIPLE;
		CS_HIGH();
		return false;
	}
	return __writeData__(WRITE_MULTIPLE_TOKEN, src);
}

uint8_t c_sdcard::__writeData__(uint8_t token, const uint8_t* src)
{
	//#ifdef OPTIMIZE_HARDWARE_SPI

	// send data - optimized loop
	SPDR = token;

	// send two byte per iteration
	for (uint16_t i = 0; i < 512; i += 2) {
		while (!(SPSR & (1 << SPIF)))
		;
		SPDR = src[i];
		while (!(SPSR & (1 << SPIF)))
		;
		SPDR = src[i+1];
	}

	// wait for last data byte
	while (!(SPSR & (1 << SPIF)))
	;

	//#else  // OPTIMIZE_HARDWARE_SPI
	//spiSend(token);
	//for (uint16_t i = 0; i < 512; i++) {
	//spiSend(src[i]);
	//}
	//#endif  // OPTIMIZE_HARDWARE_SPI
	__spi_snd__(0xff);  // dummy crc
	__spi_snd__(0xff);  // dummy crc

	status = __spi_rec__();
	if ((status & DATA_RES_MASK) != DATA_RES_ACCEPTED)
	{
		card_info.error = e_card_errors::SD_CARD_ERROR_WRITE;
		CS_HIGH();
		return false;
	}
	return true;
}


ISR(TIMER0_COMPA_vect)
{

	if (t0>0)
	{
		t0--;
	}
}
