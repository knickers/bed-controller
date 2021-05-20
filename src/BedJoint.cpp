#include "BedJoint.h"

BedJoint::BedJoint(int sensorPin, int upPin, int downPin) {
	_pinUp   = upPin;
	_pinDown = downPin;
	_angleTarget = 0;
	_angleMax    = 60;
	_mapLow  = 0;
	_mapHigh = 270;
	_lastReading = 0;
	_tolerance = 5;
	_state = OFF;

	pinMode(sensorPin,  INPUT);
	pinMode(_pinUp,   OUTPUT);
	pinMode(_pinDown, OUTPUT);

	_sensor = AnalogSmoother(sensorPin, 10);
}

BedJoint::~BedJoint() {
	// TODO do I need to call AnalogSmoother destructor?
}

void BedJoint::init() {
	_sensor.fill();
	_lastReading = _sensor.read();
}

float BedJoint::read() {
	_sensor.read();
}

int BedJoint::readingToAngle(float reading) {
	return map((int)round(reading), 0, 1024, _mapLow, _mapHigh);
}

int BedJoint::currentAngle() {
	return readingToAngle(_lastReading);
}

void BedJoint::turnOff(BED &bed) {
	_state = OFF;
	// Turn switches off
	digitalWrite(_pinUp, LOW);
	digitalWrite(_pinDown, LOW);
}

void BedJoint::update() {
	if (_state == OFF) {
		return;
	}

	float reading = _sensor.read();
	int angle = currentAngle();

	if (_state == RAISING) {
		// If the bed has reached it's target
		if (angle >= _angleTarget || angle >= _angleMax) {
			turnOff();
		}
		// If the bed hasn't moved since last time
		if (reading <= _lastReading + _tolerance) {
			//turnOff(bed);
		}
	}
	else if (_state == LOWERING) {
		// If the bed has reached it's target
		if (angle <= _angleTarget || angle <= 0) {
			turnOff(bed);
		}
		// If the bed hasn't moved since last time
		if (reading >= _lastReading - _tolerance) {
			//turnOff(bed);
		}
	}

	_lastReading = reading;
}

STATE BedJoint::setAngle(int angle) {
	Serial.printlnf("%d Set angle to: %s", (int)Time.now(), angle.c_str());

	turnOff(bed);

	_angleTarget = angle;
	int currentAngle = readAngle();

	if (_angleTarget < 0) {
		_angleTarget = 0;
	}
	else if (_angleTarget > _angleMax) {
		_angleTarget = _angleMax;
	}

	if (_angleTarget > currentAngle) {
		_state = RAISING;
		// Turn on the up switch
		digitalWrite(_pinUp, HIGH);
	}
	else if (_angleTarget < currentAngle) {
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
		digitalWrite(_pinUp, LOW);
		digitalWrite(_pinDown, LOW);
	}
	else if (state == "raise") {
		_state = RAISING;
		digitalWrite(_pinUp, HIGH); // Turn on the up switch
		digitalWrite(_pinDown, LOW);
	}
	else if (state == "lower") {
		_state = LOWERING;
		digitalWrite(_pinUp, LOW);
		digitalWrite(_pinDown, HIGH); // Turn on the down switch
	}

	return _state;
}
