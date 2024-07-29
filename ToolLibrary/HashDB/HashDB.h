//Taken from my old Telltale lib, LibTelltale!

#ifndef _HASHDB
#define _HASHDB

#include "../TelltaleToolLibrary.h"
#include "../DataStream/DataStream.h"

#include <vector>

#define HASHDB_EXT HashDB

const extern u32 MAGIC;


/*
* A database of hashes which the library uses to search for its CRCs. Similar to RTBs, except more complex with pages of entries to make quicker searching
* Use this to find a CRC to string mapping
*/
class HashDatabase_Legacy {
public:
	struct Page {
		char* pageName;
		u16 flags;
		u32 count;
		u64 offset;
		u64 size;
	};

	~HashDatabase_Legacy();
	//TAKES OWNERSHIP OF STREAM!
	HashDatabase_Legacy(DataStream* db);
	bool Open();
	u32 NumPages();
	u32 NumEntries();
	u32 Flags();
	Page* PageAt(int index);
	Page** Pages();
	Page* FindPage(const char* name);
	void FindEntry(Page* page, u64 crc,String* result);
	void FindEntry(Page* page, u64 crc, char* result);

	void DumpPage(Page* page, std::vector<String>& dest);

protected:
	DataStream* db_stream;
	Page** db_pages;
	int numPages;
	int flags;
	char* bufferedPage;
	Page* cached_page;
};

#define SEP ,

#define DB_FN(_NAME, _RETURN, _ARGS) _TTToolLib_Exp _RETURN TelltaleToolLib_HashDB_ ## _NAME(HashDatabase_Legacy* pDatabase _ARGS)

DB_FN(NumPages, int);
DB_FN(NumEntries, int);
DB_FN(Flags, int);
DB_FN(PageAt, HashDatabase_Legacy::Page*, SEP int index);
DB_FN(FindPage, HashDatabase_Legacy::Page*, SEP const char* pageName);
/*result must be a char[1024]*/
DB_FN(FindEntry, void, SEP HashDatabase_Legacy::Page* pPage SEP u64 crc SEP char* result);

class HashDatabase {
public:

	static constexpr const char MAGIC[5] = "TDB2";

	struct Page {
		std::string mPageName;
		u32 mFlags;
		u32 mCount;
		u32 mSymbolStart;
		u32 mStringStart;
	};

	std::vector<Page> mPages;
	DataStreamContainer* mpStringsStream;
	DataStreamSubStream* mpStringsSubStream;
	DataStream* mpSrcStream;
	u32 mSymbolBase;

	u64* mpBuffer;
	Page* mpBuffered;

	HashDatabase(DataStream* db);
	~HashDatabase();

	/*takes ownership*/
	bool Open();

	u32 NumPages();
	u32 NumEntries();
	Page* PageAt(int index);
	Page* FindPage(const char* name);
	void FindEntry(Page* page, u64 crc, String* result);
	void FindEntry(Page* page, u64 crc, char* result);

	void DumpPage(Page* page, std::vector<String>& dest);

	static bool Create(const char* inpath, DataStream* pOut, bool bVerbose, bool bCompress, bool bEncrypt);

	void _SetBuffer(Page* page);

};

#endif