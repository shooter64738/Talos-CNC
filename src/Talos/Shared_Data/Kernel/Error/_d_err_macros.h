#ifndef __C_KERNEL_ERROR_MACROS
#define __C_KERNEL_ERROR_MACROS
#include <stdint.h>
//#define RTRN_IF_FALSE(__Function__) if (!__Function__) return false; 
//#define RTRN_IF_TRUE(__Function__) if (__Function__) return false; 

/*
Call a function, if an error (false) is returned, add the function we are in, and
the function we called to the error stack. Use this when reporting an error that
MIGHT occur when you call another function. If no error (true) is returned then
nothing is added to the stack.
*/
#define ADD_2_STK_RTN_FALSE_IF_CALL_FALSE(Mthd_2_Call , CPUID , BASE , Mthd_We_Are_In , Mthd_Or_Line_Code)\
if (!Mthd_2_Call) \
{Talos::Kernel::Error::raise_error(BASE , Mthd_We_Are_In , Mthd_Or_Line_Code , CPUID); \
return false; }

#define ADD_2_STK_RTN_FALSE_IF_OBJECT_NULL(Nullable_Object , CPUID , BASE , Mthd_We_Are_In , Mthd_Or_Line_Code)\
if (Nullable_Object == NULL) \
{Talos::Kernel::Error::raise_error(BASE , Mthd_We_Are_In , Mthd_Or_Line_Code , CPUID); \
return false; }


//Use this when reporting an error but you are NOT calling another function and need to return false
#define ADD_2_STK_RTN_FALSE(CPUID , BASE , Mthd_We_Are_In , Mthd_Or_Line_Code)\
{Talos::Kernel::Error::raise_error(BASE , Mthd_We_Are_In , Mthd_Or_Line_Code , CPUID); \
return false; }

//#define ADD_2_STK(CPUID , BASE , Mthd_We_Are_In , Mthd_Or_Line_Code)\
//{Talos::Kernel::Error::raise_error(BASE , Mthd_We_Are_In , Mthd_Or_Line_Code , CPUID);}

#endif