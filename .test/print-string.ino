#include "Particle.h"

void setup() {
	Particle.function("printString", printString);

	Serial.begin(9600);

	while (!Serial) {
		; // Wait for serial port to connect
	}

	Serial.println("Waiting for commands...");
}

void loop() {
}

int printString(String cmd) {
	Serial.printlnf("%d Command: '%s'", (int)Time.now(), cmd.c_str());
	return 1;
}
