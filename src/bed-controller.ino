#include "Particle.h"
#include <BedJoint.h>

BedJoint head(A0, 0, 2, 1);
BedJoint feet(A1, 4, 6, 5);

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

	head.setMapping(150, 530, 0, 60);
	feet.setMapping(150, 530, 0, 45);

	RGB.control(true);
	RGB.brightness(0); // 0 - 255
	RGB.control(false);
}

void loop() {
	head.update();
	feet.update();
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

	// REGEX: /^(\w+)\s*b?y?\s*(\d*)\s*([\w ]*)$/

	cmd.trim();
	cmd.toLowerCase();

	bool endsWithDegrees = cmd.endsWith("degrees");
	if (endsWithDegrees) {
		cmd = cmd.substring(0, cmd.length()-7);
		cmd.trim();
	}

	if (cmd.startsWith("to")) {
		cmd = cmd.substring(2);
		cmd.trim();

		if (cmd == "tv") {
			return bed.setAngle(ANGLE_TV);
		}
		else { // if (endsWithDegrees) {
			return bed.setAngle(cmd.toInt());
		}
	}
	else if (cmd.startsWith("up")) {
		cmd = cmd.substring(2);
		cmd.trim();

		if (cmd.startsWith("by")) {
			cmd = cmd.substring(2);
			cmd.trim();
		}

		if (cmd == "") {
			return bed.setAngle(ANGLE_UP);
		}
		else {
			return bed.addAngle(cmd.toInt());
		}
	}
	else if (cmd.startsWith("down")) {
		cmd = cmd.substring(4);
		cmd.trim();

		if (cmd.startsWith("by")) {
			cmd = cmd.substring(2);
			cmd.trim();
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
	Serial.printlnf("%d Move head: %s", (int)Time.now(), cmd.c_str());
	return move(cmd, head);
}
int moveFeet(String cmd) {
	Serial.printlnf("%d Move feet: %s", (int)Time.now(), cmd.c_str());
	return move(cmd, feet);
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
	Serial.printlnf("%d Get head angle %s", (int)Time.now());
	return head.currentAngle();
}
int getFeetAngle() {
	Serial.printlnf("%d Get feet angle %s", (int)Time.now());
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

void publish(String event, String data) {
	if (Serial.isConnected()) {
		Serial.printlnf("%d %s", (int)Time.now(), data.c_str());
	}
	if (Particle.connected()) {
		Particle.publish(event, data, PRIVATE);
	}
}
