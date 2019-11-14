/*
* c_core_avr_2560.cpp
*
* Created: 2/27/2019 3:47:52 PM
* Author: jeff_d
*/


#include "c_disk_avr_2560.h"
#include "disk_support/diskio.h"
#include <avr/interrupt.h>
#include "../../../../Coordinator/coordinator_hardware_def.h"
#include "../../../../Coordinator/Processing/Main/Main_Process.h"

//FRESULT fr = FR_OK;
uint8_t Hardware_Abstraction_Layer::Disk::initialize()
{
	OCR0A = F_CPU / 1024 / 1000 - 1;
	TCCR0A = (1<<WGM01);
	TCCR0B = 0b101;
	TIMSK0 = (1<<OCIE0A);

	__spi_init__();
	__card_init__();

	//fr = disk_initialize(0);// & STA_NOINIT;		/* Clear STA_NOINIT */;
	FATFS fs;
	
	FIL file1;
	UINT bw;
	
	//Talos::Coordinator::Main_Process::host_serial.print_string("mount\r\n");
	////fr = f_mount(&fs, "", 1);
	//if (fr == FR_OK)
	//{
	//Talos::Coordinator::Main_Process::host_serial.print_string("open\r\n");
	////fr = f_open(&file1, "coord.txt", FA_WRITE | FA_CREATE_ALWAYS |FA_OPEN_APPEND);
	//if (fr == FR_OK)
	//{
	//Talos::Coordinator::Main_Process::host_serial.print_string("write\r\n");
	//fr = f_write(&file1, "testing\r\n\0", 10, &bw);
	//}
	//
	//}
	//
	//
	//Talos::Coordinator::Main_Process::host_serial.print_int32(fr);
	return (uint8_t) 1; //fr;
}


void Hardware_Abstraction_Layer::Disk::write(const char * filename, char * buffer, e_file_modes mode)
{
}

#define CS (1<<PB0)
#define MOSI (1<<PB2)
#define MISO (1<<PB3)
#define SCK (1<<PB1)
#define SPI_DIR DDRB
#define SPI_PORT PORTB
#define CS_LOW() (PORTB &= ~CS)
#define CS_HIGH() (PORTB |= CS)

static uint8_t status = 0;
static uint8_t card_type = 0;
static uint8_t errorCode_ = 0;
volatile uint16_t t0 = 0;;

uint8_t __spi_begin__()
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

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

#define LSBFIRST 0
#define MSBFIRST 1


uint8_t __spi_init__()
{
	Talos::Coordinator::Main_Process::host_serial.print_string("init start\r\n");
	uint8_t sckRateID = 250000;
	uint8_t chipSelectPin_ = 53;
	//inBlock_ = partialBlockRead_ = type_ = 0;
	
	// 16-bit init start time allows over a minute
	
	

	// set pin modes
	//SPI_DIR |= CS;//set chip select pin to output
	//pinMode(chipSelectPin_, OUTPUT);
	//SPI_PORT |= CS; //make output high
	//digitalWrite(chipSelectPin_, HIGH);
	

	
	uint8_t clockDiv = 7;
	uint8_t dataMode = 0;
	Talos::Coordinator::Main_Process::host_serial.print_string("spi begin\r\n");
	__spi_begin__();
	// Pack into the SPISettings class
	uint8_t bitOrder = MSBFIRST;
	//settings = SPISettings(250000, MSBFIRST, SPI_MODE0);

	uint8_t settings_spcr = (1<<SPE) | (1<<MSTR) | ((bitOrder == LSBFIRST) ? (1<<DORD) : 0) |
	(dataMode & SPI_MODE_MASK) | ((clockDiv >> 1) & SPI_CLOCK_MASK);
	uint8_t settings_spsr = clockDiv & SPI_2XCLOCK_MASK;

	
	
	// must supply min of 74 clock cycles with CS high.
	SPCR = settings_spcr;
	SPSR = settings_spsr;
	CS_HIGH();
	
}

uint8_t __card_init__()
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
			Talos::Coordinator::Main_Process::host_serial.print_string("err 1\r\n");
			errorCode_ = SD_CARD_ERROR_CMD0;
			goto fail;
		}
	}
	// check SD version
	if ((__cardCommand__(CMD8, 0x1AA) & R1_ILLEGAL_COMMAND))
	{
		card_type = SD_CARD_TYPE_SD1;
	}
	else
	{
		// only need last byte of r7 response
		for (uint8_t i = 0; i < 4; i++) status = __spi_rec__();
		if (status != 0XAA)
		{
			Talos::Coordinator::Main_Process::host_serial.print_string("err 2\r\n");
			errorCode_ = SD_CARD_ERROR_CMD8;
			goto fail;
		}
		card_type = SD_CARD_TYPE_SD2;
	}
	// initialize card and send host supports SDHC if SD2
	arg = card_type == SD_CARD_TYPE_SD2 ? 0X40000000 : 0;

	t0 = SD_INIT_TIMEOUT;
	while ((status = cardAcmd(ACMD41, arg)) != R1_READY_STATE) {
		// check for timeout
		if (!t0)
		{
			Talos::Coordinator::Main_Process::host_serial.print_string("err 3\r\n");
			errorCode_ = SD_CARD_ERROR_ACMD41;
			goto fail;
		}
	}
	// if SD2 read OCR register to check for SDHC card
	if (card_type == SD_CARD_TYPE_SD2)
	{
		if (__cardCommand__(CMD58, 0))
		{
			Talos::Coordinator::Main_Process::host_serial.print_string("err 4\r\n");
			errorCode_ = SD_CARD_ERROR_CMD58;
			goto fail;
		}
		if ((__spi_rec__() & 0XC0) == 0XC0) card_type = SD_CARD_TYPE_SDHC;
		// discard rest of ocr - contains allowed voltage range
		for (uint8_t i = 0; i < 3; i++) __spi_rec__();
	}
	CS_HIGH();
	Talos::Coordinator::Main_Process::host_serial.print_string("complete!\r\n");
	Talos::Coordinator::Main_Process::host_serial.print_string("card type = ");
	Talos::Coordinator::Main_Process::host_serial.print_int32(card_type);
	Talos::Coordinator::Main_Process::host_serial.print_string("\r\n");
	//#ifndef SOFTWARE_SPI
	//return setSckRate(sckRateID);
	//#else  // SOFTWARE_SPI
	//return true;
	//#endif  // SOFTWARE_SPI

	fail:
	Talos::Coordinator::Main_Process::host_serial.print_string("END\r\n");
	CS_HIGH();
	return false;
}
uint8_t __spi_rec__()
{
	return _____spi_transfer__(0xFF);
}
uint8_t __spi_snd__(uint8_t data)
{
	return _____spi_transfer__(data);
}
uint8_t _____spi_transfer__(uint8_t data)
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
uint8_t cardAcmd(uint8_t cmd, uint32_t arg)
{
	__cardCommand__(CMD55, 0);
	return __cardCommand__(cmd, arg);
}
uint8_t __cardCommand__(uint8_t cmd, uint32_t arg)
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
uint8_t __busy_wait__(uint16_t timeout_millis)
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

uint8_t __readBlock__(uint32_t block, uint8_t* dst)
{
	return __readData__(block, 0, 512, dst);
}

uint8_t __readData__(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst)
{
	if (count == 0) return true;
	if ((count + offset) > 512) {
		goto fail;
	}
	if (!inBlock_ || block != block_ || offset < offset_)
	{
		block_ = block;
		// use address if not SDHC card
		if (type()!= SD_CARD_TYPE_SDHC) block <<= 9;
		if (__cardCommand__(CMD17, block)) 
		{
			errorCode_ = SD_CARD_ERROR_CMD17;
			goto fail;
		}
		if (!__waitStartBlock__())
		{
			goto fail;
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
	n = count - 1;
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
		//spiRec();
	//}
	//// transfer data
	//for (uint16_t i = 0; i < count; i++) {
		//dst[i] = spiRec();
	//}
	//#endif  // OPTIMIZE_HARDWARE_SPI

	offset_ += count;
	if (!partialBlockRead_ || offset_ >= 512) {
		// read rest of data, checksum and set chip select high
		readEnd();
	}
	return true;

	fail:
	CS_HIGH();
	return false;
}

uint8_t __waitStartBlock__(void) {
	uint16_t t0 = SD_READ_TIMEOUT;
	while ((status = __spi_rec__()) == 0XFF)
	{
		if (t0 > SD_READ_TIMEOUT)
		{
			errorCode_ = SD_CARD_ERROR_READ_TIMEOUT;
			goto fail;
		}
	}
	if (status != DATA_START_BLOCK)
	{
		errorCode_ = SD_CARD_ERROR_READ;
		goto fail;
	}
	return true;

	fail:
	CS_HIGH();
	return false;
}

uint8_t __writeBlock__(uint32_t blockNumber, const uint8_t* src)
{
	#if SD_PROTECT_BLOCK_ZERO
	// don't allow write to first block
	if (blockNumber == 0) {
		errorCode_ SD_CARD_ERROR_WRITE_BLOCK_ZERO;
		goto fail;
	}
	#endif  // SD_PROTECT_BLOCK_ZERO

	// use address if not SDHC card
	if (card_type != SD_CARD_TYPE_SDHC) blockNumber <<= 9;
	if (__cardCommand__(CMD24, blockNumber))
	{
		errorCode_ =SD_CARD_ERROR_CMD24;
		goto fail;
	}
	if (!__writeData__(DATA_START_BLOCK, src)) goto fail;

	// wait for flash programming to complete
	if (!__busy_wait__ (SD_WRITE_TIMEOUT))
	{
		errorCode_ = SD_CARD_ERROR_WRITE_TIMEOUT;
		goto fail;
	}
	// response is r2 so get and check two bytes for nonzero
	if (__cardCommand__(CMD13, 0) || __spi_rec__())
	{
		errorCode_ = SD_CARD_ERROR_WRITE_PROGRAMMING;
		goto fail;
	}
	CS_HIGH();
	return true;

	fail:
	CS_HIGH();
	return false;
}

//------------------------------------------------------------------------------
/** Write one data block in a multiple block write sequence */
uint8_t __writeData__(const uint8_t* src) {
	// wait for previous write to finish
	if (!__busy_wait__(SD_WRITE_TIMEOUT))
	{
		errorCode_ = SD_CARD_ERROR_WRITE_MULTIPLE;
		CS_HIGH();
		return false;
	}
	return __writeData__(WRITE_MULTIPLE_TOKEN, src);
}
//------------------------------------------------------------------------------
// send one block of data for write block or write multiple blocks
uint8_t __writeData__(uint8_t token, const uint8_t* src)
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
		errorCode_ = SD_CARD_ERROR_WRITE;
		CS_HIGH
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


//-----------------------------volume
