#include "io_lump.h"

#include "u_endian.h"

Lump::~Lump()
{

}

byte Lump::read8()
{
	return read<byte>();
}

Sint16 Lump::read16le()
{
	return le16(read<Sint16>());
}
Sint32 Lump::read32le()
{
	return le32(read<Sint32>());
}
Sint64 Lump::read64le()
{
	return le64(read<Sint64>());
}

Sint16 Lump::read16be()
{
	return be16(read<Sint16>());
}
Sint32 Lump::read32be()
{
	return be32(read<Sint32>());
}
Sint64 Lump::read64be()
{
	return be64(read<Sint64>());
}

InputError::InputError(const Lump &lump, string error)
{
	// TODO: add lump name and path.
	UNUSED(lump);
	mError = error;
}

string InputError::toString() const
{
	return mError;
}

UnexpectedEOFError::UnexpectedEOFError(const Lump &lump)
	:InputError(lump, "unexpected end of data")
{
}
