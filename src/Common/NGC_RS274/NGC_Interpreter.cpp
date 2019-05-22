/*
*  NGC_Interpreter.cpp - NGC_RS274 controller.
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


#include "NGC_Interpreter.h"
#include "NGC_Errors.h"
#include "NGC_M_Groups.h"
#include "NGC_G_Codes.h"
#include "NGC_M_Codes.h"
#include "..\..\Coordinator\Shared\c_processor.h"

//char NGC_RS274::Interpreter::Processor::Line[CYCLE_LINE_LENGTH];
char * NGC_RS274::Interpreter::Processor::Line;
int NGC_RS274::Interpreter::Processor::HasErrors = 0;
NGC_RS274::NGC_Binary_Block NGC_RS274::Interpreter::Processor::local_block = NGC_RS274::NGC_Binary_Block();
NGC_RS274::NGC_Binary_Block*NGC_RS274::Interpreter::Processor::stager_block;
bool NGC_RS274::Interpreter::Processor::normalize_distance_units_to_mm = true;
uint16_t ngc_working_group = 0;

void NGC_RS274::Interpreter::Processor::initialize()
{
	clear_line();
	//NGC_RS274::Interpreter::Processor::local_block = c_block();
}

void NGC_RS274::Interpreter::Processor::clear_line()
{
	//memset(NGC_RS274::Interpreter::Processor::Line, 0, CYCLE_LINE_LENGTH * sizeof(char));
}

int NGC_RS274::Interpreter::Processor::process_line(NGC_RS274::NGC_Binary_Block*plan_block)
{
	//NGC_RS274::Interpreter::Processor::local_block is the block that the intepreter uses for work. If an error is found processing the line, this block will be cleared.
	//plan_block, is the block that the local_block will be copied to, if interpretation is successful.

	//We need to know the state of the latest processed block. We will point the stager_block to the block sent in to us
	NGC_RS274::Interpreter::Processor::stager_block = plan_block;

	NGC_RS274::Interpreter::Processor::local_block.reset(); //<-- Clear EVERYTHING from the local block

	//Synch local block, to the block that was sent in. This block should contain all the values that were set in the last interpretation. This ensures all modal values are kept.
	memcpy(NGC_RS274::Interpreter::Processor::local_block.unit_target_positions, plan_block->unit_target_positions, MACHINE_AXIS_COUNT * sizeof(float));
	memcpy(NGC_RS274::Interpreter::Processor::local_block.g_group, plan_block->g_group, COUNT_OF_G_CODE_GROUPS_ARRAY * sizeof(uint16_t));
	memcpy(NGC_RS274::Interpreter::Processor::local_block.m_group, plan_block->m_group, COUNT_OF_M_CODE_GROUPS_ARRAY * sizeof(uint16_t));
	memcpy(&NGC_RS274::Interpreter::Processor::local_block.persisted_values, &plan_block->persisted_values, sizeof(NGC_RS274::NGC_Binary_Block::s_persisted_values));
	memcpy(NGC_RS274::Interpreter::Processor::local_block.block_word_values, plan_block->block_word_values, COUNT_OF_BLOCK_WORDS_ARRAY * sizeof(float));

	//memcpy(&NGC_RS274::Interpreter::Processor::local_block.plane_axis, &plan_block->plane_axis, sizeof(NGC_RS274::NGC_Binary_Block::s_plane_axis));
	//memcpy(&NGC_RS274::Interpreter::Processor::local_block.arc_values, &plan_block->arc_values, sizeof(NGC_RS274::NGC_Binary_Block::s_arc_values));

	HasErrors = 0;

	//Parse gcode line data and do some rough error testing
	int ReturnValue = parse_values();

	if (ReturnValue != NGC_RS274::Interpreter::Errors::OK)
	{
		NGC_RS274::Interpreter::Processor::local_block.reset();
		HasErrors = ReturnValue;
		return ReturnValue;
	}

	/*
	After parsing and before error checking convert units to MM if this block specifies inches
	*/
	NGC_RS274::Interpreter::Processor::normalize_distance_units();

	/*Line parsing is completed. By this point, we should have a complete line with the basics there
	We do need to test for several other things though. Such as arc end points with matching radius, etc
	Perform Error Checking on the data now.

	Examples of error checking:
	G90G20X1 and no motion command (G0,G1,G2,G3) is active, an error should occur
	G2R2 and no axis end point was sent, an error should occur
	G0G2 more than 1 motion command on same line. No... bad gcoder!
	*/

	//We need to call assign planes even if the plane has not changed so the axis defines and arc define pointers get set.
	//Really this comes down to setting the 'defined' value. If I could call the class member 'get_defined' from the internal
	//struct then we wouldnt need to set the 'defined' value externally. HOwever if a plane changes from one block to the
	//next we must rotate the previous blocks axis values so that arc calculation come ot right.
	NGC_RS274::Interpreter::Processor::assign_planes(NGC_RS274::Interpreter::Processor::local_block);

	//Perform a  detailed error check on this block. If it passes, the other libraries should have no trouble processing it.
	ReturnValue = NGC_RS274::Interpreter::Processor::error_check_main();

	if (ReturnValue != NGC_RS274::Interpreter::Errors::OK)
	{
		NGC_RS274::Interpreter::Processor::local_block.reset();
		HasErrors = ReturnValue;
		return ReturnValue;
	}

	//This line interpreted without errors. Move the new values into 'plan_block' which will be updated for use by the calling method.
	memcpy(plan_block->g_group, NGC_RS274::Interpreter::Processor::local_block.g_group, COUNT_OF_G_CODE_GROUPS_ARRAY * sizeof(uint16_t));
	memcpy(&plan_block->g_code_defined_in_block, &NGC_RS274::Interpreter::Processor::local_block.g_code_defined_in_block, sizeof(uint16_t));
	memcpy(plan_block->m_group, NGC_RS274::Interpreter::Processor::local_block.m_group, COUNT_OF_M_CODE_GROUPS_ARRAY * sizeof(uint16_t));
	memcpy(&plan_block->m_code_defined_in_block, &NGC_RS274::Interpreter::Processor::local_block.m_code_defined_in_block, sizeof(uint16_t));
	memcpy(&plan_block->persisted_values, &NGC_RS274::Interpreter::Processor::local_block.persisted_values, sizeof(NGC_RS274::NGC_Binary_Block::s_persisted_values));
	memcpy(plan_block->block_word_values, NGC_RS274::Interpreter::Processor::local_block.block_word_values, COUNT_OF_BLOCK_WORDS_ARRAY * sizeof(float));
	memcpy(&plan_block->word_defined_in_block_A_Z, &NGC_RS274::Interpreter::Processor::local_block.word_defined_in_block_A_Z, sizeof(uint32_t));

	//memcpy(&plan_block->plane_axis, &NGC_RS274::Interpreter::Processor::local_block.plane_axis, sizeof(NGC_RS274::NGC_Binary_Block::s_plane_axis));
	//memcpy(&plan_block->arc_values, &NGC_RS274::Interpreter::Processor::local_block.arc_values, sizeof(NGC_RS274::NGC_Binary_Block::s_arc_values));


	//Clear the line data so its ready for next time.
	clear_line();

	return ReturnValue;
}

/*
First error check method. Determine the type of motion and perform detailed error checks for that
particular motion command.
*/
int NGC_RS274::Interpreter::Processor::error_check_main()
{

	int ReturnValue = NGC_RS274::Interpreter::Errors::OK;

	//float iAddress = 0;
	/*
	A single line or block of code could have multiple g codes in it. We check each gcode value set in the array
	and see if it has all the parameters it needs
	*/
	for (uint8_t i = 0; i < COUNT_OF_G_CODE_GROUPS_ARRAY; i++)
	{
		int gCode = NGC_RS274::Interpreter::Processor::local_block.g_group[i];

		switch (i)
		{
			case NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION: //<--G54,G55,G56,G57,G58,G59,G59.1,G59.2,G59.3
			{
				if (!BitTst(NGC_RS274::Interpreter::Processor::local_block.g_code_defined_in_block, NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION))
				break;

				//If cutter radius compensation compensation is on, selecting a new coordinate
				//system should produce an error
				if (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::CUTTER_RADIUS_COMPENSATION]
				> NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION)
				{
					return  NGC_RS274::Interpreter::Errors::COORDINATE_SETTING_INVALID_DURING_COMPENSATION;
				}
				//If G53 has been specified, clear the working offset value
				if (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] == NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM)
				{
					NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = 0;
				}
				else
				{
					//Value for 54,55,56 etc.. are checked in c_stager
				}

				break;
			}
			case NGC_RS274::Groups::G::CUTTER_RADIUS_COMPENSATION: //<--G40,G41,G42
			{
				//If cutter compensation was specified in the line, error check it.
				if (!BitTst(NGC_RS274::Interpreter::Processor::local_block.g_code_defined_in_block, NGC_RS274::Groups::G::CUTTER_RADIUS_COMPENSATION))
				break;

				ReturnValue = NGC_RS274::Interpreter::Processor::error_check_cutter_compensation();
				if (ReturnValue != NGC_RS274::Interpreter::Errors::OK)
				{
					return ReturnValue;
				}
				break;
			}

			case NGC_RS274::Groups::G::DISTANCE_MODE: //<--G90,G91
			//Nothing really to check for this group. It has no parameters
			break;
			case NGC_RS274::Groups::G::FEED_RATE_MODE: //<--G93,G94,G95
			//Nothing really to check for this group. It has no parameters
			break;
			case NGC_RS274::Groups::G::MOTION: //<--G0,G1,G02,G03,G38.2,G80,G81,G82,G83,G84,G85,G86,G87.G88.G89
			{
				//Was a motion command issued in the block or is the motion command block just 0 because it was never set?
				//If it wasn't defined ever, just break out of here. We are probably in setup mode.
				if (!BitTst(NGC_RS274::Interpreter::Processor::local_block.g_code_defined_in_block, NGC_RS274::Groups::G::MOTION))
				//&& ! c_buffer::block_buffer[plan_block].any_axis_was_defined())
				{
					break; //<--No motion command defined in the block this is just a default value. Carry on.
				}

				//If G80 is active, motion is canceled. If an axis is defined for motion, thats an error
				if (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::MOTION] == NGC_RS274::G_codes::MOTION_CANCELED
				&&  NGC_RS274::Interpreter::Processor::local_block.any_axis_was_defined())
				{
					return  NGC_RS274::Interpreter::Errors::MOTION_CANCELED_AXIS_VALUES_INVALID;
				}

				//Last check before we continue. If G80 is set, we just return from here. There is no motion mode
				if (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::MOTION] == NGC_RS274::G_codes::MOTION_CANCELED)
				return  NGC_RS274::Interpreter::Errors::OK;

				//We must have at least one axis defined for a motion command
				//Quick test here, if NO WORD values were set in the block at all, then we know there couldn't be an axis defined
				if (!NGC_RS274::Interpreter::Processor::local_block.any_axis_was_defined())
				return  NGC_RS274::Interpreter::Errors::NO_AXIS_DEFINED_FOR_MOTION;

				//When we are in time per unit feed rate mode (also known as inverse time), a feed rate must be set per line, for any motion command
				if (NGC_RS274::Interpreter::Processor::local_block.get_g_code_value_x(NGC_RS274::Groups::G::FEED_RATE_MODE) == NGC_RS274::G_codes::FEED_RATE_MINUTES_PER_UNIT_MODE
				&& !NGC_RS274::Interpreter::Processor::local_block.get_defined(('F')))
				return  NGC_RS274::Interpreter::Errors::NO_FEED_RATE_SPECIFIED_FOR_G93;

				//if G0 is active we don't need a feed rate. G1,G2,G3,all canned motions, do require a feed rate be active
				if (NGC_RS274::Interpreter::Processor::local_block.block_word_values['F' - 65] == 0 && gCode > 0)
				{
					//feedrate is now a persisted value in a block. IF it was EVER set it will get carried forward to the next block
					return  NGC_RS274::Interpreter::Errors::NO_FEED_RATE_SPECIFIED;
					/*
					If feed rate was not specified on this line, but was specified before
					we can re-use it
					*/
					//if(c_stager::persisted_values.feed_rate==0) //<--feed rate not previously set. Error.
					//{
					//return  NGC_RS274::Interpreter::Errors::NO_FEED_RATE_SPECIFIED;
					//}
				}

				//Normalize axis values to millimeters
				//normalize_distance_units('X');
				//normalize_distance_units('Y');
				//normalize_distance_units('Z');

				switch (gCode)
				{
					case NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CW:
					case NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW:
					{
						ReturnValue = error_check_arc();
						if (ReturnValue != NGC_RS274::Interpreter::Errors::OK)
						{
							return ReturnValue;
						}
						//normalize_distance_units('I');
						//normalize_distance_units('J');
						//normalize_distance_units('K');
						//normalize_distance_units('R');
					}
					break;
					case NGC_RS274::G_codes::MOTION_CANCELED:
					NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::MOTION] = NGC_RS274::G_codes::MOTION_CANCELED;
					break;
					case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING: //<--G81 drilling cycle
					case NGC_RS274::G_codes::CANNED_CYCLE_DRILLING_WITH_DWELL: //<--G82
					case NGC_RS274::G_codes::CANNED_CYCLE_PECK_DRILLING: //<--G83
					case NGC_RS274::G_codes::CANNED_CYCLE_RIGHT_HAND_TAPPING: //<--G84
					case NGC_RS274::G_codes::CANNED_CYCLE_BORING_NO_DWELL_FEED_OUT: //<--G85
					case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_RAPID_OUT: //<--G86
					case NGC_RS274::G_codes::CANNED_CYCLE_BACK_BORING: //<--G87
					case NGC_RS274::G_codes::CANNED_CYCLE_BORING_SPINDLE_STOP_MANUAL_OUT: //<--G88
					case NGC_RS274::G_codes::CANNED_CYCLE_BORING_DWELL_FEED_OUT: //<--G89
					{
						ReturnValue = error_check_canned_cycle();
						if (ReturnValue != NGC_RS274::Interpreter::Errors::OK)
						{
							return ReturnValue;
						}
						//normalize_distance_units('R');
						//normalize_distance_units(plan_block,'Q');
						//normalize_distance_units(plan_block,'L');

					}
					break;
					default:
					break;
				}

				break;
			}
			case NGC_RS274::Groups::G::NON_MODAL: //<--G4,G10,G28,G28.1,G30,G30.1,G53,G92,G92.1,G92.2,G92.3
			{
				if (!BitTst(NGC_RS274::Interpreter::Processor::local_block.g_code_defined_in_block, NGC_RS274::Groups::G::NON_MODAL))
				break;
				ReturnValue = error_check_non_modal();
				if (ReturnValue != NGC_RS274::Interpreter::Errors::OK)
				{
					return ReturnValue;
				}

				break;
			}

			case NGC_RS274::Groups::G::PATH_CONTROL_MODE: //<--G61,G61.1,G64
			//Nothing really to check for this group. It has no parameters
			break;
			case NGC_RS274::Groups::G::PLANE_SELECTION: //<--G17,G18,G19
			{
				if (!BitTst(NGC_RS274::Interpreter::Processor::local_block.g_code_defined_in_block, NGC_RS274::Groups::G::PLANE_SELECTION))
				break;

				NGC_RS274::Interpreter::Processor::error_check_plane_select(NGC_RS274::Interpreter::Processor::local_block);

				//Nothing really to check for this group. It has no parameters
				break;
			}
			case NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE: //<--G98,G99
			//Nothing really to check for this group. It has no parameters
			break;
			case NGC_RS274::Groups::G::TOOL_LENGTH_OFFSET: //<--G43,G44,G49
			{
				if (!BitTst(NGC_RS274::Interpreter::Processor::local_block.g_code_defined_in_block, NGC_RS274::Groups::G::TOOL_LENGTH_OFFSET))
				break;

				ReturnValue = error_check_tool_length_offset();
				if (ReturnValue != NGC_RS274::Interpreter::Errors::OK)
				{
					return ReturnValue;
				}
				break;
			}
			case NGC_RS274::Groups::G::UNITS: //<--G20,G21
			//Nothing really to check for this group. It has no parameters
			break;

			default:
			break;
		}
	}
	return  NGC_RS274::Interpreter::Errors::OK;
}

int NGC_RS274::Interpreter::Processor::error_check_plane_select(NGC_RS274::NGC_Binary_Block &plane_block)
{
	//Use the switch case to assign the named values.
	switch (plane_block.g_group[NGC_RS274::Groups::G::PLANE_SELECTION])
	{
		case NGC_RS274::G_codes::XY_PLANE_SELECTION:
		{
			plane_block.plane_axis.horizontal_axis.name = 'X';
			plane_block.plane_axis.vertical_axis.name = 'Y';
			plane_block.plane_axis.normal_axis.name = 'Z';
			plane_block.plane_axis.plane_error = NGC_RS274::Interpreter::Errors::MISSING_CIRCLE_AXIS_XY;

			plane_block.arc_values.horizontal_center.name = 'I';
			plane_block.arc_values.vertical_center.name = 'J';
			plane_block.arc_values.plane_error = NGC_RS274::Interpreter::Errors::MISSING_CIRCLE_OFFSET_IJ;
		}
		break;
		case NGC_RS274::G_codes::XZ_PLANE_SELECTION:
		{
			plane_block.plane_axis.horizontal_axis.name = 'X';
			plane_block.plane_axis.vertical_axis.name = 'Z';
			plane_block.plane_axis.normal_axis.name = 'Y';
			plane_block.plane_axis.plane_error = NGC_RS274::Interpreter::Errors::MISSING_CIRCLE_AXIS_XZ;
			
			plane_block.arc_values.horizontal_center.name = 'I';
			plane_block.arc_values.vertical_center.name = 'K';
			plane_block.arc_values.plane_error = NGC_RS274::Interpreter::Errors::MISSING_CIRCLE_OFFSET_IK;
		}
		break;
		case NGC_RS274::G_codes::YZ_PLANE_SELECTION:
		{
			plane_block.plane_axis.horizontal_axis.name = 'Y';
			plane_block.plane_axis.vertical_axis.name = 'Z';
			plane_block.plane_axis.normal_axis.name = 'X';
			plane_block.plane_axis.plane_error = NGC_RS274::Interpreter::Errors::MISSING_CIRCLE_AXIS_YZ;
			
			plane_block.arc_values.horizontal_center.name = 'J';
			plane_block.arc_values.vertical_center.name = 'K';
			plane_block.arc_values.plane_error = NGC_RS274::Interpreter::Errors::MISSING_CIRCLE_OFFSET_JK;
		}
		break;
	}
	//After value names are assigned, the plane and arc data can be set automatically

	NGC_RS274::Interpreter::Processor::assign_planes(plane_block);

	return  NGC_RS274::Interpreter::Errors::OK;
}

void NGC_RS274::Interpreter::Processor::assign_planes(NGC_RS274::NGC_Binary_Block &plane_block)
{
	/*
	TODO:If planes change we should probably rotate the values for the previous block since
	we use those previous values in some places, such as arc center calculations.
	*/
	if (stager_block->get_g_code_value_x(NGC_RS274::Groups::G::PLANE_SELECTION)
	!= plane_block.get_g_code_value_x(NGC_RS274::Groups::G::PLANE_SELECTION))
	{
		//Set the planes to match
		stager_block->set_group_value(NGC_RS274::Groups::G::PLANE_SELECTION,
		local_block.get_g_code_value_x(NGC_RS274::Groups::G::PLANE_SELECTION));

		//We know that the planes have changed from the previous block. Rotate the previous block axis values.
		//This will chain back into this function and also re-assign the pointers to the previous block arc
		//and plane axis values.
		error_check_plane_select(*stager_block);
	}

	//To simplify planes, set defined, and assign to the pointed value for this plane
	plane_block.plane_axis.horizontal_axis.defined = plane_block.get_defined(local_block.plane_axis.horizontal_axis.name);
	plane_block.plane_axis.vertical_axis.defined = plane_block.get_defined(local_block.plane_axis.vertical_axis.name);
	plane_block.plane_axis.normal_axis.defined = plane_block.get_defined(local_block.plane_axis.normal_axis.name);
	plane_block.plane_axis.horizontal_axis.value = &plane_block.block_word_values[local_block.plane_axis.horizontal_axis.name - 65];
	plane_block.plane_axis.vertical_axis.value = &plane_block.block_word_values[local_block.plane_axis.vertical_axis.name - 65];
	plane_block.plane_axis.normal_axis.value = &plane_block.block_word_values[local_block.plane_axis.normal_axis.name - 65];

	//To simplify arc, set defined, and assign to the pointed value for the arc
	plane_block.arc_values.horizontal_center.defined = plane_block.get_defined(local_block.arc_values.horizontal_center.name);
	plane_block.arc_values.vertical_center.defined = plane_block.get_defined(local_block.arc_values.vertical_center.name);
	plane_block.arc_values.horizontal_center.value = &plane_block.block_word_values[local_block.arc_values.horizontal_center.name - 65];
	plane_block.arc_values.vertical_center.value = &plane_block.block_word_values[local_block.arc_values.vertical_center.name - 65];
}

/*
If an arc (g02/g03) command was specified, perform detailed parameter check that applies only to arcs.
*/
int NGC_RS274::Interpreter::Processor::error_check_arc()
{
	/*The rules :
	1. Center format arcs should not be more than 180 degrees.
	2. Center format arcs CAN have an end point the same as the begin point
	3. Do not assume a previous IJK value can be used again, if it was already set
	for a previous arc.
	4. Both offset values are not required, but one is.
	5. We do not require both axis' be defined on the end point, but one of them must be
	6. If only one axis is defined, assume the missing axis value is the current location of the axis
	*/

	/*My understanding is the IJK offset values have to be set on the line. If I/J/K was already set and it is zero
	we should not assume that 0 is correct this time. So we check the bit flags to see if I or J or K was set

	It is preferable to use center format arcs. If I/J/K values are set AND the R value is set, I am deferring to
	a center format instead of the radius format. It is less error prone due to round off.
	*/

	//Must have horizontal or vertical but we don't have to have both
	if (!NGC_RS274::Interpreter::Processor::local_block.plane_axis.horizontal_axis.defined && !NGC_RS274::Interpreter::Processor::local_block.plane_axis.vertical_axis.defined)
	return NGC_RS274::Interpreter::Processor::local_block.plane_axis.plane_error;

	//Was radius specified?
	if (!NGC_RS274::Interpreter::Processor::local_block.get_defined('R'))
	{
		//Must have H or V, but we don't have to have both
		if (!NGC_RS274::Interpreter::Processor::local_block.arc_values.horizontal_center.defined && !NGC_RS274::Interpreter::Processor::local_block.arc_values.vertical_center.defined)
		return NGC_RS274::Interpreter::Processor::local_block.arc_values.plane_error;

		return error_check_center_format_arc();
	}
	else
	{
		return error_check_radius_format_arc();
	}
}

int NGC_RS274::Interpreter::Processor::error_check_center_format_arc()
{
	// Arc radius from center to target
	float target_r = hypot_f
	(-*NGC_RS274::Interpreter::Processor::local_block.plane_axis.horizontal_axis.value,
	-*NGC_RS274::Interpreter::Processor::local_block.plane_axis.vertical_axis.value);

	// Compute arc radius for mc_arc. Defined from current location to center.
	*NGC_RS274::Interpreter::Processor::local_block.arc_values.Radius =
	hypot_f(*NGC_RS274::Interpreter::Processor::local_block.plane_axis.horizontal_axis.value
	, *NGC_RS274::Interpreter::Processor::local_block.plane_axis.vertical_axis.value);

	// Compute difference between current location and target radii for final error-checks.
	float delta_r = fabs(target_r - *NGC_RS274::Interpreter::Processor::local_block.arc_values.Radius);
	//If the radius difference is greater than .005 mm we MAY, throw an error, or we MAY not
	if (delta_r > 0.005)
	{
		//If the radius difference is more than .5mm we error
		if (delta_r > 0.5)
		return  NGC_RS274::Interpreter::Errors::CENTER_FORMAT_ARC_RADIUS_ERROR_EXCEEDS_005; // [Arc definition error] > 0.5mm

		//If the radius difference is more than .001mm AND its a small arc, we error
		if (delta_r > (0.001 * *NGC_RS274::Interpreter::Processor::local_block.arc_values.Radius))
		return  NGC_RS274::Interpreter::Errors::CENTER_FORMAT_ARC_RADIUS_ERROR_EXCEEDS_PERCENTAGE; // [Arc definition error] > 0.005mm AND 0.1% radius
	}

	return  NGC_RS274::Interpreter::Errors::OK;
}

/*
These arc center calculations are pretty simple. I do not know who created it originaly
but this is a near verbatim copy of it from the Grbl control 'gcode.c' file
*/
int NGC_RS274::Interpreter::Processor::error_check_radius_format_arc()
{
	// Calculate the change in position along each selected axis
	float horizontal_delta = *NGC_RS274::Interpreter::Processor::local_block.plane_axis.horizontal_axis.value - *NGC_RS274::Interpreter::Processor::stager_block->plane_axis.horizontal_axis.value;
	float vertical_delta = *NGC_RS274::Interpreter::Processor::local_block.plane_axis.vertical_axis.value - *NGC_RS274::Interpreter::Processor::stager_block->plane_axis.vertical_axis.value;

	// First, use h_x2_div_d to compute 4*h^2 to check if it is negative or r is smaller
	// than d. If so, the sqrt of a negative number is complex and error out.
	float h_x2_div_d = 4.0 * NGC_RS274::Interpreter::Processor::square(*NGC_RS274::Interpreter::Processor::local_block.arc_values.Radius)
	- NGC_RS274::Interpreter::Processor::square(horizontal_delta)
	- NGC_RS274::Interpreter::Processor::square(vertical_delta);

	if (h_x2_div_d < 0)
	return  NGC_RS274::Interpreter::Errors::RADIUS_FORMAT_ARC_RADIUS_LESS_THAN_ZERO; // [Arc radius error]

	// Finish computing h_x2_div_d.
	h_x2_div_d = -sqrt(h_x2_div_d) / hypot_f(horizontal_delta, vertical_delta); // == -(h * 2 / d)
	// Invert the sign of h_x2_div_d if the circle is counter clockwise (see sketch below)
	if (NGC_RS274::Interpreter::Processor::local_block.get_g_code_value_x(NGC_RS274::Groups::G::MOTION) == NGC_RS274::G_codes::CIRCULAR_INTERPOLATION_CCW)
	h_x2_div_d = -h_x2_div_d;


	// Negative R is g-code-alese for "I want a circle with more than 180 degrees of travel" (go figure!),
	// even though it is advised against ever generating such circles in a single line of g-code. By
	// inverting the sign of h_x2_div_d the center of the circles is placed on the opposite side of the line of
	// travel and thus we get the inadvisable long arcs as prescribed.
	if (*NGC_RS274::Interpreter::Processor::local_block.arc_values.Radius < 0)
	{
		h_x2_div_d = -h_x2_div_d;
		*NGC_RS274::Interpreter::Processor::local_block.arc_values.Radius
		= -*NGC_RS274::Interpreter::Processor::local_block.arc_values.Radius; // Finished with r. Set to positive for mc_arc
	}
	// Complete the operation by calculating the actual center of the arc
	*NGC_RS274::Interpreter::Processor::local_block.arc_values.horizontal_center.value = (0.5 * (horizontal_delta - (vertical_delta * h_x2_div_d)));
	*NGC_RS274::Interpreter::Processor::local_block.arc_values.vertical_center.value = (0.5 * (vertical_delta + (horizontal_delta * h_x2_div_d)));

	return  NGC_RS274::Interpreter::Errors::OK;
}

int NGC_RS274::Interpreter::Processor::error_check_non_modal()
{

	float iAddress = 0;
	//Check individual gcodes for their specific values
	switch (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::NON_MODAL])
	{
		case NGC_RS274::G_codes::G10_PARAM_WRITE: //<-G10
		{
			/*
			L1 - Tool offset,radius,orientation settings - requires P,Q,R,X,W,Z
			L10 - Tool offset,radius,orientation settings calculated - requires P,Q,R,X,W,Z
			L2 - Coordinate system origin setting - requires P,R,any axis
			L20 - Coordinate system origin setting calculated - requires P,R,any axis
			*/
			//G10 requires an L, P and at least one axis word value

			if (!NGC_RS274::Interpreter::Processor::local_block.get_value_defined('L', iAddress))
			{
				return  NGC_RS274::Interpreter::Errors::COORDINATE_SETTING_MISSING_L_VALUE;
			}
			switch ((uint8_t)iAddress)
			{
				//If L was 1 or 10 then we require an axis word for tool table update
				case 1:
				case 10:
				{
					//The P value should be defined AND be an integer between 0 and 255
					if (!NGC_RS274::Interpreter::Processor::local_block.get_value_defined('P', iAddress))
					{
						return  NGC_RS274::Interpreter::Errors::TOOL_OFFSET_SETTING_MISSING_P_VALUE;
					}
					if (iAddress < 0 || iAddress>255)
					{
						return  NGC_RS274::Interpreter::Errors::TOOL_OFFSET_SETTING_P_VALUE_OUT_OF_RANGE;
					}
					if (!NGC_RS274::Interpreter::Processor::local_block.any_axis_was_defined())
					{
						return  NGC_RS274::Interpreter::Errors::COORDINATE_SETTING_MISSING_AXIS_VALUE;
					}
					break;
				}
				//If L was 2 or 20 then we require an axis word and a P word for coordinate system settings
				case 2:
				case 20:
				{
					//The P value should be defined AND be an integer between 1 and 9
					if (!NGC_RS274::Interpreter::Processor::local_block.get_value_defined('P', iAddress))
					{
						return  NGC_RS274::Interpreter::Errors::COORDINATE_SETTING_MISSING_P_VALUE;
					}
					if (iAddress < 0 || iAddress>9)
					{
						return  NGC_RS274::Interpreter::Errors::COORDINATE_SETTING_P_VALUE_OUT_OF_RANGE;
					}
					if (!NGC_RS274::Interpreter::Processor::local_block.any_axis_was_defined())
					{
						return  NGC_RS274::Interpreter::Errors::COORDINATE_SETTING_MISSING_AXIS_VALUE;
					}
					break;
				}
				default:
				{
					return  NGC_RS274::Interpreter::Errors::COORDINATE_SETTING_L_VALUE_OUT_OF_RANGE;
					break;
				}
			}

			break;
		}
	}

	return  NGC_RS274::Interpreter::Errors::OK;
}

/*
If a canned cycle (g81-g89) command was specified, perform detailed parameter check that applies
only to canned cycles.
*/
int NGC_RS274::Interpreter::Processor::error_check_canned_cycle()
{
	//Verify there is a retraction mode specified
	if (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] != NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R
	&& NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] != NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_Z)
	{
		return  NGC_RS274::Interpreter::Errors::CAN_CYCLE_RETURN_MODE_UNDEFINED;
	}

	//Check for rotational axis words. Those should not be used during canned cycles. RS274 standards page 30 section 3.5.16
	//Technically these CAN be specified, but their values have to be the same as their resting state. In other words if they
	//are specified they CANNOT cause movement
	if (BitTst(NGC_RS274::Interpreter::Processor::local_block.word_defined_in_block_A_Z, A_WORD_BIT))
	return  NGC_RS274::Interpreter::Errors::CAN_CYLCE_ROTATIONAL_AXIS_A_DEFINED;
	if (BitTst(NGC_RS274::Interpreter::Processor::local_block.word_defined_in_block_A_Z, B_WORD_BIT))
	return  NGC_RS274::Interpreter::Errors::CAN_CYLCE_ROTATIONAL_AXIS_B_DEFINED;
	if (BitTst(NGC_RS274::Interpreter::Processor::local_block.word_defined_in_block_A_Z, C_WORD_BIT))
	return  NGC_RS274::Interpreter::Errors::CAN_CYLCE_ROTATIONAL_AXIS_C_DEFINED;

	//Cutter radius compensation cannot be active in a canned cycle.
	if (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::CUTTER_RADIUS_COMPENSATION] != NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION)
	return  NGC_RS274::Interpreter::Errors::CAN_CYLCE_CUTTER_RADIUS_COMPENSATION_ACTIVE;

	//if L word specified it must be a positive integer > 1 (L is not required, only use it to repeat a cycle at the present location)
	if (BitTst(NGC_RS274::Interpreter::Processor::local_block.word_defined_in_block_A_Z, L_WORD_BIT))
	{
		//L was defined, make sure it >1 and integer
		if (NGC_RS274::Interpreter::Processor::local_block.block_word_values['L' - 65] < 2)
		return  NGC_RS274::Interpreter::Errors::CAN_CYCLE_WORD_L_LESS_THAN_2;
		//L was > 1, make sure it is an int
		//first cast the value to an int, then subtract the float value multiplied by 100. This will give the decimal only result as a whole number
		int mantissa = (NGC_RS274::Interpreter::Processor::local_block.block_word_values['L' - 65] - (int)NGC_RS274::Interpreter::Processor::local_block.block_word_values['L' - 65]) * 100;
		if (mantissa > 0)
		return  NGC_RS274::Interpreter::Errors::CAN_CYCLE_WORD_L_NOT_POS_INTEGER;
	}

	//Make sure at least one linear axis was defined on the line

	if (!NGC_RS274::Interpreter::Processor::local_block.any_linear_axis_was_defined())
	return  NGC_RS274::Interpreter::Errors::CAN_CYCLE_LINEAR_AXIS_UNDEFINED;

	//if Using R retract method (G99), make sure R was set.
	if (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] == NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R)
	{
		if (!BitTst(NGC_RS274::Interpreter::Processor::local_block.word_defined_in_block_A_Z, R_WORD_BIT) && NGC_RS274::Interpreter::Processor::local_block.get_value('R') == 0)
		return  NGC_RS274::Interpreter::Errors::CAN_CYCLE_MISSING_R_VALUE;
		//R value was set and we are in R retract mode. We cannot have an R value that is less than the current Z
		//(or whatever the retract axis is in the current plane). We cannot check that in the interpreter though.
		//The interpreter has no idea where the machine is, so we will check this value in the machine object class.

	}

	//if dwell cycle make sure P is set
	if (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::MOTION] == NGC_RS274::G_codes::CANNED_CYCLE_DRILLING_WITH_DWELL)
	{
		if (!BitTst(NGC_RS274::Interpreter::Processor::local_block.word_defined_in_block_A_Z, P_WORD_BIT) && NGC_RS274::Interpreter::Processor::local_block.get_value('P') == 0)
		return  NGC_RS274::Interpreter::Errors::CAN_CYCLE_MISSING_P_VALUE;
	}

	//if neither axis defined, it is an error
	if (!NGC_RS274::Interpreter::Processor::local_block.plane_axis.horizontal_axis.defined && !NGC_RS274::Interpreter::Processor::local_block.plane_axis.vertical_axis.defined)
	return  NGC_RS274::Interpreter::Errors::CAN_CYCLE_LINEAR_AXIS_UNDEFINED;

	return  NGC_RS274::Interpreter::Errors::OK;
}

int NGC_RS274::Interpreter::Processor::error_check_tool_length_offset()
{
	/*
	H - an integer value between 1 and 99
	*/
	float iAddress = 0;
	//See if P was defined. Can we allow P value for tool length?
	//if (!NGC_RS274::Interpreter::Processor::local_block.get_value_defined('P'))
	{
		if (!NGC_RS274::Interpreter::Processor::local_block.get_value_defined('H', iAddress))
		{
			//TODO:See if this is a strict error. IF H is missing can we assume zero?
			return  NGC_RS274::Interpreter::Errors::TOOL_OFFSET_MISSING_H_VALUE;
		}

		//See if the H value is within permitted range
		if (iAddress<0 || iAddress>COUNT_OF_TOOL_TABLE - 1)
		{
			return  NGC_RS274::Interpreter::Errors::TOOL_OFFSET_H_VALUE_OUT_OF_RANGE;
		}
	}
	//If we get here, the H was specified and valid. We can carry on.
	return  NGC_RS274::Interpreter::Errors::OK;
}

int NGC_RS274::Interpreter::Processor::error_check_cutter_compensation()
{

	float iAddress = 0;

	//TODO: add in the D and P value checks here. Create a  var for active offset amount in the interpreter.
	//See if cutter radius comp has ever been set. If it is active we must be in the XY plane to use it

	/*
	Technically cutter radius compensation can only be active for the g17 plane on a mill
	I return an error here only because the standard says so, but....
	Compensation (as it is written today) will work in any coordinate plane.
	When a coordinate code is issued (g17,18,19) it sets the pointer values in the block
	for X,Y,Z in which ever order is needed for horizontal,vertical, and normal axis.
	in g17 x is horizontal, y is vertical, and z is normal. Compensation uses the plane
	agnostic values, so really it doest matter. It may give back variable names of X/Y
	but it will calculate offset based on the related planes.

	*/
	if (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::PLANE_SELECTION] != NGC_RS274::G_codes::XY_PLANE_SELECTION)
	return  NGC_RS274::Interpreter::Errors::CUTTER_RADIUS_COMP_NOT_XY_PLANE;

	if (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::MOTION] == NGC_RS274::G_codes::MOTION_CANCELED)
	return  NGC_RS274::Interpreter::Errors::CUTTER_RADIUS_COMP_WHEN_MOTION_CANCELED;

	//If cutter radius compensation was already active, it cannot be set active again, or change sides
	//It must be turned off, and back on again
	if (BitTst(NGC_RS274::Interpreter::Processor::local_block.g_code_defined_in_block, NGC_RS274::Groups::G::CUTTER_RADIUS_COMPENSATION))
	{
		//Since cutter comp was defined in the block, we must determine if it was already on
		//by looking in the stagers g code group states.
		if (stager_block->g_group[NGC_RS274::Groups::G::CUTTER_RADIUS_COMPENSATION] > NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION
		&& NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::CUTTER_RADIUS_COMPENSATION] > NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION)
		return  NGC_RS274::Interpreter::Errors::CUTTER_RADIUS_COMP_ALREADY_ACTIVE;
	}

	//Compensation was off and we are turning it on.
	if ((local_block.g_group[NGC_RS274::Groups::G::CUTTER_RADIUS_COMPENSATION] > NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION))
	{

		//See if P was defined.
		if (!NGC_RS274::Interpreter::Processor::local_block.get_value_defined('P', iAddress))
		{
			//P was not defined, see if D was
			if (!NGC_RS274::Interpreter::Processor::local_block.get_value_defined('D', iAddress))
			{
				//P and D were both left out. naughty naughty
				return  NGC_RS274::Interpreter::Errors::CUTTER_RADIUS_COMP_MISSING_D_P_VALUE;
			}

			//See if the D value is within permitted range
			if (iAddress<0 || iAddress>COUNT_OF_TOOL_TABLE - 1)
			{
				return  NGC_RS274::Interpreter::Errors::CUTTER_RADIUS_COMP_D_VALUE_OUT_OF_RANGE;
			}
		}
		else
		{
			//See if D word defined, since we know P already was
			if (NGC_RS274::Interpreter::Processor::local_block.get_defined('D'))
			{
				return  NGC_RS274::Interpreter::Errors::CUTTER_RADIUS_COMP_D_P_BOTH_ASSIGNED;
			}
		}
	}

	return  NGC_RS274::Interpreter::Errors::OK;
}

/*
Loops through the array data handed to the ProcessLine() method and splits it into g code chunks
*/
int NGC_RS274::Interpreter::Processor::parse_values()
{

	/*Line must be primed before we get to here. It has to be pointing to a valid char array
	*/
	int ReturnValue = NGC_RS274::Interpreter::Errors::OK;
	//memset(Line, 0, 256);//<--Clear the line array
	//char *line_pointer = Line;//<--creating a pointer to an array
	//to simplify building the array data in the loops
	*NGC_RS274::Interpreter::Processor::Line = toupper(*NGC_RS274::Interpreter::Processor::Line);


	if (*NGC_RS274::Interpreter::Processor::Line == 0)
	return  NGC_RS274::Interpreter::Errors::LINE_CONTAINS_NO_DATA;
	else if (*NGC_RS274::Interpreter::Processor::Line <'A' || *NGC_RS274::Interpreter::Processor::Line >'Z')
	{
		//Skip any erroneous CR or LF.
		while (*NGC_RS274::Interpreter::Processor::Line == LF || *NGC_RS274::Interpreter::Processor::Line == CR)
		{
			NGC_RS274::Interpreter::Processor::Line++;
		}
		
		if (*NGC_RS274::Interpreter::Processor::Line <'A' || *NGC_RS274::Interpreter::Processor::Line >'Z')
		{
			//Check to see if this is a block skip, comment, etc.
			if (*NGC_RS274::Interpreter::Processor::Line != '/' && *NGC_RS274::Interpreter::Processor::Line != '(')
			{
				c_processor::host_serial.print_string("bad char:");
				c_processor::host_serial.print_int32( *NGC_RS274::Interpreter::Processor::Line);
				
				return  NGC_RS274::Interpreter::Errors::WORD_VALUE_TYPE_INVALID;
			}
		}
		
	}

	while (1)
	{
		//Is this a letter between A and Z
		if (*NGC_RS274::Interpreter::Processor::Line >= 'A' && *NGC_RS274::Interpreter::Processor::Line <= 'Z')
		{
			char Word = *NGC_RS274::Interpreter::Processor::Line;
			NGC_RS274::Interpreter::Processor::Line++;
			*NGC_RS274::Interpreter::Processor::Line = toupper(*NGC_RS274::Interpreter::Processor::Line);
			char _address[10];
			memset(_address, 0, 10);
			uint8_t i = 0;
			//move to first byte of address value
			while (1)
			{
				//If this is a letter, or a CR, break out of this inner while
				if ((*NGC_RS274::Interpreter::Processor::Line >= 'A' 
				&& *NGC_RS274::Interpreter::Processor::Line <= 'Z')
				|| *NGC_RS274::Interpreter::Processor::Line == 13
				|| *NGC_RS274::Interpreter::Processor::Line == 0)
				break;
				if ((*NGC_RS274::Interpreter::Processor::Line < '0'
				|| *NGC_RS274::Interpreter::Processor::Line>'9')
				&& (*NGC_RS274::Interpreter::Processor::Line != '.'
				&& *NGC_RS274::Interpreter::Processor::Line != '-'))
				{
					while (*NGC_RS274::Interpreter::Processor::Line != 13
					&& *NGC_RS274::Interpreter::Processor::Line != 0)
					{
						*NGC_RS274::Interpreter::Processor::Line++;
					}

					return  NGC_RS274::Interpreter::Errors::ADDRESS_VALUE_NOT_NUMERIC;
				}
				_address[i] = *NGC_RS274::Interpreter::Processor::Line; i++;


				NGC_RS274::Interpreter::Processor::Line++;
				*NGC_RS274::Interpreter::Processor::Line = toupper(*NGC_RS274::Interpreter::Processor::Line);
			}

			ReturnValue = group_word(Word, evaluate_address(_address));
			//If there was an error during line processing don't bother finishing the line, just bale.
			if (ReturnValue != NGC_RS274::Interpreter::Errors::OK)
			return ReturnValue;
		}
		//If this is a CR break out of the outer while
		if (*NGC_RS274::Interpreter::Processor::Line == 13
		|| *NGC_RS274::Interpreter::Processor::Line == 10
		|| *NGC_RS274::Interpreter::Processor::Line == 0)
		break;
	}
	return ReturnValue;
}

/*
Convert an address word into a real number if needed
*/
//TODO:This is not fully implemented for formulas, and parameters
float NGC_RS274::Interpreter::Processor::evaluate_address(char Data[])
{
	/*An address can have a real number, a parameter, or even a mathematical equation.
	This is not implemented fully. It only processes a REAL NUMBER at this time.
	When time permits I would like this to load parameters, equations, etc... and convert
	them into real numbers.
	*/

	return atof(Data);

}

/*
Add the word value to the block. G/M words are added to separated block arrays respectively.
*/
int NGC_RS274::Interpreter::Processor::group_word(char Word, float Address)
{
	//host_serial.Write(Word);
	//host_serial.Write("\r");
	//host_serial.Write_nf(Address);
	//host_serial.Write("\r");
	switch (Word)
	{
		case 'G': //<--Process words for G (G10,G20,G91, etc..)
		return _gWord(Address);
		break;
		case 'M': //<--Process words for M (M3,M90,M5, etc..)
		return _mWord(Address);
		break;
		default:
		return _pWord(Word, Address); //<--Process words for Everything else (X__,Y__,Z__,I__,D__, etc..)
		break;
	}

	return  NGC_RS274::Interpreter::Errors::INTERPRETER_DOES_NOT_UNDERSTAND_CHARACTER_IN_BLOCK;
}

/*
Assign the corresponding value to a Group number for an Address
*/
int NGC_RS274::Interpreter::Processor::_gWord(float Address)
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
		ngc_working_group = NGC_RS274::Groups::G::MOTION;
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::MOTION] = (iAddress);
		break;

		case NGC_RS274::G_codes::G10_PARAM_WRITE: //<-G10
		ngc_working_group = NGC_RS274::Groups::G::NON_MODAL;//<-G4,G10,G28,G30,G53,G92,92.2,G92.3
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::NON_MODAL] = (iAddress);
		break;

		case NGC_RS274::G_codes::RECTANGULAR_COORDINATE_SYSTEM: //<-G15
		case NGC_RS274::G_codes::POLAR_COORDINATE_SYSTEM: //<-G16
		ngc_working_group = NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION;
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::RECTANGLAR_POLAR_COORDS_SELECTION] = (iAddress);
		break;

		case NGC_RS274::G_codes::XY_PLANE_SELECTION: //<-G17

		ngc_working_group = NGC_RS274::Groups::G::PLANE_SELECTION;
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::PLANE_SELECTION] = (iAddress);
		NGC_RS274::Interpreter::Processor::error_check_plane_select(NGC_RS274::Interpreter::Processor::local_block);
		break;

		case NGC_RS274::G_codes::XZ_PLANE_SELECTION: //<-G18

		ngc_working_group = NGC_RS274::Groups::G::PLANE_SELECTION;
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::PLANE_SELECTION] = (iAddress);
		NGC_RS274::Interpreter::Processor::error_check_plane_select(NGC_RS274::Interpreter::Processor::local_block);
		break;

		case NGC_RS274::G_codes::YZ_PLANE_SELECTION: //<-G19

		ngc_working_group = NGC_RS274::Groups::G::PLANE_SELECTION;
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::PLANE_SELECTION] = (iAddress);
		NGC_RS274::Interpreter::Processor::error_check_plane_select(NGC_RS274::Interpreter::Processor::local_block);
		break;

		case NGC_RS274::G_codes::INCH_SYSTEM_SELECTION: //<-G20
		case NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION: //<-G21
		ngc_working_group = NGC_RS274::Groups::G::UNITS;
		//If units are changing, update the feed rate in the interpreter
		if (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::UNITS] != (iAddress))
		{
			//currently we are in inches and going to mm
			if (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::UNITS]
			== NGC_RS274::G_codes::INCH_SYSTEM_SELECTION)
			{
				NGC_RS274::Interpreter::Processor::local_block.set_value
				('F', NGC_RS274::Interpreter::Processor::local_block.get_value('F')*25.4);
			}
			//currently we are in mm and going to inches
			if (NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::UNITS]
			== NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION)
			{
				NGC_RS274::Interpreter::Processor::local_block.set_value
				('F', NGC_RS274::Interpreter::Processor::local_block.get_value('F') / 25.4);
			}
		}
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::UNITS] = (iAddress);
		break;

		case NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION: //<-G40
		case NGC_RS274::G_codes::START_CUTTER_RADIUS_COMPENSATION_LEFT: //<-G41
		case NGC_RS274::G_codes::START_CUTTER_RADIUS_COMPENSATION_RIGHT: //<-G42
		ngc_working_group = NGC_RS274::Groups::G::CUTTER_RADIUS_COMPENSATION;
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::CUTTER_RADIUS_COMPENSATION] = (iAddress);
		break;


		case NGC_RS274::G_codes::MOTION_IN_MACHINE_COORDINATE_SYSTEM: //<-G53
		ngc_working_group = NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION;
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = (iAddress);
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
		ngc_working_group = NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION;
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::COORDINATE_SYSTEM_SELECTION] = (iAddress);
		break;

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
		ngc_working_group = NGC_RS274::Groups::G::MOTION;
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::MOTION] = (iAddress);
		break;

		case NGC_RS274::G_codes::ABSOLUTE_DISANCE_MODE: //<-G90
		case NGC_RS274::G_codes::INCREMENTAL_DISTANCE_MODE: //<-G91
		ngc_working_group = NGC_RS274::Groups::G::DISTANCE_MODE;
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::DISTANCE_MODE] = (iAddress);
		break;

		case NGC_RS274::G_codes::FEED_RATE_MINUTES_PER_UNIT_MODE: //<-G93
		case NGC_RS274::G_codes::FEED_RATE_UNITS_PER_MINUTE_MODE: //<-G94
		case NGC_RS274::G_codes::FEED_RATE_UNITS_PER_ROTATION: //<-G95
		ngc_working_group = NGC_RS274::Groups::G::FEED_RATE_MODE;
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::FEED_RATE_MODE] = (iAddress);
		break;

		case NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_R: //<-G99
		case NGC_RS274::G_codes::CANNED_CYCLE_RETURN_TO_Z: //<-G98
		ngc_working_group = NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE;
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::RETURN_MODE_CANNED_CYCLE] = (iAddress);
		break;

		case NGC_RS274::G_codes::CANCEL_TOOL_LENGTH_OFFSET: //<-G49
		case NGC_RS274::G_codes::POSITIVE_TOOL_LENGTH_OFFSET: //<-G43
		case NGC_RS274::G_codes::USE_TOOL_LENGTH_OFFSET_FOR_TRANSIENT_TOOL: //<-G43.1
		case NGC_RS274::G_codes::NEGATIVE_TOOL_LENGTH_OFFSET: //<-G44

		ngc_working_group = NGC_RS274::Groups::G::TOOL_LENGTH_OFFSET;
		NGC_RS274::Interpreter::Processor::local_block.g_group[NGC_RS274::Groups::G::TOOL_LENGTH_OFFSET] = (iAddress);
		break;

		default:
		return  NGC_RS274::Interpreter::Errors::INTERPRETER_DOES_NOT_UNDERSTAND_G_WORD_VALUE; //<-- -1
		break;
	}

	/*
	See if we have already processed a g command from this group.
	We can make the error more meaningful if we tell it WHICH group
	was specified more than once.
	*/
	if (BitTst(NGC_RS274::Interpreter::Processor::local_block.g_code_defined_in_block, ngc_working_group))
	/*
	Since _working_g_group is the 'group' number we can add it to the base error
	value and give the user a more specific error so they know what needs
	to be corrected
	*/
	return  NGC_RS274::Interpreter::Errors::G_CODE_GROUP_NON_MODAL_ALREADY_SPECIFIED + ngc_working_group; //<--Start with group 0 and add the Group to it. Never mind, _working_g_group IS the group number

	/*
	If we havent already returned from a duplicate group being specified, set the bit flag for this
	gcode group. This will get checked if this method is called again but if a gcode for the same
	group is on the line again, the logic above will catch it and return an error
	*/
	NGC_RS274::Interpreter::Processor::local_block.g_code_defined_in_block =
	BitSet(NGC_RS274::Interpreter::Processor::local_block.g_code_defined_in_block, ngc_working_group);
	return  NGC_RS274::Interpreter::Errors::OK;
}

/*
Assign the corresponding Group number for an M code
*/
int NGC_RS274::Interpreter::Processor::_mWord(float Address)
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
			ngc_working_group = NGC_RS274::Groups::M::SPINDLE;
			NGC_RS274::Interpreter::Processor::local_block.m_group[NGC_RS274::Groups::M::SPINDLE] = (iAddress);
			break;
		}

		case NGC_RS274::M_codes::PROGRAM_PAUSE: //<-M00
		case NGC_RS274::M_codes::PROGRAM_PAUSE_OPTIONAL: //<-M01
		case NGC_RS274::M_codes::TOOL_CHANGE_PAUSE: //<-M06
		case NGC_RS274::M_codes::PALLET_CHANGE_PAUSE: //<-M60
		{
			ngc_working_group = NGC_RS274::Groups::M::STOPPING;
			NGC_RS274::Interpreter::Processor::local_block.m_group[NGC_RS274::Groups::M::STOPPING] = (iAddress);
			break;
		}

		case NGC_RS274::M_codes::COLLANT_MIST: //<-M07
		case NGC_RS274::M_codes::COOLANT_FLOOD: //<-M08
		case NGC_RS274::M_codes::COOLANT_OFF: //<-M09
		{
			ngc_working_group = NGC_RS274::Groups::M::COOLANT;
			NGC_RS274::Interpreter::Processor::local_block.m_group[NGC_RS274::Groups::M::COOLANT] = (iAddress);
			//Since we DO allow multiple coolant modes at the same time, we are just going to return here
			//No need to check if this modal group was already set on the line.
			return  NGC_RS274::Interpreter::Errors::OK;
			break;
		}

		case NGC_RS274::M_codes::ENABLE_FEED_SPEED_OVERRIDE: //<-M48
		case NGC_RS274::M_codes::DISABLE_FEED_SPEED_OVERRIDE: //<-M49
		{
			ngc_working_group = NGC_RS274::Groups::M::OVERRIDE;
			NGC_RS274::Interpreter::Processor::local_block.m_group[NGC_RS274::Groups::M::OVERRIDE] = (iAddress);
			break;
		}

		default:
		if (iAddress >= 100 && iAddress <= 199)
		{
			ngc_working_group = NGC_RS274::Groups::M::USER_DEFINED;
			NGC_RS274::Interpreter::Processor::local_block.m_group[NGC_RS274::Groups::M::USER_DEFINED] = (iAddress);
		}
		else
		{
			return  NGC_RS274::Interpreter::Errors::INTERPRETER_DOES_NOT_UNDERSTAND_M_WORD_VALUE;
		}

		break;
	}

	/*
	See if we have already processed an m command from this group.
	We can make the error more meaningful if we tell it WHICH group
	was specified more than once.
	*/
	if (BitTst(NGC_RS274::Interpreter::Processor::local_block.m_code_defined_in_block, ngc_working_group))
	return  NGC_RS274::Interpreter::Errors::M_CODE_GROUP_STOPPING_ALREADY_SPECIFIED + ngc_working_group;

	/*
	If we havent already returned from a duplicate group being specified, set the bit flag for this
	mcode group. This will get checked if this method is called again but if a mcode for the same
	group is on the line again, the logic above will catch it and return an error
	*/
	NGC_RS274::Interpreter::Processor::local_block.m_code_defined_in_block =
	BitSet(NGC_RS274::Interpreter::Processor::local_block.m_code_defined_in_block, ngc_working_group);
	return  NGC_RS274::Interpreter::Errors::OK;

}

/*
Assign the corresponding Address value for a specific G Word
*/
int NGC_RS274::Interpreter::Processor::_pWord(char Word, float iAddress)
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
	int ReturnValue = NGC_RS274::Interpreter::Errors::OK;
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
		ReturnValue = process_word_values(Word, iAddress);
		//break;
	}
	return ReturnValue;
}


/*
Assign the corresponding Address value for a G Word. Does not allow a word to be set twice in the same block
*/
int NGC_RS274::Interpreter::Processor::process_word_values(char Word, float iAddress)
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
	if (BitTst(NGC_RS274::Interpreter::Processor::local_block.word_defined_in_block_A_Z, WordNumber))
	//Return the equivalent NGC_Interpreter_Errors number
	return (G_CODE_MULTIPLIER + WordNumber);
	/*
	Since this bit was not already set, we can go ahead and set it now
	If this same word value was specified twice in the same block, we will
	catch it when it comes in again with the code above.
	*/

	NGC_RS274::Interpreter::Processor::local_block.word_defined_in_block_A_Z = BitSet(NGC_RS274::Interpreter::Processor::local_block.word_defined_in_block_A_Z, WordNumber);

	/*
	Almost all letters of the alphabet are used as words. The easiest and simplest way
	to set the different word values is to hold them in an array of values. Then we
	simply need to know which letter in the alphabet it is. A=0,B=1,C=2....Z=25. that
	value is already stored in the BitNumber we used above.
	*/

	NGC_RS274::Interpreter::Processor::local_block.block_word_values[WordNumber] = iAddress;

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
	return  NGC_RS274::Interpreter::Errors::OK;
}

/*
Converts the NGC binary block data into a string that can be sent to the controller
*/
int NGC_RS274::Interpreter::Processor::convert_to_line_index(uint8_t BlockNumber)
{
	//memset(Converter_Line, 0, 256);
	//char *line_pointer = Converter_Line;//<--creating a pointer to an array to simplify building the array data in the loops
	//char buffer[10];


	//NGC_RS274::Interpreter::Processor::convert_to_line(&c_gcode_buffer::collection[BlockNumber]);
	return 1;
}

int NGC_RS274::Interpreter::Processor::convert_to_line(NGC_RS274::NGC_Binary_Block*local_block)
{
	memset(Line, 0, 256);
	char *line_pointer = Line;//<--creating a pointer to an array to simplify building the array data in the loops
	char buffer[10];


	/*
	Only convert the values that were on the original line to a gcode string.
	If we convert more than that, the line could exceed 256 characters. Since
	we only convert 256 characters at a time, we should never be able to convert
	more than 256 back, as long as we only convert what was sent on the block.
	*/
	for (int i = 0; i < COUNT_OF_G_CODE_GROUPS_ARRAY; i++)
	{
		//See if this value was sent
		if (BitTst(local_block->g_code_defined_in_block, i))
		{
			//_serial.Write("bit set ");_serial.Write_ni(i); _serial.Write(CR);
			//Bit is set so retrieve this value
			//*line_pointer='G'; line_pointer++;
			strcat(Line, "G");
			memset(buffer, 0, 10);
			num_to_string::itog(local_block->g_group[i], buffer);
			strcat(Line, buffer);
		}
	}

	for (int i = 0; i < COUNT_OF_M_CODE_GROUPS_ARRAY; i++)
	{
		//See if this value was sent
		if (BitTst(local_block->m_code_defined_in_block, i))
		{
			//_serial.Write("bit set ");_serial.Write_ni(i); _serial.Write(CR);
			//Bit is set so retrieve this value
			//*line_pointer='G'; line_pointer++;
			strcat(Line, "M");
			memset(buffer, 0, 10);
			num_to_string::itog(local_block->m_group[i], buffer);
			strcat(Line, buffer);
			//_serial.Write("value ");_serial.Write(buffer); _serial.Write(CR);
			//_serial.Write("line ");_serial.Write(Line); _serial.Write(CR);
		}
	}
	char word[2] = { '\0', '\0' };


	for (int i = 0; i < COUNT_OF_BLOCK_WORDS_ARRAY; i++)
	{
		//See if this value was sent
		if (BitTst(local_block->word_defined_in_block_A_Z, i))
		{
			//Bit is set so retrieve this value
			//*line_pointer='G'; line_pointer++;
			word[0] = i + 65;
			strcat(Line, word);
			memset(buffer, 0, 10);

			num_to_string::ftoa(local_block->block_word_values[i], buffer, 4, 0);
			strcat(Line, buffer);

		}
	}
	//_serial.Write("Converted Line #");
	//_serial.Write(Line);
	//_serial.Write(CR);
	//c_planner::block_buffer[c_planner::machine_block_buffer_head].g_code_defined_in_block = 0;
	return 1;
}


//Internally all units are converted to MM when possible
int NGC_RS274::Interpreter::Processor::normalize_distance_units()
{
	//If already in MM mode return;
	if (NGC_RS274::Interpreter::Processor::local_block.get_g_code_value_x(NGC_RS274::Groups::G::UNITS) == NGC_RS274::G_codes::MILLIMETER_SYSTEM_SELECTION)
	{
		return  NGC_RS274::Interpreter::Errors::OK;
	}
	//If we are using the integrated controller (ARM platform), all dimensions are in MM regardless of gcode setting
	if (!NGC_RS274::Interpreter::Processor::normalize_distance_units_to_mm)
	return  NGC_RS274::Interpreter::Errors::OK;

	/*
	We are using some 3rd party controller like grbl, tinyg, smoothies, etc. Those controlelr can convert g20/g21 on their own
	IF we do it here, and the 3rd party controll does it too, 1 inch will become 254.*25.4 mm. (a big ass WRONG number)
	*/

	//Decided NOT to do this at this time.
	return  NGC_RS274::Interpreter::Errors::OK;

	//If we are in inch mode, convert all dimensional word values to MM
	NGC_RS274::Interpreter::Processor::local_block.block_word_values[A_WORD_BIT] *= MM_CONVERSION;
	NGC_RS274::Interpreter::Processor::local_block.block_word_values[B_WORD_BIT] *= MM_CONVERSION;
	NGC_RS274::Interpreter::Processor::local_block.block_word_values[C_WORD_BIT] *= MM_CONVERSION;
	NGC_RS274::Interpreter::Processor::local_block.block_word_values[I_WORD_BIT] *= MM_CONVERSION;
	NGC_RS274::Interpreter::Processor::local_block.block_word_values[J_WORD_BIT] *= MM_CONVERSION;
	NGC_RS274::Interpreter::Processor::local_block.block_word_values[K_WORD_BIT] *= MM_CONVERSION;
	NGC_RS274::Interpreter::Processor::local_block.block_word_values[Q_WORD_BIT] *= MM_CONVERSION;
	NGC_RS274::Interpreter::Processor::local_block.block_word_values[R_WORD_BIT] *= MM_CONVERSION;
	NGC_RS274::Interpreter::Processor::local_block.block_word_values[U_WORD_BIT] *= MM_CONVERSION;
	NGC_RS274::Interpreter::Processor::local_block.block_word_values[V_WORD_BIT] *= MM_CONVERSION;
	NGC_RS274::Interpreter::Processor::local_block.block_word_values[X_WORD_BIT] *= MM_CONVERSION;
	NGC_RS274::Interpreter::Processor::local_block.block_word_values[Y_WORD_BIT] *= MM_CONVERSION;
	NGC_RS274::Interpreter::Processor::local_block.block_word_values[Z_WORD_BIT] *= MM_CONVERSION;

	//NGC_RS274::Interpreter::Processor::local_block.persisted_values.feed_rate *= MM_CONVERSION;
	//TODO:: need a way to determine the units of D/H in the tool table.
	//NGC_RS274::Interpreter::Processor::local_block.persisted_values.active_h *= MM_CONVERSION;
	//NGC_RS274::Interpreter::Processor::local_block.persisted_values.active_d *= MM_CONVERSION;

	return  NGC_RS274::Interpreter::Errors::OK;
}

float NGC_RS274::Interpreter::Processor::hypot_f(float x, float y)
{
	return (sqrt(x * x + y * y));
}
float NGC_RS274::Interpreter::Processor::square(float X)
{
	return X * X;
}
//// default constructor
//NGC_RS274::Interpreter::Processor::c_interpreter()
//{
//} //c_interpreter
//
//// default destructor
//NGC_RS274::Interpreter::Processor::~c_interpreter()
//{
//} //~c_interpreter
//TODO:
//	Check for modal group violations. From NIST, section 3.4 "It is an error to put
//	a G-code from group 1 and a G-code from group 0 on the same line if both of them
//	use axis words. If an axis word-using G-code from group 1 is implicitly in effect
//	on a line (by having been activated on an earlier line), and a group 0 G-code that
//	uses axis words appears on the line, the activity of the group 1 G-code is suspended
//	for that line. The axis word-using G-codes from group 0 are G10, G28, G30, and G92"

/*
* _execute_gcode_block() - execute parsed block
*
*  Conditionally (based on whether a flag is set in gf) call the canonical
*	machining functions in order of execution as per RS274NGC_3 table 8
*  (below, with modifications):
*
*	    0. record the line number
*		1. comment (includes message) [handled during block normalization]
*		2. set feed rate mode (G93, G94 - inverse time or per minute)
*		3. set feed rate (F)
*		3a. set feed override rate (M50.1)
*		3a. set traverse override rate (M50.2)
*		4. set spindle speed (S)
*		4a. set spindle override rate (M51.1)
*		5. select tool (T)
*		6. change tool (M6)
*		7. spindle on or off (M3, M4, M5)
*		8. coolant on or off (M7, M8, M9)
*		9. enable or disable overrides (M48, M49, M50, M51)
*		10. dwell (G4)
*		11. set active plane (G17, G18, G19)
*		12. set length units (G20, G21)
*		13. cutter radius compensation on or off (G40, G41, G42)
*		14. cutter length compensation on or off (G43, G49)
*		15. coordinate system selection (G54, G55, G56, G57, G58, G59)
*		16. set path control mode (G61, G61.1, G64)
*		17. set distance mode (G90, G91)
*		18. set retract mode (G98, G99)
*		19a. homing functions (G28.2, G28.3, G28.1, G28, G30)
*		19b. update system data (G10)
*		19c. set axis offsets (G92, G92.1, G92.2, G92.3)
*		20. perform motion (G0 to G3, G80-G89) as modified (possibly) by G53
*		21. stop and end (M0, M1, M2, M30, M60)
*
*	Values in gn are in original units and should not be unit converted prior
*	to calling the canonical functions (which do the unit conversions)
*/