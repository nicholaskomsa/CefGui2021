#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <exception>

#include "exception.h"


Exception::Exception(const std::string& file, int line) {

	message = "EXCEPT: " + file + "; " + std::to_string(line) + "; ";
}

Exception::~Exception() {
	message.clear();
	message.shrink_to_fit();
}

void Exception::writeDebugString() {
	OutputDebugStringA(message.c_str());
}

void Exception::showMessage(const std::string& msg) {
	OutputDebugStringA(msg.c_str());
	MessageBoxA(NULL, msg.c_str(), "An Exception has occured", MB_OK);
}

void Exception::showMessage() {
	Exception::showMessage(message);
}