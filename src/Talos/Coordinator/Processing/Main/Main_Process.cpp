/*
* Main_Process.cpp
*
* Created: 7/12/2019 6:26:46 PM
* Author: Family
This is the main entry point for the coordinator. All things pass through here and
then move to their respective modules.

*/



#include "Main_Process.h"
#include "..\Events\c_events.h"
#include "../../../c_ring_template.h"
#include "..\Events\extern_events_types.h"
#include "..\..\..\Motion\c_gcode_buffer.h"

#include <ctype.h>

//struct s_tester
//{
	//uint8_t value1;
	//uint8_t value2;
//};

c_Serial Talos::Coordinator::Main_Process::host_serial;

//static s_tester tester_buffer[3];
//static char char_buffer[3];
//static c_ring_buffer<char> serial_ring;
//static c_ring_buffer<s_tester> class_ring;
//static c_ring_buffer<s_tester> another_class_ring;


void Talos::Coordinator::Main_Process::initialize()
{

	Talos::Coordinator::Main_Process::host_serial = c_Serial(0, 115200); //<--Connect to host
	Hardware_Abstraction_Layer::Core::start_interrupts();//<--start interrupts on hardware
	Talos::Coordinator::Main_Process::host_serial.print_string("hello!");

	Talos::Coordinator::Events::initialize();//<--init events
	Talos::Motion::NgcBuffer::initialize();
	//Talos::Coordinator::NGC_Consumer::initialize();

#ifdef MSVC
	Hardware_Abstraction_Layer::Serial::add_to_buffer(0, "g0y#525r9\r\ng1x3\r\n");
#endif


	c_ring_buffer<char>*buffer;
	buffer = &Hardware_Abstraction_Layer::Serial::_usart0_buffer;
	while (1)
	{
		char _word = 0;
		char _word_value[32];
		memset(_word_value, 0, 32);
		int8_t _valid_word = findword(&_word, buffer);
		int8_t value_type = findvaluetype(buffer);
		int8_t _valid_value = findvalue(_word_value, &value_type, buffer);
	}
}

int8_t findword(char * _word, c_ring_buffer<char> * buffer)
{
	char byte = toupper(buffer->get());
	while (byte == ' ' && byte == '\t') { byte = toupper(buffer->get()); }
	//a word must be a letter between A and Z
	if (byte >= 'A' && byte <= 'Z')
	{
		*_word = byte; return 0;
	}
	else
	{
		_word = NULL; return 1;
	}

}

int8_t findvaluetype(c_ring_buffer<char> * buffer)
{
	char byte = toupper(buffer->peek());
	while (byte == ' ' && byte == '\t') { byte = toupper(buffer->get()); }

	//a value must be a number between 0 and 9.
	//it may also contain some symbols -,.,+,#
	if ((byte >= '0' && byte <= '9') || (byte == '-' || byte == '+' || byte == '.'))
	{
		return 1;
	}
	else if (byte == '#')
	{
		return 2;
	}
	else
	{
		return 3;
	}
}

int8_t findvalue(char _word_value[], int8_t * value_type, c_ring_buffer<char> * buffer)
{
	uint8_t pos = 0;
	while (1)
	{
		char byte = toupper(buffer->peek());
		while (byte == ' ' && byte == '\t') { byte = toupper(buffer->get()); }

		//a value must be a number between 0 and 9.
		//it may also contain some symbols -,.,+,#
		if (*value_type == 1)
		{
			if ((byte >= '0' && byte <= '9') || (byte == '-' || byte == '+' || byte == '.'))
			{
				_word_value[pos++] = toupper(buffer->get());;//<--valid value
			}
			//if its not a number only these values (indicating a new word) are valid. except an end of line
			else if ((byte >= 'A' && byte <= 'Z') || (byte ==13 || byte ==10))
			{
				return 0;
			}
			else
			{
				//this is epic failure. jump to end of line
				findeol(buffer);
				return 1;
			}
		}
		else if (*value_type == 2)
		{
			//can have a #, or a digit from 0 to 9. cannot be negative and cannot have decimals
			if ((byte == '#') || (byte >= '0' && byte <= '9'))
			{
				_word_value[pos++] = toupper(buffer->get());;//<--valid value
				//There are at least 2 types of parameters. named, and numbered.
				//Peek at the next byte to determine which one
				char byte = toupper(buffer->peek());
				while (byte == ' ' && byte == '\t') { byte = toupper(buffer->get()); }
				if (byte == '<')
					*value_type = 4; //<--named parameter
			}
		}
	}
}

void findeol(c_ring_buffer<char> * buffer)
{
	//this is epic failure. read until we get to a CR or LF so we can get to the end of the line
	while (buffer->peek() != 13 && buffer->peek() != 10) { buffer->get(); }
	//we hit one of them, but dont know or care which. loop until we arent on one of them.
	while (buffer->peek() == 13 || buffer->peek() == 10) { buffer->get(); }
}





void Talos::Coordinator::Main_Process::run()
{


	//Start the eventing loop, stop loop if a critical system error occurs
	while (extern_system_events.event_manager.get((int)s_system_events::e_event_type::SystemAllOk))
	{
		//This firmware is mostly event driven. This is the main entry point for checking
		//which events have been set to execute, and then executing them.
		Talos::Coordinator::Events::process();
	}
}