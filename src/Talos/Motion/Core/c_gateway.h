/*
* c_gateway.h
*
* Created: 5/8/2019 2:18:49 PM
* Author: Family
*/


#ifndef __C_PROCESSOR_H__
#define __C_PROCESSOR_H__

#include "../Serial/c_Serial.h"
#include "../../talos_hardware_def.h"
#include "../../Shared_Data/_s_motion_data_block.h"


namespace Motion_Core
{
	class Gateway
	{
		//variables
		public:
//		static c_Serial coordinator_serial;
		static c_Serial *local_serial;
		static bool add_motion(s_motion_data_block new_blk);
		static void process_loop();
		
		static void check_control_states();
		static void process_motion();
		static void process_motion(s_motion_data_block *mot);
		static void check_hardware_faults();
		static void check_sequence_complete();
		
		static uint8_t remote;
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
};
#endif //__C_PROCESSOR_H__
