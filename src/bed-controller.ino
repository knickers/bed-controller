// Command starters:
/*
Move the bed...
Make the bed...
Lay the bed...
Sit the bed...
*/

// Commands:
/*
to X degrees
to [named position]
up
up X degrees
up by X degrees
down
down X degrees
down by X degrees
*/

#include "Particle.h"
#include <BedJoint.h>
#include <StateMachine.h>
#include <Tokenizer.h>

int ANGLE_UP = 10;
int ANGLE_HALF = 20;
int ANGLE_FULL = 45;

BedJoint   head(A0, 0, 2, 1);
BedJoint   feet(A1, 4, 6, 5);
BedJoint* _joint;

StateMachine parser = StateMachine();


/*******************************************************************/
/**************************** States *******************************/
/*******************************************************************/

State* _start = parser.addState(&nextToken);
State* _to    = parser.addState(&nextToken);
State* _for   = parser.addState(&nextToken);
State* _up    = parser.addState([]() {
	nextToken();

	if (_token == "") {
		_joint->setAngle(ANGLE_UP);
	}
});
State* _down = parser.addState([]() {
	nextToken();

	if (_token == "") {
		_joint->setAngle(0);
	}
});
State* _set = parser.addState([]() {
	if (parser.executeOnce) {
		_joint->setAngle(round(_token.toFloat())); // Use _token as the angle
	}

	nextToken();
});
State* _add = parser.addState([]() {
	if (parser.executeOnce) {
		_joint->addAngle(round(_token.toFloat())); // Use _token as the angle
	}

	nextToken();
});
State* _sub = parser.addState([]() {
	if (parser.executeOnce) {
		_joint->addAngle(round(_token.toFloat()) * -1); // Use _token as angle
	}

	nextToken();
});
State* _pos = parser.addState([]() {
	if (parser.executeOnce) {
		remainingTokenString();
		trimTokenEnd("position");
	}
});
State* _err = parser.addState([]() {
	publish("error", "Invalid named position: " + _token);
});


/********************************************************************/
/**************************** Positions *****************************/
/********************************************************************/

State* p_up   = parser.addState([]() { _joint->setAngle(ANGLE_UP); });
State* p_down = parser.addState([]() { _joint->setAngle(0); });
State* p_half = parser.addState([]() { _joint->setAngle(ANGLE_HALF); });
State* p_full = parser.addState([]() { _joint->setAngle(ANGLE_FULL); });
State* p_off  = parser.addState([]() { head.turnOff(); feet.turnOff(); });
State* p_alltheway = parser.addState([]() {
	head.setAngle(ANGLE_FULL);
	feet.setAngle(ANGLE_FULL);
});
State* p_halfway = parser.addState([]() {
	head.setAngle(ANGLE_HALF);
	feet.setAngle(ANGLE_HALF);
});


/********************************************************************/
/************************** Transitions *****************************/
/********************************************************************/

bool t_feet() {
	if (_token == "feet" ||
		_token == "legs" ||
		_token == "knees"
	) {
		_joint = &feet;
		return true;
	}
	return false;
}
bool t_head() {
	bool ok = _token == "head";
	if (ok || _joint == NULL) {
		_joint = &head;
	}
	return ok;
}
bool t_eq(String str) { return _token == str; }
bool t_to()   { return _token == "to"; }
bool t_for()  { return _token == "for"; }
bool t_up()   { return _token == "up"; }
bool t_down() { return _token == "down"; }
bool t_by()   { return _token == "by"; }
bool t_the()  { return _token == "the"; }
bool t_num()  { return isNumeric(_token); }
bool t_all()  { return true; }
bool t_deg()  { return _token == "degrees" || _token == "°"; } // \u00B0


void setup() {
	Particle.function("command", executeCommand);
	Particle.function("headState", setHeadState);
	Particle.function("feetState", setFeetState);
	Particle.function("headAngle", setHeadAngle);
	Particle.function("feetAngle", setFeetAngle);
	Particle.variable("headAngle", getHeadAngle);
	Particle.variable("feetAngle", getFeetAngle);

	Serial.begin(115200);

	head.init(150, 530, 0, 60);
	feet.init(150, 530, 0, 45);

	// StateMachine Transitions
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

	// Named Positions
	_pos->addTransition(&t_up, p_up);
	_pos->addTransition([]() { return t_eq("tv");          }, p_up);
	_pos->addTransition([]() { return t_eq("tv watching"); }, p_up);
	_pos->addTransition([]() { return t_eq("watching tv"); }, p_up);

	_pos->addTransition(&t_down, p_down);
	_pos->addTransition([]() { return t_eq("bottom"); }, p_down);

	_pos->addTransition([]() { return t_eq("company");           }, p_half);
	_pos->addTransition([]() { return t_eq("talking");           }, p_half);
	_pos->addTransition([]() { return t_eq("teeth");             }, p_half);
	_pos->addTransition([]() { return t_eq("brushing");          }, p_half);
	_pos->addTransition([]() { return t_eq("teeth brushing");    }, p_half);
	_pos->addTransition([]() { return t_eq("brush my teeth");    }, p_half);
	_pos->addTransition([]() { return t_eq("brushing my teeth"); }, p_half);

	_pos->addTransition([]() { return t_eq("lift");        }, p_alltheway);
	_pos->addTransition([]() { return t_eq("lifting");     }, p_alltheway);
	_pos->addTransition([]() { return t_eq("all the way"); }, p_alltheway);

	_pos->addTransition([]() { return t_eq("halfway");  }, p_halfway);
	_pos->addTransition([]() { return t_eq("half way"); }, p_halfway);

	_pos->addTransition([]() { return t_eq("off");    }, p_off);
	_pos->addTransition([]() { return t_eq("stop");   }, p_off);
	_pos->addTransition([]() { return t_eq("cancel"); }, p_off);

	_pos->addTransition(&t_all, _err);
}

void loop() {
	head.update();
	feet.update();
}

bool isNumeric(String str) {
	unsigned int stringLength = str.length();

	if (stringLength == 0) {
		return false;
	}

	bool seenDecimal = false;

	for (unsigned int i = 0; i < stringLength; i++) {
		if (isDigit(str.charAt(i))) {
			continue;
		}

		if (str.charAt(i) == '.') {
			if (seenDecimal) {
				return false;
			}
			seenDecimal = true;
			continue;
		}

		return false;
	}

	return true;
}

void publish(String event, String data) {
	if (Serial.isConnected()) {
		Serial.printlnf("%d %s: %s", (int)Time.now(), event.c_str(), data.c_str());
	}
	if (Particle.connected()) {
		Particle.publish(event, data, PRIVATE);
	}
}


/********************************************************************/
/************************ Particle Methods **************************/
/********************************************************************/

int executeCommand(String cmd) {
	cmd.trim();
	cmd.toLowerCase();

	resetTokenString(cmd);

	parser.exec();

	return 1;
}

int setHeadAngle(String angle) {
	Serial.printlnf("%d Set head angle: %s", (int)Time.now(), angle.c_str());
	return head.setAngle(angle.toInt());
}
int setFeetAngle(String angle) {
	Serial.printlnf("%d Set feet angle: %s", (int)Time.now(), angle.c_str());
	return feet.setAngle(angle.toInt());
}

int addHeadAngle(String angle) {
	Serial.printlnf("%d Add head angle %s", (int)Time.now(), angle.c_str());
	return head.addAngle(angle.toInt());
}
int addFeetAngle(String angle) {
	Serial.printlnf("%d Add feet angle %s", (int)Time.now(), angle.c_str());
	return feet.addAngle(angle.toInt());
}

int getHeadAngle() {
	Serial.printlnf("%d Get head angle %d",(int)Time.now(),head.currentAngle());
	return head.currentAngle();
}
int getFeetAngle() {
	Serial.printlnf("%d Get feet angle %d",(int)Time.now(),feet.currentAngle());
	return feet.currentAngle();
}

int setHeadState(String state) {
	Serial.printlnf("%d Set head state %s", (int)Time.now(), state.c_str());
	return head.setState(state);
}
int setFeetState(String state) {
	Serial.printlnf("%d Set feet state %s", (int)Time.now(), state.c_str());
	return feet.setState(state);
}
