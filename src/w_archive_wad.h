#include "common.h"
#include "w_archive.h"
#include "io_file.h"

class WadEntry;

class WadArchive : public Archive
{
	friend class WadEntry;

public:
	WadArchive(unique_ptr<File> file);

	ArchiveType type() const override;
	index_t numEntries() const override;
	ArchiveEntry& getEntry() override;
	bool nextEntry() override;

private:
	unique_ptr<File> mFile;
	std::vector<WadEntry> mEntries;

	// Iteration.
	index_t mCurrentEntry = -1;
};

struct WadEntry : ArchiveEntry
{
public:
	WadEntry(WadArchive &archive, std::string name, index_t size, fileoffset_t dataOffset);

	string name() const override;
	index_t size() const override;

	void read(void *dest) override;

private:
	std::string mName;
	index_t mSize;
	fileoffset_t mDataOffset;
};



