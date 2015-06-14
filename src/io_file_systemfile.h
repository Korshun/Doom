#pragma once

#include "io_file.h"

// A file that is an actual file on the system.
class SystemFile : public File
{
public:
	SystemFile(const std::string &path);
	~SystemFile();

	void read(void *dest, index_t size) override;
	void seek(fileoffset_t offset, int mode) override;
	fileoffset_t tell() const override;

private:
	// Handle to opened file.
	FILE *mFile;
};
