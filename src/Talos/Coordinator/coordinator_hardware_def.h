/*
 * hardware_def.h
 *
 * Created: 7/12/2019 6:30:30 PM
 *  Author: Family
 */

//To enable compilation and debugging in Microsoft Visual C++ define MSCV
#if !defined(__AVR_ATmega328P__) && !defined(__AVR_ATmega2560__) && !defined(__SAM3X8E__)
#define MSVC
#endif

#ifndef COORDINATOR_HARDWARE_DEF_H_
#define COORDINATOR_HARDWARE_DEF_H_

#ifdef __AVR_ATmega2560__
#define F_CPU 16000000UL
#include "../Platforms/AVR_2560/Talos_AVR2560_Coordinator/PlatformSpecific/c_serial_avr_2560.h"
#include "../Platforms/AVR_2560/Talos_AVR2560_Coordinator/PlatformSpecific/c_core_avr_2560.h"
#include "../Platforms/AVR_2560/Talos_AVR2560_Coordinator/PlatformSpecific/c_spi_avr_2560.h"
#include "../Platforms/AVR_2560/Talos_AVR2560_Coordinator/PlatformSpecific/c_disk_avr_2560.h"
//#include "../Platforms/AVR_2560/Talos_AVR2560_Coordinator/PlatformSpecific/c_Serial.h"
#endif

#ifdef MSVC
#define F_CPU 16000000
#include "../Platforms/WIN32/Talos_WIN32/Coordinator/Platform Specific/c_serial_win.h"
#include "../Platforms/WIN32/Talos_WIN32/Coordinator/Platform Specific/c_core_win.h"
#include "../Platforms/WIN32/Talos_WIN32/Coordinator/Platform Specific/c_spi_win.h"
//#include "../Platforms/WIN32/Talos_WIN32/Coordinator/Platform Specific/c_Serial.h"
#endif


#endif /* COORDINATOR_HARDWARE_DEF_H_ */