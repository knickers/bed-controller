#include "BedJoint.h"

BedJoint::BedJoint(int sensorPin, int upPin, int downPin, int buttonPin) {
	_pinUp       = upPin;
	_pinDown     = downPin;
	_angleTarget = 0;
	_angleMax    = 60;
	_mapFromMin  = 0;
	_mapFromMax  = 1024;
	_mapToMin    = 0;
	_mapToMax    = 270;
	_lastReading = 0;
	_tolerance   = 5;
	_state       = OFF;

	pinMode(_pinUp,    OUTPUT);
	pinMode(_pinDown,  OUTPUT);
	pinMode(buttonPin, INPUT_PULLDOWN);
	pinMode(sensorPin, INPUT);

	_sensor = new AnalogSmoother(sensorPin, 10);
	_endstop = new Debounce();
	_endstop->attach(buttonPin);
}

BedJoint::~BedJoint() {
	delete _sensor;
	delete _endstop;
}

void BedJoint::init(int fromMin, int fromMax, int toMin, int toMax) {
	_sensor->fill();
	_lastReading = _sensor->read();

	_mapFromMin = fromMin;
	_mapFromMax = fromMax;
	_mapToMin   = toMin;
	_mapToMax   = toMax;
}

int BedJoint::readingToAngle(float reading) {
	// Mega: 5V    // Mega: 3V3  // Argon
	// Min : 155   // Min : 150  // Min : 1450
	// None: 516   // None: 340  // None:
	// Max : 888   // Max : 530  // Max :
	return map((int)round(reading), _mapFromMin, _mapFromMax, _mapToMin, _mapToMax);
}

float BedJoint::lastReading() {
	return _lastReading;
}

int BedJoint::currentAngle() {
	return readingToAngle(_lastReading);
}

void BedJoint::turnOff() {
	_state = OFF;
	// Turn switches off
	digitalWrite(_pinUp, LOW);
	digitalWrite(_pinDown, LOW);
}

void BedJoint::update() {
	_endstop->update();

	if (_state == OFF) {
		//return;
	}

	float reading = _sensor->read();
	int angle = currentAngle();

	if (_state == RAISING) {
		// If the bed has reached it's target
		if (angle >= _angleTarget || angle >= _angleMax) {
			turnOff();
		}

		// If the bed hasn't moved since last time
		/*
		if (reading <= _lastReading + _tolerance) {
			turnOff();
		}
		*/
	}
	else if (_state == LOWERING) {
		// If the bed has reached it's target
		if (angle <= _angleTarget || angle <= 0) {
			turnOff();
		}

		// If the endstop switch is triggered
		if (_endstop->rose()) {
			turnOff();
			// update mapping values to new zero
		}

		// If the bed hasn't moved since last time
		/*
		if (reading >= _lastReading - _tolerance) {
			turnOff();
		}
		*/
	}

	_lastReading = reading;
}

STATE BedJoint::setAngle(int angle) {
	turnOff();

	_angleTarget = angle;
	int current = currentAngle();

	if (_angleTarget < 0) {
		_angleTarget = 0;
	}
	else if (_angleTarget > _angleMax) {
		_angleTarget = _angleMax;
	}

	if (_angleTarget > current) {
		_state = RAISING;
		// Turn on the up switch
		digitalWrite(_pinUp, HIGH);
	}
	else if (_angleTarget < current) {
		_state = LOWERING;
		// Turn on the down switch
		digitalWrite(_pinDown, HIGH);
	}

	return _state;
}

STATE BedJoint::addAngle(int angle) {
	return setAngle(_angleTarget + angle);
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
