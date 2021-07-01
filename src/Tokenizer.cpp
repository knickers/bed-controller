#include "Tokenizer.h"

Tokenizer::Tokenizer() {
	_len = 0;
	_idx = 0;
	_lastIdx = 0;
	_string = "";
	_token = "";
}

void Tokenizer::set(String str) {
	if (Serial.isConnected()) {
		Serial.printlnf("New token string: '%s'", str.c_str());
	}
	_string = str;
	_token = "";
	_len = str.length();
	_idx = 0;
	_lastIdx = 0;
}

void Tokenizer::next() {
	if (_idx >= _len) {
		_token = "";
		return;
	}

	do {
		_lastIdx = _idx;
		_idx = _string.indexOf(' ', _lastIdx);

		if (_idx == -1) {
			_idx = _len;
		}

		_token = _string.substring(_lastIdx, _idx);
		_token.trim();

		if (_idx < _len) {
			_idx++;
		}
		else { // if (_idx >= _len) {
			break;
		}
	} while (_token == "");

	if (Serial.isConnected()) {
		Serial.printlnf("Old idx: '%d', New idx: '%d', Token: '%s'",
			_lastIdx,
			_idx,
			_token.c_str()
		);
	}
}

void Tokenizer::remaining() {
	_token = _string.substring(_lastIdx);
}

void Tokenizer::trimEnd(String str) {
	if (_token.endsWith(str)) {
		_token = _token.substring(0, _token.length()-str.length());
		_token.trim();
	}
}

bool Tokenizer::eq(String str) {
	return _token == str;
}

bool Tokenizer::isNumeric() {
	unsigned int stringLength = _token.length();

	for (unsigned int i = 0; i < 10; i++) {
		if (_token == _numbers[i]) {
			_token = String(i);
			return true;
		}
	}

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

String Tokenizer::val() {
	return _token;
}
