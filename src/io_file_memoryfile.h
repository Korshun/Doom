#pragma once

#include "io_file.h"

// In-memory file.
class MemoryFile : public File
{
public:
	// Create an uninitialized memory block of given size.
	MemoryFile(index_t size);

	// Deallocate the data.
	~MemoryFile();

	// Get pointer to data.
	byte* data();
	const byte* data() const;

	void read(void *dest, index_t size) override;
	void seek(fileoffset_t offset, int mode) override;
	fileoffset_t tell() const override;

private:
	// The data.
	byte *mData;

	// Size of the file.
	index_t mSize;

	// Current position in the lump.
	index_t mOffset;
};
