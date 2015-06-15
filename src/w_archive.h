#pragma once

#include "common.h"

class MemoryFile;
class ArchiveEntry;
class ArchiveIterator;

// List of archive types.
enum class ArchiveType
{
	Wad
};

// Class for archives (wads, pk3s) of game data.
class Archive
{
public:
	virtual ~Archive() {};

	// Get archive type.
	virtual ArchiveType type() const = 0;

	// Get number of entries in the archive;
	virtual index_t numEntries() const = 0;

	// Iterate through entries.
	// Use it like this:
	// TODO: make it usable more than once
	//
	//while (archive.nextEntry())
	//{
	//	ArchiveEntry &entry = archive.getEntry();
	//	doSomethingWith(entry);
	//}
	virtual ArchiveEntry& getEntry() = 0;
	virtual bool nextEntry() = 0;
};

// An abstract entry in an archive.
// These must be subclassed by a concrete archive reader.
// These will usually be stored in an array that
// may not change once the archive has been opened.
// Therefore, storing references to ArchiveEntries is safe,
// as long as the archive they belong to is not destroyed.
class ArchiveEntry
{
public:
	ArchiveEntry(Archive &archive)
		:mArchive(archive)
	{}

	virtual ~ArchiveEntry() {}

	// Get the archive this entry belongs to.
	Archive& archive() const
	{
		return mArchive;
	}

	// Get information about the entry.
	virtual const string& name() const = 0;
	virtual index_t size() const = 0;

	// Read whole entry to a memory buffer.
	virtual void read(void *dest) = 0;

	// Read whole entry to a MemoryFile.
	unique_ptr<MemoryFile> read();

private:
	// Reference to the archive this entry belongs to.
	Archive &mArchive;
};
