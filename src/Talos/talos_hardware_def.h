/*
 * hardware_def.h
 *
 * Created: 7/12/2019 6:30:30 PM
 *  Author: Family
 */

//To enable compilation and debugging in Microsoft Visual C++ define MSCV
#ifdef TALOS_COMBINED

#if !defined(__STM32H7__)
#define MSVC
#endif

#define HOST_CPU_ID 0
#define CORD_CPU_ID 1
#define MACH_CPU_ID 2
#define SPDL_CPU_ID 3
#define PRPH_CPU_ID 4

#ifdef MSVC
#define F_CPU 16000000
#define _TICKS_PER_MICROSECOND (F_CPU/1000000)

#include "Platforms/WIN32/Talos_WIN32/Combined/Platform_Specific/c_serial_win.h"
#include "Platforms/WIN32/Talos_WIN32/Combined/Platform_Specific/c_core_win.h"
#include "Platforms/WIN32/Talos_WIN32/Combined/Platform_Specific/c_disk_win.h"
#include "Platforms/WIN32/Talos_WIN32/Combined/Platform_Specific/c_stepper_win.h"
#include "Platforms/WIN32/Talos_WIN32/Combined/Platform_Specific/sys_gpio_configs.h"
#include "Platforms/WIN32/Talos_WIN32/Combined/Platform_Specific/sys_timer_configs.h"
#endif

//#ifdef __STM32H7__
//
//#define MAX_STEP_RATE 172000 //<--This doe not limit anything. It is only for a safety check.
//#define F_CPU 460000000
//#define F_CPU_2 F_CPU/2
//#define _TICKS_PER_MICROSECOND (F_CPU/1000000)
//#include "Platforms/STM32/Talos_STM32H7_Combined/Platform_Specific/c_core_stm32h7.h"
//#include "Platforms/STM32/Talos_STM32H7_Combined/Platform_Specific/c_serial_stm32h7.h"
//#include "Platforms/STM32/Talos_STM32H7_Combined/Platform_Specific/c_disk_stm32h7.h"
//#include "Platforms/STM32/Talos_STM32H7_Combined/Platform_Specific/c_stepper_stm32h7.h"
//#include "Platforms/STM32/Talos_STM32H7_Combined/Platform_Specific/sys_gpio_configs.h"
//#include "Platforms/STM32/Talos_STM32H7_Combined/Platform_Specific/sys_timer_configs.h"
//#endif
#endif


