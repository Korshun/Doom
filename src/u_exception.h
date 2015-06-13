#pragma once

#include "common.h"

// Exception class for all exceptions in this engine.
// Derived classes' names should end with "Error".
class Exception
{
public:
	virtual ~Exception() {}
	virtual std::string toString() const = 0;
};

// Exception class for malformed or failed input.
class InputError : public Exception
{
public:
	InputError(std::string location, std::string error);
	std::string toString() const;

private:
	std::string mLocation;
	std::string mError;
};
