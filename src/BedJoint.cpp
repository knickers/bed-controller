#include "BedJoint.h"

BedJoint::BedJoint(int upPin, int downPin) {
	_state       = OFF;
	_pinUp       = upPin;
	_pinDown     = downPin;
	_target      = 0;                 // Milliseconds target to end movement
	_waitTill    = 0;
	_waitAngle   = 0;
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
	uint64_t now = System.millis();

	if (_waitTill && now >= _waitTill) {
		_waitTill = 0;
		addAngle(_waitAngle);
		_waitAngle = 0;
	}

	if (_state == OFF) {
		return;
	}

	// If the bed has been on long enough
	if (now >= _target || now >= _started+_max) {
		turnOff();
	}
}

void BedJoint::addAngle(int angle) {
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
}

void BedJoint::addDelay(int angle, uint32_t delay) {
	if (Serial.isConnected()) {
		Serial.printlnf("Delaying %d milliseconds before adding angle: '%d'",
			(int)delay,
			angle
		);
	}

	_waitTill = System.millis() + delay;
	_waitAngle = angle;
}

STATE BedJoint::getState() {
	return _state;
}
