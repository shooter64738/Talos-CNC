#include "_ngc_validate_7_cutter_radius_compensation.h"
#include "../_ngc_g_groups.h"
#include "../NGC_Comp.h"
#include <stddef.h>

//e_compensation_states NGC_RS274::Dialect::Group7::Compensation_State;


e_parsing_errors NGC_RS274::Dialect::Group7::cutter_radius_comp_validate(NGC_RS274::Block_View * v_block, NGC_RS274::Block_View * v_previous_block, e_dialects dialect)
{
	//was comp set on the line?
	if (v_block->active_view_block->g_code_defined_in_block.get((int)NGC_RS274::Groups::G::Cutter_radius_compensation))
	{
		//comp is on but turning off
		if (*v_block->current_g_codes.Cutter_radius_compensation == 400
			&& NGC_RS274::Compensation::comp_control.state == e_compensation_states::CurrentCompensationOnActive)
		{
			//set state to deactivating (last move)
			NGC_RS274::Compensation::comp_control.state = e_compensation_states::CurrentCompensationOnDeactivating;
		}
		//comp is off and turning off
		else if (*v_block->current_g_codes.Cutter_radius_compensation == 400
			&& NGC_RS274::Compensation::comp_control.state == e_compensation_states::CurrentCompensationOffNotActivating)
		{
			//comp is off and turnign off. no change, just return
			return e_parsing_errors::OK;
		}
		//comp is turning on, and is already on
		else if (*v_block->current_g_codes.Cutter_radius_compensation != 400
			&& NGC_RS274::Compensation::comp_control.state == e_compensation_states::CurrentCompensationOnActive)
		{
			// it is an error because its not g40 and comp is already on.
			return e_parsing_errors::CUTTER_RADIUS_COMP_ALREADY_ACTIVE;
		}
		//comp is turning on and is currently off
		else if (*v_block->current_g_codes.Cutter_radius_compensation != 400
			&& NGC_RS274::Compensation::comp_control.state == e_compensation_states::CurrentCompensationOffNotActivating)
		{
			//set state to activating (first move)
			//NGC_RS274::Compensation::comp_control.state = e_compensation_states::CurrentCompensationOnActive;
		}
	}
	


	//__check_dialect(v_block, dialect);

	//default the radius to the selected tool size
	NGC_RS274::Compensation::comp_control.tool_radius = .25;

	switch (*v_block->current_g_codes.Cutter_radius_compensation)
	{
	case NGC_RS274::G_codes::CANCEL_CUTTER_RADIUS_COMPENSATION:
		_G400(v_block, v_previous_block, dialect);
		break;
	case NGC_RS274::G_codes::START_CUTTER_RADIUS_COMPENSATION_LEFT_DYNAMIC:
		if (!v_block->get_word_value('P', &NGC_RS274::Compensation::comp_control.tool_radius))
			return e_parsing_errors::CUTTER_RADIUS_COMP_MISSING_P_VALUE;
	case NGC_RS274::G_codes::START_CUTTER_RADIUS_COMPENSATION_LEFT:
		_G410(v_block, v_previous_block, dialect);
		break;
	case NGC_RS274::G_codes::START_CUTTER_RADIUS_COMPENSATION_RIGHT_DYNAMIC:
		if (!v_block->get_word_value('P', &NGC_RS274::Compensation::comp_control.tool_radius))
			return e_parsing_errors::CUTTER_RADIUS_COMP_MISSING_P_VALUE;
	case NGC_RS274::G_codes::START_CUTTER_RADIUS_COMPENSATION_RIGHT:
		_G420(v_block, v_previous_block, dialect);
		break;
	default:
		break;
	}

	return e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Dialect::Group7::__check_state(NGC_RS274::Block_View * v_block)
{

	return e_parsing_errors::OK;
}

e_parsing_errors NGC_RS274::Dialect::Group7::__check_dialect(NGC_RS274::Block_View * v_block, e_dialects dialect)
{
	switch (dialect)
	{
	case e_dialects::Fanuc_A:
	case e_dialects::Fanuc_B:
	case e_dialects::Fanuc_C:
	case e_dialects::Mach3:
	case e_dialects::Haas:
	case e_dialects::Siemens_ISO:
	case e_dialects::Yasnac:
		//check for 41.1 and 42.1
		break;
	default:
		break;
	}

	return e_parsing_errors::OK;
}

//Cancel comp
e_parsing_errors NGC_RS274::Dialect::Group7::_G400(NGC_RS274::Block_View * v_block, NGC_RS274::Block_View * v_previous_block, e_dialects dialect)
{
	//if comp is going off, but is already active, this is last move (lead out)
	NGC_RS274::Compensation::process(v_block);
	return e_parsing_errors::OK;
}

//Left comp
e_parsing_errors NGC_RS274::Dialect::Group7::_G410(NGC_RS274::Block_View * v_block, NGC_RS274::Block_View * v_previous_block, e_dialects dialect)
{
	//set compensation state to activating
	//Compensation_State = e_compensation_states::CurrentCompensationOffActiving;
	NGC_RS274::Compensation::comp_control.side = e_compensation_side::CompensatingLeft;
	NGC_RS274::Compensation::process(v_block);
	return e_parsing_errors::OK;
}

//Right comp
e_parsing_errors NGC_RS274::Dialect::Group7::_G420(NGC_RS274::Block_View * v_block, NGC_RS274::Block_View * v_previous_block, e_dialects dialect)
{

	//set compensation state to activating
	//Compensation_State = e_compensation_states::CurrentCompensationOffActiving;
	NGC_RS274::Compensation::comp_control.side = e_compensation_side::CompensatingRight;
	NGC_RS274::Compensation::process(v_block);

	return e_parsing_errors::OK;
}