#ifndef BedJoint_h
#define BedJoint_h

#include "Particle.h"

enum STATE {
	OFF,
	RAISING,
	LOWERING
};

class BedJoint {
	private:
		int _pinUp;
		int _pinDown;
		uint64_t _max;          // Maximum milliseconds to remain on
		uint64_t _started;      // Milliseconds when the movement started
		uint64_t _target;       // Milliseconds when the movement is to stop
		uint64_t _rampUp;       // Milliseconds to ramp up to full speed
		uint64_t _rampDown;     // Milliseconds to coast down to stop
		uint64_t _millisAngle;  // Milliseconds per angle in degrees
		STATE _state;
	public:
		BedJoint(int upPin, int downPin);
		void  turnOff();
		void  update();
		STATE addAngle(int angle);
		STATE setState(String state);
};

#endif
