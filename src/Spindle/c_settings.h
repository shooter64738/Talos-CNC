/*
* c_settings.h
*
* Created: 2/28/2019 8:51:39 AM
* Author: jeff_d
*/


#ifndef __C_SETTINGS_H__
#define __C_SETTINGS_H__
#include <stdint.h>
#include "c_encoder.h"
namespace Spindle_Controller
{
	class c_settings
	{
	public:
		union u_serializer
		{
			struct s_values
			{
				float pP;
				float pI;
				float pD;
				uint32_t acceleration_rate;
				uint16_t encoder_ticks_per_rev;
				Spindle_Controller::c_encoder::e_rpm_type rpm_derivation;
			}values;
			char stream[(sizeof(s_values))];
		};
		static u_serializer serializer;

		static void load_defaults();
		static void load_settings();
		static void save_settings();

	protected:
	private:
		//functions
	public:
	protected:
	private:

	}; //c_settings
};
#endif //__C_SETTINGS_H__
