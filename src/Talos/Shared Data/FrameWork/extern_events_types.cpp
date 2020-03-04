
#include "extern_events_types.h"
//s_data_events Talos::Shared::FrameWork::Events::extern_data_events;
s_ancillary_events Talos::Shared::FrameWork::Events::extern_ancillary_events;
s_system_events Talos::Shared::FrameWork::Events::extern_system_events;
void(*Talos::Shared::FrameWork::Error::Handler::extern_pntr_error_handler)(e_error_behavior e_behavior
	, uint8_t data_size, e_error_group e_group, e_error_process e_process, e_record_types e_rec_type
	, e_error_source e_source, e_error_code e_code, uint8_t e_origin, e_error_stack e_stack);
void(*Talos::Shared::FrameWork::Error::Handler::extern_pntr_ngc_error_handler)(char * ngc_line, e_error_behavior e_behavior
	, uint8_t data_size, e_error_group e_group, e_error_process e_process, e_record_types e_rec_type
	, e_error_source e_source, e_error_code e_code, uint8_t e_origin, e_error_stack e_stack);
//void(*Talos::Shared::FrameWork::Error::Handler::extern_test)();