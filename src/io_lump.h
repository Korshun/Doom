#pragma once

#include "common.h"

// Type for file offsets and sizes.
// Should be 64-bit even on 32-bit systems.
// If it is 32-bit, files over 2GB are not supported.
// TODO: redefine to support 64-bit offsets.
typedef long int fileoffset_t;

// Abstract file-like object class.
// TODO: add lump name and path.
class Lump
{
public:
	virtual ~Lump();

	// Read `size` bytes from file to `dest`.
	// If a read fails, the position in the file becomes undefined
	// and InputError gets thrown.
	virtual void read(void *dest, index_t size) = 0;

	// Read one variable of type `T` from file.
	template<typename T> T read()
	{
		T t;
		read(&t, sizeof t);
		return t;
	}

	// Read fixed-size integer from file.
	// Supports Little-Endian and Big-Endian integers.
	byte   read8();
	Sint16 read16le();
	Sint32 read32le();
	Sint64 read64le();
	Sint16 read16be();
	Sint32 read32be();
	Sint64 read64be();

	// Get current position in the file.
	virtual fileoffset_t tell() const = 0;

	// Set current position in the file.
	// Mode must be one of the following:
	// SEEK_SET
	// SEEK_CUR
	// SEEK_END
	// If the seek fails, InputError is thrown.
	virtual void seek(fileoffset_t offset, int mode = SEEK_SET) = 0;
};

// Exception thrown when a read or seek fails.
class InputError : Exception
{
public:
	InputError(const Lump &lump, std::string error);
	string toString() const override;

private:
	// Error message.
	string mError;
};

// Exception thrown when trying to read over the end of file.
class UnexpectedEOFError : InputError
{
public:
	UnexpectedEOFError(const Lump &lump);
};

// Exception
