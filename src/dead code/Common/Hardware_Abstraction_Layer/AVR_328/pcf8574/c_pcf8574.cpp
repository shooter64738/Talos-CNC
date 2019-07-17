/*
* c_pcf8574.cpp
*
* Created: 11/25/2018 5:56:29 PM
* Author: jeff_d
*/



//#include "..\I2C\c_i2cMaster.h"

//// default constructor
//c_pcf8574::c_pcf8574()
//{
//} //c_pcf8574
//
//// default destructor
//c_pcf8574::~c_pcf8574()
//{
//} //~c_pcf8574


#include <avr/delay.h>
#include "c_pcf8574.h"
#include "../I2C/c_i2cMaster.h"
/*
* initialize
*/
#define F_CPU 16000000
volatile uint8_t pcf8574_pinstatus[PCF8574_MAXDEVICES];
void c_pcf8574::pcf8574_init() {
	#if PCF8574_I2CINIT == 1
	
	//init i2c
	c_i2cMaster::i2c_init();
	_delay_us(10);
	#endif

	//reset the pin status
	uint8_t i = 0;
	for(i=0; i<PCF8574_MAXDEVICES; i++)
	pcf8574_pinstatus[i] = 0;

}

/*
* get output status
*/
int8_t c_pcf8574::pcf8574_getoutput(uint8_t deviceid) {
	int8_t data = -1;
	if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES)) {
		data = pcf8574_pinstatus[deviceid];
	}
	return data;
}

/*
* get output pin status
*/
int8_t c_pcf8574::pcf8574_getoutputpin(uint8_t deviceid, uint8_t pin) {
	int8_t data = -1;
	if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES) && (pin >= 0 && pin < PCF8574_MAXPINS)) {
		data = pcf8574_pinstatus[deviceid];
		data = (data >> pin) & 0b00000001;
	}
	return data;
}

/*
* set output pins
*/
int8_t c_pcf8574::pcf8574_setoutput(uint8_t deviceid, uint8_t data) {
	if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES)) {
		pcf8574_pinstatus[deviceid] = data;
		c_i2cMaster::i2c_start(((PCF8574_ADDRBASE+deviceid)<<1) | I2C_WRITE);
		c_i2cMaster::i2c_write(data);
		c_i2cMaster::i2c_stop();
		return 0;
	}
	return -1;
}

/*
* set output pins, replace actual status of a device from pinstart for pinlength with data
*/
int8_t c_pcf8574::pcf8574_setoutputpins(uint8_t deviceid, uint8_t pinstart, uint8_t pinlength, int8_t data) {
	//example:
	//actual data is         0b01101110
	//want to change              ---
	//pinstart                    4
	//data                        101   (pinlength 3)
	//result                 0b01110110
	if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES) && (pinstart - pinlength + 1 >= 0 && pinstart - pinlength + 1 >= 0 && pinstart < PCF8574_MAXPINS && pinstart > 0 && pinlength > 0)) {
		uint8_t b = 0;
		b = pcf8574_pinstatus[deviceid];
		uint8_t mask = ((1 << pinlength) - 1) << (pinstart - pinlength + 1);
		data <<= (pinstart - pinlength + 1);
		data &= mask;
		b &= ~(mask);
		b |= data;
		pcf8574_pinstatus[deviceid] = b;
		//update device
		c_i2cMaster::i2c_start(((PCF8574_ADDRBASE+deviceid)<<1) | I2C_WRITE);
		c_i2cMaster::i2c_write(b);
		c_i2cMaster::i2c_stop();
		return 0;
	}
	return -1;
}

/*
* set output pin
*/
int8_t c_pcf8574::pcf8574_setoutputpin(uint8_t deviceid, uint8_t pin, uint8_t data) {
	if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES) && (pin >= 0 && pin < PCF8574_MAXPINS)) {
		uint8_t b = 0;
		b = pcf8574_pinstatus[deviceid];
		b = (data != 0) ? (b | (1 << pin)) : (b & ~(1 << pin));
		pcf8574_pinstatus[deviceid] = b;
		//update device
		c_i2cMaster::i2c_start(((PCF8574_ADDRBASE+deviceid)<<1) | I2C_WRITE);
		c_i2cMaster::i2c_write(b);
		c_i2cMaster::i2c_stop();
		return 0;
	}
	return -1;
}

/*
* set output pin high
*/
int8_t c_pcf8574::pcf8574_setoutputpinhigh(uint8_t deviceid, uint8_t pin) {
	return c_pcf8574::pcf8574_setoutputpin(deviceid, pin, 1);
}

/*
* set output pin low
*/
int8_t c_pcf8574::pcf8574_setoutputpinlow(uint8_t deviceid, uint8_t pin) {
	return c_pcf8574::pcf8574_setoutputpin(deviceid, pin, 0);
}


/*
* get input data
*/
int8_t c_pcf8574::pcf8574_getinput(uint8_t deviceid) {
	int8_t data = -1;
	if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES)) {
		c_i2cMaster::i2c_start(((PCF8574_ADDRBASE+deviceid)<<1) | I2C_READ);
		data = ~c_i2cMaster::i2c_readNak();
		c_i2cMaster::i2c_stop();
	}
	return data;
}

/*
* get input pin (up or low)
*/
int8_t c_pcf8574::pcf8574_getinputpin(uint8_t deviceid, uint8_t pin) {
	int8_t data = -1;
	if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES) && (pin >= 0 && pin < PCF8574_MAXPINS)) {
		data = c_pcf8574::pcf8574_getinput(deviceid);
		if(data != -1) {
			data = (data >> pin) & 0b00000001;
		}
	}
	return data;
}
