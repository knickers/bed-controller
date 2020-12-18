#include "AnalogSmoother.h"
#include "Particle.h"

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
	STATE state;
	AnalogSmoother pot;
};

struct BED head = {A0, 1, 2, 0, 50, 0, 270, OFF, AnalogSmoother(A0, 10)};
struct BED foot = {A1, 3, 4, 0, 45, 0, 270, OFF, AnalogSmoother(A1, 10)};

void setup() {
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

	RGB.control(true);
	RGB.brightness(0); // 0 - 255
	RGB.control(false);
}

int readAngle(BED bed) {
	return map(
		(int)round(bed.pot.read()),
		0, 1024,
		bed.mapLow, bed.mapHigh
	);
}

void checkAngle(BED bed) {
	if (bed.state == OFF) {
		return;
	}

	int angle = readAngle(bed);

	if (bed.state == RAISING) {
		if (angle >= bed.targetAngle) {
			// Turn off the up switch
			digitalWrite(bed.upPin, LOW);
		}
	}
	else if (bed.state == LOWERING) {
		if (angle <= bed.targetAngle) {
			// Turn off the down switch
			digitalWrite(bed.downPin, LOW);
		}
	}
}

void loop() {
	checkAngle(head);
	checkAngle(foot);
}

int setAngle(String angle, BED &bed) {
	Serial.printlnf("%d Set angle %s", (int)Time.now(), angle.c_str());

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

void publish(String event, String data) {
	if (Serial.isConnected()) {
		Serial.printlnf("%d %s", (int)Time.now(), data.c_str());
	}
	if (Particle.connected()) {
		Particle.publish(event, data, PRIVATE);
	}
}
