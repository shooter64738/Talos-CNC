#include "..\Talos.h"

/*
* c_encoder.h
*
* Created: 2/12/2019 1:20:44 PM
* Author: jeff_d
*/


#ifndef __C_ENCODER_H__
#define __C_ENCODER_H__

namespace Spindle_Controller
{
	class c_encoder
	{
		//variables
		public:
		class hal_callbacks
		{
			public:
			static void timer_capture(uint16_t time_at_vector,uint8_t port_values);
			static void position_change(uint16_t time_at_vector,uint8_t port_values);
		};
		
		static float encoder_rpm_multiplier;
		static float encoder_angle_multiplier;
		static uint16_t encoder_ticks_per_rev;
		static void initialize(uint16_t encoder_ticks_per_rev);
		protected:
		private:

		//functions
		public:
		static void update_time(uint16_t time_at_vector);
		static float current_rpm();
		static float current_angle_deg();

		protected:
		private:
		//c_encoder();
		//~c_encoder();
		//c_encoder( const c_encoder &c );
		//c_encoder& operator=( const c_encoder &c );

	}; //c_encoder
};
#endif //__C_ENCODER_H__
