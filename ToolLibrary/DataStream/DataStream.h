// This file was written by Lucas Saragosa. Im the author of this interpretation of 
// the engine and require that if you use this code or library, you give credit to me and
// the amazing Telltale Games.

#pragma once

#include "../LibraryConfig.h"
#include "../Compression.h"
#include <cstdio>
#include <memory>
#include <vector>
#include <list>

#ifndef _DATASTREAM
#define _DATASTREAM

#define DEFAULT_GROWTH_FACTOR 0x1000

#define READ DataStreamMode::eMode_Read
#define WRITE DataStreamMode::eMode_Write

//Returns a new instance as an object directly 
#define _OpenDataStreamFromDisc_(file_path, mode) DataStreamFileDisc(\
PlatformSpecOpenFile(file_path,\
	mode),\
	mode)

//Returns a new instance as a pointer which needs to be deleted. Most classes
//will delete it when done with it (consumers)
#define _OpenDataStreamFromDisc(file_path, mode) \
new _OpenDataStreamFromDisc_(file_path, mode)

//With line terminator
#define OpenDataStreamFromDisc(file_path, mode) \
_OpenDataStreamFromDisc(file_path, mode);

enum class DataStreamSeekType : unsigned char {
	eSeekType_Begin = 0,
	eSeekType_Current = 1,
	eSeekType_End = 2
};

class DataStreamSubStream;

/*
* A data stream. Abstract class which represents a stream of data (bytes) being read or written.
* Class is *not copyable* but is moveable.
*/
class DataStream {
public:
	DataStreamMode mMode;
	int mSubStreams;

	virtual bool Copy(DataStream* pDst, unsigned __int64 pDstOffset, unsigned __int64 pSrcOffset, unsigned __int64 size);

	/*
	* Serialize bytes. First is the buffer, second is the size. The mode member variable decides if its write or reading into the buffer.
	* Returns if this function was successful. 
	*/
	virtual bool Serialize(char*, unsigned __int64) = 0;

	/*
	* Serialize helper function, to write a const pointer instead.
	* This is not the writing function, Serialize writes and reads depending
	* on the mode.
	*/
	virtual bool SerializeWrite(const char* ptr, unsigned __int64 size) {
		if (IsWrite()) {
			return Serialize((char*)((void*)ptr), size);
		}
		else if (!size)return true;
		return false;
	}

	virtual bool SerializeStringRead(char* dest, unsigned __int64 size) {
		if (IsRead()) {
			return Serialize(dest, size);
		}
		return false;
	}

	virtual bool SerializeStringWrite(const char* str) {
		return SerializeWrite(str, strlen(str));
	}

	/*
	* Gets the size in bytes of this stream.
	*/
	virtual unsigned __int64 GetSize() const = 0;

	/*
	* Transfers bytes from this stream to the given stream
	*/
	virtual bool Transfer(DataStream* dst, unsigned __int64 off, unsigned __int64 size) = 0;

	/*
	* Gets the position (offset) of this stream.
	*/
	virtual unsigned __int64 GetPosition() const = 0;

	/*
	* Sets the position or offset of this stream. The position is deduced by the seek type parameter.
	*/
	virtual bool SetPosition(signed __int64, DataStreamSeekType) = 0;

	/*
	* Truncates this stream to the given new size. If its over the stream size, adds zeros (if this stream can do that, otherwise will
	* return false) otherwise it will remove all excess bytes. Not available for all types of data stream.
	* Only works in write mode (will return false if its not in write mode)
	*/
	virtual bool Truncate(unsigned __int64) = 0;

	/*
	* Gets a sub-stream of this stream. A sub-stream is a READ-ONLY (will return null if its not read mode) stream which points
	* to a section of a data stream. Like a std::string_view for a std::basic_string/std::string
	*/
	virtual DataStreamSubStream* GetSubStream(unsigned __int64 off,unsigned __int64 size);
	
	/*
	* Sets the mode of this stream. If there are substreams attached to this one and you try to set it to not read then it fails.
	*/
	bool SetMode(DataStreamMode);

	virtual bool IsRead() { return mMode == DataStreamMode::eMode_Read; }
	virtual bool IsWrite() { return mMode == DataStreamMode::eMode_Write; }
	virtual bool IsInvalid() { return mMode == DataStreamMode::eMode_Unset; }

	DataStream& operator=(DataStream&&) ;
	DataStream& operator=(DataStream const&) = delete;
	DataStream(DataStream& const) = delete;
	DataStream(DataStream&&) ;
	DataStream() : mMode(DataStreamMode::eMode_Unset), mSubStreams(0) {}
	DataStream(DataStreamMode mode) : mMode(mode), mSubStreams(0) {}

	virtual ~DataStream() = default;

};

/*
* A data stream implementation which comes from a file on disc (not actually specific to windows).
* This class can be optimized a lot (using runtime buffers like Java's BufferedReader and BufferedWriter), so feel free to reimplement that.
*/
class DataStreamFile_PlatformSpecific : public DataStream {
public:

	FileHandle mHandle = EMPTY_FILE_HANDLE;
	i64 mStreamOffset = 0, mStreamSize = 0;

	bool Serialize(char*, unsigned __int64);
	unsigned __int64 GetSize() const { return mStreamSize; }
	unsigned __int64 GetPosition() const { return mStreamOffset; };
	bool SetPosition(signed __int64, DataStreamSeekType);
	bool Truncate(unsigned __int64);
	//cant transfer from a file, only used for memory streams
	bool Transfer(DataStream* dst, unsigned __int64 off, unsigned __int64 size);

	DataStreamFile_PlatformSpecific();
	DataStreamFile_PlatformSpecific(FileHandle H, DataStreamMode);
	DataStreamFile_PlatformSpecific(DataStreamFile_PlatformSpecific&&);
	DataStreamFile_PlatformSpecific& operator=(DataStreamFile_PlatformSpecific&&);
	DataStreamFile_PlatformSpecific(DataStreamFile_PlatformSpecific const&) = delete;
	DataStreamFile_PlatformSpecific& operator=(DataStreamFile_PlatformSpecific& const) = delete;

	inline virtual ~DataStreamFile_PlatformSpecific() { 
		PlatforSpecCloseFile(mHandle);
	};

};

class DataStreamSubStream : public DataStream {
public:

	DataStream* mpBase;
	unsigned __int64 mOffset, mStreamOffset, mSize;

	bool Serialize(char*, unsigned __int64);

	unsigned __int64 GetSize() const { return mSize; }

	unsigned __int64 GetPosition() const { return mOffset; };

	bool SetPosition(signed __int64, DataStreamSeekType);

	inline bool Truncate(unsigned __int64 newz) {
		if (newz <= mSize) {
			mSize = newz;
			if (mOffset > mSize)
				mOffset = mSize;
			return true;
		}
		return false;
	};

	bool Transfer(DataStream* dst, unsigned __int64 off, unsigned __int64 size);

	virtual DataStreamSubStream* GetSubStream(unsigned __int64 off, unsigned __int64 size);

	DataStreamSubStream(DataStream*, unsigned __int64  size);//substream starts at base stream current offset, size is size (param 2)
	DataStreamSubStream(DataStream*, unsigned __int64 size, unsigned __int64 baseOffset);
	DataStreamSubStream(DataStreamSubStream&&);
	DataStreamSubStream& operator=(DataStreamSubStream&&);
	DataStreamSubStream(DataStreamSubStream& const) = delete;
	DataStreamSubStream& operator=(DataStreamSubStream& const) = delete;

	~DataStreamSubStream();

};

class DataStreamMemory : public DataStream {
public:

	u64 mSize;
	u64 mOffset;
	u64 mGFact = DEFAULT_GROWTH_FACTOR;
	void* mMemoryBuffer;

	bool Serialize(char*, unsigned __int64);
	unsigned __int64 GetSize() const { return mSize; }
	unsigned __int64 GetPosition() const { return mOffset; }
	bool SetPosition(signed __int64, DataStreamSeekType);
	bool Truncate(unsigned __int64 new_size);
	bool Transfer(DataStream* dst, unsigned __int64 off, unsigned __int64 size);

	inline unsigned __int64 GetMemoryBufferSize() {
		int memorybufsize = mSize;
		if (mSize % mGFact)memorybufsize += mGFact - (mSize % mGFact);
		return memorybufsize;
	}


	//buffer param needs to be allocated with malloc/calloc
	DataStreamMemory(void* buffer, unsigned __int64 size,DataStreamMode m) : mMemoryBuffer(buffer), mSize(size), mOffset(0), DataStream(m) {}
	DataStreamMemory(void* buffer, unsigned __int64 size, unsigned __int64 growthFactor, DataStreamMode m)
		: mMemoryBuffer(buffer), mSize(size), mOffset(0), mGFact(growthFactor), DataStream(m) {};
	DataStreamMemory(unsigned __int64 initialSize);
	DataStreamMemory(unsigned __int64 initialSize, unsigned __int64 growthFactor);
	DataStreamMemory(DataStreamMemory&&);
	DataStreamMemory& operator=(DataStreamMemory&&);
	DataStreamMemory(DataStreamMemory const&) = delete;
	DataStreamMemory& operator=(DataStreamMemory& const) = delete;
	~DataStreamMemory();

};

//*NOT COPYABLE OR MOVABLE* A legacy encrypted stream which decrypts in chunks for old games which us MBIN, MBES.
//NOT WRITABLE, ONLY READABLE!
class DataStreamLegacyEncrypted : public DataStream {

	DataStream* mpBase;
	unsigned int mHeader;//start pos
	unsigned int mEncryptSize, mEncryptInterval, mEncryptSkip;
	unsigned __int64 mSize, mOffset;
	int mCurrentBlock;

public:

	char mBuf[0x100];

	bool Serialize(char*, unsigned __int64);
	unsigned __int64 GetSize() const { return mSize + mHeader; }
	unsigned __int64 GetPosition() const { return mOffset + mHeader; }
	bool SetPosition(signed __int64, DataStreamSeekType);

	inline bool Truncate(unsigned __int64 new_size) {
		return false;
	};

	bool Transfer(DataStream* dst, unsigned __int64 off, unsigned __int64 size) { return false; }
	DataStreamLegacyEncrypted(DataStream*,int version, unsigned int startPos);
	DataStreamLegacyEncrypted(DataStreamLegacyEncrypted&&) = delete;
	DataStreamLegacyEncrypted& operator=(DataStreamLegacyEncrypted&&) = delete;
	DataStreamLegacyEncrypted(DataStreamLegacyEncrypted const&) = delete;
	DataStreamLegacyEncrypted& operator=(DataStreamLegacyEncrypted& const) = delete;

};

struct DataStreamContainerParams {

	DataStream* mpSrcStream = 0;
	DataStream* mpDstStream = 0;
	//offset which to start serialing to in the destination stream
	unsigned __int64 mDstOffset = 0;
	unsigned __int32 mWindowSize = 0;
	bool mbCompress = 0;
	bool mbEncrypt = 0;
	Compression::Library mCompressionLibrary;

	inline DataStreamContainerParams() : mWindowSize(0x10000), mCompressionLibrary(Compression::Library::ZLIB) {}

};
//A compressed/encrypted data stream container used for READING data.
//To write use Create. This is where TTNC,TTCZ,TTCE,TTCe,TTCz is.
//In read mode takes ownership of the DataStreamContainerParams::mSrcStream (deletes)
class DataStreamContainer : public DataStream {

	//page and chunk are synonymous

	bool GetChunk(unsigned __int64 index);

	inline unsigned __int64 GetCompressedPageSize(unsigned __int32 index);

public:

	typedef void (*ProgressF)(const char* _Msg, float _Progress);

	u64 mStreamOffset, mStreamPosition, mStreamSize;
	u64 mStreamStart;
	DataStreamContainerParams mParams;
	char* mpCachedPage;//0x32
	char* mpReadTransitionBuf;
	signed __int32 mCurrentIndex;// , mCacheablePages;
	unsigned __int64* mPageOffsets;
	unsigned __int64 mNumPages;
	bool ok = false;

	inline static std::shared_ptr<DataStreamContainer> ReadContainer(DataStream* pSrcStream, u64 off, u64* pSizeTransferred){
		DataStreamContainerParams params{};
		params.mbCompress = params.mbEncrypt = false;
		params.mDstOffset = 0;
		params.mpDstStream = 0;
		params.mpSrcStream = pSrcStream;
		std::shared_ptr<DataStreamContainer> container = std::make_shared<DataStreamContainer>(params);
		container->Read(off, pSizeTransferred);
		return container;
	}



	//init from src stream
	void Read(unsigned __int64 offset, unsigned __int64* pContainerSize);

	//Creates a TT data stream container with the parameters. Serializes from src to dest. srcInStreamSize is the amount of bytes to
	//serialize from the src stream from the source streams current offset
	//progres function starts progress percentage at 80 (assumming you use this with .ttarch, otherwise just modify the function input)
	static void Create(ProgressF, DataStreamContainerParams, unsigned __int64 srcInStreamSize);
	bool SetPosition(signed __int64, DataStreamSeekType);
	bool Serialize(char*, unsigned __int64);

	unsigned __int64 GetSize() const { return mStreamSize; }
	unsigned __int64 GetPosition() const { return mStreamPosition; }

	inline bool Truncate(unsigned __int64 new_size) {
		return false;
	};

	inline bool Transfer(DataStream* dst, unsigned __int64 off, unsigned __int64 size) {
		return Copy(dst, dst->GetPosition(), off, size);
	}

	inline DataStreamContainer(DataStreamContainerParams params) : DataStream(DataStreamMode::eMode_Read), 
		mParams(params), mStreamOffset(0), /*mCacheablePages(-1),*/ mpReadTransitionBuf(NULL),
		mStreamSize(0), mStreamStart(0),
		mCurrentIndex(-1), mStreamPosition(0), mNumPages(0), mPageOffsets(NULL), mpCachedPage(NULL) {}//Create

	~DataStreamContainer();

	inline Compression::Library GetCompressionLibrary() { return mParams.mCompressionLibrary; }

	inline bool IsCompressed() {
		return mParams.mbCompress;
	};

	inline bool IsEncrypted() {
		return mParams.mbEncrypt;
	}

	DataStreamContainer(DataStreamContainer&&) = default;
	DataStreamContainer& operator=(DataStreamContainer&&) = default;

	DataStreamContainer(DataStreamContainer const&) = delete;
	DataStreamContainer& operator=(DataStreamContainer& const) = delete;

};

typedef DataStreamFile_PlatformSpecific DataStreamFileDisc;

#endif