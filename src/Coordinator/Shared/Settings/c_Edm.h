/*
* c_edm.h
*
* Created: 1/14/2019 1:11:15 PM
* Author: jeff_d
*/


#ifndef __C_EDM_H__
#define __C_EDM_H__

namespace Settings
{
	class c_Edm
	{
		//variables
		public:
		public:
		float Gap_Voltage;
		float Work_Voltage;
		protected:
		private:

		//functions
		public:
		c_Edm();
		~c_Edm();
		void initialize();
		protected:
		private:
		//c_edm( const c_edm &c );
		//c_edm& operator=( const c_edm &c );
		

	}; //c_edm
};
#endif //__C_EDM_H__
