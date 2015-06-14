#include "io_file.h"

#include "u_endian.h"

File::~File()
{

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

InputError::InputError(const File &lump, string error)
{
	// TODO: add lump name and path.
	UNUSED(lump);
	mError = error;
}

string InputError::toString() const
{
	return mError;
}

UnexpectedEOFError::UnexpectedEOFError(const File &lump)
	:InputError(lump, "unexpected end of data")
{
}
