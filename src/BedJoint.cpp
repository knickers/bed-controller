#include "BedJoint.h"

BedJoint::BedJoint(int potPin, int up, int down) {
	upPin = up;
	downPin = down;
	targetAngle = 0;
	maxAngle = 60;
	mapLow = 0;
	mapHigh = 270;
	lastReading = 0;
	tolerance = 5;
	state = OFF;

	pinMode(potPin,  INPUT);
	pinMode(upPin,   OUTPUT);
	pinMode(downPin, OUTPUT);

	pot = AnalogSmoother(potPin, 10);
}

BedJoint::~BedJoint() {
	// TODO do I need to call AnalogSmoother destructor?
}

void BedJoint::init() {
	pot.fill();
	lastReading = pot.read();
}

float BedJoint::read() {
	pot.read();
}

int BedJoint::readingToAngle(float reading) {
	return map((int)round(reading), 0, 1024, mapLow, mapHigh);
}

int BedJoint::currentAngle() {
	return readingToAngle(lastReading);
}

void BedJoint::turnOff(BED &bed) {
	state = OFF;
	// Turn switches off
	digitalWrite(upPin, LOW);
	digitalWrite(downPin, LOW);
}

void BedJoint::update() {
	if (state == OFF) {
		return;
	}

	float reading = pot.read();
	int angle = currentAngle();

	if (state == RAISING) {
		// If the bed has reached it's target
		if (angle >= targetAngle || angle >= maxAngle) {
			turnOff();
		}
		// If the bed hasn't moved since last time
		if (reading <= lastReading + tolerance) {
			//turnOff(bed);
		}
	}
	else if (state == LOWERING) {
		// If the bed has reached it's target
		if (angle <= targetAngle || angle <= 0) {
			turnOff(bed);
		}
		// If the bed hasn't moved since last time
		if (reading >= lastReading - tolerance) {
			//turnOff(bed);
		}
	}

	lastReading = reading;
}

STATE BedJoint::setAngle(int angle) {
	Serial.printlnf("%d Set angle to: %s", (int)Time.now(), angle.c_str());

	turnOff(bed);

	targetAngle = angle;
	int currentAngle = readAngle();

	if (targetAngle < 0) {
		targetAngle = 0;
	}
	else if (targetAngle > maxAngle) {
		targetAngle = maxAngle;
	}

	if (targetAngle > currentAngle) {
		state = RAISING;
		// Turn on the up switch
		digitalWrite(upPin, HIGH);
	}
	else if (targetAngle < currentAngle) {
		state = LOWERING;
		// Turn on the down switch
		digitalWrite(downPin, HIGH);
	}

	return state;
}

STATE BedJoint::addAngle(int angle) {
	return setAngle(targetAngle + angle);
}

STATE BedJoint::setState(String state) {
	state.trim().toLowerCase();

	if (state == "off") {
		state = OFF;
		digitalWrite(upPin, LOW);
		digitalWrite(downPin, LOW);
	}
	else if (state == "raise") {
		state = RAISING;
		digitalWrite(upPin, HIGH); // Turn on the up switch
		digitalWrite(downPin, LOW);
	}
	else if (state == "lower") {
		state = LOWERING;
		digitalWrite(upPin, LOW);
		digitalWrite(downPin, HIGH); // Turn on the down switch
	}

	return state;
}
