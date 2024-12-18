#include "LibraryConfig.h"
#include "TTArchive2.hpp"
#include <algorithm>

bool TTArchive2::GetResourceInfo(const Symbol& s, ResourceInfo* i) {
	if (!i)return false;
	TTArchive2::ResourceEntry* e = _FindResource(s);
	if (!e)return false;
	i->mOffset = e->mOffset;
	i->mSize = e->mSize;
	return true;
}

void TTArchive2::Activate(DataStream* inArchiveStream) {
#define getint(dest,size)inArchiveStream->Serialize((char*)dest, size);
	if (mbActive)Deactivate();
	DataStreamContainerParams params;
	params.mpSrcStream = inArchiveStream;
	DataStreamContainer* c;
	inArchiveStream = c = new DataStreamContainer(params);
	mpInStream = inArchiveStream;
	u64 version{ 0 };
	c->Read(params.mpSrcStream->GetPosition(), &version);
	if(c->IsInvalid()){
		mbActive = false;
		return;
	}
	version = 0;
	if (!inArchiveStream->Serialize((char*)&version, 4))
		return;//error handled by container
	if (version == 1414807858) {//TTA2: Telltale Archive 2
		version = 0;
		goto v3;
	}
	else if (version == 1414807859) {//TTA3
		version = 1;
	v3:
		u32 i;
		getint(&i, 4);
		if (i > 15)return;
	}
	else if (version != 1414807860) {//if not TTA4
		mbActive = false;
		sprintf(TelltaleToolLib_Alloc_GetFixed1024ByteStringBuffer(), "Invalid TTARCH2 magic version - likely wrong game ID set: 0x%llX!", version);
		TelltaleToolLib_RaiseError(TelltaleToolLib_Alloc_GetFixed1024ByteStringBuffer(), ERR);
		return;
	}
	else version = 2;
	u32 namesize, resources;
	getint(&namesize, 4);
	if (namesize > 0x10000000) {
		TelltaleToolLib_RaiseError("Name table too large", ERR);
		return;
	}
	this->mNamePageCount = namesize / 0x10000;
	getint(&resources, 4);
	if (resources > 0xFFFFF){
		TelltaleToolLib_RaiseError("Too many resources", ERR);
		return;
	}
	mResources.reserve(resources);
	u32 u;
	//int* buffer = version < 1 ? new int[resources] : NULL;
	for (int i = 0; i < resources; i++) {
		ResourceEntry entry;
		getint(&entry.mNameCRC, 8);
		getint(&entry.mOffset, 8);
		if (version < 1) {//if v2
			//getint(buffer + i, 4);
			getint(&u, 4);//skipped member
		}
		getint(&entry.mSize, 4);
		getint(&u, 4);//skipped member
		getint(&entry.mNamePageIndex, 2);
		getint(&entry.mNamePageOffset, 2);
		mResources.push_back(entry);
	}
	u32 startoff = inArchiveStream->GetPosition() + namesize;
	mpNameStream = inArchiveStream->GetSubStream(startoff - namesize, namesize);
	mpResourceStream = inArchiveStream->GetSubStream(startoff,
		inArchiveStream->GetSize() - startoff);
	//delete[] buffer;
	mbActive = true;
	mVersion = version;
}

DataStream* TTArchive2::GetResourceStream(TTArchive2::ResourceEntry* entry) {
	return entry ? new DataStreamSubStream(mpResourceStream, (unsigned __int64)entry->mSize, entry->mOffset) : 0;
}

static bool _Cmp(const TTArchive2::ResourceCreateEntry& lhs, const TTArchive2::ResourceCreateEntry& rhs){
	u64 xl = CRC64_CaseInsensitive(0,lhs.name.c_str());
	u64 xr = CRC64_CaseInsensitive(0,rhs.name.c_str());
	return xl < xr;
}

__int64 FileSizeA(const char* name)
{
	struct _stat64i32 buf{0};
	if (_stat(name, &buf) != 0)
		return -1;
	return buf.st_size;
}

bool TTArchive2::Create(ProgressFunc func, DataStream* pDst, std::vector<ResourceCreateEntry>& entries,
	bool pEncrypt, bool pCompress, Compression::Library
	pCompressionLibrary, u32 pVersion) {
#define writeint(i,size) out.Serialize((char*)i,size);
	if (!pDst || pVersion > 2)return false;
	char buf1[MAX_PATH];
	char buf2[MAX_PATH];
	GetTempPathA(MAX_PATH, buf1);
	GetTempFileNameA(buf1, "create_ttarch2_ttlib", 0, buf2);
	DataStreamFileDisc out = DataStreamFileDisc(PlatformSpecOpenFile(buf2, WRITE), DataStreamMode::eMode_Write);
	u32 vh;
	if (pVersion == 0) {
		vh = 1414807858;//TTA2
	}
	else if (pVersion == 1) {
		vh = 1414807859;//TTA3
	}
	else {
		vh = 1414807860;//TTA4
	}
	writeint(&vh, 4);
	u32 i = 2;
	if (pVersion <= 1) {
		writeint(&i, 4);
	}
	i = 0;
	for (auto& it : entries)
		i += 1 + it.name.length();
	u32 ntz = i;
	i += 0x10000 - (i % 0x10000);
	writeint(&i, 4);
	i = (u32)entries.size();
	writeint(&i, 4);
	if(func)
		func("Writing Headers", 10);
	u64 curroff = 0;
	u64 size = 0;
	u32 curnameoff = 0;
	std::sort(entries.begin(), entries.end(), _Cmp);
	for (i = 0; i < entries.size(); i++) {
		ResourceCreateEntry* entry = &entries[i];
		u64 crc = CRC64_CaseInsensitive(0, entry->name.c_str());
		writeint(&crc, 8);
		writeint(&curroff, 8);
		crc = 0;
		if(pVersion < 1)
			writeint(&crc, 4);
		size = entry->mpStream == 0 ? FileSizeA(entry->open_later.c_str()) : entry->mpStream->GetSize();
		crc = size;
		writeint(&crc, 4);
		crc = 0;
		writeint(&crc, 4);
		crc = curnameoff / 0x10000;
		writeint(&crc, 2);
		crc = curnameoff % 0x10000;
		writeint(&crc, 2);
		curnameoff += (u32)entry->name.length() + 1;
		curroff += size;
	}
	float pr = 10;
	float incr = 10.0f / (float)entries.size();
	for (int x = 0; x < (int)entries.size(); x++) {
		out.SerializeWrite(entries[x].name.c_str(), (u32)entries[x].name.length() + 1);
		//func(NULL, pr);
		pr += incr;
	}
	int rem = 0x10000 - (ntz%0x10000);
	char* temp = (char*)calloc(1, rem);
	out.Serialize(temp, rem);
	free(temp);
	incr = 60.0f / (float)entries.size();
	for (int x = 0; x < (int)entries.size(); x++) {
		if(entries[x].open_later.length() != 0 && entries[x].mpStream == 0){
			DataStreamFileDisc fileStream = _OpenDataStreamFromDisc_(entries[x].open_later.c_str(), READ);
			fileStream.Copy(&out, out.GetPosition(), 0, fileStream.GetSize());
		}
		else {
			entries[x].mpStream->Copy(&out, out.GetPosition(), 0, entries[x].mpStream->GetSize());
		}
		if (func)
			func(entries[x].name.c_str(), pr);
		pr += incr;
	}
	pr = 80;//in case
	out.SetPosition(0, DataStreamSeekType::eSeekType_Begin);
	out.SetMode(DataStreamMode::eMode_Read);
	DataStreamContainerParams p;
	p.mpSrcStream = &out;
	p.mpDstStream = pDst;
	p.mDstOffset = pDst->GetPosition();
	p.mbEncrypt = pEncrypt;
	p.mbCompress = pCompress;
	p.mCompressionLibrary = pCompressionLibrary;
	pr = 85;
	DataStreamContainer::Create(func,p, out.GetSize());
	pr = 100;
	if (func)
		func("Created container", pr);
	return true;
}

String TTArchive2::GetResourceName(const Symbol& name) {
	if (!mbActive)return NULL;
	TTArchive2::ResourceEntry* e = _FindResource(name);
	if (!e)return NULL;
	if (!mpNamePageCache)
		mpNamePageCache = (char*)malloc(0x10000);
	if (!mpNamePageCache)
		return NULL;
	if (mNamePageCacheIndex != e->mNamePageIndex) {
		mpNameStream->SetPosition(e->mNamePageIndex << 16,
			DataStreamSeekType::eSeekType_Begin);
		mpNameStream->Serialize(mpNamePageCache, 0x10000);
		mNamePageCacheIndex = e->mNamePageIndex;
	}
	return String{ mpNamePageCache + e->mNamePageOffset };
}

bool TTArchive2::HasResource(const Symbol& sym) {
	return _FindResource(sym) != NULL;
}

TTArchive2::ResourceEntry* TTArchive2::_FindResource(const Symbol& sym) {
	if (mbActive) {
		i32 p = 0;
		i32 r = (i32)mResources.size() - 1;
		i32 q = (r + p) / 2;

		while (p <= r)
		{
			if (mResources[q].mNameCRC == sym.GetCRC()) {
				return mResources.data() + q;
			}
			else
			{
				if (mResources[q].mNameCRC < sym.GetCRC())
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
	return NULL;
}

void TTArchive2::Deactivate() {
	mNamePageCacheIndex = -1;
	if (mpResourceStream)
		delete mpResourceStream;
	if (mpNameStream)
		delete mpNameStream;
	if (mResources.size())
		mResources.clear();
	if (mpInStream)
		delete mpInStream;
	if (mpNamePageCache)
		free(mpNamePageCache);
	mpResourceStream = 0;
	mpNameStream = 0;
	mpInStream = 0;
	mpNamePageCache = 0;
	mbActive = false;
}