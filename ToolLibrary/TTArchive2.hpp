#ifndef _TTARCHIVE
#define _TTARCHIVE

#include "TelltaleToolLibrary.h"
#include <vector>
#include <string>
#include "Meta.hpp"

struct ResourceInfo {
	u64 mOffset;
	u64 mSize;
	//mSourceType,mCreationTime,mLastModifiedTime
};

//.TTARCH2 files. Takes ownership (deletes when done) of passed in stream when reading
class TTArchive2 {
public:

	struct ResourceEntry {
		u64 mNameCRC;
		u64 mOffset;
		u32 mSize;
		u16 mNamePageIndex, mNamePageOffset;
	};

	struct ResourceCreateEntry {
		DataStream* mpStream;
		std::string open_later;//instead of mpStream, will use this file path.
		std::string name;
	};

	/**
	 * Opens the archive from the given data stream. Use macros to open the data stream and pass it. You grant ownership to this class, so do not delete the stream this class will do that for you.
	 */
	void Activate(DataStream* inArchiveStream);

	/**
	 * Returns if this archive contains a given resource symbol name.
	 */
	bool HasResource(const Symbol&);

	/**
	 * Returns the name of the given resource hash.
	 */
	String GetResourceName(const Symbol&);

	/**
	 * Gets resource information about the given resource. The result is stored in the argument pointer. Returns if successfull.
	 */
	bool GetResourceInfo(const Symbol&, ResourceInfo*);
	
	/**
	 * Opens a resource stream which is readable. You should delete it after use.
	 */
	DataStream* GetResourceStream(ResourceEntry*);

	/**
	 * Deactivate is called automatically in the destructor. Call this at any time to release all resources and pointers held by this class.
	 */
	void Deactivate();

	/**
	 * Function prototype for creating an archive
	 */
	typedef void (*ProgressFunc)(const char* _Msg, float _Progress);

	/**
	 * Creates a .TTARCH2. Returns if success. Writes to pDst stream (Ideally a file stream to disk) (does not delete this or own it). Pass in the files as an array
	 */
	static bool Create(ProgressFunc, DataStream* pDst, std::vector<ResourceCreateEntry>& files, bool pEncrypt,
		bool pCompress, Compression::Library
		pCompressionLibrary = Compression::Library::ZLIB, u32 pVersion = 2);

	/**
	 * Gets a resource entry pointer for the given resource name.
	 */
	ResourceEntry* _FindResource(const Symbol&);

	/**
	 * Gets the name of the resoure at the given index. Index is from 0 to NumResources() - 1
	 */
	inline Symbol GetResourceAtIndex(int index){
		return mResources[index].mNameCRC;
	}

	/**
	 * Returns the number of resources inside this archive.
	 */
	inline int NumResources() {
		return (int)mResources.size();
	}

	// --------------------------------------------------------------------------------------------------------------- INTERNAL ------------------------------------------------------------------------------------------------------------------

	struct LessRCE_p {

		inline bool operator()(const ResourceCreateEntry*& lhs, const ResourceCreateEntry*& rhs){
			return CRC64_CaseInsensitive(0, lhs->name.c_str()) < CRC64_CaseInsensitive(0, rhs->name.c_str());
		}

	};

	DataStream* mpNameStream = nullptr;
	DataStream* mpResourceStream = nullptr;
	std::vector<TTArchive2::ResourceEntry> mResources;
	u32 mVersion = 0;
	char* mpNamePageCache = nullptr;
	i32 mNamePageCacheIndex = 0;
	u32 mNamePageCount = 0;
	bool mbActive;//by lib, for lib
	DataStream* mpInStream = nullptr;//for lib
	void* userData = 0;

	inline TTArchive2() : mbActive(false), mpInStream(NULL), mNamePageCacheIndex(-1)
	, mpNamePageCache(NULL), mpNameStream(NULL), mpResourceStream(NULL) {}

	inline ~TTArchive2() {
		if (mbActive)
			Deactivate();
	}

};

#endif