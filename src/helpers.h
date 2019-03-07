/*
*  helpers.h - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2018 Jeff Dill
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


#ifndef HELPERS_H_
#define HELPERS_H_

#define BitSet(arg,posn) ((arg) | (1UL << (posn)))
#define BitClr(arg,posn) ((arg) & ~(1UL << (posn)))
#define BitTst(arg,posn) bool((arg) & (1UL << (posn)))

#define BitGet(p,m) bool((p) & (1UL << (m)))

#define BitFlp(arg,posn) ((arg) ^ (1UL << (posn))
#define Bit(x) (0x01 << (x))
#define LongBit(x) ((unsigned long)0x00000001 << (x))

//Copied from grbl. These macros shoudl be chagned out eventualy. 
#define bit(n) (1 << (n))
#define bit_true(x,mask) ((x) |= (mask))
#define bit_false(x,mask) ((x) &= ~(mask))
#define bit_istrue(x,mask) (((x) & (mask)) != 0)
#define bit_isfalse(x,mask) (((x) & (mask)) == 0)


#endif /* HELPERS_H_ */