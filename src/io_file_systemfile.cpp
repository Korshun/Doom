#include "io_file_systemfile.h"

SystemFile::SystemFile(const std::string &path)
{
	mFile = fopen(path.c_str(), "rb");
	if (mFile == NULL)
		throw InputError(*this, format("failed to open file: {0}", strerror(errno)));
}

SystemFile::~SystemFile()
{
	if (mFile != NULL)
		fclose(mFile);
}

void SystemFile::read(void *dest, index_t size)
{
	assert(size > 0);
	if (fread(dest, size, 1, mFile) != 1)
	{
		if (feof(mFile))
			throw UnexpectedEOFError(*this);
		else
			throw InputError(*this, format("failed to read {0} bytes: {1}", size, strerror(errno)));
	}
}

fileoffset_t SystemFile::tell() const
{
	assert(ftell(mFile) >= 0);
	return ftell(mFile);
}

void SystemFile::seek(fileoffset_t offset, int mode)
{
	if (fseek(mFile, offset, mode) != 0)
		throw InputError(*this, format("failed to move to offset {0}: {1}", offset, strerror(errno)));
}
