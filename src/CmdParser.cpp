#include "CmdParser.h"

CmdParser::CmdParser() {
	_parser = new StateMachine();

	// States
	_start = _parser->addState(&s_token);
	_to    = _parser->addState(&s_token);
	_for   = _parser->addState(&s_token);
	_up    = _parser->addState(&s_up);
	_down  = _parser->addState(&s_down);
	_set   = _parser->addState(&s_set);
	_pos   = _parser->addState(&s_pos);
	_add   = _parser->addState(&s_add);
	_sub   = _parser->addState(&s_sup);

	// Transitions
	_start->addTransition(&t_feet, _start);
	_start->addTransition(&t_head, _start);
	_start->addTransition(&t_to,   _to);
	_start->addTransition(&t_for,  _for);
	_start->addTransition(&t_up,   _up);
	_start->addTransition(&t_down, _down);
	_start->addTransition(&t_all,  _pos);

	_to->addTransition(&t_the, _to);
	_to->addTransition(&t_num, _set);
	_to->addTransition(&t_all, _pos);

	_for->addTransition(&t_the, _for);
	_for->addTransition(&t_all, _pos);

	_up->addTransition(&t_to,  _to);
	_up->addTransition(&t_by,  _up);
	_up->addTransition(&t_for, _for);
	_up->addTransition(&t_num, _add);
	_up->addTransition(&t_all, _pos);

	_down->addTransition(&t_to,  _to);
	_down->addTransition(&t_by,  _down);
	_down->addTransition(&t_for, _for);
	_down->addTransition(&t_num, _sub);
	_down->addTransition(&t_all, _pos);

	_set->addTransition(&t_deg, _set);

	_add->addTransition(&t_deg, _add);

	_sub->addTransition(&t_deg, _sub);
}

CmdParser::~CmdParser() {
	delete _parser;
	delete _start;
	delete _to;
	delete _for;
	delete _up;
	delete _down;
	delete _set;
	delete _pos;
	delete _add;
	delete _sub;
}

void CmdParser::exec(String cmd) {
	cmd.trim();
	cmd.toLowerCase();
	_cmd = cmd;
	_bed = NULL;
	_token = "";
	_tokenIdx = 0;
	_lastTokenIdx = 0;

	_parser->run();
}

/**************************** States *******************************/

void CmdParser::s_token() {
	_lastTokenIdx = _tokenIdx;
	_tokenIdx = _cmd.indexOf(' ', _lastTokenIdx);
	_token = _cmd.subString(_lastTokenIdx, _tokenIdx);
	_token.trim();
}

void CmdParser::s_up() {
	s_token();

	if (_token == "") {
		_bed->setAngle(ANGLE_UP);
	}
}

void CmdParser::s_down() {
	s_token();

	if (_token == "") {
		head.setAngle(0);
		feet.setAngle(0);
	}
}

void CmdParser::s_set() {
	if (_parser->executeOnce) {
		_bed->setAngle(round(_token.toFloat())); // Use _token as the angle
	}

	s_token();
}

void CmdParser::s_add() {
	if (_parser->executeOnce) {
		_bed->addAngle(round(_token.toFloat())); // Use _token as the angle
	}

	s_token();
}

void CmdParser::s_sub() {
	if (_parser->executeOnce) {
		_bed->addAngle(round(_token.toFloat()) * -1); // Use _token as the angle
	}

	s_token();
}

void CmdParser::s_pos() {
	_token = _cmd.subString(_lastTokenIdx);

	if (_token.endsWith("position")) {
		_token = _token.substring(0, _token.length()-8);
		_token.trim();
	}

	switch (_token) {
		case "tv":
		case "tv watching":
			_bed->setAngle(ANGLE_TV);
			break;
		case "up":
		case "top":
			_bed->setAngle(ANGLE_UP);
			break;
		case "bottom":
			_bed->setAngle(0);
			break;
		case "lift":
		case "lifting":
			head.setAngle(ANGLE_FULL);
			feet.setAngle(ANGLE_FULL);
			break;
		case "company":
		case "talking":
		case "teeth":
		case "brushing":
		case "teeth brushing":
			_bed->setAngle(ANGLE_TALKING);
			break;
		case "halfway":
		case "half way":
			head.setAngle(ANGLE_HALF);
			feet.setAngle(ANGLE_HALF);
			break;
		case "all the way":
			head.setAngle(ANGLE_FULL);
			feet.setAngle(ANGLE_FULL);
			break;
		default:
			publish("error", "Invalid named position: " + _token);
			break;
	}
}

/************************** Transitions *****************************/

bool CmdParser::t_feet() {
	if (_token == "feet") {
		_bed = &feet;
		return true;
	}
	return false;
}

bool CmdParser::t_head() {
	bool ok = _token == "head";
	if (ok || _bed == NULL) {
		_bed = &head;
	}
	return ok;
}

bool CmdParser::t_to() {
	return _token == "to";
}

bool CmdParser::t_for() {
	return _token == "for";
}

bool CmdParser::t_up() {
	return _token == "up";
}

bool CmdParser::t_down() {
	return _token == "down";
}

bool CmdParser::t_the() {
	return _token == "the";
}

bool CmdParser::t_num() {
	return isNumeric(_token);
}

bool CmdParser::t_all() {
	return true;
}

bool CmdParser::t_by() {
	return _token == "by";
}

bool CmdParser::t_deg() {
	return _token == "degrees" || _token == "°"; // \u00B0
}
