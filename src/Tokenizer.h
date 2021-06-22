#ifndef Tokenizer_h
#define Tokenizer_h

#include "Particle.h"

class Tokenizer {
	private:
		char _string[64];
		char* _token;
		char* _sep = " \t";
	public:
		Tokenizer();
		void set(char* str);
		void next();
		void remaining();
		void trimEnd(char* str);
		bool eq(char* str);
		bool isNumeric();
		float toFloat();
		char* val();
};

#endif
