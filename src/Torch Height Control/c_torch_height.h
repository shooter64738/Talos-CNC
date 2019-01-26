/*
* c_torch_height.h
*
* Created: 1/26/2019 11:28:06 AM
* Author: jeff_d
*/


#ifndef __C_TORCH_HEIGHT_H__
#define __C_TORCH_HEIGHT_H__
#include <stdint.h>
#define DIGITAL_INPUT_PORT 0
#define LOWER_LIMIT_SWITCH_INPUT_PIN 0
#define MATERIAL_SWITCH_INPUT_PIN 1
#define UPPER_LIMIT_SWITCH_INPUT_PIN 2

namespace Plasma_Control
{
	namespace Errors
	{
		static const uint8_t OK = 0;
		static const uint8_t MATERIAL_OUT_OF_RANGE = 1;
	};
	enum class e_torch_positions
	{
		PIERCE,
		CUT
	};
	class c_torch_height
	{
	public:
		
		//variables
		public:
		protected:
		private:

		//functions
		public:
			static uint8_t move_head(Plasma_Control::e_torch_positions Target);

		protected:
		private:
		c_torch_height( const c_torch_height &c );
		c_torch_height& operator=( const c_torch_height &c );
		c_torch_height();
		~c_torch_height();

		

	}; //c_torch_height
};

#endif //__C_TORCH_HEIGHT_H__
