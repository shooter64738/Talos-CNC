/*
* c_processor.h
*
* Created: 5/8/2019 2:18:49 PM
* Author: Family
*/


#ifndef __C_PROCESSOR_H__
#define __C_PROCESSOR_H__

#include "hardware_def.h"
#include "c_segment_arbitrator.h"
#include "c_interpollation_software.h"
#include "c_motion_core.h"
#include "c_interpollation_hardware.h"


class c_processor
{
	//variables
	public:
	static c_Serial host_serial;
	static void initialize();
	
	protected:
	private:

	//functions
	public:
	//c_processor();
	//~c_processor();
	protected:
	private:
	//c_processor( const c_processor &c );
	//c_processor& operator=( const c_processor &c );

}; //c_processor

#endif //__C_PROCESSOR_H__
