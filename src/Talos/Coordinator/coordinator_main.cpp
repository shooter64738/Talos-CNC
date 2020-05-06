
/*
 * Talos_ARM3X8E.cpp
 *
 * Created: 7/12/2019 6:07:22 PM
 * Author : Family
 */ 

#include "../Shared Data/NewFrameWork/Start/c_framework_start.h"

int main(void)
{
	//init framework base
	Talos::NewFrameWork::Base::f_initialize();
	//init framework comms
	Talos::NewFrameWork::Comm::f_initialize(NULL, NULL, NULL, NULL, NULL);
	//init framwork cpus
	Talos::NewFrameWork::CPU::f_initialize(0, 0, 1, 2, 3, NULL);
	//init data handler reader
	Talos::NewFrameWork::DataHandler::Binary::f_initialize(Talos::NewFrameWork::CPU::cluster[Talos::NewFrameWork::CPU::host_id]);
	
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('G');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('0');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('X');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\r');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\n');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\8');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\8');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\4');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\5');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\6');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\7');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\8');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\9');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\5');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\4');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\2');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\2');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\2');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');
	Talos::NewFrameWork::CPU::cluster[0].hw_data_buffer.put('\1');

	while(1)
	Talos::NewFrameWork::CPU::service_events();
}
