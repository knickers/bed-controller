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

State* s_start = parser.addState(&nextToken);
State* s_for   = parser.addState(&nextToken);
State* s_to    = parser.addState([]() {
	token.next();

	if (token.eq("") || token.eq("degrees")) {
		joint->addAngle(2);
	}
});
State* s_up    = parser.addState([]() {
	token.next();

	if (token.eq("")) {
		joint->addAngle(ANGLE_UP);
	}
});
State* s_down = parser.addState([]() {
	token.next();

	dir = -1;

	if (token.eq("")) {
		joint->addAngle(-ANGLE_UP);
	}
});
State* s_add = parser.addState([]() {
	if (parser.executeOnce) {
		joint->addAngle(dir * round(token.toFloat()));
	}

	token.next();
});
State* s_pos = parser.addState([]() {
	if (parser.executeOnce) {
		token.remaining();
		token.trimEnd("position");
		token.trimEnd("up");
		if (token.trimEnd("down")) {
			dir = -1;
		}
	}
});
State* s_err = parser.addState([]() {
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
	feet.addDelay(dir * ANGLE_FULL/2, 500);
});
State* p_halfway = parser.addState([]() {
	head.addAngle(dir * ANGLE_HALF);
	feet.addDelay(dir * ANGLE_HALF/2, 500);
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
bool t_to()   { return token.eq("to") || token.eq("too"); }
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
	s_start->addTransition(&t_feet, s_start);
	s_start->addTransition(&t_head, s_start);
	s_start->addTransition(&t_to,   s_to);
	s_start->addTransition(&t_for,  s_for);
	s_start->addTransition(&t_up,   s_up);
	s_start->addTransition(&t_down, s_down);
	s_start->addTransition(&t_all,  s_pos);
	s_start->addTransition([]() { return token.eq(""); }, p_off);

	s_to->addTransition(&t_the, s_to);
	s_to->addTransition(&t_num, s_add);
	s_to->addTransition(&t_all, s_pos);

	s_for->addTransition(&t_the, s_for);
	s_for->addTransition(&t_all, s_pos);

	s_up->addTransition(&t_to,  s_to);
	s_up->addTransition(&t_by,  s_up);
	s_up->addTransition(&t_for, s_for);
	s_up->addTransition(&t_num, s_add);
	s_up->addTransition(&t_all, s_pos);

	s_down->addTransition(&t_to,  s_to);
	s_down->addTransition(&t_by,  s_down);
	s_down->addTransition(&t_for, s_for);
	s_down->addTransition(&t_num, s_add);
	s_down->addTransition(&t_all, s_pos);

	s_add->addTransition(&t_deg, s_add);

	// Named Positions
	s_pos->addTransition(&t_up, p_up);
	s_pos->addTransition(&t_down, p_down);
	s_pos->addTransition([]() { return token.eq("bottom"); }, p_down);

	s_pos->addTransition([]() { return token.eq("halfway");     }, p_halfway);
	s_pos->addTransition([]() { return token.eq("half way");    }, p_halfway);
	s_pos->addTransition([]() { return token.eq("all the way"); }, p_alltheway);

	s_pos->addTransition([]() { return token.eq("");       }, p_off);
	s_pos->addTransition([]() { return token.eq("now");    }, p_off);
	s_pos->addTransition([]() { return token.eq("off");    }, p_off);
	s_pos->addTransition([]() { return token.eq("stop");   }, p_off);
	s_pos->addTransition([]() { return token.eq("cancel"); }, p_off);

	s_pos->addTransition(&t_all, s_err);

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
			Time.format(TIME_FORMAT_ISO8601_FULL).c_str(),
			angle.c_str()
		);
	}
	head.addAngle(angle.toInt());
	return head.getState();
}

int setFeetAngle(String angle) {
	if (Serial.isConnected()) {
		Serial.printlnf("%s Set feet angle: %s",
			Time.format(TIME_FORMAT_ISO8601_FULL).c_str(),
			angle.c_str()
		);
	}
	feet.addAngle(angle.toInt());
	return feet.getState();
}
