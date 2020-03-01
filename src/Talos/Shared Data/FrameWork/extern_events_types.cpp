//
//#include <stdint.h>
//#include "../../_bit_flag_control.h"
//#include "../_e_motion_state.h"
//#include "../_s_framework_error.h"
//#include "../../c_ring_template.h"
////s_data_events extern_data_events;
//s_data_events extern_data_events;
//s_ancillary_events extern_ancillary_events;
//s_system_events extern_system_events;
//s_motion_controller_events extern_motion_control_events;
//void(*Talos::Shared::FrameWork::Error::Handler::extern_pntr_error_handler)(c_ring_buffer<char> * buffer, s_framework_error error);
//void(*Talos::Shared::FrameWork::Error::Handler::extern_test)();
#include "extern_events_types.h"
s_data_events Talos::Shared::FrameWork::Events::extern_data_events;
s_ancillary_events Talos::Shared::FrameWork::Events::extern_ancillary_events;
s_system_events Talos::Shared::FrameWork::Events::extern_system_events;
s_motion_controller_events Talos::Shared::FrameWork::Events::extern_motion_control_events;
void(*Talos::Shared::FrameWork::Error::Handler::extern_pntr_error_handler)(c_ring_buffer<char> * buffer, s_framework_error error);
void(*Talos::Shared::FrameWork::Error::Handler::extern_pntr_ngc_error_handler)(char * ngc_line, s_framework_error error);
//void(*Talos::Shared::FrameWork::Error::Handler::extern_test)();