#include <stdint.h>
#include <stm32h7xx_hal.h>
#include <stm32_hal_legacy.h>

//#include "../FatFs/ff_gen_drv.h"
#include "../FatFs/ff.h"
#include "../FatFs/diskio.h"

#ifndef __SPI_SD_CARD_H__
#define __SPI_SD_CARD_H__

//Card commands
#define SD_CMD_GO_IDLE_STATE          0U   /* CMD0 = 0x40  */
#define SD_CMD_SEND_OP_COND           1U   /* CMD1 = 0x41  */
#define SD_CMD_SEND_IF_COND           8U   /* CMD8 = 0x48  */
#define SD_CMD_SEND_CSD               9U   /* CMD9 = 0x49  */
#define SD_CMD_SEND_CID               10U  /* CMD10 = 0x4A */
#define SD_CMD_STOP_TRANSMISSION      12U  /* CMD12 = 0x4C */
#define SD_CMD_SEND_STATUS            13U  /* CMD13 = 0x4D */
#define SD_CMD_SET_BLOCKLEN           16U  /* CMD16 = 0x50 */
#define SD_CMD_READ_SINGLE_BLOCK      17U  /* CMD17 = 0x51 */
#define SD_CMD_READ_MULT_BLOCK        18U  /* CMD18 = 0x52 */
#define SD_CMD_SET_BLOCK_COUNT        23U  /* CMD23 = 0x57 */
#define SD_CMD_WRITE_SINGLE_BLOCK     24U  /* CMD24 = 0x58 */
#define SD_CMD_WRITE_MULT_BLOCK       25U  /* CMD25 = 0x59 */
#define SD_CMD_PROG_CSD               27U  /* CMD27 = 0x5B */
#define SD_CMD_SET_WRITE_PROT         28U  /* CMD28 = 0x5C */
#define SD_CMD_CLR_WRITE_PROT         29U  /* CMD29 = 0x5D */
#define SD_CMD_SEND_WRITE_PROT        30U  /* CMD30 = 0x5E */
#define SD_CMD_SD_ERASE_GRP_START     32U  /* CMD32 = 0x60 */
#define SD_CMD_SD_ERASE_GRP_END       33U  /* CMD33 = 0x61 */
#define SD_CMD_UNTAG_SECTOR           34U  /* CMD34 = 0x62 */
#define SD_CMD_ERASE_GRP_START        35U  /* CMD35 = 0x63 */
#define SD_CMD_ERASE_GRP_END          36U  /* CMD36 = 0x64 */
#define SD_CMD_UNTAG_ERASE_GROUP      37U  /* CMD37 = 0x65 */
#define SD_CMD_ERASE                  38U  /* CMD38 = 0x66 */
#define SD_CMD_SD_APP_OP_COND         41U  /* CMD41 = 0x69 */
#define SD_CMD_APP_CMD                55U  /* CMD55 = 0x77 */
#define SD_CMD_READ_OCR               58U  /* CMD55 = 0x79 */

//Card Types
#define SDSC_NORM_CAPACITY     0UL /* SD Standard Capacity */
#define SDHC_LARG_CAPACITY     1UL /* SD High Capacity     */

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum
	{
		SD_ANSWER_R1_EXPECTED,
		SD_ANSWER_R1B_EXPECTED,
		SD_ANSWER_R2_EXPECTED,
		SD_ANSWER_R3_EXPECTED,
		SD_ANSWER_R4R5_EXPECTED,
		SD_ANSWER_R7_EXPECTED,
	}SD_Answer_t;

	typedef enum
	{
		/* R1 answer value */
		SD_R1_NO_ERROR = 0x00U,
		SD_R1_IN_IDLE_STATE = 0x01U,
		SD_R1_ERASE_RESET = 0x02U,
		SD_R1_ILLEGAL_COMMAND = 0x04U,
		SD_R1_COM_CRC_ERROR = 0x08U,
		SD_R1_ERASE_SEQUENCE_ERROR = 0x10U,
		SD_R1_ADDRESS_ERROR = 0x20U,
		SD_R1_PARAMETER_ERROR = 0x40U,

		/* R2 answer value */
		SD_R2_NO_ERROR = 0x00U,
		SD_R2_CARD_LOCKED = 0x01U,
		SD_R2_LOCKUNLOCK_ERROR = 0x02U,
		SD_R2_ERROR = 0x04U,
		SD_R2_CC_ERROR = 0x08U,
		SD_R2_CARD_ECC_FAILED = 0x10U,
		SD_R2_WP_VIOLATION = 0x20U,
		SD_R2_ERASE_PARAM = 0x40U,
		SD_R2_OUTOFRANGE = 0x80U,

		/**
		  * @brief  Data response error
		  */
		SD_DATA_OK = 0x05U,
		SD_DATA_CRC_ERROR = 0x0BU,
		SD_DATA_WRITE_ERROR = 0x0DU,
		SD_DATA_OTHER_ERROR = 0xFFU
	} SD_Error_t;

	typedef struct
	{
		uint32_t Reserved1 : 2U;               /* Reserved */
		uint32_t DeviceSize : 12U;             /* Device Size */
		uint32_t MaxRdCurrentVDDMin : 3U;      /* Max. read current @ VDD min */
		uint32_t MaxRdCurrentVDDMax : 3U;      /* Max. read current @ VDD max */
		uint32_t MaxWrCurrentVDDMin : 3U;      /* Max. write current @ VDD min */
		uint32_t MaxWrCurrentVDDMax : 3U;      /* Max. write current @ VDD max */
		uint32_t DeviceSizeMul : 3U;           /* Device size multiplier */
	} SD_Version_1_t;


	typedef struct
	{
		uint32_t Reserved1 : 6U;               /* Reserved */
		uint32_t DeviceSize : 22U;             /* Device Size */
		uint32_t Reserved2 : 1U;               /* Reserved */
	} SD_Version_2_t;

	/**
	  * @brief  Card Specific Data: CSD Register
	  */
	typedef struct
	{
		/* Header part */
		uint32_t  CSDStruct : 2U;            /* CSD structure */
		uint32_t  Reserved1 : 6U;            /* Reserved */
		uint32_t  TAAC : 8U;                 /* Data read access-time 1 */
		uint32_t  NSAC : 8U;                 /* Data read access-time 2 in CLK cycles */
		uint32_t  MaxBusClkFrec : 8U;        /* Max. bus clock frequency */
		uint32_t  CardComdClasses : 12U;     /* Card command classes */
		uint32_t  RdBlockLen : 4U;           /* Max. read data block length */
		uint32_t  PartBlockRead : 1U;        /* Partial blocks for read allowed */
		uint32_t  WrBlockMisalign : 1U;      /* Write block misalignment */
		uint32_t  RdBlockMisalign : 1U;      /* Read block misalignment */
		uint32_t  DSRImpl : 1U;              /* DSR implemented */

		/* v1 or v2 struct */
		union csd_version
		{
			SD_Version_1_t v1;
			SD_Version_2_t v2;
		} version;

		uint32_t  EraseSingleBlockEnable : 1U;  /* Erase single block enable */
		uint32_t  EraseSectorSize : 7U;         /* Erase group size multiplier */
		uint32_t  WrProtectGrSize : 7U;         /* Write protect group size */
		uint32_t  WrProtectGrEnable : 1U;       /* Write protect group enable */
		uint32_t  Reserved2 : 2U;               /* Reserved */
		uint32_t  WrSpeedFact : 3U;             /* Write speed factor */
		uint32_t  MaxWrBlockLen : 4U;           /* Max. write data block length */
		uint32_t  WriteBlockPartial : 1U;       /* Partial blocks for write allowed */
		uint32_t  Reserved3 : 5U;               /* Reserved */
		uint32_t  FileFormatGrouop : 1U;        /* File format group */
		uint32_t  CopyFlag : 1U;                /* Copy flag (OTP) */
		uint32_t  PermWrProtect : 1U;           /* Permanent write protection */
		uint32_t  TempWrProtect : 1U;           /* Temporary write protection */
		uint32_t  FileFormat : 2U;              /* File Format */
		uint32_t  Reserved4 : 2U;               /* Reserved */
		uint32_t  crc : 7U;                     /* Reserved */
		uint32_t  Reserved5 : 1U;               /* always 1*/

	} SD_CardSpecificData_t;

	/**
	  * @brief  Card Identification Data: CID Register
	  */
	typedef struct
	{
		uint32_t  ManufacturerID;       /* ManufacturerID */
		uint32_t  OEM_AppliID;          /* OEM/Application ID */
		uint32_t  ProdName1;            /* Product Name part1 */
		uint32_t  ProdName2;            /* Product Name part2*/
		uint32_t  ProdRev;              /* Product Revision */
		uint32_t  ProdSN;               /* Product Serial Number */
		uint32_t  Reserved1;            /* Reserved1 */
		uint32_t  ManufactDate;         /* Manufacturing Date */
		uint32_t  CID_CRC;              /* CID CRC */
		uint32_t  Reserved2;            /* always 1 */
	} SD_CardIdData_t;

	/**
	  * @brief SD Card information
	  */
	typedef struct
	{
		SD_CardSpecificData_t Csd;
		SD_CardIdData_t Cid;
		uint32_t CardCapacity;              /*!< Card Capacity */
		uint32_t CardBlockSize;             /*!< Card Block Size */
		uint32_t LogBlockNbr;               /*!< Specifies the Card logical Capacity in blocks   */
		uint32_t LogBlockSize;              /*!< Specifies logical block size in bytes           */
	} SD_CardInfo_t;


	//These are exposed functions that can be called from outside sd_card.c
	int32_t SD_IO_Init(SPI_TypeDef* SPIxInstance);
		
	DWORD get_fattime(void);
	DSTATUS pntr_from_link_drvr_SD_initialize(BYTE);
	DSTATUS pntr_from_link_drvr_SD_status(BYTE);
	DRESULT pntr_from_link_drvr_SD_read(BYTE, BYTE*, DWORD, UINT);
	DRESULT pntr_from_link_drvr_SD_write(BYTE, const BYTE*, DWORD, UINT);
	DRESULT pntr_from_link_drvr_SD_io(BYTE lun, BYTE cmd, void* buff);

#ifdef __cplusplus
}
#endif
#endif // !__SPI_SD_CARD_H__