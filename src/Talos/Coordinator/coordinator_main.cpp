
/*
 * Talos_ARM3X8E.cpp
 *
 * Created: 7/12/2019 6:07:22 PM
 * Author : Family
 */ 

#include "Processing/Main/Main_Process.h"

int main(void)
{
	Talos::Coordinator::Main_Process::initialize();
	Talos::Coordinator::Main_Process::run();
}
