/*
 * hardware_def.h
 *
 * Created: 7/12/2019 6:30:30 PM
 *  Author: Family
 */

//To enable compilation and debugging in Microsoft Visual C++ define MSCV
#ifdef TALOS_MOTION

#if !defined(__AVR_ATmega328P__) && !defined(__AVR_ATmega2560__) && !defined(__SAM3X8E__) && !defined(__STM32H745ZIQ__)
#define MSVC
#endif

#define STARTUP_CLASS_WARNING 0
#define STARTUP_CLASS_CRITICAL 1
#define STARTUP_CLASS_OPTIONAL 2

#define HARDWARE_COM_PORTS_USED 2

#ifndef MOTION_HARDWARE_DEF_H_
#define MOTION_HARDWARE_DEF_H_

#ifdef __AVR_ATmega2560__
#define F_CPU 16000000UL
#define _TICKS_PER_MICROSECOND (F_CPU/1000000)
#include "../Platforms/AVR_2560/Talos_AVR2560_Motion/PlatformSpecific/c_serial_avr_2560.h"
#include "../Platforms/AVR_2560/Talos_AVR2560_Motion/PlatformSpecific/c_core_avr_2560.h"
#include "../Platforms/AVR_2560/Talos_AVR2560_Motion/PlatformSpecific/c_spi_avr_2560.h"
#include "../Platforms/AVR_2560/Talos_AVR2560_Motion/PlatformSpecific/c_disk_avr_2560.h"
#include "../Platforms/AVR_2560/Talos_AVR2560_Motion/PlatformSpecific/c_motion_core_avr_2560_stepper.h"
#include "../Platforms/AVR_2560/Talos_AVR2560_Motion/PlatformSpecific/c_coordinator_avr2560_inputs.h"
#include "../Platforms/AVR_2560/Talos_AVR2560_Motion/PlatformSpecific/c_coordinator_avr2560_spindle.h"
#endif

#ifdef __SAM3X8E__
#define F_CPU 84000000UL
#define MAX_STEP_RATE 172000 //<--This doe not limit anything. It is only for a safety check.

//in the c_motion_core_arm_3x8e_stepper we define the clock scale at half the cpu speed.
//on the 8bit avr, the clock could run at full cpu speed. To get the motion time computations
//to come out right we need to adjust ticks per micro second for this cpu at this clock speed
//#define SELECTED_TIMER_CLOCK TC_CMR_TCCLKS_TIMER_CLOCK1. The only clock that actually runs
//at cpu speed is the sys_clk and it will be difficult to control as a stepper driver. At 42
//mhz clock speed I am able to get a reliable 172khz pulse rate from the processor.
#define _TICKS_PER_MICROSECOND ((F_CPU/2)/1000000)
#define F_CPU_2 F_CPU/2

//#include "../Platforms/ARM_3X8E/Talos_ARM3X8E_Motion/PlatformSpecific/c_motion_core_arm_3x8e_stepper.h"
//#include "../Platforms\ARM_3X8E\Talos_ARM3X8E\Platform Specific\c_motion_core_arm_3x8e_inputs.h"
//#include "../Platforms/ARM_3X8E/Talos_ARM3X8E_Motion/PlatformSpecific/c_motion_core_arm_3x8e_spindle.h"
#include "../Platforms/ARM_3X8E/Talos_ARM3X8E_Motion/PlatformSpecific/c_disk_arm_3x8e.h"
#include "../Platforms/ARM_3X8E/Talos_ARM3X8E_Motion/PlatformSpecific/c_core_arm_3x8e.h"
#include "../Platforms/ARM_3X8E/Talos_ARM3X8E_Motion/PlatformSpecific/c_serial_arm_3x8e.h"
#include "../Platforms/ARM_3X8E/Talos_ARM3X8E_Motion/PlatformSpecific/c_spi_arm_3x8e.h"
#include "../Platforms/ARM_3X8E/Talos_ARM3X8E_Motion/PlatformSpecific/c_motion_core_arm_3x8e_inputs.h"
#include "../Platforms/ARM_3X8E/Talos_ARM3X8E_Motion/PlatformSpecific/c_motion_core_arm_3x8e_stepper.h"
#include "../Platforms/ARM_3X8E/Talos_ARM3X8E_Motion/PlatformSpecific/c_motion_core_arm_3x8e_spindle.h"
#endif

#ifdef __STM32H745ZIQ__
#define F_CPU 8000000UL //<-- not CPU speed but TIMER speed.
#define HOST_CPU_ID 0
#define CORD_CPU_ID 1
#define MACH_CPU_ID 2
#define SPDL_CPU_ID 3
#define PRPH_CPU_ID 4
#define MAX_STEP_RATE 172000 //<--This doe not limit anything. It is only for a safety check.
//#define F_CPU 16000000
#define F_CPU_2 F_CPU/2
#define _TICKS_PER_MICROSECOND (F_CPU/1000000)
#include "Platforms/STM32/Talos_STM32H745_Motion/Platform_Specific/c_core_stm32h745.h"
#include "Platforms/STM32/Talos_STM32H745_Motion/Platform_Specific/c_serial_stm32h745.h"
#include "Platforms/STM32/Talos_STM32H745_Motion/Platform_Specific/c_disk_stm32h745.h"
#include "Platforms/STM32/Talos_STM32H745_Motion/Platform_Specific/c_stepper_stm32h745.h"
#include "Platforms/STM32/Talos_STM32H745_Motion/Platform_Specific/sys_gpio_configs.h"
#include "Platforms/STM32/Talos_STM32H745_Motion/Platform_Specific/sys_timer_configs.h"

#include "Platforms/WIN32/Talos_WIN32/Motion/Platform Specific/c_motion_core_win_inputs.h"
#include "Platforms/WIN32/Talos_WIN32/Motion/Platform Specific/c_motion_core_win_spindle.h"
#include "Platforms/WIN32/Talos_WIN32/Motion/Platform Specific/c_motion_core_win_spindle.h"

#endif

#ifdef MSVC
#define HOST_CPU_ID 0
#define CORD_CPU_ID 1
#define MACH_CPU_ID 2
#define SPDL_CPU_ID 3
#define PRPH_CPU_ID 4
#define MAX_STEP_RATE 172000 //<--This doe not limit anything. It is only for a safety check.
#define F_CPU 64000000UL
#define F_CPU_2 F_CPU/2
#define _TICKS_PER_MICROSECOND (F_CPU/1000000)
#include "Platforms/WIN32/Talos_WIN32/Motion/Platform Specific/c_disk_win.h"
#include "Platforms/WIN32/Talos_WIN32/Motion/Platform Specific/c_core_win.h"
#include "Platforms/WIN32/Talos_WIN32/Motion/Platform Specific/c_motion_core_win_stepper.h"
#include "Platforms/WIN32/Talos_WIN32/Motion/Platform Specific/c_motion_core_win_inputs.h"
#include "Platforms/WIN32/Talos_WIN32/Motion/Platform Specific/c_motion_core_win_spindle.h"
#include "Platforms/WIN32/Talos_WIN32/Motion/Platform Specific/c_serial_win.h"
#endif


#endif



#else ifdef TALOS_COORDINATOR
#if !defined(__AVR_ATmega328P__) && !defined(__AVR_ATmega2560__) && !defined(__SAM3X8E__) && !defined(__STM32H745ZIQ__)
#define MSVC
#endif

#define HARDWARE_COM_PORTS_USED 3

#define STARTUP_CLASS_WARNING 0
#define STARTUP_CLASS_CRITICAL 1
#define STARTUP_CLASS_OPTIONAL 2

#ifndef COORDINATOR_HARDWARE_DEF_H_
#define COORDINATOR_HARDWARE_DEF_H_

#ifdef __AVR_ATmega2560__
#define F_CPU 16000000UL
#define HOST_CPU_ID 0
#define CORD_CPU_ID 1
#define MACH_CPU_ID 2
#define SPDL_CPU_ID 3
#define PRPH_CPU_ID 4
#include "../Platforms/AVR_2560/Talos_AVR2560_Coordinator/PlatformSpecific/c_serial_avr_2560.h"
#include "../Platforms/AVR_2560/Talos_AVR2560_Coordinator/PlatformSpecific/c_core_avr_2560.h"
#include "../Platforms/AVR_2560/Talos_AVR2560_Coordinator/PlatformSpecific/c_spi_avr_2560.h"
#include "../Platforms/AVR_2560/Talos_AVR2560_Coordinator/PlatformSpecific/c_disk_avr_2560.h"
#endif

#ifdef __STM32H745ZIQ__
#define F_CPU 460000000UL
#define _TICKS_PER_MICROSECOND (F_CPU/1000000)
#define HOST_CPU_ID 0
#define CORD_CPU_ID 1
#define MACH_CPU_ID 2
#define SPDL_CPU_ID 3
#define PRPH_CPU_ID 4
#include "Platforms/STM32/Talos_STM32H745_Coordinator/Platform_Specific/c_core_stm32h745.h"
#include "Platforms/STM32/Talos_STM32H745_Coordinator/Platform_Specific/c_serial_stm32h745.h"
#include "Platforms/STM32/Talos_STM32H745_Coordinator/Platform_Specific/c_disk_stm32h745.h"
//#include "../Platforms/STM32F/Talos_STM32F1_Coordinator/PlatformSpecific/c_spi_STM32F103.h"


#endif

#ifdef MSVC
#define F_CPU 16000000
#define _TICKS_PER_MICROSECOND (F_CPU/1000000)
#define HOST_CPU_ID 0
#define CORD_CPU_ID 1
#define MACH_CPU_ID 2
#define SPDL_CPU_ID 3
#define PRPH_CPU_ID 4
#include "Platforms/WIN32/Talos_WIN32/Coordinator/Platform Specific/c_serial_win.h"
#include "Platforms/WIN32/Talos_WIN32/Coordinator/Platform Specific/c_core_win.h"
#include "Platforms/WIN32/Talos_WIN32/Coordinator/Platform Specific/c_spi_win.h"
#include "Platforms/WIN32/Talos_WIN32/Coordinator/Platform Specific/c_disk_win.h"
#endif


#endif /* COORDINATOR_HARDWARE_DEF_H_ */
#endif