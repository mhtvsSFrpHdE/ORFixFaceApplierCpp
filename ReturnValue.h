#pragma once

struct ReturnNone {};

template <typename T = ReturnNone>
struct ReturnValue {
	bool success = false;
	T returnBody;

	ReturnValue() {};

	ReturnValue(bool _success, T _returnBody) {
		success = _success;
		returnBody = _returnBody;
	}

	ReturnValue(bool _success) {
		success = _success;
		returnBody = T();
	}
};

/* Return int example
ReturnValue<int> ReturnIntExample() {
	ReturnValue<int> result(true, 3);

	return result;
}
ReturnValue<int> ReturnIntDemo() {
	ReturnValue<int> result(true, 3);

	return result;
}
*/

/* Return multipe type example
#include <string>
struct ReturnComplex {
	int errorCode = 0;
	std::wstring errorMessage = L"";

	ReturnComplex() {}

	ReturnComplex(int _errorCode, std::wstring _errorMessage) {
		errorCode = _errorCode;
		errorMessage = _errorMessage;
	}
};
ReturnValue<ReturnComplex> ReturnComplexDemo() {
	ReturnValue<ReturnComplex> result(true, ReturnComplex(10, L"Error message in complex object"));

	return result;
}
*/

/* Return only success true or false example
ReturnValue<> ReturnNothingDemo() {
	ReturnValue<> result(true);

	return result;
}
*/
