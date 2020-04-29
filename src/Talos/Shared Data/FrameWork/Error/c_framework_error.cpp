
#include "c_framework_error.h"

void(*Talos::Shared::FrameWork::Error::extern_pntr_error_handler)();
void(*Talos::Shared::FrameWork::Error::extern_pntr_ngc_error_handler)(char * ngc_line);
s_framework_error Talos::Shared::FrameWork::Error::framework_error;