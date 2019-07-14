/*
*  Stager_Errors.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef PLANNER_ERRORS_H_
#define PLANNER_ERRORS_H_

class Stager_Errors
{
	public:
	static const uint8_t OK=0;
	static const uint8_t Profile_Buffer_Full=1;
	static const uint8_t No_Motion_For_NGC_Block=2;
	static const uint8_t Segment_Buffer_Full=3;
	static const uint8_t Segment_Canceled=4;
	static const uint8_t Profile_Buffer_Empty=5;
	static const uint8_t NGC_Buffer_Empty=6;
	static const uint8_t NGC_Buffer_Full=7;
	static const uint8_t Hold_Block_For_Cutter_Compensation=8;
	static const uint8_t Staging_Buffer_Full = 9;
};


#endif /* PLANNER_ERRORS_H_ */