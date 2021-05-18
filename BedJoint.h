#ifndef BedJoint_h
#define BedJoint_h

#include "Particle.h"
#include "AnalogSmoother.h"

enum STATE {
	OFF,
	RAISING,
	LOWERING
};

class BedJoint {
	public:
		BedJoint(int potPin, int up, int down);
		~BedJoint();
		void init();
		int  readingToAngle();
		int  currentAngle();
		void turnOff();
		void update();
		STATE setAngle(int angle);
		STATE addAngle(int angle);
		STATE setState(String state);
	private:
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

#endif
