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

//#define bit_get(p,m) ((p) & (m))
//#define bit_set(p,m) ((p) |= (m))
//#define bit_clear(p,m) ((p) &= ~(m))
//#define bit_flip(p,m) ((p) ^= (m))
//#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
//#define BIT(x) (1 << (x))
//#define LONGBIT(x) ((unsigned long)0x00000001 << (x))


#endif /* HELPERS_H_ */