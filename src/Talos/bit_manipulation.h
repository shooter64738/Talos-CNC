/*
 * bit_manipulation.h
 *
 * Created: 7/12/2019 7:31:38 PM
 *  Author: Family
 */ 


#ifndef BIT_MANIPULATION_H_
#define BIT_MANIPULATION_H_


#define BitSet(arg,posn) ((arg) | (1UL << (posn)))
#define BitSet_(arg,posn) ((arg) |= (1UL << (posn)))
#define BitClr(arg,posn) ((arg) & ~(1UL << (posn)))
#define BitClr_(arg,posn) ((arg) &= ~(1UL << (posn)))
#define BitTst(arg,posn) bool((arg) & (1UL << (posn)))
#define BitLong(n) (1UL << (n))
#define BitGet(p,m) bool((p) & (1UL << (m)))

#define BitFlp(arg,posn) ((arg) ^ (1UL << (posn))
#define Bit(x) (0x01 << (x))
#define LongBit(x) ((unsigned long)0x00000001 << (x))

//Copied from grbl. These macros should be changed out eventually.
#define bit(n) (1 << (n))
#define bit_true(x,mask) ((x) |= (mask))
#define bit_false(x,mask) ((x) &= ~(mask))
#define bit_istrue(x,mask) (((x) & (mask)) != 0)
#define bit_isfalse(x,mask) (((x) & (mask)) == 0)

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))


#endif /* BIT_MANIPULATION_H_ */