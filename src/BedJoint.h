#ifndef BedJoint_h
#define BedJoint_h

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
		int _pinButton;
		int _angleTarget;
		int _angleMax;
		int _mapFromMin;
		int _mapFromMax;
		int _mapToMin;
		int _mapToMax;
		float _lastReading;
		float _tolerance;
		STATE _state;
		AnalogSmoother *_sensor;
	public:
		BedJoint(int sensorPin, int upPin, int downPin, int buttonPin);
		~BedJoint();
		void init();
		void setMapping(int fromMin, int fromMax, int toMin, int toMax);
		int  readingToAngle(float reading);
		int  currentAngle();
		void turnOff();
		void update();
		STATE setAngle(int angle);
		STATE addAngle(int angle);
		STATE setState(String state);
};

#endif
