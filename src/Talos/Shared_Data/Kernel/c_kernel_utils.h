#ifndef __C_KERNEL_UTILS
#define __C_KERNEL_UTILS

#if !defined(__AVR_ATmega328P__) && !defined(__AVR_ATmega2560__) && !defined(__SAM3X8E__) && !defined(__STM32H745ZIQ__)
#define __PACK__( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#else
#define __PACK__( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#endif

