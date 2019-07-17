/*
* c_pcf8574.h
*
* Created: 11/25/2018 5:56:29 PM
* Author: jeff_d
*/


#ifndef __C_PCF8574_H__
#define __C_PCF8574_H__
#include <stdint.h>
//pin status
#define PCF8574_MAXDEVICES 1 //max devices, depends on address (3 bit)


#define PCF8574_ADDRBASE (0x3F) //device base address
#define PCF8574_I2CINIT 1 //init i2c
#define PCF8574_MAXPINS 8 //max pin per device

//settings
//#define PCF8574_I2CFLEURYPATH "../i2chw/i2cmaster.h" //define the path to i2c fleury lib
#include <stdint.h>
class c_pcf8574
{
	//variables
	public:
	protected:
	private:

	//functions
	public:
	//functions
	static void pcf8574_init();
	static int8_t pcf8574_getoutput(uint8_t deviceid);
	static int8_t pcf8574_getoutputpin(uint8_t deviceid, uint8_t pin);
	static int8_t pcf8574_setoutput(uint8_t deviceid, uint8_t data);
	static int8_t pcf8574_setoutputpins(uint8_t deviceid, uint8_t pinstart, uint8_t pinlength, int8_t data);
	static int8_t pcf8574_setoutputpin(uint8_t deviceid, uint8_t pin, uint8_t data);
	static int8_t pcf8574_setoutputpinhigh(uint8_t deviceid, uint8_t pin);
	static int8_t pcf8574_setoutputpinlow(uint8_t deviceid, uint8_t pin);
	static int8_t pcf8574_getinput(uint8_t deviceid);
	static int8_t pcf8574_getinputpin(uint8_t deviceid, uint8_t pin);

	protected:
	private:
	c_pcf8574( const c_pcf8574 &c );
	c_pcf8574& operator=( const c_pcf8574 &c );
	c_pcf8574();
	~c_pcf8574();

}; //c_pcf8574

#endif //__C_PCF8574_H__
