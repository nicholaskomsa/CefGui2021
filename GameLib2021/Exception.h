#pragma once

#include <string>


struct Exception	: public std::exception {
	std::string message;

	Exception(const std::string& file, int line);
	~Exception();

	Exception& operator<<(const char* value) {
		return *this << std::string(value);
	}
	Exception& operator<<(char* value) {
		return *this << std::string(value);
	}
	Exception& operator<<(const std::string& value) {
		message = message + value;
		return *this;
	}

	template<typename T>
	Exception& operator<<(const T& value) {
		message = message + std::to_string(value);
		return *this;
	}

	void writeDebugString();


	static void showMessage(const std::string& message);
	void showMessage();
};

#define EXCEPT throw Exception( __FILE__, __LINE__ )
