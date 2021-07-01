#ifndef Tokenizer_h
#define Tokenizer_h

#include "Particle.h"

class Tokenizer {
	private:
		int _len;
		int _idx;
		int _lastIdx;
		String _string;
		String _token;
		String _numbers[10] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
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
