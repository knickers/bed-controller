#ifndef Tokenizer_h
#define Tokenizer_h

#include "Particle.h"
#define NUMBER_LIST_LENGTH 11

class Tokenizer {
	private:
		int _len;
		int _idx;
		int _lastIdx;
		String _string;
		String _token;
		String _numbers[NUMBER_LIST_LENGTH] = {
			"zero",
			"one",
			"two",
			"three",
			"four",
			"five",
			"six",
			"seven",
			"eight",
			"nine",
			"ten"
		};
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
