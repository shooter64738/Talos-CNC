struct s_jog_data_block
{
	const BinaryRecords::e_record_types __rec_type__ = BinaryRecords::e_record_types::Jog;
	float axis_value;
	uint8_t axis;
	uint32_t flag = (int)BinaryRecords::e_block_flag::block_state_normal;
	uint32_t _check_sum = 0;
};