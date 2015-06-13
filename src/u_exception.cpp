#include "u_exception.h"

InputError::InputError(std::string location, std::string error)
{
	mLocation = location;
	mError = error;
}

std::string InputError::toString()
{
	return mLocation + ": " + mError;
}
