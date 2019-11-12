/*
* c_serial_avr_2560.h
*
* Created: 2/27/2019 3:54:31 PM
* Author: jeff_d
*/


#ifndef __C_DISK_ARM_3X8E_H__
#define __C_DISK_ARM_3X8E_H__

/* Definitions for MMC/SDC command */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(32)		/* ERASE_ER_BLK_START */
#define CMD33	(33)		/* ERASE_ER_BLK_END */
#define CMD38	(38)		/* ERASE */
#define	CMD48	(48)		/* READ_EXTR_SINGLE */
#define	CMD49	(49)		/* WRITE_EXTR_SINGLE */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

#include <stdint.h>
#include <stddef.h>
#include "disk/ff.h"
#include "disk/diskio.h"
//#include "diskio.h"
//#include "pff.h"
//#include "xitoa.h"
//#include "suart.h"



namespace Hardware_Abstraction_Layer
{
	class Disk
	{
		//variables
		public:
		static uint32_t time_out_ticks;
		struct s_file_info
		{
			FIL _handle;
			BYTE _mode = FA_READ;
			char * _name;
		};
		
		static s_file_info machine_param_file;
		static s_file_info global_named_param_file;
		static s_file_info local_named_param_file;
		
		protected:
		private:
		//functions
		public:
		static uint16_t initialize();
		
		
		protected:
		private:
		static void ___configure_timer();
		static FRESULT __open_machine_parameters();
		static void __close_machine_parameters();
		static FRESULT __open_named_parameters();
		static void __close_named_parameters();
	};
};

#endif //__SAM3X8E__

DSTATUS mmc_disk_initialize ();
void power_off (void);
void power_on (void);
BYTE send_cmd (BYTE cmd,DWORD arg);
void deselect (void);
int select (void);	/* 1:Successful, 0:Timeout */
DSTATUS mmc_disk_status (void);
DRESULT mmc_disk_read (BYTE *buff,DWORD sector,UINT count);
DRESULT mmc_disk_write (const BYTE *buff,DWORD sector,UINT count);
int rcvr_datablock (BYTE *buff,UINT btr);
void rcvr_spi_multi (BYTE *p, UINT cnt);
int xmit_datablock (const BYTE *buff,BYTE token);
void xmit_spi_multi (const BYTE *p,UINT cnt);
int wait_ready (UINT wt);
BYTE sd_send_byte(BYTE data);
