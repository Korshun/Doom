#include "w_wad.h"

#include "w_archive.h"
#include "io_file_systemfile.h"
#include "io_file_memoryfile.h"

Wad::Wad(unique_ptr<File> lump)
{
	mFile = std::move(lump);

	char identification[4];
	fileoffset_t dirOffset;

	mFile->read(identification, sizeof identification);
	mNumLumps = (index_t)mFile->read32le();
	dirOffset = (fileoffset_t)mFile->read32le();

	if (strncmp(identification, "IWAD", 4) && strncmp(identification, "PWAD", 4))
	{
		throw InputError(*mFile, "not a wad file");
	}

	if (mNumLumps < 0)
	{
		throw InputError(*mFile, "negative number of lumps");
	}

	if (dirOffset < 0)
	{
		throw InputError(*mFile, "negative lump directory offset");
	}

	mFile->seek(dirOffset);

	for (index_t i = 0; i < mNumLumps; i++)
	{
		try
		{
			LumpInfo info;
			char name[9];

			info.dataOffset = (fileoffset_t)mFile->read32le();
			info.size = (index_t)mFile->read32le();
			mFile->read(name, 8);

			name[8] = '\0';
			info.name = std::string(name);

			if (info.dataOffset < 0)
			{
				throw InputError(*mFile, format("lump {0} has negative data offset", i));
			}

			if (info.size < 0)
			{
				throw InputError(*mFile, format("lump {0} has negative size", i));
			}

			mLumps.push_back(info);
		}
		catch (InputError &e)
		{
			mNumLumps--;
			printf("%s\n", e.toString().c_str());
			continue;
		}
	}
}

index_t Wad::numLumps() const
{
	return mNumLumps;
}

std::string Wad::lumpName(index_t lumpnum) const
{
	assert(lumpnum >= 0 && lumpnum < numLumps());
	return mLumps[lumpnum].name;
}

index_t Wad::lumpSize(index_t lumpnum) const
{
	assert(lumpnum >= 0 && lumpnum < numLumps());
	return mLumps[lumpnum].size;
}

void Wad::readLump(index_t lumpnum, void *dest)
{
	mFile->seek(mLumps[lumpnum].dataOffset);
	mFile->read(dest, mLumps[lumpnum].size);
}

// Legacy stuff.

// Only one wad for now.

Wad *thewad;

void**			lumpcache;

char** lumpnames;


#include "i_system.h"
#include "z_zone.h"


//
// LUMP BASED ROUTINES.
//

//
// W_AddFile
// All files are optional, but at least one file must be
//  found (PWAD, if all required lumps are present).
// Files with a .wad extension are wadlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
//
// If filename starts with a tilde, the file is handled
//  specially to allow map reloads.
// But: the reload feature is a fragile hack...
void W_AddFile (char *filename)
{
	try
	{
#define PRELOAD_WAD
#ifdef PRELOAD_WAD
		SystemFile file(filename);
		file.seek(0, SEEK_END);
		fileoffset_t size = file.tell();
		file.seek(0, SEEK_SET);

		unique_ptr<MemoryFile> preloaded(new MemoryFile(size));
		file.read(preloaded->data(), size);

		thewad = new Wad(std::move(preloaded));
#else
		thewad = new Wad(unique_ptr<File>(new SystemFile(filename)));
#endif
	}
	catch (InputError &e)
	{
		I_Error("%s", e.toString().c_str());
	}

	lumpnames = new char*[thewad->numLumps()];
	for (index_t i = 0; i < thewad->numLumps(); i++)
	{
		lumpnames[i] = new char[8];
		memcpy(lumpnames[i], thewad->lumpName(i).c_str(), 8);
	}

	lumpcache = new void*[thewad->numLumps()];
	memset(lumpcache, 0, thewad->numLumps() * sizeof(void*));
}


// W_InitMultipleFiles
// Pass a null terminated list of files to use.
// All files are optional, but at least one file
//  must be found.
// Files with a .wad extension are idlink files
//  with multiple lumps.
// Other files are single lumps with the base filename
//  for the lump name.
// Lump names can appear multiple times.
// The name searcher looks backwards, so a later file
//  does override all earlier ones.
//
void W_InitMultipleFiles (char** filenames)
{	
	W_AddFile(filenames[0]);
}

//
// W_InitFile
// Just initialize from a single file.
//
void W_InitFile (char* filename)
{
	W_AddFile(filename);
}

//
// W_NumLumps
//
int W_NumLumps (void)
{
    return thewad->numLumps();
}

//
// W_CheckNumForName
// Returns -1 if name not found.
//

char* mystrupr(char *str)
{
	while (*str != '\0')
	{
		*str = toupper(*str);
		str++;
	}

	return str;
}

int W_CheckNumForName (char* name)
{
	char upname[9];

	strcpy(upname, name);
	mystrupr(upname);
	
	for (index_t i = thewad->numLumps() - 1; i >= 0; i--)
	{
		if (!strncmp(lumpnames[i], upname, 8))
			return (int)i;
	}

    // TFB. Not found.
    return -1;
}


//
// W_GetNumForName
// Calls W_CheckNumForName, but bombs out if not found.
//
int W_GetNumForName (char* name)
{
    int	i;

    i = W_CheckNumForName (name);
    
    if (i == -1)
      I_Error ("W_GetNumForName: %s not found!", name);
      
    return i;
}


//
// W_LumpLength
// Returns the buffer size needed to load the given lump.
//
int W_LumpLength (int lump)
{
    if (lump >= thewad->numLumps())
	I_Error ("W_LumpLength: %i >= numlumps",lump);

    return thewad->lumpSize(lump);
}

const char* W_LumpName(int lump)
{
	return thewad->lumpName(lump).c_str();
}

//
// W_ReadLump
// Loads the lump into the given buffer,
//  which must be >= W_LumpLength().
//
void
W_ReadLump
( int		lump,
  void*		dest )
{
	thewad->readLump(lump, dest);
}




//
// W_CacheLumpNum
//
void*
W_CacheLumpNum
( int		lump,
  int		tag )
{
    byte*	ptr;

    if ((unsigned)lump >= thewad->numLumps())
    	I_Error ("W_CacheLumpNum: %i >= numlumps",lump);
		
    if (!lumpcache[lump])
    {
	// read the lump in
	
	//printf ("cache miss on lump %i\n",lump);
	ptr = Z_Malloc (W_LumpLength (lump), tag, &lumpcache[lump]);
	W_ReadLump (lump, lumpcache[lump]);
    }
    else
    {
	//printf ("cache hit on lump %i\n",lump);
	Z_ChangeTag (lumpcache[lump],tag);
    }
	
    return lumpcache[lump];
}



//
// W_CacheLumpName
//
void*
W_CacheLumpName
( char*		name,
  int		tag )
{
    return W_CacheLumpNum (W_GetNumForName(name), tag);
}


