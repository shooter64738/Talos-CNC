/* 
* c_protocol.h
*
* Created: 3/5/2019 2:57:02 PM
* Author: jeff_d
*/


#ifndef __C_PROTOCOL_H__
#define __C_PROTOCOL_H__
	// Line buffer size from the serial input stream to be executed.
	#ifndef LINE_BUFFER_SIZE
	#define LINE_BUFFER_SIZE 256
	#endif


class c_protocol
{
//variables
public:
protected:
private:

//functions
public:

	// Starts Grbl main loop. It handles all incoming characters from the serial port and executes
	// them as they complete. It is also responsible for finishing the initialization procedures.
	static void protocol_main_loop();

	// Checks and executes a realtime command at various stop points in main program
	static void protocol_execute_realtime();
	static void protocol_exec_rt_system();

	// Executes the auto cycle feature, if enabled.
	static void protocol_auto_cycle_start();

	// Block until all buffered steps are executed
	static void protocol_buffer_synchronize();

	static void protocol_exec_rt_suspend();

protected:
private:

}; //c_protocol

#endif //__C_PROTOCOL_H__
