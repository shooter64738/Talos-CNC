#include "cache_data.h"


char * Talos::Shared::c_cache_data::pntr_binary_buffer_array;
char Talos::Shared::c_cache_data::binary_buffer_array[__FRAMEWORK_BINARY_BUFFER_SIZE];

//s_control_message Talos::Shared::c_cache_data::system_record;
//s_control_message *Talos::Shared::c_cache_data::pntr_system_record;

s_control_message Talos::Shared::c_cache_data::system_message_group[4];
s_control_message Talos::Shared::c_cache_data::temp_system_message;


s_motion_control_settings_encapsulation Talos::Shared::c_cache_data::motion_configuration_record;
s_motion_control_settings_encapsulation *Talos::Shared::c_cache_data::pntr_motion_configuration_record;

s_txt_line_record Talos::Shared::c_cache_data::txt_record;
s_ngc_block Talos::Shared::c_cache_data::ngc_block_record;

uint8_t(*Talos::Shared::c_cache_data::pntr_write_ngc_block_record)(s_ngc_block * write_block);
uint8_t(*Talos::Shared::c_cache_data::pntr_read_ngc_block_record)(s_ngc_block * read_block);

uint32_t Talos::Shared::c_cache_data::tic_count=0;
