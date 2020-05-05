#include "cache_data.h"


s_motion_control_settings_encapsulation Talos::Shared::c_cache_data::motion_configuration_record;

s_txt_line_record Talos::Shared::c_cache_data::txt_record;
s_ngc_block Talos::Shared::c_cache_data::ngc_block_record;

uint8_t(*Talos::Shared::c_cache_data::pntr_write_ngc_block_record)(s_ngc_block * write_block);
uint8_t(*Talos::Shared::c_cache_data::pntr_read_ngc_block_record)(s_ngc_block * read_block);

uint32_t Talos::Shared::c_cache_data::tic_count=0;
