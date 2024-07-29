#pragma warning(disable C4267 C4244 C4554 C4477)
#include "HashDB.h"#
#include <algorithm>
#include "../HashManager.h"

const u32 MAGIC = 0x54544c42;

HashDatabase_Legacy::HashDatabase_Legacy(DataStream* stream) {
	if (!stream)throw "bad stream";
	this->db_stream = stream;
	this->db_pages = NULL;
	this->flags = 0;
	this->numPages = 0;
	this->cached_page = NULL;
	this->bufferedPage = NULL;
}

HashDatabase_Legacy::Page* HashDatabase_Legacy::PageAt(int index) {
	if (index >= this->NumPages())return NULL;
	return this->db_pages ? this->db_pages[index] : NULL;
}

HashDatabase_Legacy::Page** HashDatabase_Legacy::Pages() {
	return this->db_pages;
}

DB_FN(NumPages, int) {
	return pDatabase->NumPages();
}

DB_FN(NumEntries, int) {
	return pDatabase->NumEntries();
}

DB_FN(Flags, int) {
	return pDatabase->Flags();
}

DB_FN(PageAt, HashDatabase_Legacy::Page*, SEP int index) {
	return pDatabase->PageAt(index);
}

DB_FN(FindPage, HashDatabase_Legacy::Page*, SEP const char* pageName) {
	return pDatabase->FindPage(pageName);
}

DB_FN(FindEntry, void, SEP HashDatabase_Legacy::Page* pPage SEP u64 crc SEP char* r) {
	return pDatabase->FindEntry(pPage, crc, r);
}

void HashDatabase_Legacy::FindEntry(HashDatabase_Legacy::Page* page, u64 crc, String* result) {
	char res[1024];
	res[0] = 0;
	FindEntry(page, crc, res);
	if (res[0] == 0)
		return;
	result->operator=(res);
}

void HashDatabase_Legacy::FindEntry(HashDatabase_Legacy::Page* page, u64 crc, char* result) {
	if (!result || !crc)
		return;
	result[0] = 0;
	if (page) {
		if (!(cached_page && cached_page == page)) {
			this->db_stream->SetPosition(page->offset, DataStreamSeekType::eSeekType_Begin);
			if (bufferedPage)free(bufferedPage);
			bufferedPage = (char*)malloc(page->count * 0xC);
			this->db_stream->Serialize(bufferedPage, page->count * 0xC);
			cached_page = page;
		}
		char* cp;
		u32 c;
		for (int i = 0; i < page->count; i++) {
			cp = (bufferedPage + 0xC * i);
			if (*((u64*)(cp)) == crc) {
				cp = (bufferedPage + 0xC * i + 8);
				c = *((u32*)(cp));
				db_stream->SetPosition((page->count * 0xC) + page->offset + c, DataStreamSeekType::eSeekType_Begin);
				int len = 0;
				if (i + 1 != page->count) {
					cp = (bufferedPage + (0xC * (i+1)) + 8);
					len = *((u32*)(cp));
					len -= c;
				}
				else {
					len = page->size - (0xC * page->count) - c;
				}
				if (len >= 1024) {
					TelltaleToolLib_RaiseError("hash database entry too large (>=1024)", ErrorSeverity::ERR);
					return;
				}
				db_stream->Serialize(result, len);
				return;
			}
		}
		return;
	}
	else {
		for (int i = 0; i < NumPages(); i++) {
			FindEntry(PageAt(i), crc, result);
			if ((*result))
				return;
		}
		return;
	}
}


void HashDatabase_Legacy::DumpPage(Page* page, std::vector<String>& dest)
{
	char result[1024];
	if (page) {
		if (!(cached_page && cached_page == page)) {
			this->db_stream->SetPosition(page->offset, DataStreamSeekType::eSeekType_Begin);
			if (bufferedPage)free(bufferedPage);
			bufferedPage = (char*)malloc(page->count * 0xC);
			this->db_stream->Serialize(bufferedPage, page->count * 0xC);
			cached_page = page;
		}
		dest.reserve(page->count);
		char* cp;
		u32 c;
		for (int i = 0; i < page->count; i++) {
			cp = (bufferedPage + 0xC * i + 8);
			c = *((u32*)(cp));
			db_stream->SetPosition((page->count * 0xC) + page->offset + c, DataStreamSeekType::eSeekType_Begin);
			int len = 0;
			if (i + 1 != page->count) {
				cp = (bufferedPage + (0xC * (i + 1)) + 8);
				len = *((u32*)(cp));
				len -= c;
			}
			else {
				len = page->size - (0xC * page->count) - c;
			}
			if (len >= 1024) {
				TelltaleToolLib_RaiseError("hash database entry too large (>=1024)", ErrorSeverity::ERR);
				return;
			}
			db_stream->Serialize(result, len);
			bool cont = false;
			if (page->count < 5000) {
				for (auto it = dest.begin(); it != dest.end(); it++)
					if (*it == result) {
						cont = true;
						break;
					}
				if (cont)
					continue;
			}
			dest.push_back(String{ result });
		}
	}
}

bool HashDatabase_Legacy::Open() {
	if (!this->db_stream)return false;
	DataStream* stream = this->db_stream;
	int magic;
	stream->Serialize((char*)&magic, 4);
	if (magic != MAGIC)return false;
	stream->Serialize((char*)&flags, 1);
	stream->Serialize((char*)&this->numPages, 4);
	if (this->db_pages)return false;//cannot open another
	this->db_pages = (HashDatabase_Legacy::Page**)calloc(sizeof(HashDatabase_Legacy::Page*), this->numPages);
	if (!this->db_pages)return false;
	for (int i = 0; i < this->numPages; i++) {
		HashDatabase_Legacy::Page* page = new Page();
		stream->Serialize((char*)&page->count, 4);
		stream->Serialize((char*)&page->flags, 1);
		int len;
		stream->Serialize((char*)&len, 4);
		char* tmp = new char[len + 1];
		tmp[len] = 0;
		stream->Serialize(tmp, len);
		page->pageName = tmp;
		stream->Serialize((char*)&page->size, 4);
		page->offset = stream->GetPosition();
		stream->SetPosition(page->size + stream->GetPosition(), DataStreamSeekType::eSeekType_Begin);
		this->db_pages[i] = page;
	}
	return true;
}

u32 HashDatabase_Legacy::Flags() {
	return this->flags;
}

HashDatabase_Legacy::Page* HashDatabase_Legacy::FindPage(const char* n) {
	if (!this->db_pages)return NULL;
	int pages = this->NumPages();
	for (int i = 0; i < pages; i++) {
		if (!_stricmp(PageAt(i)->pageName, n)) return PageAt(i);
	}
	return NULL;
}

u32 HashDatabase_Legacy::NumEntries() {
	u32 ret = 0;
	int pages = NumPages();
	for (int i = 0; i < pages; i++) {
		ret += PageAt(i)->count;
	}
	return ret;
}

u32 HashDatabase_Legacy::NumPages() {
	return this->numPages;
}

HashDatabase_Legacy::~HashDatabase_Legacy() {
	if (this->db_stream)
		delete db_stream;
	for (int i = 0; i < NumPages(); i++) {
		delete[] PageAt(i)->pageName;
		delete PageAt(i);
	}
	if (this->db_pages)
		free(this->db_pages);
	if (this->bufferedPage)
		free(bufferedPage);

}

// DATA BASE VERSION 2

HashDatabase::HashDatabase(DataStream* db)
{
	mpStringsStream = 0;
	mpSrcStream = db;
	mpBuffer = 0;
	mpStringsSubStream = 0;
	mpBuffered = 0;
	mSymbolBase = 0;
}

HashDatabase::~HashDatabase()
{
	if (mpBuffered)
		delete[] mpBuffer;
	mpBuffer = 0;
	mpBuffered = 0;
	mSymbolBase  = 0;
	if (mpStringsSubStream)
		delete mpStringsSubStream;
	mpStringsStream->mParams.mpSrcStream = 0;
	if(mpStringsStream)
		delete mpStringsStream;
	delete mpSrcStream;
	mpSrcStream = 0;
	mPages.clear();
}

u32 HashDatabase::NumPages()
{
	return (u32)mPages.size();
}

u32 HashDatabase::NumEntries()
{
	u32 total = 0;
	for (auto it = mPages.begin(); it != mPages.end(); it++)
		total += it->mCount;
	return total;
}

HashDatabase::Page* HashDatabase::PageAt(int index)
{
	return mPages.data() + index;
}

HashDatabase::Page* HashDatabase::FindPage(const char* name)
{
	for (auto it = mPages.begin(); it != mPages.end(); it++)
		if (!_stricmp(name,it->mPageName.c_str()))
			return &(*it);
	return 0;
}

void HashDatabase::FindEntry(Page* page, u64 crc, String* result)
{
	char res[1024];
	res[0] = 0;
	FindEntry(page, crc, res);
	if (res[0] == 0)
		return;
	result->operator=(res);
}

void HashDatabase::FindEntry(Page* page, u64 crc, char* result)
{
	if (page == 0) {
		//search all
		result[0] = 0;
		for(int i = 0; i < mPages.size(); i++){
			FindEntry(mPages.data() + i, crc, result);
			if (result[0] != 0)
				break;
		}
		return;
	}
	if (crc == 0)
		return;
	_SetBuffer(page);
	i32 p = 0;
	i32 r = page->mCount - 1;
	i32 q = (r + p) / 2;
	i32 counter = 0;

	while (p <= r)
	{
		counter++;
		if (mpBuffer[q] == crc){
			// FOUND HASH
			mpStringsStream->SetPosition(page->mStringStart + (q << 2), DataStreamSeekType::eSeekType_Begin);
			u32 off[2]{ 0,0 };
			//there is always another string, the empty crc string footer (crc != 0)
			mpStringsStream->Serialize((char*)off, 8);
			mpStringsStream->SetPosition(page->mStringStart + ((page->mCount+1) << 2) + off[0], DataStreamSeekType::eSeekType_Begin);
			u32 len = off[1] - off[0];
			mpStringsStream->Serialize(result, len);
			result[len] = 0;
			return;
		}
		else
		{
			if (mpBuffer[q] < crc)
			{
				p = q + 1;
				q = (r + p) / 2;
			}
			else
			{
				r = q - 1;
				q = (r + p) / 2;
			}
		}
	}
}

void HashDatabase::DumpPage(Page* page, std::vector<String>& dest)
{
	dest.reserve(page->mCount);
	u32* offs = new u32[page->mCount + 1];
	mpStringsStream->SetPosition(page->mStringStart, DataStreamSeekType::eSeekType_Begin);
	mpStringsStream->Serialize((char*)offs, (unsigned long long)(page->mCount+1) << 2);
	for(u32 i = 0; i < page->mCount; i++){
		u32 len = offs[i + 1] - offs[i];
		String str(len,'\0');
		mpStringsStream->Serialize(const_cast<char*>(str.c_str()), len);
		dest.push_back(std::move(str));
	}
}

bool HashDatabase::Open()
{
	char header[4];
	this->mpSrcStream->Serialize(header, 4);
	if (memcmp(header, MAGIC, 4) != 0){
		printf("Cannot open HashDatabase. Very likely that the database is using version 1. Please check your install.");
		return false;
	}
	u32 numPages{ 0 };
	mpSrcStream->Serialize((char*)&numPages, 4);
	mPages.reserve(numPages);
	u32 numSymbols = 0;
	for(u32 i = 0; i < numPages; i++){
		Page pg{};
		u32 len{};
		mpSrcStream->Serialize((char*)&len, 4);
		String nm(len, '\0');
		mpSrcStream->Serialize((char*)&pg.mCount, 4);
		mpSrcStream->Serialize((char*)&pg.mFlags, 4);
		mpSrcStream->Serialize((char*)&pg.mStringStart, 4);
		mpSrcStream->Serialize((char*)&pg.mSymbolStart, 4);
		mpSrcStream->Serialize(const_cast<char*>(nm.c_str()), len);
		pg.mPageName = std::move(nm);
		numSymbols += pg.mCount;
		mPages.push_back(std::move(pg));
	}
	mSymbolBase = (u32)mpSrcStream->GetPosition();
	u64 strOff = (u64)mSymbolBase + ((u64)numSymbols << 3llu);
	mpSrcStream->SetPosition(strOff, DataStreamSeekType::eSeekType_Begin);
	mpStringsSubStream = new DataStreamSubStream(mpSrcStream, mpSrcStream->GetSize() - strOff, strOff);
	DataStreamContainerParams params{};
	params.mpSrcStream = mpStringsSubStream;
	mpStringsStream = new DataStreamContainer(params);
	mpStringsStream->Read(0, &strOff);
	return mpStringsStream->ok;
}


bool sorter(const std::string& lhs, const std::string& rhs) {
	u64 hl = CRC64_CaseInsensitive(0, lhs.c_str());
	u64 hr = CRC64_CaseInsensitive(0, rhs.c_str());
	return hl < hr;
}

bool repl(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

inline bool iequals(const std::string& aa, const std::string& bb)
{
	const char* a = aa.c_str(), * b = bb.c_str();
	for (;; a++, b++) {
		int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
		if (d != 0 || !*a)
			return d;
	}
}

bool HashDatabase::Create(const char* fp, DataStream* pOut, bool bVerbose, bool bCompress, bool bEnc)
{
	if (bEnc)
		bCompress = true;
	std::vector<Page> pages{};
	std::vector<std::vector<std::string>> values{};
	FILE* stream{};
	fopen_s(&stream, fp, "r");
	if (!stream)
		return false;
	if (!pOut || !fp)
		return false;
	static char _buf[512];
	Page currentPage{};
	std::vector<std::string> cur_values{};
	for(;;){
		char* buf = fgets(_buf, 512, stream);
		if (!buf)
			break;
		if (strlen(buf) >= 8 && !_stricmp(std::string(buf).substr(0,7).c_str(), "NEWPAGE")) {
			if(currentPage.mPageName.length() != 0){
				if (bVerbose)
					printf("-collected page %s: %d hashes\n", currentPage.mPageName.c_str(), (u32)cur_values.size());
				pages.push_back(std::move(currentPage));
				values.push_back(std::move(cur_values));
				currentPage.mCount = currentPage.mFlags = currentPage.mStringStart = currentPage.mSymbolStart = 0;
			}
			currentPage.mPageName = buf + 8;
			if (bVerbose)
				printf("-collecting page %s\n", currentPage.mPageName.c_str());
			repl(currentPage.mPageName, "\n", "");
			repl(currentPage.mPageName, "\r", "");
		}else{
			std::string s = buf;
			repl(s, "\n", "");
			repl(s, "\r", "");
			bool exit = false;
			for(auto it = cur_values.begin(); it != cur_values.end(); it++){
				if (!iequals(*it, s)) {
					exit = true;
					break;
				}
			}
			if (exit)
				continue;
			currentPage.mFlags += (u32)s.length();
			cur_values.push_back(std::move(s));
		}
	}
	pages.push_back(std::move(currentPage));
	values.push_back(std::move(cur_values));
	int i = 0;
	u32 symbolStart = 0;
	u32 stringStart = 0;
	u32 write = 0;
	pOut->Serialize((char*)MAGIC, 4);
	write = (u32)pages.size();
	pOut->Serialize((char*)&write, 4);
	if (bVerbose)
		printf("-sorting string and hashes\n");
	for (auto x = values.begin(); x != values.end(); x++) {
		std::sort(x->begin(), x->end(), &sorter);
	}
	if (bVerbose)
		printf("-writing headers\n");
	for(auto it = pages.begin(); it != pages.end(); it++,i++){
		it->mCount = (u32)values[i].size();
		it->mSymbolStart = symbolStart;
		it->mStringStart = stringStart;
		symbolStart += (it->mCount) << 3;
		stringStart += ((it->mCount+1) << 2) + it->mFlags;
		it->mFlags = 0;
		write = (u32)it->mPageName.length();
		pOut->Serialize((char*)&write, 4);
		write = it->mCount;
		pOut->Serialize((char*)&write, 4);
		write = it->mFlags;
		pOut->Serialize((char*)&write, 4);
		write = it->mStringStart;
		pOut->Serialize((char*)&write, 4);
		write = it->mSymbolStart;
		pOut->Serialize((char*)&write, 4);
		pOut->Serialize((char*)it->mPageName.c_str(), it->mPageName.length());
	}
	i = 0;
	fclose(stream);
	if (bVerbose)
		printf("-writing hashes\n");
	for(auto it = pages.begin(); it != pages.end(); it++,i++){
		std::vector<std::string>& hashes = values[i];
		for(auto s = hashes.begin();s!=hashes.end();s++){
			u64 hash = CRC64_CaseInsensitive(0, s->c_str());
			pOut->Serialize((char*)&hash, 8);
		}
		if (bVerbose)
			printf("\t-written hashes for %s\n", it->mPageName.c_str());
	}
	DataStreamMemory stringMemoryStream{stringStart + 0x10000};
	if (bVerbose)
		printf("-writing strings\n");
	i = 0;
	for (auto it = pages.begin(); it != pages.end(); it++, i++) {
		std::vector<std::string>& hashes = values[i];
		u32 off = 0;
		for (auto s = hashes.begin(); s != hashes.end(); s++) {
			stringMemoryStream.Serialize((char*)&off, 4);
			off += (u32)s->length();
		}
		stringMemoryStream.Serialize((char*)&off, 4);
		for (auto s = hashes.begin(); s != hashes.end(); s++) {
			stringMemoryStream.Serialize((char*)s->c_str(), s->length());
		}
		if (bVerbose)
			printf("\t-written strings for %s\n", it->mPageName.c_str());
	}
	pages.clear();
	values.clear();
	if (bVerbose)
		printf("-compiling into container\n");
	DataStreamContainerParams params{};
	params.mbCompress = bCompress;
	params.mCompressionLibrary = Compression::Library::ZLIB;
	params.mbEncrypt = bEnc;
	params.mDstOffset = pOut->GetPosition();
	params.mWindowSize = 0x10000;
	params.mpSrcStream = &stringMemoryStream;
	params.mpDstStream = pOut;
	u64 sSize = stringMemoryStream.GetPosition();
	stringMemoryStream.SetPosition(0, DataStreamSeekType::eSeekType_Begin);
	stringMemoryStream.SetMode(DataStreamMode::eMode_Read);
	DataStreamContainer::Create(0, params, sSize);
	printf("-done\n");
	return true;
}

void HashDatabase::_SetBuffer(Page* page)
{
	if (mpBuffered == page)
		return;
	if(mpBuffered && mpBuffered->mCount >= page->mCount){
		;
	}else{
		mpBuffer = new u64[page->mCount];
	}
	mpBuffered = page;
	mpSrcStream->SetPosition(mSymbolBase + page->mSymbolStart, DataStreamSeekType::eSeekType_Begin);
	mpSrcStream->Serialize((char*)mpBuffer, (unsigned long long)page->mCount << 3llu);
}