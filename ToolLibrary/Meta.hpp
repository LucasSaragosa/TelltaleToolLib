// This file was written by Lucas Saragosa. The code derives from Telltale Games' Engine.
// I do not intend to take credit for it, however; Im the author of this interpretation of 
// the engine and require that if you use this code or library, you give credit to me and
// the amazing Telltale Games.

#ifndef _META
#define _META

#pragma warning (disable : 4018 4244 4267 4554 6387 4099 4305 4715 6387 4319 4227)

#include "Types/UtilityTypes.h"
#include "TelltaleToolLibrary.h"
#include <string>
#include <math.h>
#include <typeinfo>
#include <stack>
#include <vector>
#include <stack>
#include "Types/TRange.h"
#include "HashManager.h"
#include "HashDB/HashDB.h"

// --------------------------------------------------------------------------------------- MACROS ----------------------------------------------------------------------------------------

//set to false to disallow the debug section of meta stream files to be loaded. default true
#ifndef METASTREAM_ENABLE_DEBUG
#define METASTREAM_ENABLE_DEBUG true
#endif

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
#define LITTLE_ENDIAN 0
#define BIG_ENDIAN 1
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
#define LITTLE_ENDIAN 1
#define BIG_ENDIAN 0
#elif defined(_MSC_VER) 
#define LITTLE_ENDIAN 1
#define BIG_ENDIAN 0
#else
#error "Could not detect endian"
#endif

#define METAOP_FUNC_DEF(_FuncName) MetaOpResult MetaOperation_##_FuncName(void *,MetaClassDescription*,MetaMemberDescription*,void*);
#define METAOP_FUNC_IMPL(_FuncName) MetaOpResult Meta::MetaOperation_##_FuncName(void *pObj,MetaClassDescription* pObjDescription,\
	MetaMemberDescription *pContextDescription,void *pUserData)

#define METAOP_FUNC_IMPL_(NS,_FuncName) MetaOpResult NS::MetaOperation_##_FuncName(void *pObj,MetaClassDescription* pObjDescription,\
	MetaMemberDescription *pContextDescription,void *pUserData)

#define METAOP_FUNC_IMPL__(_FuncName) MetaOpResult MetaOperation_##_FuncName(void *pObj,MetaClassDescription* pObjDescription,\
	MetaMemberDescription *pContextDescription,void *pUserData)

#define CAST_METAOP(Ty, _name) MetaStream* meta = static_cast<MetaStream*>(pUserData); Ty* _name = static_cast<Ty*>(pObj);

// --------------------------------------------------------------------------------------- DEBUG STRING TYPE (INT) ----------------------------------------------------------------------------------------

struct DebugString : String {

	static METAOP_FUNC_DEF(SerializeAsync);

};

// --------------------------------------------------------------------------------------- VERSION HEADER STRINGS ----------------------------------------------------------------------------------------

/**
 * Any MetaOp returns a MetaOpResult. These are the valid return codes.
 */
enum MetaOpResult {
	eMetaOp_Fail = 0x0,
	eMetaOp_Succeed = 0x1,
	eMetaOp_Invalid = 0x2,
	eMetaOp_OutOfMemory = 0x3,
	eMetaOp_SymbolNotFoundInDB = 0x4,
	eMetaOp_SymbolNotFound = 0x4,
	eMetaOp_MAX = 0x5,//normally 4
};

const u32 EncrypedVersionHeaders[] = {
	0x64AFDEAA,//version 3
	0x64AFDEBB,//version 3 (encrypted MCOM)
	0xFB4A1764,//version 2
	0xEB794091,//version 2
	0x64AFDEFB,//version 2
};

//The reason they are reversed is because in the executables these are stored as INTS and because of the endian they are reversed
constexpr const char VersionHeaders[][5] = {
#if LITTLE_ENDIAN == 1
	"SEBM",//version 0, Meta Binary Encrypted Stream
	"NIBM",//version 0, Meta BINary
	"ERTM",//version 3, Meta Type REference (I think, more like meta try reverse engineer (assholes, god im lonely)) maybe MT in MTRE is meta
	"MOCM",//version 3, Meta COMpressed, compressed version of MTRE. this might be wrong, im thinking its more meta communication?
	//maybe for example transporting stuff in the engine?
	"4VSM",//version 4, Meta Stream Version 4
	"5VSM",//version 5
	"6VSM",//version 6
#else
	"MBES",//version 0, Meta Binary Encrypted Stream
	"MBIN",//version 0, Meta BINary
	"MTRE",//version 3, no clue
	"MCOM",//version 3, Meta COMpressed
	"MSV4",//version 4, Meta Stream Version 4
	"MSV5",//version 5
	"MSV6",//version 6
#endif
};

constexpr const char* VersionHeadersStr[7] = {
	"MBES - Meta Binary Encrypted Stream",//version 0, Meta Binary Encrypted Stream
	"MBIN - Meta Binary",//version 0, Meta BINary
	"MTRE = Meta Try Reverse Engineer (this) - I just did :P",//version 3, no clue
	"MCOM - Meta Compressed",//version 3, Meta COMpressed
	"MSV4 - Mea Stream Version 4",//version 4, Meta Stream Version 4
	"MSV5 - Meta Stream Version 5",//version 5
	"MSV6 - Meta Stream Version 6",//version 6
};

constexpr const char* spVersNameFormat = "%s(%s).vers";

// --------------------------------------------------------------------------------------- FUNCTION DECLARATIONS (INTERNAL & HELPERS) ----------------------------------------------------------------------------------------

class MetaStream;
struct MetaMemberDescription;
class Symbol;
struct MetaClassDescription;
struct MetaSerializeAccel;

// INTERNAL SERIALIZED VERSION INFO REGISTRAR
void __ReleaseSVI_Internal();
void __RegisterSVI_Internal(void* svi);

template<typename T, typename U> constexpr size_t memberOffset(U T::* member)
{
	return (char*)&((T*)nullptr->*member) - (char*)nullptr;
}

template<typename Base, typename Derived> constexpr size_t parentOffset() {
	return (reinterpret_cast<char*>(static_cast<Base*>(reinterpret_cast<Derived*>(0x10000000))) - reinterpret_cast<char*>(0x10000000));
}

/**
 * Gets the META header version magic string for the given version number (0 to 6 inclusive). See VersionHeaderStr array.
 */
static constexpr u32 GetMetaMagic(int versionID) {
	if (versionID >= 0 && versionID <= 6)return *((u32*)VersionHeaders[versionID]);
	return 0;
}

/**
 * Helper function to template format a string
 */
template<typename ... Args>
std::string string_format(const std::string& format, Args ... args);

/**
 * Helper function. Converts a symbol to a string, into the format Symbol<XXX> where XXX is the upper case hex CRC64.
 */
std::string SymToString(Symbol* pSymbol);

// --------------------------------------------------------------------------------------- FUNCTION DECLARATIONS (EXPOSED) ----------------------------------------------------------------------------------------

// LIBRARY DLL EXPORTED FUNCTIONS.

_TTToolLib_Exp MetaStream* TelltaleToolLib_CreateAndOpenMetaStream(const char* pFileMemory, int fileSize);

/*returns the new meta stream file data, poutsize contains the size. deletes pStream. IF outsize == nullptr, then assuming you dont want the data*/
_TTToolLib_Exp void* TelltaleToolLib_DeleteAndFlushMetaStream(MetaStream* pStream, u64* pOutSize);

_TTToolLib_Exp MetaOpResult TelltaleToolLib_SerializeMetaStream(MetaStream* pStream, MetaClassDescription* pClass, void* pClassInstance);

_TTToolLib_Exp void TelltaleToolLib_MetaStreamSwitchToWrite(MetaStream* pStream);

/*
* Tries to find the symbol name for the given symbol. Make sure the global hash database is set. This is will try search all pages in the
* current game set (using set blowfish key). This can be slow! If the value is found, the outpageref pointer is set to the page it was
* found in in case you want to find more symbol names in that page. Returns <NotFound> if not found, or <Empty> if the CRC is 0.
* Use once the library is initialized.
*/
String FindSymbolName(const Symbol&, HashDatabase::Page*& outpageref);

/*
* Gets the constant list of file extension strings that the library currently supports.
*/
const std::vector<const char*>* GetMetaFileExtensions();

/**
 * Gets an MCD by its type info name, eg T3Texture
 */
MetaClassDescription* GetMetaClassDescription(const char* typeInfoName);

/**
 * Removes 'class ' 'struct ' 'std::' etc from type name to make it a Meta typename.
 */
String MetaVersion_ConvertTypeName(const String& from);

/**
 * Returns if the given member exists in the current selected game in the version of the game engine.
 */
bool MetaVersion_MemberExistsInCurrentVersion(MetaMemberDescription* pDesc, MetaStream* stream);

/**
 * Does the same as MetaVersion_MembersExists...
 */
bool Meta_IsMemberDisabled(MetaMemberDescription* member, i32 streamVersion);

MetaSerializeAccel* MetaSerialize_GenerateAccel(MetaClassDescription*);

//Serialize or deserialize the given type. Use PerformMetaSerializeAsync which gets the meta class description for you.
//If you are using this library not as a template library (but a compiled one) then use this!
MetaOpResult PerformMetaSerializeFull(MetaStream* pStream, void* pObj, MetaClassDescription* pDesc);

template<typename T> MetaOpResult PerformMetaSerializeAsync(MetaStream* pStream, T* pObj) {
	MetaClassDescription* desc = GetMetaClassDescription(typeid(T).name());
	if (!desc || !pStream)return eMetaOp_Fail;
	return PerformMetaSerializeFull(pStream, pObj, desc);
}

/**
 * The Meta namespace contains most of the functions to work with serialized and managed types in the Meta reflection system.
 * Most functions here are overriden by types if needed. These are the default ones if they are not overriden.
 */
namespace Meta {

	/**
	 * This MetaOp sets the SVI version CRC hash which is found in all Meta headers in newer games (a standard CRC32).
	 */
	MetaOpResult MetaOperation_SerializedVersionInfo(void* pObj,
		MetaClassDescription* pObjDescription, MetaMemberDescription* pContextDesc,
		void* pUserData);

	struct Equivalence {
		bool mbEqual;
		void* mpOther;
	};

	typedef void (*EnumerateMembersFunc)(void*, MetaClassDescription*, MetaMemberDescription*);

	struct EnumerateMembersInfo {
		EnumerateMembersFunc mpFunc;
		std::vector<void*> mArgs;//DCArray<void*>
	};

	struct ConvertFromInfo {
		const void* mpFromObject;
		MetaClassDescription* mpFromObjDescription;
	};

	METAOP_FUNC_DEF(GetObjectName)

	METAOP_FUNC_DEF(EnumerateMembers)

	/**
	 * Destroys a given object - calls the destructor.
	 */
	METAOP_FUNC_DEF(Destroy)

	/**
	 * The default serializer. This serializes each member sequentially. This is normally called first and then the type has a custom serializer
	 * which reads any other extra data which isn't standard.
	 */
	MetaOpResult MetaOperation_SerializeAsync(void*, MetaClassDescription*,
		MetaMemberDescription*, void*);

	/**
	 * Ignore this.
	 */
	METAOP_FUNC_DEF(SerializeMain)

	/**
	 * Not any use.
	 */
	METAOP_FUNC_DEF(Equivalence)

	/**
	 * Not any use. Use PerformMetaSerializeAsync or PerformMetaSerializeFull
	 */
	METAOP_FUNC_DEF(AsyncSave)

};

// --------------------------------------------------------------------------------------- META TYPE DEFINITIONS ----------------------------------------------------------------------------------------

class ContainerInterface;

typedef ContainerInterface* ContainerInterfaceCastFn(void* praw);

const struct MetaStreamParams {
	bool mbCompress;
};

/**
 * Meta stream mode. Types of modes which a stream can be currently set to.
 */
enum class MetaStreamMode {
	eMetaStream_Closed = 0x0, eMetaStream_Read = 0x1, eMetaStream_Write = 0x2,
};

/**
 * Stored in the header of all serialized meta files. The type name symbol hash, and then version CRC from the SVI.
 */
struct MetaVersionInfo {
	u64 mTypeSymbolCrc;
	u32 mVersionCrc;
};

/**
 * Function spec for any Meta operation function.
 */
typedef MetaOpResult(*MetaOperation)(void* pObj, MetaClassDescription* pObjDesc, MetaMemberDescription* pContextDesc, void* pUserData);

/**
 * Serialized version info (SVI) stored the format of a given file in a .VERS file. This stored the hashes, version hashes, member names and blocked flags. This class for a given type
 * is very useful for determining the binary format for legacy types and is useful for backward compatibility.
 */
struct SerializedVersionInfo {

	SerializedVersionInfo* mpNext = nullptr;

	/**
	 * Type Member description 
	 */
	struct MemberDesc {
		String mName;
		String mTypeName;
		u64 mTypeNameSymbolCrc;
		bool mbBlocked;
		u32 mSize;
		u32 mVersionCrc;
		MetaMemberDescription* mpMemberDesc;
	};

	String mFileName;
	u64 mTypeSymbolCrc = 0;
	u32 mVersionCrc = 0;
	u32 mSize = 0;
	bool mbBlocked = false;
	std::vector<MemberDesc> mMembers;//DCArrayNM<MemberDesc> mMembers;
	bool mbOldVersion = false;//true for games in old .ttarch (sam max 1-3 etc), false for new games (twau>=)

	/**
	 * Call this to retrieve the SVI for the given MCD. This can be called once and will return the same pointer after that (It is cached in the MCD).
	 */
	static SerializedVersionInfo* RetrieveCompiledVersionInfo(MetaClassDescription* pObjDescription);

	//Originally would save to <Tool>/Meta/<file> This saves in .vers format. This writes everything (including header).
	//Vers file (serialized versions) names are in the format %s1(%s2).vers , where %s1 is the type name, 
	//and %s2 is the base 36 of the version CRC. Returns a datastream pointer, which you need to delete
	//Leave the versName as empty if you already have  the type symbol CRC set and it will be set to mFileName = versName = calc_crc...()
	// NOTE: THIS WILL ONLY WORK WHEN mbOldVersion is FALSE. OLD VERSIONS CANNOT BE SAVED DUE TO THEIR TYPE NAMES (any they shouldn't be saved !!)
	DataStream* Save(String* pVersName);

	/**
	 * Makes the file name into (%s)%s.vers format.
	 */
	static String MakeVersionFileName(const char* typeName, u32 tn);

	//TAKES OWNERSHIP OF STREAM PARAM, IT WILL BE DELETED AFTER USE
	//Loads this version information from a .vers.
	//There should be three parameters (type name crc, version crc) and the engine would load the .vers from that
	void RetrieveVersionInfo(/*u64 typeNameCrc, u32 typeVersionCrc, */ const char* versFileName, DataStream* stream);

};

/**
 * A Meta Stream manages the serializing of bytes into a data stream from a Meta type.
 */
class MetaStream {

public:

	enum RuntimeFlags {
		eWriteback = 0x1,
		eStreamIsCompiledVersion = 0x2,
		eIsUndo = 0x4
	};

	enum SectionType : u32 {
		eSection_Header = 0x0,
		eSection_Default = 0x1,
		eSection_Debug = 0x2,
		eSection_Async = 0x3,
		eSection_Count = 0x4
	};

	enum class StreamType {
		eStream_Binary = 0x0,
		eStream_JSON = 0x1
	};

	//Gets the stream type (always binary unless overriden by JSON)
	INLINE virtual MetaStream::StreamType GetStreamType() { return StreamType::eStream_Binary; }

	/**
	 * This saves the meta stream and writes the header. This then closes it and puts this class into a closed state, releasing resources. 
	 * Returns the full size of the written stream.
	 */
	virtual u64 Close();

	/**
	 * Attach. This function attaches a given data stream input (takes ownership, do not delete!) to this class. This will read the header but nothing else. With the header, you can know the primary 
	 * type which is serialized (look into mVersionInfo, the first element). Then you can perform the serialize meta operation on a type passing in this stream, and it will read it for you.
	 */
	virtual bool Attach(DataStream*, MetaStreamMode, MetaStreamParams);

	/**
	 * Same as Attach.
	 */
	virtual void Open(DataStream*, MetaStreamMode, MetaStreamParams);

	virtual void DisableDebugSection();

	virtual u64 GetPartialStreamSize();

	void PushMissingMemberCallback(bool(*cb)(
		SerializedVersionInfo::MemberDesc*, void*),
		void* mpUserData);

	void PopMissingMemberCallback();

protected:

	virtual i64 ReadData(void*, u32);
	virtual i64 WriteData(void*, u32);

public:

	/**
	 * Any subsequent reads or writes from from the async section of the stream
	 */
	virtual bool BeginAsyncSection();

	/**
	 * Any subsequent reads or writes from from the default section of the stream
	 */
	virtual void EndAsyncSection();

	/**
	 * Returns if this meta stream has anything in the async section (size nonzero).
	 */
	virtual bool HasAsyncSection();

	/**
	 * Any subsequent reads or writes from from the debug section of the stream
	 */
	virtual bool BeginDebugSection();

	/**
	 * Any subsequent reads or writes from from the default section of the stream
	 */
	virtual void EndDebugSection();

	/**
	 * Returns if the meta stream has anything in the debug section (size nonzero).
	 */
	virtual bool HasDebugSection();

	/**
	 * Returns the current number of bytes in the current section.
	 */
	virtual u64 GetSize();

	/**
	 * Returns the current position in the current section.
	 */
	virtual u64 GetPos();

	/**
	 * Set the current position in the current section of the stream.
	 */
	virtual void SetPos(u64);

	/**
	 * Skip ahead some number of bytes in the current section of the stream.
	 */
	virtual void Advance(int numBytes);

	/**
	 * Begins a block. This adds a block size to the serialized data. At the EndBlock() call, the size is calculated and written for you,
	 * with the current position unchanged.
	 */
	virtual void BeginBlock();

	/**
	 * See BeginBlock()
	 */
	virtual void EndBlock();

	/**
	 * If reading, this skips to the end of the current block.
	 */
	virtual void SkipToEndOfCurrentBlock();

	// JSON OVERRIDEN FUNCTIONS

	virtual bool NeedsKey();
	virtual void BeginObject(const char*, bool bArray);
	virtual void EndObject(const char*);
	virtual void BeginObject(const char*, MetaClassDescription*, MetaMemberDescription*, bool bArray);
	virtual void EndObject(const char*, MetaClassDescription*, MetaMemberDescription*);

	/**
	 * Adds a version. Internal.
	 */
	void AddVersion(const SerializedVersionInfo*);

	/**
	 * Finds meta header version info for the given type symbol. Can be used to see if the given type is serialized in this stream.
	 */
	MetaVersionInfo* GetStreamVersion(u64 typeSymbolCrc);

	/**
	 * Finds the meta header version info for the given type MCD. See the version with CRC.
	 */
	MetaVersionInfo* GetStreamVersion(MetaClassDescription* d);

	/**
	 * Serializes a string.
	 */
	virtual void serialize_String(String*);

	/**
	 * Serializes a symbol
	 */
	virtual void serialize_Symbol(Symbol*);

	/**
	 * Serializes a bool (writes as 0x31 or 0x30)
	 */
	virtual void serialize_bool(bool*);

	/**
	 * Serialized bytes.
	 */
	int serialize_bytes(void*, u32);

	// INTERNAL

	bool _ReadHeader(DataStream* partial, u64, u64* pOutBytesNeeded);
	void _WriteHeader();
	void _FinalizeStream();
	bool _SetSection(SectionType);

	/**
	 * Serializes a double64.
	 */
	virtual void serialize_double(long double*);

	/**
	 * Serializes a float32
	 */
	virtual void serialize_float(float*);

	/**
	 * Serializes a U16
	 */
	virtual void serialize_uint16(u16*);

	/**
	 * Serialized a U32
	 */
	virtual void serialize_uint32(u32*);

	/**
	 * Serialized a U64
	 */
	virtual void serialize_uint64(u64*);

	/**
	 * Serialized a U8
	 */
	virtual void serialize_int8(char*);

	// JSON OVERRIDEABLE

	virtual void HintSymbol(const char* symbolData);
	virtual void InsertStringHint(const char* pK, const char* pS);
	virtual const char* SkipStringHint(const char* pK);
	virtual void Key(const char* pS);

	/**
	 * Sets the current mode. Again takes ownership and will clear any current state.
	 */

	void SwitchToMode(MetaStreamMode newMode, DataStream* inputOrOutputStream);

	MetaStream();
	~MetaStream();

	struct BlockInfo {
		u32 mBlockLength;//or start pos
	};

	struct SectionInfo {
		DataStream* mpStream;//mpDataStream
		u64 mCompressedSize;
		std::stack<BlockInfo> mBlockInfo;//LIFO
		bool mbEnable;
		bool mbCompressed;
		//mReadBuffer, dont need it

		//u64 mStreamSize, mStreamOffset, mStreamPosition;

		SectionInfo() {
			mbEnable = true;
			mpStream = nullptr;
			mCompressedSize = 0;
			mbCompressed = false;
		}

	};

	struct WriteParams {//not in telltale tool, used for this library to write meta streams
		bool mbEncryptStream : 1;
		u8 mEncryptVersion : 7;//1 2 or 3
		bool mbSerializeAsCompressVersion;//MCOM. not supported (unknown int, and no files of this type in public, must be engine private).
		
		WriteParams() : mbEncryptStream(false), mEncryptVersion(3), mbSerializeAsCompressVersion(false) {}

	};

	//normally is separated into SubStreamInfo, each one could be a metastream or just data. for simplicity just done it like this

	SectionInfo mSection[(int)SectionType::eSection_Count];
	std::vector<MetaVersionInfo> mVersionInfo;
	std::vector<MetaClassDescription*> mSerializedTypes;
	MetaStreamParams mParams{};
	int mDebugSectionDepth = 0;
	SectionType mCurrentSection = eSection_Default;
	WriteParams mWriteParams{};

	struct MissingMemberCallbackInfo {
		bool(*mMissingMemberCallback)(SerializedVersionInfo::MemberDesc*, void*);
		void* mpUserData;
	};

	std::vector<MissingMemberCallbackInfo> mMissingMemberCallbacks;

	/*
	* Stream Versions:
	* 0: MBIN (and MBES if encrypted)
	* 1: Encrypted MBIN (weird headers)
	* 2: MTRE (if the meta stream header is not MTRE its a weird encrypted header)
	* 3: MCOM
	* 4: MSV4 - No default section, just async and debug
	* 5: MSV5 
	* 6: MSV6
	*/
	u32 mStreamVersion = 0;
	DataStream* mpReadWriteStream = nullptr;
	MetaStreamMode mMode = MetaStreamMode::eMetaStream_Closed;
	//Blowfish* mpBlowfish;
	Flags mRuntimeFlags;//flag values: RuntimeFlags enum
	//char mName[260];

	INLINE bool IsRead() {
		return mMode == MetaStreamMode::eMetaStream_Read;
	}

	INLINE bool IsWrite() {
		return mMode == MetaStreamMode::eMetaStream_Write;
	}

	bool mbDontDeleteStream = false;//by lib

	bool mbErrored = false;

	INLINE void BeginObject(Symbol* pSymbol, bool bArray) {
		BeginObject(SymToString(pSymbol).c_str(), bArray);
	}

	INLINE void EndObject(Symbol* pSymbol) {
		EndObject(SymToString(pSymbol).c_str());
	}

	INLINE void InjectVersionInfo(MetaStream& from) {
		for(auto it = from.mVersionInfo.begin(); it != from.mVersionInfo.end(); it++){
			MetaVersionInfo toAdd{ *it };
			for (auto it1 = mVersionInfo.begin(); it1 != mVersionInfo.end(); it1++) {
				if(it1->mTypeSymbolCrc == it->mTypeSymbolCrc){
					toAdd = { 0,0 };
					if (it1->mVersionCrc != it->mVersionCrc){
						it->mVersionCrc = it1->mVersionCrc;
					}
					break;
				}
			}
			if(toAdd.mTypeSymbolCrc != 0){
				mVersionInfo.push_back(toAdd);
			}
		}
		mStreamVersion = from.mStreamVersion;
	}

};

// TYPE DEFINITIONS FOR META OPERATION DEFAULTS.

typedef void* (*FuncNew)(void);
typedef void (*FuncDestroy)(void*);
typedef void (*FuncDelete)(void*);
typedef void (*FuncCopyConstruct)(void*, void*);
typedef void (*FuncConstruct)(void*);
typedef void (*FuncCastToConcreteObject)(void**, MetaClassDescription**);

extern i32 sMetaTypesCount;

/**
 * Symbol class. This is a CRC64 of the lower case of a string. Used everywhere in this library.
 */
class Symbol {
private:
	static char smSymbolBuffer[sizeof(u64) * 2 + 1];//1byte= 2 hex chars
	u64 mCrc64;
public:

	struct CompareCRC {
		bool operator()(const Symbol _Left, const Symbol _Right) const {
			return _Left.mCrc64 < _Right.mCrc64;
		}
	};

	constexpr Symbol(void) : mCrc64(0) {}

	constexpr Symbol(const char* pString) : mCrc64(CRC64_CaseInsensitive(0, pString)) {}

	//constexpr Symbol(const String& pString) {
	//	mCrc64 = CRC64_CaseInsensitive(0, pString.c_str());
	//}

	constexpr Symbol(u64 crc) : mCrc64(crc) {}
	//constexpr Symbol(const char* const pString) {
	//	mCrc64 = CRC64_CaseInsensitive(0, pString);
	//}

	constexpr Symbol& operator=(const Symbol& rhs) {
		this->mCrc64 = rhs.mCrc64;
		return *this;
	}

	inline Symbol operator+(const char* rhs){
		return Symbol(CRC64_CaseInsensitive(mCrc64, rhs));
	}

	inline Symbol& operator+=(const char* rhs){
		mCrc64 = CRC64_CaseInsensitive(mCrc64, rhs);
		return *this;
	}

	INLINE const char* CRCAsCstr() const {//not async
		sprintf(smSymbolBuffer, "%llx", mCrc64);
		return smSymbolBuffer;
	}

	constexpr INLINE void SetCRC(u64 crc) { mCrc64 = crc; }

	constexpr Symbol& Concat(const char* p) {
		mCrc64 = CRC64_CaseInsensitive(mCrc64, p);
		return *this;
	}

	constexpr INLINE u64 GetCRC() const {
		return mCrc64;
	}

	//AsConcat isnt needed

	INLINE operator const char* () const { return CRCAsCstr(); }

	static MetaOpResult MetaOperation_Equivalence(void* pObj,
		MetaClassDescription* pClassDescription, MetaMemberDescription* pContextDescription,
		void* pUserData) {//userdata=meta::equivalence
		static_cast<Meta::Equivalence*>(pUserData)->mbEqual = static_cast<Symbol*>(pObj)->mCrc64 ==
			static_cast<Symbol*>(static_cast<Meta::Equivalence*>(pUserData)->mpOther)->mCrc64;
		return MetaOpResult::eMetaOp_Succeed;
	}

	static MetaOpResult MetaOperation_SerializeAsync(void* pObj, MetaClassDescription* pClassDescription,
		MetaMemberDescription* pContextDescription, void* pUserData) {
		static_cast<MetaStream*>(pUserData)->serialize_Symbol(static_cast<Symbol*>(pObj));
		return eMetaOp_Succeed;
	}

	INLINE bool operator==(const Symbol& o) {
		return o.mCrc64 == mCrc64;
	}

	INLINE bool operator!=(const Symbol& o) {
		return o.mCrc64 != mCrc64;
	}

	static const Symbol kEmptySymbol;
	static const Symbol kTransitionMapKey;
	static const Symbol kAnimGroupArmLeft;
	static const Symbol kAnimGroupArmRight;
	static const Symbol kAnimGroupBodyLower;
	static const Symbol kAnimGroupBodyUpper;
	static const Symbol kAnimGroupBrowLeft;
	static const Symbol kAnimGroupBrowRight;
	static const Symbol kAnimGroupBrows;
	static const Symbol kAnimGroupCheekLeft;
	static const Symbol kAnimGroupCheekRight;
	static const Symbol kAnimGroupCheeks;
	static const Symbol kAnimGroupExtra;
	static const Symbol kAnimGroupEyeLids;
	static const Symbol kAnimGroupEyes;
	static const Symbol kAnimGroupFace;
	static const Symbol kAnimGroupFingersLeft;
	static const Symbol kAnimGroupFingersRight;
	static const Symbol kAnimGroupFootLeft;
	static const Symbol kAnimGroupFootRight;
	static const Symbol kAnimGroupHair;
	static const Symbol kAnimGroupHandRight;
	static const Symbol kAnimGroupHandLeft;
	static const Symbol kAnimGroupHead;
	static const Symbol kAnimGroupIndexFLeft;
	static const Symbol kAnimGroupIndexFRight;
	static const Symbol kAnimGroupLegLeft;
	static const Symbol kAnimGroupLegRight;
	static const Symbol kAnimGroupMouth;
	static const Symbol kAnimGroupNeck;
	static const Symbol kAnimGroupRoot;
	static const Symbol kAnimGroupShoulderLeft;
	static const Symbol kAnimGroupShoulderRight;
	static const Symbol kAnimGroupGroupSkirt;
	static const Symbol kAnimGroupGroupTail;
	static const Symbol kAnimGroupThumbLeft;
	static const Symbol kAnimGroupThumbRight;

	static const Symbol kPropKeyViewportOrigin;
	static const Symbol kPropKeyViewportSize;
	static const Symbol kPropKeyViewportCamera;
	static const Symbol kPropKeyViewportRelative;
	static const Symbol kPropKeyAllowInterpolation;
	static const Symbol kBGMStartPos;

	static const Symbol sAssertSymbol;
	static const Symbol sAssertTimeStampSymbol;
	static const Symbol sAssertFileSymbol;
	static const Symbol sAssertLineSymbol;
	static const Symbol sGeneralSymbol;
	static const Symbol sChoreSymbol;
	static const Symbol sLuaSymbol;
	static const Symbol sGraphicsSymbol;
	static const Symbol sDialogSymbol;
	static const Symbol sAudioSymbol;
	static const Symbol sQtSymbol;//telltale use qt in the tool!!!
	static const Symbol sExportSymbol;
	static const Symbol sCoreSymbol;

	static const Symbol kPropKeySource;
	static const Symbol kPropKeyWidth;
	static const Symbol kPropKeyHeight;
	static const Symbol kPropKeyPause;
	static const Symbol kPropKeyVolume;
	static const Symbol kPropKeyMediaTimeout;
	static const Symbol kPropKeyVolumeMaster;
	static const Symbol kPropKeyVolumeMusic;
	static const Symbol kPropKeyVolumeSound;
	static const Symbol kPropKeyVolumeVoice;
	static const Symbol kPropKeyMuteVoices;
	static const Symbol kPropKeyVolumeAmbient;
	static const Symbol kPropKeyMixMaster;
	static const Symbol kPropKeyMixMusic;
	static const Symbol kPropKeyMixSound;
	static const Symbol kPropKeyMixVoice;
	static const Symbol kPropKeyMixAmbient;
	static const Symbol kPropKeySoundMode;
	static const Symbol kCompressAnimations;
	static const Symbol kEncryptMetaStreams;
	static const Symbol kPropKeyROMRuntimeLoad;
	static const Symbol kPropKeyROMErrorDialogResource;
	static const Symbol kPropKeyROMErrorTitle;
	static const Symbol kPropKeyROMErrorMessage;
	static const Symbol kPropKeyROMPackSubDir;
	static const Symbol kPropKeyTextMinSec;
	static const Symbol kPropKeyTextSecPerLetter;
	static const Symbol kPropKeyUserSpaceAsArchive;
	static const Symbol kPropKeyDisableDevFolders;
	static const Symbol kPropKeyUsageOS;
	static const Symbol kPropKeyUsageCPU;
	static const Symbol kPropKeyUsageGPU;
	static const Symbol kPropKeyUsageDeviceType;
	static const Symbol kPropKeyUsageDeviceModel;
	static const Symbol kPropKeyUsageDeviceName;
	static const Symbol kPropKeyUsageLaunchCount;
	static const Symbol kPropKeyUsageShutdownCount;
	static const Symbol kPropKeyUsageSessionExecutionTime;
	static const Symbol kPropKeyUsageTotalExecutionTime;
	static const Symbol kChoreAgentGroupFilterIncludeNonSkeleton;
	static const Symbol kPropOpenLocalizations;
	static const Symbol kHDScreenShotResolution;
	static const Symbol kBlocking;
	static const Symbol kBlockingOffset;
	static const Symbol kBlockingOffsetContribution;
	static const Symbol kResourceNameSuffix;
	static const Symbol kLookAtBlockingAnimation;
	static const Symbol kLookAtBlockingOffsetAnimation;
	static const Symbol kLookAtBlockingOffsetContributionAnimation;

};

//would have used to be inclined getmetaaclassdescription functions, this would have had template specializations for all types.
template<typename T>
struct MetaClassDescription_Typed {

	static MetaClassDescription* GetMetaClassDescription() {
		return GetMetaClassDescription(NULL);
	}

	static MetaClassDescription* GetMetaClassDescription
	(const char* type);//originally this would be specialized

	static void* New(void) {
		void* ptr = operator new(sizeof(T));
		new (ptr) T();
		return ptr;
	}

	static void Destroy(void* pObj) {
		static_cast<T*>(pObj)->~T();
	}

	static void Construct(void* pObj) {
		new(pObj) T();
	}

	static void CopyConstruct(void* pDest, void* pSrc) {
		Construct(pDest);
		*static_cast<T*>(pDest) = *static_cast<T*>(pSrc);
	}

	static void Delete(void* pObj) {
		static_cast<T*>(pObj)->~T();
		operator delete(pObj);
	}

};

// --------------------------------------------------------------------------------------- META ENUMS ----------------------------------------------------------------------------------------

enum VTableFunction {
	eVTableNew = 0,
	eVTableDelete = 1,
	eVTableConstruct = 2,
	eVTableCopyConstruct = 3,
	eVTableDestroy = 4,
	//eVTableCastToConcreteObject = 5,
	eVTableCount = 5//6
};

enum MetaFlag : int {
	//Dont serialize this type
	MetaFlag_MetaSerializeDisable = 1,
	//Dont put a block size infront of this type
	MetaFlag_MetaSerializeBlockingDisabled = 2,
	//Place in game engine property menu
	MetaFlag_PlaceInAddPropMenu = 4,
	//No caption in the panel its in
	MetaFlag_NoPanelCaption = 8,
	//This type is a base class
	MetaFlag_BaseClass = 0x10,
	//Dont show this in the game engine editor
	MetaFlag_EditorHide = 0x20,
	//Is an enum, of type int
	MetaFlag_EnumIntType = 0x40,
	//Is an enum of type string
	MetaFlag_EnumStringType = 0x80,
	//Is a container type (eg a map, list, array)
	MetaFlag_ContainerType = 0x100,
	//Is a script enum type (enum used in lua scripts)
	MetaFlag_ScriptEnum = 0x200,
	//The name of this type (in meta member descriptions) is allocated on the heap
	MetaFlag_Heap = 0x400,
	//Serialized or created from lua scripts
	MetaFlag_ScriptTransient = 0x800,
	//Not seen this used yet
	MetaFlag_SelectAgentType = 0x1000,
	//Not seen this used yet, just object state is a meta operation
	MetaFlag_SkipObjectState = 0x2000,
	//Not seen this used yet
	MetaFlag_NotCacheable = 0x4000,
	//This type wraps an enum. Eg, this type is a struct EnumPlatformType{PlatformType mType}, where PlatformType is an enum
	//Reason its like this is because the enumplatformtype struct inherits from EnumBase and that has a seperate description
	MetaFlag_EnumWrapperClass = 0x8000,
	//Not seen this used yet
	MetaFlag_TempDescription = 0x10000,
	//This type is a handle (reference to a file). If serialized this is a CRC
	MetaFlag_Handle = 0x20000,
	//This type has a list of flags present with it (eg FlagsT3LightEnvGroupSet)
	MetaFlag_FlagType = 0x40000,
	//Not seen this used yet
	MetaFlag_SelectFolderType = 0x80000,
	//This type has no members
	MetaFlag_Memberless = 0x100000,
	//This type is a renderable resource (eg a texture or font)
	MetaFlag_RenderResource = 0x200000,
	//If this type has a block around it but the size of the serialized version is not always one value
	MetaFlag_MetaSerializeNonBlockedVariableSize = 0x400000 | MetaFlag_MetaSerializeBlockingDisabled,
	//Not seen this used yet
	MetaFlag_EmbeddedCacheableResource = 0x800000,
	//Not seen this used yet
	MetaFlag_VirtualResource = 0x1000000,
	//Not seen this used yet
	MetaFlag_DontAsyncLoad = 0x2000000,
	//If this type is not a meta file
	MetaFlag_IsNotMetaFile = 0x4000000,
	//If this type has been initialized
	Internal_MetaFlag_Initialized = 0x20000000,
	//By library, if this is set then the class is a proxy class and is prone to deletion (see reset proxy meta)
	Internal_MetaFlag_IsProxied = 0x40000000,
	Internal_MetaFlag_ShowAsHex = 0x10000000
};

/*
*
00000050 ReadData        dq ? ; offset 80
00000058 WriteData       dq ? ; offset 88
00000060 ReadDataStream  dq ? ; offset 96
00000068 BeginAsyncSection dq ? ; offset 104
00000070 EndAsyncSection dq ? ; offset 112
00000078 HasAsyncSection dq ? ; offset 120
00000080 BeginDebugSection dq ? ; offset 128
00000088 EndDebugSection dq ? ; offset 136
00000090 HasDebugSection dq ? ; offset 144
00000098 GetSize         dq ? ; offset 152
000000A0 GetPos          dq ? ; offset 160
000000A8 SetPos          dq ? ; offset 168
000000B0 Advance         dq ? ; offset 176
000000B8 BeginBlock      dq ? ; offset 184
000000C0 EndBlock        dq ? ; offset 192
000000C8 SkipToEndOfCurrentBlock dq ? ; offset 200
000000D0 BeginObject     dq ? ; offset 208
000000E8 EndObject       dq ? ; offset 232
00000100 BeginAnonObject dq ? ; offset 256
00000108 EndAnonObject   dq ? ; offset 264
00000110 SetObjectAsArrayType dq ? ; offset 272
00000118 AddVersion      dq ? ; offset 280
00000120 serialize_float dq ? ; offset 288
00000128 serialize_double dq ? ; offset 296
00000130 serialize_int   dq ? ; offset 304
00000138 serialize_uint  dq ? ; offset 312
00000140 serialize_int8  dq ? ; offset 320
00000148 serialize_uint8 dq ? ; offset 328
00000150 serialize_int16 dq ? ; offset 336
00000158 serialize_uint16 dq ? ; offset 344
00000160 serialize_int32 dq ? ; offset 352
00000168 serialize_uint32 dq ? ; offset 360
00000170 serialize_int64 dq ? ; offset 368
00000178 serialize_uint64 dq ? ; offset 376
00000180 serialize_String dq ? ; offset 384
00000188 serialize_Symbol dq ? ; offset 392
00000190 serialize_bool  dq ? ; offset 400
00000198 serialize_bytes dq ? ; offset 408*/

struct MetaMemberDescription;
struct MetaClassDescription;

struct MetaSerializeAccel {

	MetaSerializeAccel() : mpFunctionMain(NULL), mpFunctionAsync(NULL), mpMemberDesc(NULL) {}

	MetaOpResult(__cdecl* mpFunctionAsync)(void*, MetaClassDescription*,
		MetaMemberDescription*, void*);
	MetaOpResult(__cdecl* mpFunctionMain)(void*, MetaClassDescription*,
		MetaMemberDescription*, void*);
	MetaMemberDescription* mpMemberDesc;
};

struct MetaOperationDescription {

	enum sIDs {
		eMetaOpZero = 0x0,
		eMetaOpOne = 0x1,
		eMetaOpTwo = 0x2,//AddToChore
		eMetaOpThree = 0x3,
		//add to panel
		eMetaOpFour = 0x4,
		eMetaOpFive = 0x5,
		//convert
		eMetaOpSix = 0x6,
		eMetaOpSeven = 0x7,
		eMetaOpEight = 0x8,
		//equivalence
		eMetaOpNine = 0x9,
		eMetaOpTen = 0x0A,
		eMetaOpEleven = 0x0B,//GetLength
		eMetaOpTwelve = 0x0C,
		eMetaOpThirteen = 0x0D,
		eMetaOpFourteen = 0x0E,
		eMetaOpFifteen = 0x0F,//ObjectState
		eMetaOpSixteen = 0x10,//OpenEditor
		eMetaOpSeventeen = 0x11,
		eMetaOpEighteen = 0x12,//ScriptLock
		eMetaOpNineteen = 0x13,//ScriptUnlock
		eMetaOpTwenty = 0x14,
		eMetaOpTwentyOne = 0x15,//SerializedVersionInfo
		eMetaOpTwentyTwo = 0x16,//SetObjectName
		eMetaOpTwentyThree = 0x17,//ToString
		eMetaOpTwentyFour = 0x18,//GetName
		eMetaOpTwentyFive = 0x19,
		eMetaOpTwentySix = 0x1A,
		eMetaOpTwentySeven = 0x1B,
		eMetaOpTwentyEight = 0x1C,//CollectTyped
		eMetaOpTwentyNine = 0x1D,
		eMetaOpThirty = 0x1E,
		eMetaOpThirtyOne = 0x1F,
		eMetaOpThirtyTwo = 0x20,
		eMetaOpThirtyThree = 0x21,
		eMetaOpThirtyFour = 0x22,
		eMetaOpThirtyFive = 0x23,
		eMetaOpThirtySix = 0x24,
		eMetaOpThirtySeven = 0x25,
		eMetaOpThirtyEight = 0x26,
		eMetaOpThirtyNine = 0x27,
		eMetaOpForty = 0x28,
		eMetaOpFortyOne = 0x29,
		eMetaOpFortyTwo = 0x2A,
		eMetaOpFortyThree = 0x2B,
		eMetaOpFortyFour = 0x2C,
		eMetaOpFortyFive = 0x2D,
		eMetaOpFortySix = 0x2E,
		eMetaOpFortySeven = 0x2F,
		eMetaOpFortyEight = 0x30,
		eMetaOpFortyNine = 0x31,
		eMetaOpFifty = 0x32,
		eMetaOpFiftyOne = 0x33,
		eMetaOpFiftyTwo = 0x34,
		eMetaOpFiftyThree = 0x35,
		eMetaOpFiftyFour = 0x36,//PreloadDependentResources
		eMetaOpFiftyFive = 0x37,
		eMetaOpFiftySix = 0x38,
		eMetaOpFiftySeven = 0x39,
		eMetaOpFiftyEight = 0x3A,
		eMetaOpFiftyNine = 0x3B,
		eMetaOpSixty = 0x3C,
		eMetaOpSixtyOne = 0x3D,
		eMetaOpSixtyTwo = 0x3E,
		eMetaOpGetAssetCost = 0x3F,
		eMetaOpFromUiString = 0x40,
		eMetaOpToUiString = 0x41,
		eMetaOpWidgetFromClass = 0x42,
		eMetaOpUserOpen = 0x43,
		eMetaOpSave = 0x44,
		eMetaOpGetVersion = 0x45,
		eMetaOpCopy = 0x46,
		eMetaOpDelete = 0x47,
		eMetaOpAddToCache = 0x48,
		eMetaOpRemoveFromCache = 0x49,
		eMetaOpSerializeAsync = 0x4A,
		eMetaOpSerializeMain = 0x4B,
		eMetaOpDestroy = 0x4C,
		eMetaOpGetEstimatedVramUsage = 0x4D,
		eMetaOpEnumerateMembers = 0x4E,
		eMetaOpSetupPropertyValue = 0x4F,
		eMetaOpSavePropertyValue = 0x50,
		eMetaOpRecursiveVersionCRC = 0x51,
		eMetaOpNewResource = 0x52,
		eNumMetaOps = 0x53,
	};

	sIDs id;
	MetaOperation mpOpFn;
	MetaOperationDescription* mpNext;

};

// --------------------------------------------------------------------------------------- META IMPORTANT TYPES ----------------------------------------------------------------------------------------

/**
 * A meta class description. THis is the fundamental type which holds reflection information about a given C++ struct or class or even enum.
 */
struct MetaClassDescription {

	const char* mpExt;
	const char* mpTypeInfoName;
	const char* mpTypeInfoExternalName;//for lib
	u64 mHash;
	Flags mFlags;
	u32 mClassSize;
	u32 mClassAlign;
	SerializedVersionInfo* mpCompiledVersionSerializedVersionInfo;//atomic, lazily eval
	MetaMemberDescription* mpFirstMember;
	MetaOperationDescription* mMetaOperationsList;
	MetaClassDescription* pNextMetaClassDescription;
	void* mpVTable[5/*6*/];
	MetaSerializeAccel* mpSerializeAccel;//atomic
	ContainerInterfaceCastFn* mpVTable_ToContainerInterface;//by lib to not need to implements cast stuff. if list/map/set/etc then this casts inst to containerinterface (may add/rem some bytes from class offset in memory).
	bool mbNameIsHeapAllocated;//created by lib
	bool mbIsIntrinsic;//created by lib, intrinsics arent added to header. where does this filter?? no clue, so i have to add this
	//oh wait for the value above, nevermind just realised metaoperation_serializeasync is not ever called on intrinsics.
	//its only called other objects. since we check for the overloaded specialization function for serialize which is set.
	bool mbTypeUnfinished = false;//if true, this type has not been seen serialized yet and needs pending file examples. lib will let you know if we find one, so i can add support for the file
	bool mbHiddenInternal = false;//internal to code such as custom types not in the telltale tool so wont be found using getmetaclassdescription functions


	String* GetToolDescriptionName(String* result);

	void Delete(void* pObj);

	void Destroy(void* pObj);

	void* New();

	void Construct(void*);

	void CopyConstruct(void*, void*);

	inline MetaClassDescription() : mbNameIsHeapAllocated(false), mbIsIntrinsic(false), mpExt(NULL) {}
	~MetaClassDescription();

	bool MatchesHash(u64 hash);

	void GetDescriptionSymbol(Symbol*);

	Symbol GetDescriptionSymbol();

	MetaMemberDescription* GetMemberDescription(const char* memberName);

	INLINE MetaMemberDescription* GetMemberDescription(String* _Str) {
		return GetMemberDescription(_Str->c_str());
	}

	INLINE void Initialize(const std::type_info& info) {
		//i know its slow but it doesnt need to be super  fast and i cba to change this (heap allocations/deallocations)
		char* buf = (char*)calloc(1, strlen(info.name()) + 1);
		memcpy(buf, info.name(), strlen(info.name()));
		TelltaleToolLib_MakeInternalTypeName(&buf);
		mbNameIsHeapAllocated = true;
		Initialize(buf);
	}

	INLINE void Initialize2(const char* s) {
		//i know its slow but it doesnt need to be super  fast and i cba to change this (heap allocations/deallocations)
		char* buf = (char*)calloc(1, strlen(s) + 1);
		memcpy(buf, s, strlen(s));
		TelltaleToolLib_MakeInternalTypeName(&buf);
		mbNameIsHeapAllocated = true;
		Initialize(buf);
	}

	//DO NOT USE typeid(type).name(), THIS IS NOT THE RIGHT FORMAT, use typeid(type) and use the overloaded function!!
	void Initialize(const char*);

	void Insert();

	bool IsDerivedFrom(MetaClassDescription* pDesc);

	void InstallSpecializedMetaOperation(MetaOperationDescription*);

	MetaOperation GetOperationSpecialization(int ID);

	//void CastToConcreteObject(void** pObj, MetaClassDescription** pDesc); //doesnt exist
	void* CastToBase(const void* pObj, MetaClassDescription* pBaseClassDesc);

	inline MetaMemberDescription* GetEnumMember();

};

extern MetaClassDescription* spFirstMetaClassDescription;

struct MetaEnumDescription {
	const char* mpEnumName;
	i32 mFlags;
	i32 mEnumIntValue;
	MetaEnumDescription* mpNext;
};

struct MetaFlagDescription {
	const char* mpFlagName;
	i32 mFlagValue;
	MetaFlagDescription* mpNext;
};

struct MetaMemberDescription {

	const char* mpName;
	/*ony use for intrinsics for json (eg ints etc, not compound types or enums)*/
	const char* mpIntrinsicKeyOverride;
	i64 mOffset;
	i32 mFlags;
	MetaClassDescription* mpHostClass;
	MetaMemberDescription* mpNextMember;
	union {
		MetaEnumDescription* mpEnumDescriptions;
		MetaFlagDescription* mpFlagDescriptions;
	};
	MetaClassDescription* mpMemberDesc;
	i32 mMinMetaVersion;//by lib for other game support
	TRange<int> mGameIndexVersionRange;
	i32 mSkipVersion;



	inline MetaMemberDescription() {
		mMinMetaVersion = -1;
		mSkipVersion = -1;
		mpName = NULL;
		mOffset = 0;
		mFlags = 0;
		mpHostClass = NULL;
		mpNextMember = NULL;
		mpIntrinsicKeyOverride = NULL;
		mpMemberDesc = NULL;
		mpEnumDescriptions = NULL;
		mGameIndexVersionRange.min = -1;
		mGameIndexVersionRange.max = -1;
	}

	~MetaMemberDescription();

};

METAOP_FUNC_DEF(SerializeIntrinsicAsyncuint8)
METAOP_FUNC_DEF(SerializeIntrinsicAsyncuint16)
METAOP_FUNC_DEF(SerializeIntrinsicAsyncuint32)
METAOP_FUNC_DEF(SerializeIntrinsicAsyncuint64)
METAOP_FUNC_DEF(SerializeIntrinsicAsyncint8)
METAOP_FUNC_DEF(SerializeIntrinsicAsyncint16)
METAOP_FUNC_DEF(SerializeIntrinsicAsyncint32)
METAOP_FUNC_DEF(SerializeIntrinsicAsyncint64)
METAOP_FUNC_DEF(SerializeIntrinsicAsyncfloat)
METAOP_FUNC_DEF(SerializeIntrinsicAsyncdouble)
METAOP_FUNC_DEF(SerializeIntrinsicAsyncbool)
METAOP_FUNC_DEF(SerializeIntrinsicAsyncString)

//Object state meta operations are like minecrafts 'getmetaint' or whatever from block classes, gets info as an int (defults to CRC32)
METAOP_FUNC_DEF(ObjectStateIntrinsic1);
METAOP_FUNC_DEF(ObjectStateIntrinsic2);
METAOP_FUNC_DEF(ObjectStateIntrinsic4);
METAOP_FUNC_DEF(ObjectStateIntrinsic8);
METAOP_FUNC_DEF(ObjectStateIntrinsicString);

METAOP_FUNC_DEF(EquivalenceIntrinsic1);
METAOP_FUNC_DEF(EquivalenceIntrinsic2);
METAOP_FUNC_DEF(EquivalenceIntrinsic4);
METAOP_FUNC_DEF(EquivalenceIntrinsic8);
METAOP_FUNC_DEF(EquivalenceIntrinsicString);

//There are more like Arithmetic, Comparison, Interpolate, but this library doesnt need them until needed.

// --------------------------------------------------------------------------------------- USEFUL META FUNCTIONS WHICH ARE INLINED ----------------------------------------------------------------------------------------


inline const char* GetMetaEnumName(MetaMemberDescription* pMem, int enumVal) {
	MetaEnumDescription* pEnum = pMem->mpEnumDescriptions;
	while (pEnum) {
		if (pEnum->mEnumIntValue == enumVal)
			return pEnum->mpEnumName;
		pEnum = pEnum->mpNext;
	}
	return "";
}

inline const char* GetMetaFlagName(MetaMemberDescription* pMem, int enumVal) {
	MetaFlagDescription* pEnum = pMem->mpFlagDescriptions;
	while (pEnum) {
		if (pEnum->mFlagValue == enumVal)
			return pEnum->mpFlagName;
		pEnum = pEnum->mpNext;
	}
	return "";
}

inline MetaClassDescription* _GetMetaClassDescription(const std::type_info& tinf) {
	MetaClassDescription* clazz = TelltaleToolLib_GetFirstMetaClassDescription();
	const char* tn = tinf.name();
	while (clazz != NULL) {
		if (!_stricmp(tn, clazz->mpTypeInfoExternalName))
			return clazz;
		TelltaleToolLib_GetNextMetaClassDescription(&clazz);
	}
	return NULL;
}

template<typename T>
MetaClassDescription* GetMetaClassDescription() {
	return _GetMetaClassDescription(typeid(T));
}

// --------------------------------------------------------------------------------------- IMPORTANT UTIL TYPES, EG BITSETS. ----------------------------------------------------------------------------------------

//.T3BULLET FILES
struct PhysicsData {
	BinaryBuffer mDataBuffer;
};

template<int N>
struct __BitSet_BaseN {
	static constexpr int _TyN = (N / 32) + (N % 32 == 0 ? 0 : 1);
};

template<u32 _Index, typename... _Tys> using _NthType = typename std::tuple_element<_Index, std::tuple<_Tys...>>::type;

template <u32 _Index, typename... _Tys> decltype(auto) _NthElem(_Tys&&... ts) {
	return std::get<_Index>(std::forward_as_tuple(ts...));
}

//Same type tether, used for bitset variadic start and end count
template<typename _MemTy, u32... Offsets>
struct _TetheredMembers {

	void* mpStruct;

	inline _TetheredMembers(void* pTether) {
		mpStruct = pTether;
	}

	template<u32 _MemberIndex>
	inline _MemTy* Get() const {
		if(this->mpStruct == 0){
			TTL_Log("ERROR: Cannot get tethered member as struct is NULL!");
			__debugbreak();
			return 0;
		}
		return ((_MemTy*)((u8*)this->mpStruct + _NthElem<_MemberIndex>(Offsets...)));
	}

};

//By library
enum BitSetTetheredMemberIndex : u32 {
	eT3Tether_First = 0,
	eT3Tether_Count = 1,
};

template<int N>
struct BitSetBase {

	u32 mWords[N];

	inline BitSetBase() {
		memset(mWords, 0, N << 2);
	}

	static constexpr int NumWords = N;

	static METAOP_FUNC_IMPL__(SerializeAsync) {
		for (int i = 0; i < N; i++) {
			((MetaStream*)pUserData)->serialize_uint32(((BitSetBase<N>*)pObj)->mWords + i);
		}
		return eMetaOp_Succeed;
	}

};

template<u32 _First, u32 _Count, bool bTemplate = true>
struct _VBitSetBase {

	inline _VBitSetBase(void*) {}

	inline u32 GetFirst() const {
		return _First;
	}

	inline u32 GetCount() const {
		return _Count;
	}

	inline void* _GetTether() const {
		return 0;
	}

	inline void SetTether(void*) {}

};

template<u32 _FirstOffset, u32 _CountOffset>
struct _VBitSetBase<_FirstOffset, _CountOffset, false>{

	_TetheredMembers<u32, _FirstOffset, _CountOffset> mTethers;

	inline _VBitSetBase(void* pTether) : mTethers(pTether) {}

	inline u32 GetFirst() const {
		return *mTethers.Get<eT3Tether_First>();
	}

	inline u32 GetCount() const {
		return *mTethers.Get<eT3Tether_Count>();
	}

	inline void* _GetTether() const {
		return mTethers.mpStruct;
	}

	inline void SetTether(void* pTether) {
		mTethers.mpStruct = pTether;
	}

};

template<typename T, u32 NumWords, typename _Base>
struct _BitSet : BitSetBase<NumWords>, _Base {

	bool operator[](T f) const {
		u32 v = (u32)f - this->GetFirst();
		return (this->mWords[v >> 5] & (1 << (v & 0x1F))) != 0;
	}

	bool operator==(const _BitSet& rhs) const {
		for (int i = 0; i < NumWords; i++) {
			if (this->mWords[i] != rhs.mWords[i])
				return false;
		}
		return true;
	}

	bool operator!=(const _BitSet& rhs) const {
		for (int i = 0; i < NumWords; i++) {
			if (this->mWords[i] != rhs.mWords[i])
				return true;
		}
		return false;
	}

	void SetAll(bool bValue) {
		memset(this->mWords, bValue ? 0xFFFFFFFF : 0, NumWords << 2);
		if (bValue && (this->GetCount() % 32))
			this->mWords[NumWords - 1] = (1u << (this->GetCount() % 32)) - 1;
	}

	void Set(T f, bool bValue) {
		u32 v = (u32)f - this->GetFirst();
		if (bValue)
			this->mWords[v >> 5] |= (1 << (v & 0x1F));
		else
			this->mWords[v >> 5] &= ~(1 << (v & 0x1F));
	}

	void Mask(const _BitSet& rhs) {
		for (int i = 0; i < NumWords; i++) {
			this->mWords[i] &= rhs.mWords[i];
		}
	}

	bool IsZero() {
		for (int i = 0; i < NumWords; i++) {
			if (this->mWords[i])
				return false;
		}
		return true;
	}

	/*import bits*/
	void Set(const _BitSet& rhs) {
		for (int i = 0; i < NumWords; i++) {
			this->mWords[i] |= rhs.mWords[i];
		}
	}

	void Clear(T f) {
		Set(f, false);
	}

	void GetTypeInformation(MetaClassDescription** pDestType, int* pDestNum, int* pDestInitial) {
		*pDestInitial = (int)this->GetFirst();
		*pDestNum = (int)this->GetCount();
		*pDestType = GetMetaClassDescription<T>();
	}

	static METAOP_FUNC_IMPL__(SerializeAsync) {
		const int _N = NumWords;
		for (int i = 0; i < _N; i++) {
			((MetaStream*)pUserData)->serialize_uint32(((_BitSet*)pObj)->mWords + i);
		}
		return eMetaOp_Succeed;
	}

	_BitSet(T val, void* pTether = 0) : _BitSet(pTether) {
		Set(val, 1);
	}

	_BitSet(void* pTether = 0) : _Base(pTether), BitSetBase<NumWords>() {}

	_BitSet(const _BitSet& rhs) = default;
	_BitSet(_BitSet&& rhs) = default;
	_BitSet& operator=(const _BitSet& rhs) = default;
	_BitSet& operator=(_BitSet&& rhs) = default;

	template<typename _TRhs, u32 _RhsNum, typename _RhsBase, std::enable_if_t<!std::is_same_v<_BitSet<T, NumWords, _Base>, _BitSet<_TRhs, _RhsNum, _RhsBase>>, int> = 0>
	_BitSet<T, NumWords, _Base>& operator=(const _BitSet<_TRhs, _RhsNum, _RhsBase>& rhs) {
		if (this->_GetTether() == 0)
			this->SetTether(rhs._GetTether());
		if (this->GetCount() == rhs.GetCount() && this->GetFirst() == rhs.GetFirst()) {
			//Normal copy
			for (u32 i = 0; i < min(_RhsNum, NumWords); i++) {
				this->mWords[i] = rhs.mWords[i];
			}
		}
		else {
			//Ranged copy
			for (u32 srcIndex = rhs.GetFirst(); srcIndex <= rhs.GetCount() + rhs.GetFirst(); srcIndex++) {
				if (srcIndex >= this->GetFirst() && srcIndex < this->GetFirst() + this->GetCount()) {
					//now in valid range
					Set((T)(srcIndex), rhs[(T)(srcIndex)]);
				}
			}
		}
		return *this;
	}

	template<typename _TRhs, u32 _RhsNum, typename _RhsBase, std::enable_if_t<!std::is_same_v<_BitSet<T, NumWords, _Base>, _BitSet<_TRhs, _RhsNum, _RhsBase>>, int> = 0>
	_BitSet<T, NumWords, _Base>(const _BitSet<_TRhs, _RhsNum, _RhsBase>& rhs) : _BitSet(rhs._GetTether()) {
		*this = rhs;
	};

};

/**
 * Normal BitSet. T is the enum, Num is the number of enum values, which determines the actual size of this struct.
 * Initial value is the initial value in the enum which is subtracted when you set an enum to get the bit index.
 * Use this if you want to reduce the bitset size because you only require a specific section of enum values.
 */
template<typename T, u32 Num, u32 InitialVal = 0>
using BitSet = _BitSet<T, __BitSet_BaseN<Num>::_TyN, _VBitSetBase<InitialVal, Num, true>>;

/**
 * This is not a variable size bitset, it is a bitset which the first enum value and the number of enum values (see normal BitSet)
 * is dynamic in the sense that those two integers come from a struct pointer which can vary, it shouldn't change during the lifetime
 * of an instance of this object. First and second offset are the member offset into the struct which is required in the constructor.
 * You can change the struct pointer manually using SetTether(pStruct). NumEnumValsBound determines the size of this struct.
 * You should ensure the first and count enum values do not point ouside of the max number of bits in NumEnumValsBound range.
 */
template<typename T, u32 NumEnumValsBound,u32 FirstOffset, u32 CountOffset>
using VBitSet = _BitSet<T, __BitSet_BaseN<NumEnumValsBound>::_TyN, _VBitSetBase<FirstOffset, CountOffset, false>>;

// --------------------------------------------------------------------------------------- INLINE DEFINITIONS ----------------------------------------------------------------------------------------

template<typename ... Args>
std::string string_format(const std::string& format, Args ... args)
{
	int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	//if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
	auto size = static_cast<size_t>(size_s);
	std::unique_ptr<char[]> buf(new char[size]);
	std::snprintf(buf.get(), size, format.c_str(), args ...);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}

INLINE std::string SymToString(Symbol* pSymbol) {
	return string_format("Symbol<%I64X>", pSymbol->GetCRC());
}

inline MetaMemberDescription* MetaClassDescription::GetEnumMember() {
	MetaMemberDescription* first = mpFirstMember;
	while (first) {
		if (first->mFlags & MetaFlag_EnumIntType)
			return first;
		first = first->mpNextMember;
	}
	return 0;
}


#endif
