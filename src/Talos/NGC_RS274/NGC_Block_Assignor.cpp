/*
*  NGC_Block.cpp - NGC_RS274 controller.
*  A component of Talos
*
*  Copyright (c) 2016-2019 Jeff Dill
*
*  Talos is free software: you can redistribute it and/or modify
*  it under the terms of the GNU LPLPv3 License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  Talos is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with Talos.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "NGC_Block_Assignor.h"
#include <string.h>
#include "../_bit_manipulation.h"
#include "_ngc_g_Groups.h"
#include "_ngc_m_Groups.h"
#include "NGC_Block_View.h"
#include "_ngc_errors_interpreter.h"
#include "_ngc_dialect_enum.h"
#include "Dialect/_ngc_validate_16_plane_rotation.h"
#include "Dialect/_ngc_validate_1_motion.h"
#include "NGC_Error_Check.h"

//NGC_RS274::Block_Assignor::Block_Assignor(){}
//NGC_RS274::Block_Assignor::~Block_Assignor(){}

uint16_t _ngc_working_group = 0;

uint32_t NGC_RS274::Block_Assignor::group_word(char Word, float Address, s_ngc_block *new_block)
{
	switch (Word)
	{
		case 'G': //<--Process words for G (G10,G20,G91, etc..)
		return _gWord(Address, new_block);
		break;
		case 'M': //<--Process words for M (M3,M90,M5, etc..)
		return _mWord(Address, new_block);
		break;
		default:
		return _pWord(Word, Address, new_block); //<--Process words for Everything else (X__,Y__,Z__,I__,D__, etc..)
		break;
	}

	return  c_bit_errors::set(c_bit_errors::e_interpret_error::INTERPRETER_DOES_NOT_UNDERSTAND_CHARACTER_IN_BLOCK);
}

/*
Assign the corresponding value to a Group number for an Address
*/
uint32_t NGC_RS274::Block_Assignor::_gWord(float Address, s_ngc_block *new_block)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| This methods primary purpose is to determine which group a gcode belongs to. The value|
	| for that group is then stored in an array of 14 different slots. To determine a value |
	| set for a particular group you just need to access the array _g_group. the value in the|
	| group is *G_CODE_MULTIPLIER so its TRUE value would be /100 because it may contain
	| decimals. the Get_GCode_Value() method is the primary accessor for this information from outside|
	| this library. It handles the *G_CODE_MULTIPLIER/G_CODE_MULTIPLIER internaly so there
	| is no need to modify the code |
	|***************************************************************************************|
	*/
	//Convert this to an int so we can store smaller types
	uint16_t iAddress = Address * G_CODE_MULTIPLIER;
	//_serial.Write("i address ");_serial.Write_ni(iAddress);_serial.Write(CR);
	//_serial.Write("f address ");_serial.Write_nf(Address);_serial.Write(CR);
	/*
	It is illegal to specify more than one command from the same group. We set a bit for
	each of the 14 respective groups to catch more than one g command from the same group.
	For example G0,G1,G2,F3 are all in the motion group (group 1). Only one of those can
	be specified per line.
	*/


	
	switch (iAddress)
	{
		case NGC_RS274::G_codes::RAPID_POSITIONING: //<-G00
		case NGC_RS274::G_codes::LINEAR_INTERPOLATION: //<-G01
		case NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW: //<-G02
		case NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW: //<-G03
		case NGC_RS274::G_codes::MOTION_CANCELED: //<-G80
		case NGC_RS274::G_codes::CANNED_CYCLE_BACK_BORING: //<-G87
		case NGC_RS274::G_codes::CANNED_CYCLE_BORING_DWELL_FEED_OUT: //<-G89
		case NGC_RS274::G_codes::CANNED_CYCLE_BORING_NO_DWELL_FEED_OUT: //<-G85
		case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_MANUAL_OUT: //<-G88
		case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_RAPID_OUT: //<-G86
		case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING: //<-G81
		case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING_WITH_DWELL: //<-G82
		case NGC_RS274::G_codes::CANNED_CYCLE_PECK_DRILLING: //<-G83
		case NGC_RS274::G_codes::CANNED_CYCLE_RIGHT_HAND_TAPPING: //<-G84
		//Any of these are motions, but I want to also differentiate between a typical interpolation and a canned cycle
		_ngc_working_group = NGC_RS274::Groups::G::Motion;
		//Flag it as a motion change
		//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::Motion);
		
		
		if (iAddress > NGC_RS274::G_codes::MOTION_CANCELED)
		{
			//Flag it as a canned cycle change.
			//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::Canned_Cycle_Active);
		}
		new_block->g_group[NGC_RS274::Groups::G::Motion] = (iAddress);

		//NGC_RS274::Error_Check::dialect_verify[_ngc_working_group] = NGC_RS274::Dialect::Group1::motion_validate;

		break;

		case NGC_RS274::G_codes::G10_PARAM_WRITE: //<-G10
		_ngc_working_group = NGC_RS274::Groups::G::NON_MODAL;//<-G4,G10,G28,G30,G53,G92,92.2,G92.3
		//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::Non_modal);
		new_block->g_group[NGC_RS274::Groups::G::NON_MODAL] = (iAddress);
		break;

		case NGC_RS274::G_codes::RECTANGULAR_COORDINATE_SYSTEM: //<-G15
		case NGC_RS274::G_codes::POLAR_COORDINATE_SYSTEM: //<-G16
		_ngc_working_group = NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION;
		//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::RECTANGLAR_POLAR_COORDS_SELECTION);
		new_block->g_group[NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION] = (iAddress);
		break;

		case NGC_RS274::G_codes::XY_PLANE_SELECTION: //<-G17
		case NGC_RS274::G_codes::XZ_PLANE_SELECTION: //<-G18
		case NGC_RS274::G_codes::YZ_PLANE_SELECTION: //<-G19

		_ngc_working_group = NGC_RS274::Groups::G::PLANE_SELECTION;
		//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::PLANE_SELECTION);
		new_block->g_group[NGC_RS274::Groups::G::PLANE_SELECTION] = (iAddress);
		//Block_Assignor::error_check_plane_select(new_block, previous_block);
		break;

		case NGC_RS274::G_codes::INCH_SYSTEM_SELECTION: //<-G20
		case NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION: //<-G21
		_ngc_working_group = NGC_RS274::Groups::G::Units;
		//If units are changing, update the feed rate in the interpreter
		//if (new_block->g_group[NGC_RS274::Groups::G::Units] != (iAddress))
		//{
		//	//currently we are in inches and going to mm
		//	if (new_block->g_group[NGC_RS274::Groups::G::Units]
		//		== NGC_RS274::G_codes::INCH_SYSTEM_SELECTION)
		//	{
		//		new_block->set_value
		//		('F', new_block->get_value('F')*25.4);
		//	}
		//	//currently we are in mm and going to inches
		//	if (new_block->g_group[NGC_RS274::Groups::G::Units]
		//		== NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION)
		//	{
		//		new_block->set_value
		//		('F', new_block->get_value('F') / 25.4);
		//	}
		//}
		//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::Units);
		new_block->g_group[NGC_RS274::Groups::G::Units] = (iAddress);
		break;

		case NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION: //<-G40
		case NGC_RS274::G_codes::START_CUTTER_RADIUS_COMPENSATION_LEFT: //<-G41
		case NGC_RS274::G_codes::START_CUTTER_RADIUS_COMPENSATION_RIGHT: //<-G42
		_ngc_working_group = NGC_RS274::Groups::G::Cutter_radius_compensation;
		//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::Cutter_radius_compensation);
		new_block->g_group[NGC_RS274::Groups::G::Cutter_radius_compensation] = (iAddress);
		break;


		case NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM: //<-G53
		_ngc_working_group = NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION;
		//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::COORDINATE_SYSTEM_SELECTION);
		new_block->g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = (iAddress);
		break;

		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_1_G54: //<-G54, G55, G56, G57, G58, G59, G59.1, G59.2, G59.3
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_2_G55:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_3_G56:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_4_G57:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_5_G58:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59_1:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59_2:
		case NGC_RS274::G_codes::WORK_OFFSET_POSITION_6_G59_3:
		//this->WorkOffsetValue = iAddress;
		_ngc_working_group = NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION;
		//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::COORDINATE_SYSTEM_SELECTION);
		new_block->g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = (iAddress);
		break;

		case NGC_RS274::G_codes::ABSOLUTE_DISANCE_MODE: //<-G90
		case NGC_RS274::G_codes::INCREMENTAL_DISTANCE_MODE: //<-G91
		_ngc_working_group = NGC_RS274::Groups::G::DISTANCE_MODE;
		//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::DISTANCE_MODE);
		new_block->g_group[NGC_RS274::Groups::G::DISTANCE_MODE] = (iAddress);
		break;

		case NGC_RS274::G_codes::FEED_RATE_MINUTES_PER_UNIT_MODE: //<-G93
		case NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE: //<-G94
		case NGC_RS274::G_codes::FEED_RATE_UNITS_PER_ROTATION: //<-G95
		_ngc_working_group = NGC_RS274::Groups::G::Feed_rate_mode;
		//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::Feed_rate_mode);
		new_block->g_group[NGC_RS274::Groups::G::Feed_rate_mode] = (iAddress);
		break;

		case NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R: //<-G99
		case NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_Z: //<-G98
		_ngc_working_group = NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE;
		//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::RETURN_MODE_CANNED_CYCLE);
		new_block->g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = (iAddress);
		break;

		case NGC_RS274::G_codes::CANCEL_TOOL_LENGTH_OFFSET: //<-G49
		case NGC_RS274::G_codes::POSITIVE_TOOL_LENGTH_OFFSET: //<-G43
		case NGC_RS274::G_codes::USE_TOOL_LENGTH_OFFSET_FOR_TRANSIENT_TOOL: //<-G43.1
		case NGC_RS274::G_codes::NEGATIVE_TOOL_LENGTH_OFFSET: //<-G44
		_ngc_working_group = NGC_RS274::Groups::G::Tool_length_offset;
		//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::Tool_length_offset);
		new_block->g_group[NGC_RS274::Groups::G::Tool_length_offset] = (iAddress);
		break;

		case NGC_RS274::G_codes::PLANE_ROTATION_START: //<-G68
		case NGC_RS274::G_codes::PLANE_ROTATION_CANCEL: //<-G69
			_ngc_working_group = NGC_RS274::Groups::G::PLANE_ROTATION;
			//new_block->block_events.set(_group_value_changed(new_block->g_group[NGC_RS274::Groups::G::Motion], iAddress), (int)e_block_event::Tool_length_offset);
			new_block->g_group[NGC_RS274::Groups::G::PLANE_ROTATION] = (iAddress);
			break;

		default:
		return c_bit_errors::set(c_bit_errors::e_interpret_error::INTERPRETER_DOES_NOT_UNDERSTAND_G_WORD_VALUE); //<-- -1
		break;
	}

	/*
	See if we have already processed a g command from this group.
	We can make the error more meaningful if we tell it WHICH group
	was specified more than once.
	*/
	if (new_block->g_code_defined_in_block.get((int)_ngc_working_group))
		/*
		Since _working_g_group is the 'group' number we can add it to the base error
		value and give the user a more specific error so they know what needs
		to be corrected
		*/
		return c_bit_errors::set( (c_bit_errors::e_g_error)
			((int)c_bit_errors::e_g_error::G_CODE_GROUP_NON_MODAL_ALREADY_SPECIFIED + _ngc_working_group));

	/*
	If we havent already returned from a duplicate group being specified, set the bit flag for this
	gcode group. This will get checked if this method is called again but if a gcode for the same
	group is on the line again, the logic above will catch it and return an error
	*/
	new_block->g_code_defined_in_block.set(_ngc_working_group);
	
	return  c_bit_errors::ok;
}

/*
Assign the corresponding Group number for an M code
*/
uint32_t NGC_RS274::Block_Assignor::_mWord(float Address, s_ngc_block *new_block)
{
	/*
	|****************************************************************************************|
	|                              IMPORTANT INFORMATION                                     |
	| This methods primary purpose is to determine which group an mcode belongs to. The value|
	| for that group is then stored in an array of 5 different slots. To determine a value   |
	| set for a particular group you just need to access the array _m_group. The value in the|
	| group is *100 so its TRUE value would be /100 because it may contain decimals.         |
	| the Get_MCode_Value() method is the primary accessor for this information from outside |
	| this library. It handles the *100/100 internal so there is no need to modify the code  |
	|****************************************************************************************|
	*/
	//Convert this to an int so we can store smaller types
	int iAddress = (int)Address * G_CODE_MULTIPLIER;

	switch (iAddress)
	{
		case NGC_RS274::M_codes::SPINDLE_ON_CW: //<-M03
		case NGC_RS274::M_codes::SPINDLE_ON_CCW: //<-M04
		case NGC_RS274::M_codes::SPINDLE_STOP: //<-M05
		{
			_ngc_working_group = NGC_RS274::Groups::M::SPINDLE;
			//new_block->block_events.set(_group_value_changed(new_block->m_group[NGC_RS274::Groups::M::SPINDLE], iAddress), (int)e_block_event::Spindle_mode);
			new_block->m_group[NGC_RS274::Groups::M::SPINDLE] = (iAddress);
			break;
		}

		case NGC_RS274::M_codes::PROGRAM_PAUSE: //<-M00
		case NGC_RS274::M_codes::PROGRAM_PAUSE_OPTIONAL: //<-M01
		case NGC_RS274::M_codes::TOOL_CHANGE_PAUSE: //<-M06
		{
			_ngc_working_group = NGC_RS274::Groups::M::TOOL_CHANGE;
			//new_block->block_events.set(_group_value_changed(new_block->m_group[NGC_RS274::Groups::M::SPINDLE], iAddress), (int)e_block_event::Tool_Change_Request);
			new_block->m_group[NGC_RS274::Groups::M::TOOL_CHANGE] = (iAddress);
			break;
		}
		case NGC_RS274::M_codes::PALLET_CHANGE_PAUSE: //<-M60
		{
			_ngc_working_group = NGC_RS274::Groups::M::STOPPING;
			//new_block->block_events.set(_group_value_changed(new_block->m_group[NGC_RS274::Groups::M::SPINDLE], iAddress), (int)e_block_event::STOPPING);
			new_block->m_group[NGC_RS274::Groups::M::STOPPING] = (iAddress);
			break;
		}

		case NGC_RS274::M_codes::COLLANT_MIST: //<-M07
		case NGC_RS274::M_codes::COOLANT_FLOOD: //<-M08
		case NGC_RS274::M_codes::COOLANT_OFF: //<-M09
		{
			_ngc_working_group = NGC_RS274::Groups::M::COOLANT;
			//new_block->block_events.set(_group_value_changed(new_block->m_group[NGC_RS274::Groups::M::SPINDLE], iAddress), (int)e_block_event::Coolant);
			new_block->m_group[NGC_RS274::Groups::M::COOLANT] = (iAddress);
			//Since we DO allow multiple coolant modes at the same time, we are just going to return here
			//No need to check if this modal group was already set on the line.
			return  c_bit_errors::ok;
			break;
		}

		case NGC_RS274::M_codes::ENABLE_FEED_SPEED_OVERRIDE: //<-M48
		case NGC_RS274::M_codes::DISABLE_FEED_SPEED_OVERRIDE: //<-M49
		{
			_ngc_working_group = NGC_RS274::Groups::M::OVERRIDE;
			//new_block->block_events.set(_group_value_changed(new_block->m_group[NGC_RS274::Groups::M::SPINDLE], iAddress), (int)e_block_event::OVERRIDE);
			new_block->m_group[NGC_RS274::Groups::M::OVERRIDE] = (iAddress);
			break;
		}

		default:
		if (iAddress >= 100 && iAddress <= 199)
		{
			_ngc_working_group = NGC_RS274::Groups::M::USER_DEFINED;
			//new_block->block_events.set(_group_value_changed(new_block->m_group[NGC_RS274::Groups::M::SPINDLE], iAddress), (int)e_block_event::USER_DEFINED);
			new_block->m_group[NGC_RS274::Groups::M::USER_DEFINED] = (iAddress);
		}
		else
		{
			return c_bit_errors::set(c_bit_errors::e_interpret_error::INTERPRETER_DOES_NOT_UNDERSTAND_M_WORD_VALUE);
		}

		break;
	}

	/*
	See if we have already processed an m command from this group.
	We can make the error more meaningful if we tell it WHICH group
	was specified more than once.
	*/
	if (new_block->m_code_defined_in_block.get((int)_ngc_working_group))
		return c_bit_errors::set(c_bit_errors::e_m_error::M_CODE_GROUP_COOLANT_ALREADY_SPECIFIED);

	/*
	If we havent already returned from a duplicate group being specified, set the bit flag for this
	mcode group. This will get checked if this method is called again but if a mcode for the same
	group is on the line again, the logic above will catch it and return an error
	*/
	new_block->m_code_defined_in_block.set(_ngc_working_group);
	return  c_bit_errors::ok;

}

/*
Assign the corresponding Address value for a specific G Word
*/
uint32_t NGC_RS274::Block_Assignor::_pWord(char Word, float iAddress, s_ngc_block *new_block)
{
	//TODO: I dont recall what I had to do!
	//We can't just assume these values are usable as is. We have
	//to know which GCode is being processed right now to determine
	//what to do with the values we have. For example G4 X1 would
	//be dwell for 1 second. G0 X1 would be a move X from current
	//position to 1 (if G90 active), or to current position+1
	//(if in G91 active)

	//Convert this to an int so we can store smaller types
	//float iAddress = atof(Address);
	uint32_t ReturnValue = c_bit_errors::ok;
	//If the working group is a motion group, then the x,y,z,a,b,c,u,v,w value is an axis value

	//the NIST standard states it is an error to have an axis word on a line for a motion and a non modal group at the same time (page 20)
	/*switch (_working_g_group)
	{
	case  NGC_RS274::Groups::G::MOTION_GROUP:
	ReturnValue = _Process_MOTION_GROUP(Word, iAddress);
	break;
	case  NGC_RS274::Groups::G::NON_MODAL:
	ReturnValue = _Process_NON_MODAL_GROUP(Word, iAddress);
	break;
	default:*/
	{
		//Catch any words that doesnt have to be in a  group, such as feedrate(F), offsets(D), etc..
		ReturnValue = _process_word_values(Word, iAddress, new_block);
		//break;
	}
	return ReturnValue;
}

uint32_t NGC_RS274::Block_Assignor::_process_word_values
(char Word, float iAddress, s_ngc_block *new_block)
{
	/*
	|***************************************************************************************|
	|                              IMPORTANT INFORMATION                                    |
	| There is probably no need to process block information based on the last 'group' that |
	| was set active while processing. but it may help catch syntax errors in the code line |
	| that would otherwise have to be caught after the error check is done. All group       |
	| processing methods call '_Process_Word_Values' so theres not a code duplication or a  |
	| performance hit that I can tell. It just seems more useful this way.                   |
	|***************************************************************************************|
	*/
	/*
	Always check to see if an axis word was defined already for a different group. NGC standard says you cant do that.

	Each word value that corresponds to an axis is checked. If the bit flag for that axis is already set
	that means the axis was either:
	1. Defined twice in the same line with the same motion command ie (G0 X4 Y4 X2)
	2. Defined twice but for two different commands ie (G0 X4 G4 P1 X1)
	*/

	/*
	Since we are working with char types we can check the word without explicitly defining it
	We can determine the bit number for this word by subtracting 65 from it since A=65 and thats
	the first 'Word'
	*/
	uint8_t WordNumber = (Word - 'A');
	if (new_block->word_flags.get(WordNumber))
	//Return the equivalent NGC_Interpreter_Errors number
	return c_bit_errors::set((c_bit_errors::e_word_error) WordNumber);
	/*
	Since this bit was not already set, we can go ahead and set it now
	If this same word value was specified twice in the same block, we will
	catch it when it comes in again with the code above.
	*/

	new_block->word_flags.set(WordNumber);

	/*
	Almost all letters of the alphabet are used as words. The easiest and simplest way
	to set the different word values is to hold them in an array of values. Then we
	simply need to know which letter in the alphabet it is. A=0,B=1,C=2....Z=25. that
	value is already stored in the BitNumber we used above.
	*/

	new_block->word_values[WordNumber] = iAddress;

	/*
	I am leaving the switch case but commented out, so we can add 'per word'
	checking/processing if we decide we need to. But at this stage I don't
	think that would be needed. We cant do detailed checking until the entire
	line is loaded because people write gcode in all sorts of odd order
	*/
	//switch (Word) //Alternatively use WordNumber. WordNumber 0 is A, WordNumber 25 is Z
	//{
	//case 'A':
	//	break;
	//default:
	//	break;
	//}
	return  c_bit_errors::ok;
}

//bool NGC_RS274::Block_Assignor::_group_value_changed(uint16_t old_value, uint16_t new_value)
//{
//	//return true if they are different
//	return !(new_value == old_value);
//}
