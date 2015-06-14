#include "common.h"
#include "io_file_memoryfile.h"

MemoryFile::MemoryFile(index_t size)
{
	mSize = size;
	mData = new byte[mSize];
	mOffset = 0;
}

MemoryFile::~MemoryFile()
{
	delete mData;
}

byte* MemoryFile::data()
{
	return mData;
}

const byte* MemoryFile::data() const
{
	return mData;
}

void MemoryFile::read(void *dest, index_t size)
{
	assert(size > 0);

	if (mOffset + size > mSize)
		throw UnexpectedEOFError(*this);

	memcpy(dest, &mData[mOffset], (size_t)size);
	mOffset += size;
}

void MemoryFile::seek(fileoffset_t offset, int mode)
{
	switch (mode)
	{
	case SEEK_SET:
		assert(offset >= 0);
		if ((index_t)offset >= mSize)
			throw UnexpectedEOFError(*this);

		mOffset = (index_t)offset;
		break;

	case SEEK_CUR:
		seek(mOffset + offset, SEEK_SET);
		break;

	case SEEK_END:
		// FIXME: is this right?
		seek(mSize - 1 - offset, SEEK_SET);
		break;

	default:
		assert(0);
		break;
	}
}

fileoffset_t MemoryFile::tell() const
{
	return (fileoffset_t)mOffset;
}
