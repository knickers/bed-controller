#include "Particle.h"
#include "AnalogSmoother.h"

BedJoint head(A0, 9, 2);
BedJoint feet(A1, 3, 4);

int ANGLE_UP = 10;
int ANGLE_TV = 10;

void setup() {
	Particle.function("moveHead", moveHead);
	Particle.function("moveFeet", moveFeet);
	Particle.function("headState", setHeadState);
	Particle.function("feetState", setFeetState);
	Particle.function("headAngle", setHeadAngle);
	Particle.function("feetAngle", setFeetAngle);
	Particle.variable("headAngle", getHeadAngle);
	Particle.variable("feetAngle", getFeetAngle);

	Serial.begin(115200);

	head.init();
	feet.init();

	RGB.control(true);
	RGB.brightness(0); // 0 - 255
	RGB.control(false);
}

void loop() {
	head.update();
	feet.update();
}

int setHeadAngle(String angle) {
	return head.setAngle(angle.toInt());
}
int setFeetAngle(String angle) {
	return feet.setAngle(angle.toInt());
}

int getHeadAngle() {
	return head.currentAngle();
}
int getFeetAngle() {
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

int move(String cmd, BedJoint &bed) {
	// Command starters:
	/*
	Move my bed...
	Lay my bed...
	Sit my bed...
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

	// REGEX: /^(up|down|to)\s*(by)?\s*(\d+)?\s*()?$/

	cmd.trim().toLowerCase();

	bool endsWithDegrees = cmd.endsWith("degrees");
	if (endsWithDegrees) {
		cmd = cmd.substring(0, cmd.length()-7).trim();
	}

	if (cmd.startsWith("to")) {
		cmd = cmd.substring(2).trim();

		if (endsWithDegrees) {
			return bed.setAngle(cmd.toInt());
		}
		else if (cmd == "tv") {
			return bed.setAngle(ANGLE_TV);
		}
	}
	else if (cmd.startsWith("up")) {
		cmd = cmd.substring(2).trim();

		if (cmd.startsWith("by")) {
			cmd = cmd.substring(2).trim();
		}

		if (cmd == "") {
			return bed.setAngle(ANGLE_UP);
		}
		else {
			return bed.addAngle(cmd.toInt());
		}
	}
	else if (cmd.startsWith("down")) {
		cmd = cmd.substring(4).trim();

		if (cmd.startsWith("by")) {
			cmd = cmd.substring(2).trim();
		}

		if (cmd == "") {
			return bed.setAngle(0);
		}
		else {
			return bed.addAngle(-cmd.toInt());
		}
	}

	Serial.printlnf("%d Error parsing command: '%s'", (int)Time.now(), cmd.c_str());
	return bed.setAngle(0);
}
int moveHead(String cmd) {
	return move(cmd, head);
}
int moveFeet(String cmd) {
	return move(cmd, feet);
}

void publish(String event, String data) {
	if (Serial.isConnected()) {
		Serial.printlnf("%d %s", (int)Time.now(), data.c_str());
	}
	if (Particle.connected()) {
		Particle.publish(event, data, PRIVATE);
	}
}
