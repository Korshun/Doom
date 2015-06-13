#pragma once

#include "common.h"

// Type for file offsets and sizes.
// Should be 64-bit even on 32-bit systems.
// If it is 32-bit, files over 2GB are not supported.
typedef long long fileoffset_t;

// Class for file reading.
class File
{
public:
	File(const std::string &path);
	~File();

	// Read `size` bytes from file to `dest`.
	// If a read fails, the position in the file becomes undefined
	// and InputError gets thrown.
	void read(void *dest, index_t size);

	// Read one variable of type `T` from file.
	template<typename T> T read()
	{
		T t;
		read(&t, sizeof t);
		return t;
	}

	// Read fixed-size integer from file.
	// Supports Litte-Endian and Big-Endian integers.
	byte   read8();
	Sint16 read16le();
	Sint32 read32le();
	Sint64 read64le();
	Sint16 read16be();
	Sint32 read32be();
	Sint64 read64be();

	// Get current position in the file.
	fileoffset_t tell();

	// Set current position in the file.
	// Mode must be one of the following:
	// SEEK_SET
	// SEEK_CUR
	// SEEK_END
	// If the seek fails, InputError is thrown.
	void seek(fileoffset_t offset, int mode = SEEK_SET);

private:
	std::string mPath;
	FILE *mFile;
};
