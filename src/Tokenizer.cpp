#include "Tokenizer.h"

Tokenizer::Tokenizer() {
}

void Tokenizer::set(char* str) {
	if (Serial.isConnected()) {
		Serial.printlnf("New token string: '%s'", str.c_str());
	}
	_string = str;
	_token = NULL;
}

void Tokenizer::next() {
	if (_token == NULL) {
		_token = strtok(_string, _sep);
	}
	else {
		_token = strtok(NULL, _sep);
	}

	if (Serial.isConnected()) {
		Serial.printlnf("Token: '%s' String: '%s'", _token, _string);
	}
}

void Tokenizer::remaining() {
	_token = _string;
}

void Tokenizer::trimEnd(char* str) {
	if (_token.endsWith(str)) {
		_token = _token.substring(0, _token.length()-str.length());
		_token.trim();
	}
}

bool Tokenizer::eq(char* str) {
	return _token == str;
}

bool Tokenizer::isNumeric() {
	unsigned int stringLength = _token.length();

	if (stringLength == 0) {
		return false;
	}

	bool seenDecimal = false;

	for (unsigned int i = 0; i < stringLength; i++) {
		if (isDigit(_token.charAt(i))) {
			continue;
		}

		if (_token.charAt(i) == '.') {
			if (seenDecimal) {
				return false;
			}
			seenDecimal = true;
			continue;
		}

		return false;
	}

	return true;
}

float Tokenizer::toFloat() {
	return _token.toFloat();
}

char* Tokenizer::val() {
	return _token;
}
