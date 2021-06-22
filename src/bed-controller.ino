#include "Particle.h"
#include <BedJoint.h>
#include <StateMachine.h>
#include <Tokenizer.h>

#define ANGLE_UP   10
#define ANGLE_HALF 20
#define ANGLE_FULL 45
#define ANGLE_MAX  60

BedJoint  head(0, 2);
BedJoint  feet(4, 6);
BedJoint* joint;

StateMachine parser = StateMachine();

Tokenizer token = Tokenizer();
void nextToken() { token.next(); }

/*******************************************************************/
/**************************** States *******************************/
/*******************************************************************/

State* _start = parser.addState(&nextToken);
State* _to    = parser.addState(&nextToken);
State* _for   = parser.addState(&nextToken);
State* _up    = parser.addState([]() {
	token.next();

	if (token.eq("")) {
		joint->addAngle(ANGLE_UP);
	}
});
State* _down = parser.addState([]() {
	token.next();

	if (token.eq("")) {
		head.addAngle(-ANGLE_MAX);
		feet.addAngle(-ANGLE_MAX);
	}
});
State* _set = parser.addState([]() {
	if (parser.executeOnce) {
		joint->addAngle(round(token.toFloat()));
	}

	token.next();
});
State* _add = parser.addState([]() {
	if (parser.executeOnce) {
		joint->addAngle(round(token.toFloat()));
	}

	token.next();
});
State* _sub = parser.addState([]() {
	if (parser.executeOnce) {
		joint->addAngle(round(token.toFloat()) * -1);
	}

	token.next();
});
State* _pos = parser.addState([]() {
	if (parser.executeOnce) {
		token.remaining();
		token.trimEnd("position");
	}
});
State* _err = parser.addState([]() {
	head.turnOff();
	feet.turnOff();
	publish("error", "Invalid named position: '" + token.val() + "'");
});


/********************************************************************/
/**************************** Positions *****************************/
/********************************************************************/

State* p_up   = parser.addState([]() { joint->addAngle(ANGLE_UP); });
State* p_down = parser.addState([]() { joint->addAngle(-ANGLE_MAX); });
State* p_half = parser.addState([]() { joint->addAngle(ANGLE_HALF); });
State* p_full = parser.addState([]() { joint->addAngle(ANGLE_FULL); });
State* p_off  = parser.addState([]() { head.turnOff(); feet.turnOff(); });
State* p_alltheway = parser.addState([]() {
	head.addAngle(ANGLE_FULL);
	feet.addAngle(ANGLE_FULL);
});
State* p_halfway = parser.addState([]() {
	head.addAngle(ANGLE_HALF);
	feet.addAngle(ANGLE_HALF);
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
bool t_up()   { return token.eq("up"); }
bool t_down() { return token.eq("down"); }
bool t_by()   { return token.eq("by"); }
bool t_the()  { return token.eq("the"); }
bool t_num()  { return token.isNumeric(); }
bool t_all()  { return !token.eq(""); }
bool t_deg()  { return token.eq("degrees") || token.eq("°"); } // \u00B0


void setup() {
	Particle.function("command", executeCommand);
	Particle.function("headState", setHeadState);
	Particle.function("headAngle", setHeadAngle);
	Particle.function("feetState", setFeetState);
	Particle.function("feetAngle", setFeetAngle);

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
	_pos->addTransition([]() { return token.eq("tv");          }, p_up);
	_pos->addTransition([]() { return token.eq("tv watching"); }, p_up);
	_pos->addTransition([]() { return token.eq("watching tv"); }, p_up);

	_pos->addTransition(&t_down, p_down);
	_pos->addTransition([]() { return token.eq("bottom"); }, p_down);

	_pos->addTransition([]() { return token.eq("company");           }, p_half);
	_pos->addTransition([]() { return token.eq("talking");           }, p_half);
	_pos->addTransition([]() { return token.eq("teeth");             }, p_half);
	_pos->addTransition([]() { return token.eq("brushing");          }, p_half);
	_pos->addTransition([]() { return token.eq("teeth brushing");    }, p_half);
	_pos->addTransition([]() { return token.eq("brush my teeth");    }, p_half);
	_pos->addTransition([]() { return token.eq("brushing my teeth"); }, p_half);

	_pos->addTransition([]() { return token.eq("lift");        }, p_alltheway);
	_pos->addTransition([]() { return token.eq("lifting");     }, p_alltheway);
	_pos->addTransition([]() { return token.eq("all the way"); }, p_alltheway);

	_pos->addTransition([]() { return token.eq("halfway");     }, p_halfway);
	_pos->addTransition([]() { return token.eq("half way");    }, p_halfway);
	_pos->addTransition([]() { return token.eq("halfway up");  }, p_halfway);
	_pos->addTransition([]() { return token.eq("half way up"); }, p_halfway);

	_pos->addTransition([]() { return token.eq("");       }, p_off);
	_pos->addTransition([]() { return token.eq("now");    }, p_off);
	_pos->addTransition([]() { return token.eq("off");    }, p_off);
	_pos->addTransition([]() { return token.eq("stop");   }, p_off);
	_pos->addTransition([]() { return token.eq("cancel"); }, p_off);

	_pos->addTransition(&t_all, _err);

	Serial.begin(115200);
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

	joint = NULL;

	token.set(cmd);

	parser.exec();

	return 1;
}

int setHeadState(String state) {
	if (Serial.isConnected()) {
		Serial.printlnf("%d Set head state: %s", (int)Time.now(), state.c_str());
	}
	return head.setState(state);
}
int setHeadAngle(String angle) {
	if (Serial.isConnected()) {
		Serial.printlnf("%d Set head angle: %s", (int)Time.now(), angle.c_str());
	}
	return head.addAngle(angle.toInt());
}

int setFeetState(String state) {
	if (Serial.isConnected()) {
		Serial.printlnf("%d Set feet state: %s", (int)Time.now(), state.c_str());
	}
	return feet.setState(state);
}
int setFeetAngle(String angle) {
	if (Serial.isConnected()) {
		Serial.printlnf("%d Set feet angle: %s", (int)Time.now(), angle.c_str());
	}
	return feet.addAngle(angle.toInt());
}
