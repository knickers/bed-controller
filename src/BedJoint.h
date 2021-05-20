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
	private:
		int _pinUp;
		int _pinDown;
		int _angleTarget;
		int _angleMax;
		int _mapLow;
		int _mapHigh;
		float _lastReading;
		float _tolerance;
		STATE _state;
		AnalogSmoother _sensor;
	public:
		BedJoint(int sensorPin, int upPin, int downPin);
		~BedJoint();
		void init();
		int  readingToAngle();
		int  currentAngle();
		void turnOff();
		void update();
		STATE setAngle(int angle);
		STATE addAngle(int angle);
		STATE setState(String state);
};

#endif
