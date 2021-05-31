#ifndef Tokenizer_h
#define Tokenizer_h

#include "Particle.h"

String _string = "";
String _token = "";
int _tokenIdx = 0;
int _lastTokenIdx = 0;

void resetTokenString(String str) {
	Serial.printlnf("New token string: '%s'", str.c_str());
	_string = str;
	_token = "";
	_tokenIdx = 0;
	_lastTokenIdx = 0;
}

void nextToken() {
	_lastTokenIdx = _tokenIdx;
	_tokenIdx = _string.indexOf(' ', _lastTokenIdx);
	_token = _string.substring(_lastTokenIdx, _tokenIdx);
	_token.trim();

	Serial.printlnf("Old idx: %d, New idx: %d, Token: %s",
		_lastTokenIdx,
		_tokenIdx,
		_token.c_str()
	);
}

void remainingTokenString() {
	_token = _string.substring(_lastTokenIdx);
}

void trimTokenEnd(String str) {
	if (_token.endsWith(str)) {
		_token = _token.substring(0, _token.length()-str.length());
		_token.trim();
	}
}

#endif
