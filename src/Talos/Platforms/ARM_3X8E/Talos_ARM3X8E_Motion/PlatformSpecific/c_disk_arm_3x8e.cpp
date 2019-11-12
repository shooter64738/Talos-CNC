/*
* c_serial_avr_2560.cpp
*
* Created: 2/27/2019 3:54:31 PM
* Author: jeff_d
*/


#include "c_disk_arm_3x8e.h"
#include "disk\diskio.h"
#include "c_spi_arm_3x8e.h"
#include "..\..\..\..\Motion\Processing\Main\Main_Process.h"

static volatile
DSTATUS Stat = STA_NOINIT;	/* Disk status */
static volatile
uint32_t Timer1, Timer2;	/* 100Hz decrement timer */
static
BYTE CardType;			/* Card type flags (b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing) */

FATFS FatFs;

Hardware_Abstraction_Layer::Disk::s_file_info Hardware_Abstraction_Layer::Disk::machine_param_file;
Hardware_Abstraction_Layer::Disk::s_file_info Hardware_Abstraction_Layer::Disk::global_named_param_file;
Hardware_Abstraction_Layer::Disk::s_file_info Hardware_Abstraction_Layer::Disk::local_named_param_file;
#define CS_LOW() (PIOA->PIO_CODR |= PIO_PA28A_SPI0_NPCS0)
#define CS_HIGH() (PIOA->PIO_SODR |= PIO_PA28A_SPI0_NPCS0)

//#define spi_cs_low() while(!(PIOD->PIO_PDSR & (PIO_PD8))); do { PIOA->PIO_CODR = PIO_PA28; } while (0)
//#define spi_cs_high() do { PIOA->PIO_SODR = PIO_PA28; } while (0)
#define _BV(bit) (1 << (bit))

#define SPI_CLOCK_FREQ(baud)	((uint32_t)(MASTER_CLOCK/baud)<<8) //Variable Frequency, baud in Hz

#define TIME_OUT_TIMER_CLOCK TC_CMR_TCCLKS_TIMER_CLOCK4
uint32_t Hardware_Abstraction_Layer::Disk::time_out_ticks = 0;
//#define UPDATE_INTERVAL_TIMER_CLOCK TC_CMR_TCCLKS_TIMER_CLOCK1
//#define STEP_CLOCK_DIVIDER 1
//#define SYS_TICKS (84)

uint16_t Hardware_Abstraction_Layer::Disk::initialize()
{
	FRESULT fr = FR_OK;
	UINT bw;
	//Hardware_Abstraction_Layer::Disk::___configure_timer();
	//return 1;
	Hardware_Abstraction_Layer::Disk::time_out_ticks++;
	Stat = disk_initialize(1);
	if (Stat!= 0)
	return Stat;
	
	Talos::Motion::Main_Process::host_serial.print_string("mount\r\n");
	fr = f_mount(&FatFs, "", 0);
	
	FIL file1;
	
	if (fr == FR_OK)
	{
		Talos::Motion::Main_Process::host_serial.print_string("open\r\n");
		fr = f_open(&file1, "talos.txt", FA_WRITE | FA_CREATE_ALWAYS |FA_OPEN_APPEND);
		if (fr == FR_OK)
		{
			Talos::Motion::Main_Process::host_serial.print_string("write\r\n");
				fr = f_write(&file1, "testing\r\n\0", 10, &bw);
		}
	}
	//mount the drive and register the work area
	//f_mount(&FatFs, "", 0);
	//machine_param_file._name="mac_par.txt";
	//machine_param_file._mode = FA_WRITE | FA_CREATE_ALWAYS;
	//global_named_param_file._name="gn_par.txt";
	//local_named_param_file._name="ln_par.txt";
	return fr;
}
volatile uint32_t sys_ticks = 0;
void SysTick_Handler(void)
{
	sys_ticks++;
	//if (sys_ticks >= (840000)) //tick seconds
	//{
	//Hardware_Abstraction_Layer::Disk::time_out_ticks++;
	//sys_ticks = 0;
	//}
	//if (sys_ticks >= (84000)) //tick centi-seconds
	//{
	//Hardware_Abstraction_Layer::Disk::time_out_ticks++;
	//sys_ticks = 0;
	//}
	if (sys_ticks >= (840)) //tick milliseconds
	{
		
		sys_ticks = 0;
		if (Timer1) Timer1--;
		if (Timer2) Timer2--;
	}
	//if (sys_ticks >= (84000000)/1)
	//{
	//	sys_ticks = 0;
	//}
	/* DO FUN STUFF HERE
	* OR Just increment a counter to deal with in the main loop */
}

void Hardware_Abstraction_Layer::Disk::___configure_timer()
{
	//Setup a timer. This timer will determine when we have timed out for spindle_at_speed
	PMC->PMC_PCER0 |= 1 << ID_TC4;
	//TIME_OUT_TIMER_CLOCK value is 84,000,000 (cpu speed) / 128. /1000 is 656.250 for 1 milli second
	//we will track how man times the timer fires and when we reach the specified timeout period we
	//will raise the timeout error.
	TC1->TC_CHANNEL[1].TC_CMR =  TIME_OUT_TIMER_CLOCK | TC_CMR_WAVSEL_UP_RC;// | TC_CMR_ACPA_TOGGLE;
	//TC1->TC_CHANNEL[1].TC_RA = 240; //<--stepper pulse on time
	TC1->TC_CHANNEL[1].TC_RC = 656;//250;//<--total time between steps
	TC1->TC_CHANNEL[1].TC_IER = TC_IER_CPCS | TC_IER_CPAS;
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
	NVIC_SetPriority(TC4_IRQn, 3);
	NVIC_EnableIRQ (TC4_IRQn);
	
	//if (Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->spindle_synch_wait_time_ms)
	//{
	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
	//}
}

void Timer1_Chan0_Handler_irq4(void)
{
	
	uint32_t status_reg = TC1->TC_CHANNEL[1].TC_SR;
	//UART->UART_THR = 'C';
	//TC1->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;
	//Check to see if this is the A interrupt flag. If it is this is the 'on time' pulse start
	if (status_reg & TC_IER_CPAS)
	{
		//UART->UART_THR = 'A';
	}
	//Check to see if this is the C interrupt flag. If it is this is the 'total time' pulse end
	if (status_reg & TC_IER_CPCS)
	{
		//UART->UART_THR = 'B';
		Hardware_Abstraction_Layer::Disk::time_out_ticks++;
		//if (time_out_ticks == Hardware_Abstraction_Layer::MotionCore::Spindle::spindle_encoder->spindle_synch_wait_time_ms)
		//{
		//	TC1->TC_CHANNEL[1].TC_CCR = TC_CCR_CLKDIS;
		//	time_out_ticks = 0;
		//}
	}
}

FRESULT Hardware_Abstraction_Layer::Disk::__open_machine_parameters()
{
	FRESULT ret_code = FRESULT::FR_OK;
	
	//char line[100]; /* Line buffer */
	//FRESULT fr;     /* FatFs return code */
	//

	ret_code = f_open(&machine_param_file._handle, machine_param_file._name, machine_param_file._mode);
	if (ret_code) return ret_code;

	///* Read every line and display it */
	//while (f_gets(line, sizeof line, &fil)) {
	//printf(line);
	//}

	/* Close the file */
	//f_close(&fil);

	//return 0;
	
	return ret_code;

}

void Hardware_Abstraction_Layer::Disk::__close_machine_parameters()
{
	/* Close the file */
	f_close(&machine_param_file._handle);
}

FRESULT Hardware_Abstraction_Layer::Disk::__open_named_parameters()
{
	FRESULT ret_code = FRESULT::FR_OK;
	ret_code = f_open(&machine_param_file._handle, machine_param_file._name, machine_param_file._mode);
	if (ret_code) return ret_code;
	return ret_code;

}

void Hardware_Abstraction_Layer::Disk::__close_named_parameters()
{
	/* Close the file */
	f_close(&machine_param_file._handle);
}

void delay_ms(uint32_t delay_time)
{
	Timer1 = delay_time;
	while(Timer1 > 0){}
}

static void spi_init(void)
{
	Hardware_Abstraction_Layer::SPI::initialize_general();
	return;
	
	PMC->PMC_PCER0 |= _BV(ID_PIOA);	//Enable Clock PortA
	
	//configure for input			//MISO
	PIOA->PIO_PDR |= PIO_PA25;
	PIOA->PIO_ODR |= PIO_PA25;		//Input
	
	PIOA->PIO_PDR |= PIO_PA26;		//MOSI
	PIOA->PIO_OER |= PIO_PA26;		//MOSI	Output
	PIOA->PIO_ABSR &= ~PIO_PA26;	//Peripheral A
	
	PIOA->PIO_PDR |= PIO_PA27;		//SPCK
	PIOA->PIO_OER |= PIO_PA27;		//SPCK	Output
	PIOA->PIO_ABSR &= ~PIO_PA27;	//Peripheral A
	
	PIOA->PIO_OER |= PIO_PA28;		//NPCS0	Output
/*	PIOA->PIO_ABSR &= ~PIO_PA28;	//Peripheral A
	PIOA->PIO_PUER |= PIO_PA28;		//pull-up*/

	//Enable clock for the SPI0 peripheral
	PMC->PMC_PCER0 |= _BV(ID_SPI0);

	//Disable the SPI0 peripheral so we can configure it.
	SPI0->SPI_CR = SPI_CR_SPIDIS;

	//Set as Master, Fixed Peripheral Select, Mode Fault Detection disabled and
	//	Peripheral Chip Select is PCS = xxx0 NPCS[3:0] = 1110
	SPI0->SPI_MR = SPI_MR_MSTR | SPI_MR_MODFDIS | 0x000e0000;
	
	//SPCK baudrate = MCK / SCBR = 84MHz / 128 = 656250Hz
	SPI0->SPI_CSR[0] |= 0x0000FF00  | 1 << 1;
	SPI0->SPI_CSR[0] |= SPI_CSR_SCBR(250);
	//SPI0->SPI_CSR[0] |= 0x0000FFFF  | 1 << 1;
	//SPI0->SPI_CSR[0] |= SPI_CSR_SCBR(128);
	
	//Enable the SPI0 unit
	SPI0->SPI_CR = SPI_CR_SPIEN;


//	spi_set_speed(SD_SPEED_400KHZ);
}

DSTATUS mmc_disk_initialize ()
{
	spi_init();
	
	BYTE n, cmd, ty, ocr[4];
	BYTE ret=0;
	power_off();						// Turn off the socket power to reset the card
	delay_ms(10);		// Wait for 100ms
	if (Stat & STA_NODISK) return Stat;	// No card in the socket?

	power_on();							// Turn on the socket power
	//spi_cs_high();
	CS_HIGH();
	//-----FCLK_SLOW();
	for (n = 10; n; n--)
	{
		delay_ms(10);
		ret = sd_send_byte(0xFF);	// 80 dummy clocks
	}
	//spi_cs_low();
	CS_LOW();
	//Talos::Motion::Main_Process::host_serial.print_string("disk init start\r\n");
	ty = 0;
	if (send_cmd(CMD0, 0) == 1)
	{
		//Talos::Motion::Main_Process::host_serial.print_string("init 1\r\n");
		// Put the card SPI mode
		Timer1 = 1000;
		// Initialization timeout of 1000 msec
		if (send_cmd(CMD8, 0x1AA) == 1)
		{
			//Talos::Motion::Main_Process::host_serial.print_string("init 2\r\n");
			// Is the card SDv2?
			for (n = 0; n < 4; n++) ocr[n] = sd_send_byte(0xFF);	// Get trailing return value of R7 resp
			if (ocr[2] == 0x01 && ocr[3] == 0xAA)
			{
				//Talos::Motion::Main_Process::host_serial.print_string("volt check\r\n");
				// The card can work at vdd range of 2.7-3.6V
				while (Timer1 && send_cmd(ACMD41, 1UL << 30));	// Wait for leaving idle state (ACMD41 with HCS bit)
				if (Timer1 && send_cmd(CMD58, 0) == 0)
				{		// Check CCS bit in the OCR
					for (n = 0; n < 4; n++) ocr[n] = sd_send_byte(0xFF);
					ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	// Check if the card is SDv2
				}
			}
		}
		else
		{
			//Talos::Motion::Main_Process::host_serial.print_string("else\r\n");
			// SDv1 or MMCv3
			if (send_cmd(ACMD41, 0) <= 1)
			{
				ty = CT_SD1; cmd = ACMD41;	// SDv1
			}
			else
			{
				ty = CT_MMC; cmd = CMD1;	// MMCv3
			}
			while (Timer1 && send_cmd(cmd, 0));			// Wait for leaving idle state
			if (!Timer1 || send_cmd(CMD16, 512) != 0)	// Set R/W block length to 512
			ty = 0;
		}
	}
	
	CardType = ty;
	deselect();

	if (ty)
	{//Initialization succeded
		Stat &= ~STA_NOINIT;		// Clear STA_NOINIT
		//-----FCLK_FAST();
		//Talos::Motion::Main_Process::host_serial.print_string("GOOD TO GO!\r\n");
	} else
	{			// Initialization failed
		power_off();
		//Talos::Motion::Main_Process::host_serial.print_string("EPIC FAIL!\r\n");
	}

	return Stat;
}
void power_off (void)
{
	///* Disable SPI function */
	//SPCR = 0;
	//
	//
	///* De-configure MOSI/MISO/SCLK/CS pins (set hi-z) */
	//// Set SCK/MOSI/CS as hi-z, INS#/WP as pull-up
	//// CHECK THIS
	//DDR_SPI &= ~(1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_CS);
	//PORT_SPI &= ~(1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_CS);
	//
	//
	///* Trun socket power off (nothing to do if no power controls) */
	////To be filled
}
void power_on (void)
{
	///* Trun socket power on and wait for 10ms+ (nothing to do if no power controls) */
	////To be filled
	//
	//
	///* Configure MOSI/MISO/SCLK/CS pins */
	////CHECK THIS
	//PORT_SPI = (1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_CS);
	//DDR_SPI = (1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_CS);
	//
	///* Enable SPI module in SPI mode 0 */
	//SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0);
}
BYTE send_cmd (BYTE cmd,DWORD arg)
{
	BYTE n, res;
	//Talos::Motion::Main_Process::host_serial.print_string("cmd 1\r\n");
	if (cmd & 0x80)
	{	// ACMD<n> is the command sequence of CMD55-CMD<n>
		//Talos::Motion::Main_Process::host_serial.print_string("cmd 2\r\n");
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
		//Talos::Motion::Main_Process::host_serial.print_string("cmd 2 ok\r\n");
	}
	// Select the card and wait for ready except to stop multiple block read
	if (cmd != CMD12)
	{
		//Talos::Motion::Main_Process::host_serial.print_string("cmd 3\r\n");
		deselect();
		if (!select()) return 0xFF;
		//Talos::Motion::Main_Process::host_serial.print_string("cmd 3ok \r\n");
	}

	//Talos::Motion::Main_Process::host_serial.print_string("cmd 4\r\n");
	// Send command packet
	sd_send_byte(0x40 | cmd);				// Start + Command index
	sd_send_byte((BYTE)(arg >> 24));		// Argument[31..24]
	sd_send_byte((BYTE)(arg >> 16));		// Argument[23..16]
	sd_send_byte((BYTE)(arg >> 8));		// Argument[15..8]
	sd_send_byte((BYTE)arg);				// Argument[7..0]
	n = 0x01;							// Dummy CRC + Stop
	if (cmd == CMD0) n = 0x95;			// Valid CRC for CMD0(0) + Stop
	if (cmd == CMD8) n = 0x87;			// Valid CRC for CMD8(0x1AA) Stop
	sd_send_byte(n);

	// Receive command response
	if (cmd == CMD12) sd_send_byte(0xFF);// Skip a stuff byte when stop reading
	n = 10;								// Wait for a valid response in timeout of 10 attempts
	do
	res = sd_send_byte(0xFF);
	while ((res & 0x80) && --n);
	//Talos::Motion::Main_Process::host_serial.print_string("send\r\n");
	if (res == 0 )
	{
		//Talos::Motion::Main_Process::host_serial.print_string("cmd 4 OK!\r\n");
	}
	else
	{
		//Talos::Motion::Main_Process::host_serial.print_string("cmd 4 FAIL!\r\n");
	}
	
	return res;			// Return with the response value
}
void deselect (void)
{
	CS_HIGH();		// Set CS# high
	sd_send_byte(0xFF);	// Dummy clock (force DO hi-z for multiple slave SPI)
}
int select (void)	/* 1:Successful, 0:Timeout */
{
	//Talos::Motion::Main_Process::host_serial.print_string("select 1\r\n");
	CS_LOW();		/* Set CS# low */
	sd_send_byte(0xFF);	/* Dummy clock (force DO enabled) */
	//Talos::Motion::Main_Process::host_serial.print_string("select 2\r\n");
	//
	if (wait_ready(10500)) return 1;	/* Leading busy check: Wait for card ready */
	//
	deselect();		/* Timeout */
	//Talos::Motion::Main_Process::host_serial.print_string("select 3\r\n");
	return 0;
}
DSTATUS mmc_disk_status (void)
{
	return Stat;
}
DRESULT mmc_disk_read (BYTE *buff,DWORD sector,UINT count)
{
	BYTE cmd;


	if (!count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;

	if (!(CardType & CT_BLOCK)) sector *= 512;	// Convert to byte address if needed

	cmd = count > 1 ? CMD18 : CMD17;			//  READ_MULTIPLE_BLOCK : READ_SINGLE_BLOCK
	if (send_cmd(cmd, sector) == 0)
	{
		do {
			if (!rcvr_datablock(buff, 512)) break;
			buff += 512;
		} while (--count);
		if (cmd == CMD18) send_cmd(CMD12, 0);	// STOP_TRANSMISSION
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}
DRESULT mmc_disk_write (const BYTE *buff,DWORD sector,UINT count)
{
	if (!count) return RES_PARERR;
	if (Stat & STA_NOINIT) return RES_NOTRDY;
	if (Stat & STA_PROTECT) return RES_WRPRT;

	if (!(CardType & CT_BLOCK)) sector *= 512;	// Convert to byte address if needed

	if (count == 1) {	// Single block write
		if ((send_cmd(CMD24, sector) == 0)	// WRITE_BLOCK
		&& xmit_datablock(buff, 0xFE))
		{
			count = 0;
		}
	}
	else {				// Multiple block write
		if (CardType & CT_SDC) send_cmd(ACMD23, count);
		if (send_cmd(CMD25, sector) == 0)
		{	// WRITE_MULTIPLE_BLOCK
			do {
				if (!xmit_datablock(buff, 0xFC)) break;
				buff += 512;
			} while (--count);
			if (!xmit_datablock(0, 0xFD)) count = 1;	// STOP_TRAN token
		}
	}
	deselect();

	return count ? RES_ERROR : RES_OK;
}
int rcvr_datablock (BYTE *buff,UINT btr)
{
	BYTE token;


	Timer1 = 20;
	do
	{							// Wait for data packet in timeout of 200ms
		token = sd_send_byte(0xFF);
	} while ((token == 0xFF) && Timer1);
	if (token != 0xFE) return 0;	// If not valid data token, retutn with error

	rcvr_spi_multi(buff, btr);		// Receive the data block into buffer
	sd_send_byte(0xFF);		// Discard CRC
	sd_send_byte(0xFF);

	return 1;						// Return with success
}
void rcvr_spi_multi (BYTE *p, UINT cnt)
{
	//why is this getting 2 bytes at a time in such a weird way?
	do {
		//set byte into output buffer
		SPI0->SPI_TDR = 0xFF;
		//wait for tx to complete
		while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
		//load read byte into buffer
		*p++ = SPI0->SPI_RDR;
		//set byte into output buffer
		SPI0->SPI_TDR = 0xFF;
		//wait for tx to complete
		while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
		//load read byte into buffer
		*p++ = SPI0->SPI_RDR;
	} while (cnt -= 2);

	
	//do {
	//SPDR = 0xFF;
	//while (!(SPSR & (1<<SPIF)));
	//*p++ = SPDR;
	//SPDR = 0xFF;
	//while (!(SPSR & (1<<SPIF)));
	//*p++ = SPDR;
	//} while (cnt -= 2);
}
int xmit_datablock (const BYTE *buff,BYTE token)
{
	BYTE resp;


	if (!wait_ready(500)) return 0;		// Leading busy check: Wait for card ready to accept data block

	sd_send_byte(token);			// Xmit data token
	if (token == 0xFD) return 1;		//Do not send data if token is StopTran

	xmit_spi_multi(buff, 512);			//Data
	sd_send_byte(0xFF); sd_send_byte(0xFF);		// Dummy CRC

	resp = sd_send_byte(0xFF);				// Receive data resp

	return (resp & 0x1F) == 0x05 ? 1 : 0;	// Data was accepted or not

	/* Busy check is done at next transmission */
}
void xmit_spi_multi (const BYTE *p,UINT cnt)
{
	//why is this getting 2 bytes at a time in such a weird way?
	do {
		//set byte into output buffer
		SPI0->SPI_TDR = *p++;
		//wait for tx to complete
		while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
		//set byte into output buffer
		SPI0->SPI_TDR = *p++;
		//wait for tx to complete
		while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
	} while (cnt -= 2);
	//do {
	//SPDR = *p++;
	//while (!(SPSR & (1<<SPIF)));
	//SPDR = *p++;
	//while (!(SPSR & (1<<SPIF)));
	//} while (cnt -= 2);
}
int wait_ready (UINT wt)
{
	BYTE d;


	Timer2 = wt / 10;
	do
	d = sd_send_byte(0xFF);

	// This loop takes a time. Insert rot_rdq() here for multitask envilonment.

	while (d != 0xFF && Timer2);

	return (d == 0xFF) ? 1 : 0;
}
BYTE sd_send_byte(BYTE data)
{
	//Wait for previous transfer to complete
	while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
	
	//load the Transmit Data Register with the value to transmit
	SPI0->SPI_TDR = data;
	
	//Wait for data to be transferred to serializer
	while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
	
	while ( (SPI0->SPI_SR & SPI_SR_RDRF) == 0 ) ;

	return SPI0->SPI_RDR & 0x00FF ;
	
	
	
	//BYTE rx;
	//while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
	////place tx byte in xmit buffer
	//SPI0->SPI_TDR = data;
	//while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0);
	//rx = SPI0->SPI_RDR;
	//return rx;
}
