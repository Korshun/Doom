#pragma once

#include "common.h"

// Exception class for all exceptions in this engine.
// Derived classes' names should end with "Error".
class Exception
{
public:
	virtual ~Exception() {}
	virtual std::string toString() = 0;
};

// Exception class for malformed or failed input.
class InputError : Exception
{
public:
	InputError(std::string location, std::string error);
	std::string toString();

private:
	std::string mLocation;
	std::string mError;
};
