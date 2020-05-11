struct s_peripheral_panel
{
	const BinaryRecords::e_record_types __rec_type__ = BinaryRecords::e_record_types::Peripheral_Control_Setting;
	s_peripheral_group_processing Processing;
	s_peripheral_group_overrides OverRides;
	s_peripheral_group_jogging Jogging;
	uint32_t _check_sum = 0;

};