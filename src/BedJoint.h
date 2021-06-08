#ifndef BedJoint_h
#define BedJoint_h

#include "AnalogSmoother.h"
#include "Debounce.h"

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
		int _angleMax;
		int _angleTarget;
		int _mapFromMin;
		int _mapFromMax;
		int _mapToMin;
		int _mapToMax;
		float _lastReading;
		float _tolerance;
		STATE _state;
		AnalogSmoother* _sensor;
		Debounce* _endstop;
	public:
		BedJoint(int sensorPin, int upPin, int downPin, int buttonPin);
		~BedJoint();
		void  init(int fromMin, int fromMax, int toMin, int toMax);
		int   readingToAngle(float reading);
		int   currentAngle();
		void  turnOff();
		void  update();
		float lastReading();
		STATE setAngle(int angle);
		STATE addAngle(int angle);
		STATE setState(String state);
};

#endif
