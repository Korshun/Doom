#include "w_archive_wad.h"

WadArchive::WadArchive(unique_ptr<File> file)
{
	mFile = std::move(file);

	// Read wad signature.
	char identification[4];
	mFile->read(identification, 4);
	if (strncmp(identification, "IWAD", 4) && strncmp(identification, "PWAD", 4))
		throw InputError(*mFile, "not a wad file");

	// Read number of lumps.
	index_t numLumps = (index_t)file->read32le();
	if (numLumps < 0)
		throw InputError(*mFile, "negative number of lumps");

	// Read directory offset.
	fileoffset_t dirOffset = (fileoffset_t)file->read32le();
	if (dirOffset < 0)
		throw InputError(*mFile, "negative directory offset");

	// Go to the lump directory.
	mFile->seek(dirOffset);

	// Read all lumps.
	mEntries.reserve(numLumps);
	for (index_t i = 0; i < numLumps; i++)
	{
		try
		{
			// Read lump data offset.
			fileoffset_t dataOffset = (fileoffset_t)mFile->read32le();
			if (dataOffset < 0)
				throw InputError(*mFile, format("lump {0} has negative data offset", i));

			// Read lump size.
			index_t size = (index_t)mFile->read32le();
			if (size < 0)
				throw InputError(*mFile, format("lump {0} has negative size", i));

			// Read lump name.
			char name[8];
			mFile->read(name, 8);

			// Add to entry list.
			mEntries.push_back(WadEntry(*this, string(name, 8), size, dataOffset));
		}
		catch (InputError &e)
		{
			// Print error.
			printf("%s\n", e.toString().c_str());

			// Skip the lump.
			continue;
		}
	}
}

ArchiveType WadArchive::type() const
{
	return ArchiveType::Wad;
}


index_t WadArchive::numEntries() const
{
	return (index_t)mEntries.size();
}

ArchiveEntry& WadArchive::getEntry()
{
	assert(mCurrentEntry >= 0 && mCurrentEntry < numEntries());
	return mEntries[mCurrentEntry];
}

bool WadArchive::nextEntry()
{
	mCurrentEntry++;
	if (mCurrentEntry == numEntries())
		return false;
	return true;
}

WadEntry::WadEntry(WadArchive &archive, std::string name, index_t size, fileoffset_t dataOffset)
	:ArchiveEntry(archive)
{
	mName = name;
	mSize = size;
	mDataOffset = dataOffset;
}

string WadEntry::name() const
{
	return mName;
}

index_t WadEntry::size() const
{
	return mSize;
}

void WadEntry::read(void *dest)
{
	assert(archive().type() == ArchiveType::Wad);
	WadArchive &wad = (WadArchive&)archive();
	wad.mFile->seek(mDataOffset);
	wad.mFile->read(dest, mSize);
}
