#pragma once

#include "common.h"

// Exception class for all exceptions in this engine.
// Derived classes' names should end with "Error".
//
// std::exception sucks, what() returns const char* instead of string.
// Why should I deal with error message memory management in exception handlers?
class Exception
{
public:
	virtual ~Exception() {}
	virtual string toString() const = 0;
};
