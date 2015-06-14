#pragma once

#include "common.h"

class File;

// Class for wad file reading.
class Wad
{
public:
	Wad(unique_ptr<File> lump);

	// Get number of lumps.
	index_t numLumps() const;

	// Get lump name and size.
	std::string lumpName(index_t lumpnum) const;
	index_t lumpSize(index_t lumpnum) const;

	// Read lump to memory.
	void readLump(index_t lumpnum, void *dest);

private:
	struct LumpInfo
	{
		std::string name;
		index_t size;
		index_t dataOffset;
	};

	unique_ptr<File> mFile;
	std::vector<LumpInfo> mLumps;
	index_t mNumLumps;
};

// Legacy interface.
void    W_InitMultipleFiles (char** filenames);

int	W_CheckNumForName (char* name);
int	W_GetNumForName (char* name);

int	W_LumpLength (int lump);
const char* W_LumpName (int lump);
void    W_ReadLump (int lump, void *dest);

void*	W_CacheLumpNum (int lump, int tag);
void*	W_CacheLumpName (char* name, int tag);
