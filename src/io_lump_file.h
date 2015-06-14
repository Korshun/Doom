#pragma once

#include "io_lump.h"

// Lump that is an actual file on the system.
class FileLump : public Lump
{
public:
	FileLump(const std::string &path);
	~FileLump();

	void read(void *dest, index_t size) override;
	void seek(fileoffset_t offset, int mode) override;
	fileoffset_t tell() const override;

private:
	// Handle to opened file.
	FILE *mFile;
};
