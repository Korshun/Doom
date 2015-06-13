#include "u_endian.h"

#define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

#ifdef __GNUC__
Sint16 be16(Sint16 value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	// TODO: remove the hack.
	//return __builtin_bswap16(value);
	return __builtin_bswap32(value<<16);
#else
	return value;
#endif
}
Sint32 be32(Sint32 value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return __builtin_bswap32(value);
#else
	return value;
#endif
}
Sint64 be64(Sint64 value)
{
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return __builtin_bswap64(value);
#else
	return value;
#endif
}

Sint16 le16(Sint16 value)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return __builtin_bswap16(value);
#else
	return value;
#endif
}
Sint32 le32(Sint32 value)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return __builtin_bswap32(value);
#else
	return value;
#endif
}
Sint64 le64(Sint64 value)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	return __builtin_bswap64(value);
#else
	return value;
#endif
}
#else
#error "Endianess conversion functions are not implemented for this compiler."
#endif
