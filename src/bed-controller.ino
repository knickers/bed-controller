#include "Particle.h"
#include "AnalogSmoother.h"

enum STATE {
	OFF,
	RAISING,
	LOWERING
};

struct BED {
	int potPin;
	int upPin;
	int downPin;
	int targetAngle;
	int maxAngle;
	int mapLow;
	int mapHigh;
	float lastReading;
	float tolerance;
	STATE state;
	AnalogSmoother pot;
};

struct BED head = {A0, 9, 2, 0, 50, 0, 270, 0, 5, OFF, AnalogSmoother(A0, 10)};
struct BED foot = {A1, 3, 4, 0, 45, 0, 270, 0, 5, OFF, AnalogSmoother(A1, 10)};

void setup() {
	Particle.function("headState", setHeadState);
	Particle.function("footState", setFootState);
	Particle.function("headAngle", setHeadAngle);
	Particle.function("footAngle", setFootAngle);
	Particle.variable("headAngle", getHeadAngle);
	Particle.variable("footAngle", getFootAngle);

	Serial.begin(115200);

	pinMode(head.potPin,  INPUT);
	pinMode(foot.potPin,  INPUT);
	pinMode(head.upPin,   OUTPUT);
	pinMode(foot.upPin,   OUTPUT);
	pinMode(head.downPin, OUTPUT);
	pinMode(foot.downPin, OUTPUT);

	head.pot.fill();
	foot.pot.fill();

	head.lastReading = head.pot.read();
	foot.lastReading = foot.pot.read();

	RGB.control(true);
	RGB.brightness(0); // 0 - 255
	RGB.control(false);
}

int getAngle(float value, int mapLow, int mapHigh) {
	return map((int)round(value), 0, 1024, mapLow, mapHigh);
}
int readAngle(BED &bed) {
	return getAngle(bed.pot.read(), bed.mapLow, bed.mapHigh);
}

void turnOff(BED &bed) {
	bed.state = OFF;
	// Turn switches off
	digitalWrite(bed.upPin, LOW);
	digitalWrite(bed.downPin, LOW);
}

void checkAngle(BED &bed) {
	if (bed.state == OFF) {
		return;
	}

	float value = bed.pot.read();
	int angle = getAngle(value, bed.mapLow, bed.mapHigh);

	if (bed.state == RAISING) {
		// If the bed has reached it's target
		if (angle >= bed.targetAngle) {
			turnOff(bed);
		}
		// If the bed hasn't moved since last time
		if (value <= bed.lastReading + bed.tolerance) {
			//turnOff(bed);
		}
	}
	else if (bed.state == LOWERING) {
		// If the bed has reached it's target
		if (angle <= bed.targetAngle) {
			turnOff(bed);
		}
		// If the bed hasn't moved since last time
		if (value >= bed.lastReading - bed.tolerance) {
			//turnOff(bed);
		}
	}

	bed.lastReading = value;
}

void loop() {
	checkAngle(head);
	checkAngle(foot);
}

int setAngle(String angle, BED &bed) {
	Serial.printlnf("%d Set angle %s", (int)Time.now(), angle.c_str());

	turnOff(bed);

	bed.targetAngle = angle.toInt();
	int currentAngle = readAngle(bed);

	if (bed.targetAngle < 0) {
		bed.targetAngle = 0;
	}
	else if (bed.targetAngle > bed.maxAngle) {
		bed.targetAngle = bed.maxAngle;
	}

	if (bed.targetAngle > currentAngle) {
		bed.state = RAISING;
		// Turn on the up switch
		digitalWrite(bed.upPin, HIGH);
	}
	else if (bed.targetAngle < currentAngle) {
		bed.state = LOWERING;
		// Turn on the down switch
		digitalWrite(bed.downPin, HIGH);
	}

	return bed.state;
}
int setHeadAngle(String angle) {
	return setAngle(angle, head);
}
int setFootAngle(String angle) {
	return setAngle(angle, foot);
}

int getHeadAngle() {
	return readAngle(head);
}
int getFootAngle() {
	return readAngle(foot);
}

int setState(String state, BED &bed) {
	state.toLowerCase();

	if (state == "OFF") {
		bed.state = OFF;
		digitalWrite(bed.upPin, LOW);
		digitalWrite(bed.downPin, LOW);
	}
	else if (state == "raise") {
		bed.state = RAISING;
		digitalWrite(bed.upPin, HIGH); // Turn on the up switch
		digitalWrite(bed.downPin, LOW);
	}
	else if (state == "lower") {
		bed.state = LOWERING;
		digitalWrite(bed.upPin, LOW);
		digitalWrite(bed.downPin, HIGH); // Turn on the down switch
	}

	return bed.state;
}
int setHeadState(String state) {
	Serial.printlnf("%d Set head state %s", (int)Time.now(), state.c_str());
	return setState(state, head);
}
int setFootState(String state) {
	Serial.printlnf("%d Set foot state %s", (int)Time.now(), state.c_str());
	return setState(state, foot);
}

void publish(String event, String data) {
	if (Serial.isConnected()) {
		Serial.printlnf("%d %s", (int)Time.now(), data.c_str());
	}
	if (Particle.connected()) {
		Particle.publish(event, data, PRIVATE);
	}
}
