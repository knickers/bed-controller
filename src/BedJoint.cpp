#include "BedJoint.h"

BedJoint::BedJoint(int upPin, int downPin) {
	_state       = OFF;
	_pinUp       = upPin;
	_pinDown     = downPin;
	_target      = 0;                 // Milliseconds target to end movement
	_rampUp      = 100;               // Milliseconds to ramp up to full speed
	_rampDown    = 500;               // Milliseconds to coast down to stop
	_millisAngle = 305;               // Milliseconds per angle in degrees
	_max         = _millisAngle * 60; // Maximum milliseconds to remain on
}

void BedJoint::turnOff() {
	_state = OFF;
	// Turn switches off
	digitalWrite(_pinUp, LOW);
	digitalWrite(_pinDown, LOW);
}

void BedJoint::update() {
	if (_state == OFF) {
		return;
	}

	uint64_t now = System.millis();

	// If the bed has been on long enough
	if (now >= _target || now >= _started+_max) {
		turnOff();
	}
}

STATE BedJoint::addAngle(int angle) {
	if (Serial.isConnected()) {
		Serial.printlnf("Adding angle: '%d'", angle);
	}

	turnOff();

	_started = System.millis();

	// TODO account for ramp up and down
	_target = abs(angle)*_millisAngle + _started;

	if (angle > 0) {
		_state = RAISING;
		// Turn on the up switch
		digitalWrite(_pinUp, HIGH);
	}
	else if (angle < 0) {
		_state = LOWERING;
		// Turn on the down switch
		digitalWrite(_pinDown, HIGH);
	}
	/*
	else if (angle == 0) {
		turnOff();
	}
	*/

	return _state;
}

STATE BedJoint::setState(String state) {
	state.trim();
	state.toLowerCase();

	if (state == "off") {
		_state = OFF;
		digitalWrite(_pinUp,   LOW);
		digitalWrite(_pinDown, LOW);
	}
	else if (state == "raise") {
		_state = RAISING;
		digitalWrite(_pinDown, LOW);
		digitalWrite(_pinUp,   HIGH); // Turn on the up switch
	}
	else if (state == "lower") {
		_state = LOWERING;
		digitalWrite(_pinUp,   LOW);
		digitalWrite(_pinDown, HIGH); // Turn on the down switch
	}

	return _state;
}
