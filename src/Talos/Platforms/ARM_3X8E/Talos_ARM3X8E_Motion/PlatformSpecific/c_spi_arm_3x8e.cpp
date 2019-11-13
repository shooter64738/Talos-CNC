/*
* c_serial_avr_2560.cpp
*
* Created: 2/27/2019 3:54:31 PM
* Author: jeff_d
*/


#include "c_spi_arm_3x8e.h"
//#include "sam.h"
#define spi_get_pcs(chip_sel_id) ((~(1u<<(chip_sel_id)))&0xF)
static uint8_t spi0_data[SPI_BUFFER_SIZE];
c_ring_buffer<uint8_t> Hardware_Abstraction_Layer::SPI::spi_buffer;

#define CONFIG_SPI_MASTER_DELAY_BS  0
#define CONFIG_SPI_MASTER_DELAY_BCT 0
#define CONFIG_SPI_MASTER_BITS_PER_TRANSFER  SPI_CSR_BITS_8_BIT
#define div_ceil(a, b)      (((a) + (b) - 1) / (b))
#define SPI_CLOCK_SPEED 4000000UL

uint16_t Hardware_Abstraction_Layer::SPI::initialize()
{
	spi_buffer.initialize(spi0_data,SPI_BUFFER_SIZE);
	//initialize_general();
	initialize_disk();
	return 0;
	//initialize2();
	//return;
	
	
}

void Hardware_Abstraction_Layer::SPI::initialize_general()
{
	PMC->PMC_PCER0 |= (1<<ID_PIOA);    // SPI0 power ON
	PMC->PMC_PCER0 |= PMC_PCER0_PID24;    // SPI0 power ON
	
	// Program the PIO controllers to assign
	// the SPI output pins to their peripheral functions (page 679)
	PMC->PMC_PCER0 |= (1<<ID_PIOA);	//Enable Clock PortA
	
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

	// SPI Disable
	SPI0->SPI_CR = SPI_CR_SPIDIS;// SPI is in slave mode after software reset !!
	// Perform a SPI software reset twice, like SAM does.
	SPI0->SPI_CR = SPI_CR_SWRST;
	SPI0->SPI_CR = SPI_CR_SWRST;
	
	
	//SPI0->SPI_MR =0;
	//// SPI0 operates in Master mode, local loopback
	//SPI0->SPI_MR = SPI_MR_MSTR
	//| SPI_MR_MODFDIS             // Disable Mode Fault detection
	//| SPI_MR_WDRBT               // Wait data read before Transfer
	////| SPI_MR_LLB                 // Local loopback (MISO -->MOSI)
	////| SPI_MR_PCS(0b1110)
	//| SPI_MR_DLYBCS(0b01111111); // Maximum delay between Chip Selects
	////| SPI_MR_PS
	//SPI0->SPI_MR &=~(SPI_MR_PS);//Fixed peripheral select
	//SPI0->SPI_MR &=~(SPI_MR_PCSDEC);//Chip connected directly.
	//SPI0->SPI_MR |=SPI_MR_PCS(0b0000);//CS 0
//
//
	//// Data transfer parameters
	//SPI0->SPI_CSR[0] |= SPI_CSR_CPOL          // Inactive state value of SPCK is logic level one
	//| SPI_CSR_NCPHA       // Data is captured on the leading edge of SPCK and changed on the following edge
	//| SPI_CSR_CSNAAT      // Chip select active after transfer
	//| SPI_CSR_BITS_8_BIT // Bits per transfer
	//| SPI_CSR_SCBR(100)   // slowest bit rate
	//| SPI_CSR_DLYBS(100); // Maximum delay from NPCS falling edge (activation)
	//// to the first valid SPCK transition
	
	// Data transfer parameters
	// Save the divisor
	uint32_t scbr = SPI0->SPI_CSR[0] & 0XFF00;
	// Disable SPI
	SPI0->SPI_CR = SPI_CR_SPIDIS;
	// reset SPI
	SPI0->SPI_CR = SPI_CR_SWRST;
	// no mode fault detection, set master mode
	SPI0->SPI_MR = SPI_MR_PCS(0) | SPI_MR_MODFDIS | SPI_MR_MSTR;
	// mode 0, 8-bit,
	SPI0->SPI_CSR[0] = 200 | SPI_CSR_CSAAT | SPI_CSR_NCPHA;
	

	// Receive Data Register Full Interrupt and
	// Transmit Data Register Empty Interrupt Enable
	//SPI0->SPI_IER = SPI_IER_RDRF;// | SPI_IER_TDRE;
	//NVIC_EnableIRQ(SPI0_IRQn);

	SPI0->SPI_CR = SPI_CR_SPIEN;            // Enable SPI0
}

/** SPI Chip Select behavior modes while transferring. */
//typedef enum spi_cs_behavior {
///** CS does not rise until a new transfer is requested on different chip select. */
//SPI_CS_KEEP_LOW = SPI_CSR_CSAAT,
///** CS rises if there is no more data to transfer. */
//SPI_CS_RISE_NO_TX = 0,
///** CS is de-asserted systematically during a time DLYBCS. */
//SPI_CS_RISE_FORCED = SPI_CSR_CSNAAT
//} spi_cs_behavior_t;

void Hardware_Abstraction_Layer::SPI::initialize_disk()
{
	PMC->PMC_PCER0 |= (1<<ID_PIOA);	//Enable Clock PortA
	//enable peripheral clock
	PMC->PMC_PCER0 |= (1<<ID_SPI0); //PMC_PCER0_PID24

	int16_t baud_div = div_ceil(84000000, SPI_CLOCK_SPEED);
	//#spi_set_transfer_delay(p_spi, device->id, CONFIG_SPI_MASTER_DELAY_BS, CONFIG_SPI_MASTER_DELAY_BCT);
	//set xfer delay
	SPI0->SPI_CSR[0] &= ~(SPI_CSR_DLYBS_Msk | SPI_CSR_DLYBCT_Msk);
	SPI0->SPI_CSR[0] |= SPI_CSR_DLYBS(CONFIG_SPI_MASTER_DELAY_BS) | SPI_CSR_DLYBCT(CONFIG_SPI_MASTER_DELAY_BCT);

	//#spi_set_bits_per_transfer(p_spi, device->id,CONFIG_SPI_MASTER_BITS_PER_TRANSFER);
	//set bits per transfer
	SPI0->SPI_CSR[0] &= (~SPI_CSR_BITS_Msk);
	SPI0->SPI_CSR[0] |= CONFIG_SPI_MASTER_BITS_PER_TRANSFER;

	//#spi_set_baudrate_div(p_spi, device->id, baud_div);
	//set baud rate divisor
	SPI0->SPI_CSR[0] &= (~SPI_CSR_SCBR_Msk);
	SPI0->SPI_CSR[0] |= SPI_CSR_SCBR(baud_div);

	//#spi_configure_cs_behavior(p_spi, device->id, SPI_CS_KEEP_LOW);
	//set cs behavior (keep low)
	SPI0->SPI_CSR[0] |= SPI_CSR_CSAAT;
	
	//#spi_set_clock_polarity(p_spi, device->id, flags >> 1);
	//set clock polarity
	SPI0->SPI_CSR[0] |= SPI_CSR_CPOL;
	
	//#spi_set_clock_phase(p_spi, device->id, ((flags & 0x1) ^ 0x1));
	//set clock phase
	SPI0->SPI_CSR[0] |= SPI_CSR_NCPHA;
	//SPI0->SPI_CSR[0] &= (~SPI_CSR_NCPHA);
	
	//enable spi
	SPI0->SPI_CR = SPI_CR_SPIEN;
	
	//select device id
	//spi_set_peripheral_chip_select_value(SPI0, (~(1 << device->id)));
	uint8_t device_id = 0;
	SPI0->SPI_MR &= (~SPI_MR_PCS_Msk);
	SPI0->SPI_MR |= SPI_MR_PCS((~(1 << device_id)));
}


////Basic config
//SPI0->SPI_CR = SPI_CR_SPIDIS ;
//SPI0->SPI_CR = SPI_CR_SWRST;
//SPI0->SPI_CR = SPI_CR_SWRST;
//SPI0->SPI_MR = (SPI_MR_MSTR | SPI_MR_MODFDIS | SPI_MR_PS | SPI_MR_WDRBT);
//SPI0->SPI_CR = SPI_CR_SPIEN ;
//
////configure MISO
//PIOA->PIO_ODR |= PIO_PA25A_SPI0_MISO; //input
//PIOA->PIO_ABSR &= ~PIO_PA25A_SPI0_MISO; //select peripheral A
//PIOA->PIO_PUER |= PIO_PA25A_SPI0_MISO; //enable pullup
//
////configure MOSI
//PIOA->PIO_OER |= PIO_PA26A_SPI0_MOSI; //output
//PIOA->PIO_ABSR &= ~PIO_PA26A_SPI0_MOSI; //select peripheral A
//PIOA->PIO_PUER |= PIO_PA26A_SPI0_MOSI; //enable pullup
//
////configure SCK
//PIOA->PIO_OER |= PIO_PA27A_SPI0_SPCK; //output
//PIOA->PIO_ABSR &= ~PIO_PA27A_SPI0_SPCK; //select peripheral A
//PIOA->PIO_PUER |= PIO_PA27A_SPI0_SPCK; //enable pullup

//make slave select low
//PIOA->PIO_CODR |= PIO_PA28A_SPI0_NPCS0;
//make slave slect high
//PIOA->PIO_SODR |= PIO_PA28A_SPI0_NPCS0;

//Initialize all chip select configurations to default
//for(uint32_t x=0; x<MAX_SW_CHANNELS; x++){
//settings[x].CPOL = 0u<<0;					//Clock inactive state = LOW
//settings[x].NCPHA = 0u<<1;					//Data changed Low->High, captured High->Low
//settings[x].BITS = 0xFu<<4;					//8-Bit Mode
//settings[x].FREQ = SPI_CLOCK_FREQ(1000000); //4MHz Clock
//}
//}

void Hardware_Abstraction_Layer::SPI::initialize2()
{
	PMC->PMC_PCER0 |= (1<<ID_PIOA);	//Enable Clock PortA
	PMC->PMC_PCER0 |= (1<<ID_SPI0); //PMC_PCER0_PID24
	//REG_PMC_PCER0 |= PMC_PCER0_PID24;      // Power up SPI clock
	REG_SPI0_WPMR = (0<<SPI_WPMR_WPEN);   // Unlock user interface for SPI
	
	//Instance SPI0, MISO: PA25, (MISO), MOSI: PA26, (MOSI), SCLK: PA27, (SCLK), NSS: PA28, (NPCS0)
	PIOA->PIO_PDR |= PIO_PDR_P25;//DSR control for bit. Removes PIO control and hands control to a peripheral
	PIOA->PIO_ABSR &= ~PIO_ABSR_P25;//Set control bit for peripheral A (SPI MISO)
	
	//REG_PIOA_ABSR |= PIO_ABSR_P25;   // Transfer Pin control from PIO to SPI
	//REG_PIOA_PDR |= PIO_PDR_P25;       // Set MISO pin to an output

	PIOA->PIO_PDR |= PIO_PDR_P26;//DSR control for bit. Removes PIO control and hands control to a peripheral
	PIOA->PIO_ABSR &= ~PIO_ABSR_P26;//Set control bit for peripheral A (SPI MOSI)

	//REG_PIOA_ABSR |= PIO_ABSR_P26;     // Transfer Pin control from PIO to SPI
	//REG_PIOA_PDR |= 0<<PIO_PDR_P26;   // Set MOSI pin to an input

	PIOA->PIO_PDR |= PIO_PDR_P27;//DSR control for bit. Removes PIO control and hands control to a peripheral
	PIOA->PIO_ABSR &= ~PIO_ABSR_P27;//Set control bit for peripheral A (SPI SCLK)

	//REG_PIOA_ABSR |= PIO_ABSR_P27;     // Transfer Pin control from PIO to SPI
	//REG_PIOA_PDR |= 0<<PIO_PDR_P27;   // Set SCLK pin to an input

	PIOA->PIO_PDR |= PIO_PDR_P28;//DSR control for bit. Removes PIO control and hands control to a peripheral
	PIOA->PIO_ABSR &= ~PIO_ABSR_P28;//Set control bit for peripheral A (SPI NSS0)
	
	//REG_PIOA_ABSR |= PIO_ABSR_P28;     // Transfer Pin control from PIO to SPI
	//REG_PIOA_PDR |= 0<<PIO_PDR_P28;   // Set NSS pin to an input

	SPI0->SPI_MR|= SPI_MR_MSTR;//Set master/slave register to master
	//REG_SPI0_MR = 1;            // Slave mode
	
	//SPI0->SPI_CR |= (SPI_CR_SPIEN);//Set control register to enabled
	//REG_SPI0_CR = 1;             // Enable SPI
	
	//SPI0->SPI_IER =
	//SPI_IER_RDRF| //receive data register full interrupt
	//SPI_IER_OVRES; //overrun error interrupt
	
	//Enable interrupts
	//REG_SPI0_IER = 1<<SPI_IER_RDRF|1<<SPI_IER_OVRES|1<<SPI_IER_NSSR;

	// Shift on falling edge and transfer 8 bits.
	SPI0->SPI_CSR[0] = SPI_CSR_NCPHA|SPI_CSR_BITS_8_BIT;
	
	//SPI0->SPI_IER = SPI_IER_RDRF;
	//NVIC_EnableIRQ(SPI0_IRQn);
	//SPI0->SPI_CR = SPI_CR_SPIEN;
}

uint8_t Hardware_Abstraction_Layer::SPI::send_byte(uint8_t * data)
{
	uint8_t newbyte = 0;
	//place tx byte in xmit buffer
	SPI0->SPI_TDR = *data;
	return 0;
	
	////wait for tx to complete
	//while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
	//
	////wait for rx to complete
	//while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0);
	////place rx byte in the variable
	////* data = SPI0->SPI_RDR;
	//newbyte = SPI0->SPI_RDR;
	//return newbyte+1;
	
}

int Hardware_Abstraction_Layer::SPI::sd_send_byte(int data)
{
	uint8_t newbyte = 0;
	//place tx byte in xmit buffer
	SPI0->SPI_TDR = data;
	return 0;
	
	////wait for tx to complete
	//while ((SPI0->SPI_SR & SPI_SR_TDRE) == 0);
	//
	////wait for rx to complete
	//while ((SPI0->SPI_SR & SPI_SR_RDRF) == 0);
	////place rx byte in the variable
	////* data = SPI0->SPI_RDR;
	//newbyte = SPI0->SPI_RDR;
	//return newbyte+1;
	
}

void SPI0_Handler()
{
	//  uint16_t DataReceived;
	uint32_t status = SPI0->SPI_SR;

	//if ((status & SPI_SR_TDRE) == SPI_SR_TDRE)
	//{
	//SPI0->SPI_TDR |= SPI_TDR_TD(SPI0->SPI_TDR);
	//}

	if ((status & SPI_SR_RDRF))
	{
		
		uint8_t data = SPI0->SPI_RDR & SPI_RDR_RD_Msk;
		Hardware_Abstraction_Layer::SPI::spi_buffer.put(data);
	}
	
}


