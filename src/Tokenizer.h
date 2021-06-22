#ifndef Tokenizer_h
#define Tokenizer_h

#include "Particle.h"

class Tokenizer {
	private:
		int    _idx;
		int    _lastIdx;
		String _string;
		String _token;
	public:
		Tokenizer();
		void set(String str);
		void next();
		void remaining();
		void trimEnd(String str);
		bool eq(String str);
		bool isNumeric();
		float toFloat();
		String val();
};

#endif
