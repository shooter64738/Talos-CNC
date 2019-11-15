/*
* c_sdcard.h
*
* Created: 11/14/2019 1:11:39 PM
* Author: jeff_d
*/

//Much of this is duplicated from arduino code, but it was atoo 'heavy'
//for my purposes so this is jsut a lighter version of that code.
#ifndef __C_SDCARD_H__
#define __C_SDCARD_H__

#include <stdint.h>
#include <avr/interrupt.h>

#define CS (1<<PB0)
#define MOSI (1<<PB2)
#define MISO (1<<PB3)
#define SCK (1<<PB1)
#define SPI_DIR DDRB
#define SPI_PORT PORTB
#define CS_LOW() (PORTB &= ~CS)
#define CS_HIGH() (PORTB |= CS)

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

#define LSBFIRST 0
#define MSBFIRST 1


class c_sdcard
{
	//variables
	public:

	enum class e_card_types:uint8_t
	{
		Unknown = 0,
		SD1 = 1,
		SD2 = 2,
		SDHC = 3
	};
	
	enum class e_card_errors:uint8_t
	{
		//No errors
		SD_CARD_OK = 0X0,
		/** timeout error for command CMD0 */
		SD_CARD_ERROR_CMD0 = 0X1,
		/** CMD8 was not accepted - not a valid SD card*/
		SD_CARD_ERROR_CMD8 = 0X2,
		/** card returned an error response for CMD17 (read block) */
		SD_CARD_ERROR_CMD17 = 0X3,
		/** card returned an error response for CMD24 (write block) */
		SD_CARD_ERROR_CMD24 = 0X4,
		/**  WRITE_MULTIPLE_BLOCKS command failed */
		SD_CARD_ERROR_CMD25 = 0X05,
		/** card returned an error response for CMD58 (read OCR) */
		SD_CARD_ERROR_CMD58 = 0X06,
		/** SET_WR_BLK_ERASE_COUNT failed */
		SD_CARD_ERROR_ACMD23 = 0X07,
		/** card's ACMD41 initialization process timeout */
		SD_CARD_ERROR_ACMD41 = 0X08,
		/** card returned a bad CSR version field */
		SD_CARD_ERROR_BAD_CSD = 0X09,
		/** erase block group command failed */
		SD_CARD_ERROR_ERASE = 0X0A,
		/** card not capable of single block erase */
		SD_CARD_ERROR_ERASE_SINGLE_BLOCK = 0X0B,
		/** Erase sequence timed out */
		SD_CARD_ERROR_ERASE_TIMEOUT = 0X0C,
		/** card returned an error token instead of read data */
		SD_CARD_ERROR_READ = 0X0D,
		/** read CID or CSD failed */
		SD_CARD_ERROR_READ_REG = 0X0E,
		/** timeout while waiting for start of read data */
		SD_CARD_ERROR_READ_TIMEOUT = 0X0F,
		/** card did not accept STOP_TRAN_TOKEN */
		SD_CARD_ERROR_STOP_TRAN = 0X10,
		/** card returned an error token as a response to a write operation */
		SD_CARD_ERROR_WRITE = 0X11,
		/** attempt to write protected block zero */
		SD_CARD_ERROR_WRITE_BLOCK_ZERO = 0X12,
		/** card did not go ready for a multiple block write */
		SD_CARD_ERROR_WRITE_MULTIPLE = 0X13,
		/** card returned an error to a CMD13 status check after a write */
		SD_CARD_ERROR_WRITE_PROGRAMMING = 0X14,
		/** timeout occurred during write programming */
		SD_CARD_ERROR_WRITE_TIMEOUT = 0X15,
		/** incorrect rate selected */
		SD_CARD_ERROR_SCK_RATE = 0X16
	};

	struct s_card_info
	{
		e_card_types type;
		e_card_errors error;
	};

	static s_card_info card_info;

	protected:
	private:

	//functions
	public:
	static s_card_info initialize();
	static uint8_t __writeBlock__(uint32_t blockNumber, const uint8_t* src);
	static uint8_t __readBlock__(uint32_t block, uint8_t* dst);
	//c_sdcard();
	//~c_sdcard();
	protected:
	private:

	


	static uint8_t __spi_init__();
	static c_sdcard::e_card_errors __card_init__();
	static uint8_t __spi_begin__();
		
	static uint8_t __spi_rec__();
	static uint8_t __spi_snd__(uint8_t data);
	static uint8_t _____spi_transfer__(uint8_t data);
	static uint8_t cardAcmd(uint8_t cmd, uint32_t arg);
	static uint8_t __cardCommand__(uint8_t cmd, uint32_t arg);
	static uint8_t __busy_wait__(uint16_t timeout_millis);
	
	static uint8_t __readData__(uint32_t block, uint16_t offset, uint16_t count, uint8_t* dst);
	static void readEnd(void);
	static uint8_t __waitStartBlock__(void);
	
	static uint8_t __writeData__(const uint8_t* src);
	static uint8_t __writeData__(uint8_t token, const uint8_t* src);
	//c_sdcard( const c_sdcard &c );
	//c_sdcard& operator=( const c_sdcard &c );

}; //c_sdcard

#endif //__C_SDCARD_H__
