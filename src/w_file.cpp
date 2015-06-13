#include "w_file.h"

#include "u_endian.h"

File::File(const std::string &path)
{
	mPath = path;
	mFile = fopen(path.c_str(), "rb");
	if (mFile == nullptr)
	{
		throw InputError(mPath, format("failed to open file: {0}", strerror(errno)));
	}
}

File::~File()
{
	fclose(mFile);
}

void File::read(void *dest, index_t size)
{
	assert(size > 0);
	if ((index_t)fread(dest, size, 1, mFile) != size)
	{
		throw InputError(mPath, format("failed to read {0} bytes from the file: {1}", size, strerror(errno)));
	}
}

fileoffset_t File::tell()
{
	return ftell(mFile);
}

void File::seek(fileoffset_t offset, int mode)
{
	if (fseek(mFile, offset, mode) != 0)
	{
		throw InputError(mPath, format("failed to move to offset {0} in file: {1}", offset, strerror(errno)));
	}
}

byte File::read8()
{
	return read<byte>();
}

Sint16 File::read16le()
{
	return le16(read<Sint16>());
}
Sint32 File::read32le()
{
	return le32(read<Sint32>());
}
Sint64 File::read64le()
{
	return le64(read<Sint64>());
}

Sint16 File::read16be()
{
	return be16(read<Sint16>());
}
Sint32 File::read32be()
{
	return be32(read<Sint32>());
}
Sint64 File::read64be()
{
	return be64(read<Sint64>());
}


