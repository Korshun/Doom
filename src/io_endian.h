#pragma once

#include "common.h"

// Endianness conversion functions.

// Convert from Big-Endian.
Sint16 be16(Sint16 value);
Sint32 be32(Sint32 value);
Sint64 be64(Sint64 value);

// Convert from Little-Endian.
Sint16 le16(Sint16 value);
Sint32 le32(Sint32 value);
Sint64 le64(Sint64 value);
