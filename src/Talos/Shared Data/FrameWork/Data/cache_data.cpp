#include "cache_data.h"

s_system_message Talos::Shared::c_cache_data::status_record;
s_ngc_line_record Talos::Shared::c_cache_data::ngc_line_record;
s_ngc_block Talos::Shared::c_cache_data::ngc_block_record;
uint8_t(*Talos::Shared::c_cache_data::pntr_write_ngc_block_record)(s_ngc_block * write_block);
uint8_t(*Talos::Shared::c_cache_data::pntr_read_ngc_block_record)(s_ngc_block * read_block);

