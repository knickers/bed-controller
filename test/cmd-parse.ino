//#include "Arduino.h"

void setup() {
	Serial.begin(9600);

	while (!Serial) {
		; // Wait for serial port to connect
	}
}

void loop() {
	if (Serial.available() > 0) {
		parseCommand(Serial.readString());
		Serial.println("");
	}
}

void parseCommand(String cmd) {
	Serial.print("Received: ");
	Serial.print(cmd);

	cmd.trim();
	cmd.toLowerCase();

	Serial.print("Trimmed and lowered: ");
	Serial.println(cmd);

	bool endsWithDegrees = cmd.endsWith("degrees");
	if (endsWithDegrees) {
		Serial.println("Removing 'degrees' from end of string.");
		cmd = cmd.substring(0, cmd.length()-7);
		cmd.trim();
	}

	if (cmd.startsWith("to")) {
		cmd = cmd.substring(2);
		cmd.trim();

		if (cmd == "tv") {
			Serial.println("TO TV");
			return;
			//return bed.setAngle(ANGLE_TV);
		}
		else { //if (endsWithDegrees) {
			Serial.print("TO ");
			Serial.print(cmd.toInt());
			Serial.println(" DEGREES");
			return;
			//return bed.setAngle(cmd.toInt());
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
			Serial.println("UP TO PRESET ANGLE");
			return;
			//return bed.setAngle(ANGLE_UP);
		}
		else {
			Serial.print("UP ");
			Serial.print(cmd.toInt());
			Serial.println(" DEGREES");
			return;
			//return bed.addAngle(cmd.toInt());
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
			Serial.println("DOWN (all the way)");
			return;
			//return bed.setAngle(0);
		}
		else {
			Serial.print("DOWN ");
			Serial.print(cmd.toInt());
			Serial.println(" DEGREES");
			return;
			//return bed.addAngle(-cmd.toInt());
		}
	}

	Serial.print("Error parsing command: ");
	Serial.println(cmd);
}
