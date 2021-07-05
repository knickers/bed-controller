#include "Particle.h"
#include <BedJoint.h>
#include <StateMachine.h>
#include <Tokenizer.h>

#define PIN_HEAD_UP 0
#define PIN_HEAD_DN 1
#define PIN_FEET_UP 2
#define PIN_FEET_DN 3

#define ANGLE_UP   20
#define ANGLE_HALF 35
#define ANGLE_FULL 50
#define ANGLE_MAX  60

// Head bottom slop ~~ 2 sec ~~ 8 degrees

BedJoint  head(PIN_HEAD_UP, PIN_HEAD_DN);
BedJoint  feet(PIN_FEET_UP, PIN_FEET_DN);
BedJoint* joint;
int8_t dir;

StateMachine parser = StateMachine();

Tokenizer token = Tokenizer();
void nextToken() { token.next(); }

/*******************************************************************/
/**************************** States *******************************/
/*******************************************************************/

State* _start = parser.addState(&nextToken);
State* _for   = parser.addState(&nextToken);
State* _to    = parser.addState([]() {
	token.next();

	if (token.eq("") || token.eq("degrees")) {
		joint->addAngle(2);
	}
});
State* _up    = parser.addState([]() {
	token.next();

	if (token.eq("")) {
		joint->addAngle(ANGLE_UP);
	}
});
State* _down = parser.addState([]() {
	token.next();

	dir = -1;

	if (token.eq("")) {
		joint->addAngle(-ANGLE_UP);
	}
});
State* _add = parser.addState([]() {
	if (parser.executeOnce) {
		joint->addAngle(dir * round(token.toFloat()));
	}

	token.next();
});
State* _pos = parser.addState([]() {
	if (parser.executeOnce) {
		token.remaining();
		token.trimEnd("position");
		token.trimEnd("up");
		if (token.trimEnd("down")) {
			dir = -1;
		}
	}
});
State* _err = parser.addState([]() {
	head.turnOff();
	feet.turnOff();
	publish("bed/error", "Invalid named position: '" + token.val() + "'");
});


/********************************************************************/
/**************************** Positions *****************************/
/********************************************************************/

State* p_up   = parser.addState([]() { joint->addAngle(ANGLE_UP); });
State* p_down = parser.addState([]() { joint->addAngle(-ANGLE_FULL); });
State* p_half = parser.addState([]() { joint->addAngle(ANGLE_HALF); });
State* p_full = parser.addState([]() { joint->addAngle(ANGLE_FULL); });
State* p_off  = parser.addState([]() { head.turnOff(); feet.turnOff(); });
State* p_alltheway = parser.addState([]() {
	head.addAngle(dir * ANGLE_FULL);
	feet.addAngle(dir * ANGLE_FULL/2);
});
State* p_halfway = parser.addState([]() {
	head.addAngle(dir * ANGLE_HALF);
	feet.addAngle(dir * ANGLE_HALF/2);
});


/********************************************************************/
/************************** Transitions *****************************/
/********************************************************************/

bool t_feet() {
	if (token.eq("feet") ||
		token.eq("legs") ||
		token.eq("knees")
	) {
		joint = &feet;
		return true;
	}
	return false;
}
bool t_head() {
	bool ok = token.eq("head");
	if (ok || joint == NULL) {
		joint = &head;
	}
	return ok;
}
bool t_to()   { return token.eq("to"); }
bool t_for()  { return token.eq("for"); }
bool t_up()   { return token.eq("up") || token.eq("of"); }
bool t_down() { return token.eq("down"); }
bool t_by()   { return token.eq("by"); }
bool t_the()  { return token.eq("the"); }
bool t_num()  { return token.isNumeric(); }
bool t_all()  { return !token.eq(""); }
bool t_deg()  {
	return token.eq("degrees") || token.eq("degree") || token.eq("°"); // \u00B0
}


void setup() {
	Particle.function("command", executeCommand);
	Particle.function("headAngle", setHeadAngle);
	Particle.function("feetAngle", setFeetAngle);

	pinMode(PIN_HEAD_UP, OUTPUT);
	pinMode(PIN_HEAD_DN, OUTPUT);
	pinMode(PIN_FEET_UP, OUTPUT);
	pinMode(PIN_FEET_DN, OUTPUT);

	// StateMachine Transitions
	_start->addTransition(&t_feet, _start);
	_start->addTransition(&t_head, _start);
	_start->addTransition(&t_to,   _to);
	_start->addTransition(&t_for,  _for);
	_start->addTransition(&t_up,   _up);
	_start->addTransition(&t_down, _down);
	_start->addTransition(&t_all,  _pos);
	_start->addTransition([]() { return token.eq(""); }, p_off);

	_to->addTransition(&t_the, _to);
	_to->addTransition(&t_num, _add);
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
	_down->addTransition(&t_num, _add);
	_down->addTransition(&t_all, _pos);

	_add->addTransition(&t_deg, _add);

	// Named Positions
	_pos->addTransition(&t_up, p_up);
	_pos->addTransition(&t_down, p_down);
	_pos->addTransition([]() { return token.eq("bottom"); }, p_down);

	_pos->addTransition([]() { return token.eq("halfway");     }, p_halfway);
	_pos->addTransition([]() { return token.eq("half way");    }, p_halfway);
	_pos->addTransition([]() { return token.eq("all the way"); }, p_alltheway);

	_pos->addTransition([]() { return token.eq("");       }, p_off);
	_pos->addTransition([]() { return token.eq("now");    }, p_off);
	_pos->addTransition([]() { return token.eq("off");    }, p_off);
	_pos->addTransition([]() { return token.eq("stop");   }, p_off);
	_pos->addTransition([]() { return token.eq("cancel"); }, p_off);

	_pos->addTransition(&t_all, _err);

	//Serial.begin(115200);
}

void loop() {
	head.update();
	feet.update();
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

	publish("bed/command", cmd);

	joint = NULL;
	dir = 1;

	token.set(cmd);

	parser.exec();

	return 1;
}

int setHeadAngle(String angle) {
	if (Serial.isConnected()) {
		Serial.printlnf("%s Set head angle: %s",
			Time.format(TIME_FORMAT_ISO8601_FULL),
			angle.c_str()
		);
	}
	return head.addAngle(angle.toInt());
}

int setFeetAngle(String angle) {
	if (Serial.isConnected()) {
		Serial.printlnf("%s Set feet angle: %s",
			Time.format(TIME_FORMAT_ISO8601_FULL),
			angle.c_str()
		);
	}
	return feet.addAngle(angle.toInt());
}
