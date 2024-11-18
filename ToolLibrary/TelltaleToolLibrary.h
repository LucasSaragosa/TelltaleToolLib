// This file was written by Lucas Saragosa. If you use this code or library,
// I ask you to you give credit to me and the amazing Telltale Games.


#ifndef _TTToolLib
#define _TTToolLib

#include "LibraryConfig.h"

#include <type_traits>
#include <string>
#include "DataStream/DataStream.h"
#include "Blowfish.h"
#include <memory>
#include <sstream>
#include <iomanip>

// ============================================================= TYPE DEFINITIONS =============================================================

#define _TTTLib_PrintMetaClasses() TelltaleToolLib_DumpClassInfo(_PrintfDumper)

struct Flags;
template<typename T> class Ptr;
struct MetaClassDescription;
struct MetaMemberDescription;
enum MetaOpResult;
struct RenderConfiguration;
class ToolLibRenderAdapters;
class MetaStream;
class HashDatabase;
class TTArchive2;
class DataStream;
class MetaClassDescription;
class TelltaleVersionDatabase;

enum ErrorSeverity {
	NOTIFY,
	WARN,
	ERR,
	CRITICAL_ERROR
};

//Tool library Error callback function proto. You can set your own callback which will be called with any errors so you can log it to wherever.
typedef void (*ErrorCallbackF)(const char* _Msg, ErrorSeverity _Severity);

typedef void (*DumpClassInfoF)(const char* const _Fmt, const char* _ParamA, const char* _ParamB);

void _PrintfDumper(const char* const _Fmt, const char* _ParamA, const char* _ParamB);

// ============================================================= EXPORTED LIBRARY FUNCTION HELPER ENUMS =============================================================

enum class StringOp {
	SET,//param new value. no return (0)
	GET,//get value, param ignored.
	CLR,//clear, param and return ignore.
};

enum class IntrinType {
	U8,
	I8,
	U16,
	I16,
	I32,
	U32,
	I64,
	U64,
	STR,//STRING(8)
	SYM,//SYMBOL(8)
	FLG,//FLAGS(4)
	FLT,//FLOAT(4)
	DBL,//DOUBLE(8)
	BLN,//BOOL(1)
	VC2,//VEC2(8)
	VC3,//VEC3(12)
	VC4,//VEC4(16)
	QUA,//QUAT(16)
	COL,//COLOR(16) RGBA
	RGI,//TRANGE<INT>(8)
	RGF,//TRANGE<INT>(8)
	RCF,//TRECT<FLOAT>(16) bottom top right left
	SPR,//SPHERE(16) center xyz (f), radius
};

enum class ContainerOp {
	GET,//GetElement(index, ignore, ignore)
	ADD,//AddElement(index, key (Or 0), moved element)
	REM,//RemoveElement(index, index - in map index is still index), ignore).
	CNT,//GetSize(ignore, ignore, ignore)
	KTY,//KeyType(ignore, ignore, ignore)
	VTY,//ValType(ignore, ignore, ignore)
	CHK,//IsContainer(meta class, ignore , ignore) checks if container type. container is ignored.
	CLR,//clear
};

enum MetaMemberDescriptionParam {
	eMMDP_Name = 1,//stores 8 bytes (ptr)
	eMMDP_Offset = 2,//stores 8 bytes
	eMMDP_Flags = 3,//stores 4 bytes
	eMMDP_HostClass = 4,//stores 8 bytes (ptr)
	eMMDP_MemberClassDesc = 5,//stores 8 bytes (ptr)
	eMMDP_NextMember = 6,//stores 8 bytes (ptr)
	eMMDP_GameVersionRange = 7,//stores 12 bytes, first is skip version, second are min and max for game range this member is in (else -1 for all 3 ints)
};

enum GFXPlatformShaderType
{
	eGFXPlatformShader_None = 0xFFFFFFFF,
	eGFXPlatformShader_Vertex = 0x0,
	eGFXPlatformShader_Pixel = 0x1,
	eGFXPlatformShader_Geometry = 0x2,
	eGFXPlatformShader_Count = 0x3,
	eGFXPlatformShader_Compute = 0x3,
	eGFXPlatformShader_CountIncludingCompute = 0x4,
};

//Modified. Telltale renderer is layered: platform layer (GFXPlatform class prefix) and T3 (telltale 3D) later (T3GFX prefix). This is combination.
enum GFXPlatformResourceType
{
	eGFXPlatformResource_Shader = 0x0,
	eGFXPlatformResource_Program = 0x1,
	eGFXPlatformResource_Buffer = 0x2,
	eGFXPlatformResource_VertexState = 0x3,
	eGFXPlatformResource_Texture = 0x4,
	eGFXPlatformResource_Count = 0x5,
};

enum GFXPlatformAllocationType
{
	eGFXPlatformAllocation_Unknown = 0x0,
	eGFXPlatformAllocation_RenderTarget = 0x1,
	eGFXPlatformAllocation_ShadowMap = 0x2,
	eGFXPlatformAllocation_DiffuseTexture = 0x3,
	eGFXPlatformAllocation_NormalmapTexture = 0x4,
	eGFXPlatformAllocation_LightmapTexture = 0x5,
	eGFXPlatformAllocation_DetailTexture = 0x6,
	eGFXPlatformAllocation_AmbientOcclusionTexture = 0x7,
	eGFXPlatformAllocation_FontTexture = 0x8,
	eGFXPlatformAllocation_ParticleTexture = 0x9,
	eGFXPlatformAllocation_MiscTexture = 0xA,
	eGFXPlatformAllocation_StaticMesh = 0xB,
	eGFXPlatformAllocation_TextMesh = 0xC,
	eGFXPlatformAllocation_NPRLineMesh = 0xD,
	eGFXPlatformAllocation_BokehMesh = 0xE,
	eGFXPlatformAllocation_DynamicMesh = 0xF,
	eGFXPlatformAllocation_GenericBuffer = 0x10,
	eGFXPlatformAllocation_ParticleMesh = 0x11,
	eGFXPlatformAllocation_Effect = 0x12,
	eGFXPlatformAllocation_EffectShader = 0x13,
	eGFXPlatformAllocation_Uniform = 0x14,
	eGFXPlatformAllocation_StreamingUniform = 0x15,
	eGFXPlatformAllocation_AmbientOcclusion = 0x16,
	eGFXPlatformAllocation_Count = 0x17,
};

enum MetaClassDescriptionParam {
	eMCDP_Extension = 1,//stores 8 bytes (ptr)
	eMCDP_Name = 2,//same as calling getmetaclassdescriptionname
	eMCDP_Hash = 3,//stores 8 bytes
	eMCDP_Flags = 4,//stores 4 bytes
	eMCDP_ClassSize = 5,//stores 4 bytes
	eMCDP_FirstMemberPtr = 6,//stores 8 bytes (ptr)
	eMCDP_OperationsList = 7,//stores 8 bytes (ptr)
	eMCDP_VTable = 8,//stores 8 bytes (ptr to array of ptrs)
	eMCDP_SerializeAccel = 9, //stores 8 bytes (ptr)
};

/**
 * Binary buffer help type. Holds ownership semantics, deletes when not used.
 */
struct BinaryBuffer {

	char* mpData;
	int mDataSize;

	inline BinaryBuffer() : mpData(NULL), mDataSize(0) {}

	inline ~BinaryBuffer() {
		if (mpData)
			delete[] mpData;
	}

	inline bool SetData(int dataSize, const void* pData, int align = 4) {
		if (mpData)
			delete[] mpData;
		if (dataSize > 0) {
			mpData = (char*)_aligned_malloc(dataSize, align);
			if (mpData) {
				mDataSize = dataSize;
				if (pData)
					memcpy(mpData, pData, mDataSize);
				else
					memset(mpData, 0, mDataSize);
			}
			else return false;
		}
		return true;
	}

	inline void Memset(char v) {
		if (mpData)
			memset(mpData, v, mDataSize);
	}

	inline void Swap(BinaryBuffer& other) {
		char* my = mpData;
		mpData = other.mpData;
		other.mpData = my;
	}

	inline void Clear() {
		if (mpData)
			free(mpData);
		mpData = 0;
	}

	inline bool AllocData(/*u64*/ int dataSize, int align = 4) {
		if (mpData)
			delete[] mpData;
		if (dataSize > 0) {
			mpData = (char*)_aligned_malloc(dataSize, align);
			if (mpData) {
				mDataSize = dataSize;
			}
			else return false;
		}
		return true;
	}

	inline BinaryBuffer(const BinaryBuffer& other) {
		mpData = NULL;
		mDataSize = 0;
		SetData(other.mDataSize, other.mpData);
	}

	inline BinaryBuffer& operator=(const BinaryBuffer& other) {
		AllocData(other.mDataSize);
		if (mpData && other.mpData)
			memcpy(mpData, other.mpData, mDataSize);
		return *this;
	}

	inline BinaryBuffer(BinaryBuffer&& rhs) noexcept {
		mpData = rhs.mpData;
		mDataSize = rhs.mDataSize;
		rhs.mDataSize = 0;
		rhs.mpData = 0;
	}

	inline BinaryBuffer& operator=(BinaryBuffer&& rhs) noexcept {
		mpData = rhs.mpData;
		mDataSize = rhs.mDataSize;
		rhs.mDataSize = 0;
		rhs.mpData = 0;
		return *this;
	}

	static MetaOpResult MetaOperation_SerializeAsync(void* pObj, MetaClassDescription* pObjDesc,
		MetaMemberDescription* pCtx, void* pUserData);

};

// ============================================================= GFX RESOURCE BASE =============================================================

//Eg in opengl GLuint handle
struct T3GFXResource {

	GFXPlatformResourceType mResourceType;
	u32 mbResourceAssigned = false;
	u64 mFrameUsed = 0;
	void* mpRenderPlatformHandle;

	inline u32 GetRenderPlatformHandle_UInt(){
		return ((u32*)(&mpRenderPlatformHandle))[0];
	}

	inline u32 GetRenderPlatformHandle_HI_UInt() {
		return ((u32*)(&mpRenderPlatformHandle))[1];
	}

	inline void SetUsedOnFrame(u64 frame){
		mFrameUsed = frame;
	}

	inline void SetRenderPlatformHandle_UInt(u32 v){
		((u32*)(&mpRenderPlatformHandle))[0] = v;
		mbResourceAssigned = true;
	}

	inline void SetRenderPlatformHandle_HI_UInt(u32 v) {
		((u32*)(&mpRenderPlatformHandle))[1] = v;
		mbResourceAssigned = true;
	}
	
	inline T3GFXResource(GFXPlatformResourceType type) : mpRenderPlatformHandle(0llu), mResourceType(type) {
		OnCreate();
	}

	void OnDestroyed();

	void OnCreate();

};

struct GFXPlatformVertexAttributeSet
{
	unsigned __int64 mAttributes;
};

struct GFXPlatformShader : T3GFXResource {

	inline GFXPlatformShader() : T3GFXResource(eGFXPlatformResource_Shader) {}

	struct ParameterHeader
	{
		u16 mScalarSize = 0;
		char mLocation = (char)-1;
		char mSamplerLocation = (char)-1;
		char mBindType = 0;
	};

	GFXPlatformShaderType mShaderType;
	GFXPlatformVertexAttributeSet mAttributes;
	BinaryBuffer mShaderProgram;
	ParameterHeader mParameters[150];

};

struct GFXPlatformProgram : T3GFXResource {

	struct ParameterHeader
	{
		u16 mVertexScalarSize = 0;
		u16 mPixelScalarSize = 0;
		u16 mGeometryScalarSize = 0;
		char mVertexLocation = (char)-1;
		char mVertexSamplerLocation = (char)-1;
		char mGeometryLocation = (char)-1;
		char mGeometrySamplerLocation = (char)-1;
		char mPixelLocation = (char)-1;
		char mPixelSamplerLocation = (char)-1;
		char mPixelBindType = 0;
	};

	inline GFXPlatformProgram() : T3GFXResource(eGFXPlatformResource_Program) {}

	std::shared_ptr<GFXPlatformShader> mpVertexShader;
	std::shared_ptr<GFXPlatformShader> mpPixelShader;
	std::shared_ptr<GFXPlatformShader> mpGeometryShader;

	ParameterHeader mParameters[150];

};

enum GFXPlatformCapability
{
	eGFXPlatformCap_ResourceThread = 0x0,
	eGFXPlatformCap_BindProgramBuffer = 0x1,
	eGFXPlatformCap_BindProgramBufferData = 0x2,
	eGFXPlatformCap_BindProgramGenericBuffer = 0x3,
	eGFXPlatformCap_ShaderClipPlanes = 0x4,
	eGFXPlatformCap_VertexStateKeyIsProgram = 0x5,
	eGFXPlatformCap_VertexStateKeyIsVertexShader = 0x6,
	eGFXPlatformCap_ComputeShader = 0x7,
	eGFXPlatformCap_AsyncCompute = 0x8,
	eGFXPlatformCap_GeometryShader = 0x9,
	eGFXPlatformCap_Depth16 = 0xA,
	eGFXPlatformCap_PreloadDraw = 0xB,
	eGFXPlatformCap_SaveProgramBinary = 0xC,
	eGFXPlatformCap_DrawBaseIndex = 0xD,
	eGFXPlatformCap_SampleGather4 = 0xE,
	eGFXPlatformCap_ZeroStrideVertexBuffer = 0xF,
	eGFXPlatformCap_ThreadSafeBuffers = 0x10,
	eGFXPlatformCap_Count = 0x11,
};

//plain .BIN T3EffectCache_<gameid>.bin
struct T3EffectCacheVersionDatabase;

// ============================================================= EXPORTED FUNCTION DECLARATIONS. HERE FIND THE DOCS =============================================================

/*Used to manage the state of loading older classes from older games and engine versions.*/
/*You are strongly advised NOT to modify any of these variables, use the functions and if you really*/
/*need to then only read them!*/
/**
 * The real use of these is that if enabled and loaded for the currently set game ID, is that
 * FindMetaClassDescription will return a *proxy* meta class description instead to ensure
 * its the correct version of the currently set game ID.
 */
struct ProxyMetaState {

	const char* mpStateGameID = NULL;
	TelltaleVersionDatabase* mpStateGameDB = NULL;
	MetaClassDescription* mpProxyClassList = NULL;

};

/**
 * Returns true if the given game ID can support proxy classes, which is determined if the game ID
 * has its Version Database (.VersDB) loaded, which can be done from the TelltaleToolLib_SetProxyVersionDatabases function.
 * This may also return false if the given game is not supported for version databases yet. See VersDB.h MINIMUM_VERSDB
 * and also Blowfish.h for the minimum game and therefore games (ones including and after) that *are* supported.
 */
_TTToolLib_Exp bool TelltaleToolLib_ProxyClassesAvail(const char* pGameID);

/**
 * 
 * Generates (either calling new and appending to the proxy class list, or retrieving existing one) a new proxy meta class
 * description for the type symbol hash. If the hash is invalid for the current game (ie the type does not exit in the game
 * engine at that point) then this will return NULL. bGetOnly can be true if you don't want to create one but check if it 
 * is already loaded.
 * 
 */
_TTToolLib_Exp MetaClassDescription* TelltaleToolLib_GenerateMetaClassDescriptionProxy(ProxyMetaState* pState, u64 typeSymbolCrc, bool bGetOnly = false);

/**
 * If you want to use the proxy meta state for older games before reading in a file or getting a meta class description,
 * then enable it first. Returns the previous state. This is DEFAULT FALSE!
 */
_TTToolLib_Exp bool TelltaleToolLib_SetUsingProxyMetaStates(bool bOnOff);

/*
*
* Resets the given proxy state, clearing all of its proxy classes from memory. Also called when shutting down.
* This clears and DELETES all proxy class descriptions which were alive so be careful with your references to them, check their
* proxy flag if its set before calling this, then you know this will delete it.
* 
*/
_TTToolLib_Exp void TelltaleToolLib_ResetProxyMetaState(ProxyMetaState* pState);

/**
 * 
 * Gets the proxy meta state for the library. This is a static object and should not be deleted.
 * Pass in the game ID which it is attached to, or null to use the currently set one. This will return
 * NULL is the game ID is invalid or if TelltaleToolLib_ProxyClassesAvail is false for the given ID!
 * 
 */
_TTToolLib_Exp ProxyMetaState* TelltaleToolLib_GetProxyMetaState(const char* pGameID = nullptr);
/*
* Gets the version of the TelltaleTool library as a string.
*/
_TTToolLib_Exp const char* TelltaleToolLib_GetVersion();

/**
 * Loads the effect cache enumerations BIN files from the given folder
 */
_TTToolLib_Exp void TelltaleToolLib_LoadEffectCaches(const char* pFolder);

/**
 * Loads the effect cache enumerations BIN files from the given archive
 */
_TTToolLib_Exp void TelltaleToolLib_LoadEffectCachesFromArchive(TTArchive2& archive);

_TTToolLib_Exp T3EffectCacheVersionDatabase* TelltaleToolLib_GetEffectCacheDB(const char* pGameID);

/*
* Dumps class information in a tabbed format
*/
_TTToolLib_Exp void TelltaleToolLib_DumpClassInfo(DumpClassInfoF _Dumper);

/*
* Makes the type name of the given string pointer to one which is ready to be hashed. 
* The parameter needs to be a pointer to a string which is allocated with calloc/malloc.
* This string is freed and the new string (allocated with calloc) is set to the parameter pointer's value.
*/
_TTToolLib_Exp void TelltaleToolLib_MakeInternalTypeName(char**);

/*
* If you have lots of .VersDB files which contain dumped game meta class descriptions in V2 format, then you can
* call this after Initialize to set the folder which contains the files. The library will try and load each file from
* this folder which is a game ID with a VersDB extension, eg WDC.VersDB or MCSM.VersDB. Returns the number of loaded
* or found version databases. Check the error callbacks if any could not be loaded. Returns 0 if the folder was invalid or 
* contained no files. If there was an error loading a database it returns -1. 
* IMPORTANT: The folder must be a full path from C: !
*/
_TTToolLib_Exp int TelltaleToolLib_SetProxyVersionDatabases(const char* pFolder);

//See the normal version. This searches the archive for all files in format GAMEID.VersDB (GAMEID is any game id) and then loads them.
//You can delete the archive or whatever after, it reads in and don't hold onto anything from it.
_TTToolLib_Exp int TelltaleToolLib_SetProxyVersionDatabasesFromArchive(TTArchive2& archive);

/*
* Initialize the library. This must be called before you call any file reading and writing functions.
* Must pass a game id for the game that you are going to be working with files from. This is used for decryption keys.
* See TelltaleToolLib_SetBlowfishKey. The game id *can* be NULL, where it will be set to the default game id which can be 
* found in the Blowfish translation unit. If you pass an invalid game id, this will return false and fail. Passing NULL succeeds 
* with the default ID, this goes aswell for the SetBlowfishKey function.
*/
_TTToolLib_Exp bool TelltaleToolLib_Initialize(const char* game_id);

/*Returns true if the given game id is an old telltale game (.ttarch, not .ttarch2 etc). Used for different file saving types*/
_TTToolLib_Exp bool TelltaleToolLib_IsGameIDOld(const char* game_id);

/**
 * Finds a member variable in the given Meta class. If it exists it is returned. These are all static memory so no need to free.
 */
_TTToolLib_Exp MetaMemberDescription* TelltaleToolLib_FindMember(MetaClassDescription* clazz, const char* memberVarName);

/*
* Returns the first meta class description in the list of all meta class description types.
*/
_TTToolLib_Exp MetaClassDescription* TelltaleToolLib_GetFirstMetaClassDescription();

/*
* Inserts the next meta class description in the list into the parameter, or NULL is inserted if there are no more after it.
*/
_TTToolLib_Exp void TelltaleToolLib_GetNextMetaClassDescription(MetaClassDescription**);

/*
* Gets various information parameters from the given MCD and stores them in the function params
*/
_TTToolLib_Exp void TelltaleToolLib_GetMetaClassDescriptionInfo(MetaClassDescription*, void*,MetaClassDescriptionParam);

/*
* Gets various information parameters from the given MMD and stores them in the function params (middle one)
*/
_TTToolLib_Exp void TelltaleToolLib_GetMetaMemberDescriptionInfo(MetaMemberDescription*, void*, MetaMemberDescriptionParam);

/*
* Gets the next member in the meta class description from the parameter and stores it in the parameter. If there is none after the passed
* one then NULL is inserted.
*/
_TTToolLib_Exp void TelltaleToolLib_GetNextMetaMemberDescription(MetaMemberDescription**);

/*
* Sets the current game ID which is used for decryption of files when needed. Game IDs can be found in the github repo.
*/
_TTToolLib_Exp bool TelltaleToolLib_SetBlowfishKey(const char* game_id);

/*
* Gets the game id of the currently set blowfish key
*/
_TTToolLib_Exp const char* TelltaleToolLib_GetBlowfishKey();

/**
 * Create a class instance using the Meta reflection system of the given type.
 */
_TTToolLib_Exp void* TelltaleToolLib_CreateClassInstance(MetaClassDescription* pClass);

/**
 * See CreateClassInstance, this deletes it safely from memory. This calls its destructor.
 */
_TTToolLib_Exp void TelltaleToolLib_DeleteClassInstance(MetaClassDescription* pClass, void* pInstance);

/*
* Searches for the given meta class description file extension or name. If the second parameter is false, then its by extension
* else its by the name. For example, for telltale texture files, use ("T3Texture",true) or ("d3dtx",false) to get the class for it.
*/
_TTToolLib_Exp MetaClassDescription* TelltaleToolLib_FindMetaClassDescription(const char*, bool bByNameOrExt);

/*
* Same as TelltaleToolLib_FindMetaClassDescription, but searches by hash.
*/
_TTToolLib_Exp MetaClassDescription* TelltaleToolLib_FindMetaClassDescription_ByHash(u64);

/*
* Gets the name of the given meta class description
*/
_TTToolLib_Exp const char* TelltaleToolLib_GetMetaClassDescriptionName(MetaClassDescription*);

/*
* When initialize is called (else this would return 0), this returns the amount of meta types that are initialized.
*/
_TTToolLib_Exp i32 TelltaleToolLib_GetMetaTypesCount();

/*
* Returns if this library is initialized yet.
*/
_TTToolLib_Exp bool TelltaleToolLib_Initialized();

/**
 * Gets the loaded dynamic library by name, without extension (done by platform), eg for windows oo2core_5 as the parameter loads LibBin/oo2_core64.dll (Always 64), dll for win
 */
_TTToolLib_Exp LibraryHandle TelltaleToolLib_GetLibrary(const char* pName);

/*
* Reads a meta stream from the given data stream source. Must have pClass serialized first. If correct then the dest instance will contain the data from the file. 
*/
_TTToolLib_Exp bool TelltaleToolLib_ReadMetaStream(DataStream* pIn, MetaClassDescription* pClass, void* pDestInstance);

/*
* Writes a instance of pClass to a meta stream and flushes the bytes to pOut.
*/ 
_TTToolLib_Exp bool TelltaleToolLib_WriteMetaStream(DataStream* pOut, MetaClassDescription* pClass, void* pInstance);

/*
* Creates a datastream for the given file path.
*/
_TTToolLib_Exp DataStream* TelltaleToolLib_CreateDataStream(const char* fp, DataStreamMode);

/*
* Deletes the given data stream. Only use this if you know what you're doing, normally its deleted internally.
*/
_TTToolLib_Exp void TelltaleToolLib_DeleteDataStream(DataStream*);

/*
* A static 1024 byte char buffer for strings. This can be used by other programs to store strings passed in between the library.
*/
_TTToolLib_Exp char* TelltaleToolLib_Alloc_GetFixed1024ByteStringBuffer();

/*
* A static 8 byte buffer to store pointers. Useful when using for loops which need to get the next class/member and can store the 
* pointer to it in this buffer to save memory allocations.
*/
_TTToolLib_Exp char* TelltaleToolLib_Alloc_GetFixed8BytePointerBuffer();

//These next two family of functions return a buffer allocated with MALLOC! So use FREE. Returns NULL if a problem occurred.
//These only work for LUA files.

_TTToolLib_Exp u8* TelltaleToolLib_EncryptScript(u8* data, u32 size, u32* outsize);
_TTToolLib_Exp u8* TelltaleToolLib_DecryptScript(u8* data, u32 size, u32* outsize);

//These next two family of functions return a buffer allocated with MALLOC! So use FREE. Returns NULL if a problem occurred.
//These only work for LENC files.

_TTToolLib_Exp u8* TelltaleToolLib_EncryptLencScript(u8* data, u32 size, u32 *outsize);
_TTToolLib_Exp u8* TelltaleToolLib_DecryptLencScript(u8* data, u32 size, u32* outsize);

/*
* Sets the global hash database used to search CRCs. It is very important that its set before you read or write most types (eg PropertySet)
* You grant ownership, so don't ever delete this! must be allocated with new. Can be set to NULL.
* You should always set this after initialization. It may work without but older files may fail on serialize!
*/
_TTToolLib_Exp void TelltaleToolLib_SetGlobalHashDatabase(HashDatabase*);

/*
* Does the same as the normal set global hash db, but creates it for you. Pass in the reading file stream to it. This stream 
* is deleted by the library so DONT DELETE IT!
* You should always set this after initialization. It may work without but older files may fail on serialize!
*/
_TTToolLib_Exp void TelltaleToolLib_SetGlobalHashDatabaseFromStream(DataStream*);

/*
* Gets the game index of the given game id. This is used to match when a game is released with others. Used internally make sure the 
* current file is being read correctly with the correct version range.
*/
_TTToolLib_Exp i32 TelltaleToolLib_GetGameKeyIndex(const char* pGameID);

/*
* Gets the global hash database, or NULL if its not been set.
*/
_TTToolLib_Exp HashDatabase* TelltaleToolLib_GetGlobalHashDatabase();

//If null returned then property key does not exist
_TTToolLib_Exp void* TelltaleToolLib_GetPropertySetValue(void* prop, unsigned long long keyhash, MetaClassDescription** pOptionalOutType);

//Moves pInst into property set. Ownership granted from you. Returns if success. Can exist or not, if exists overriden and old value deleted.
_TTToolLib_Exp bool TelltaleToolLib_SetPropertySetValue(void* prop, unsigned long long kh, MetaClassDescription* pType, void* pInst);

_TTToolLib_Exp void TelltaleToolLib_RemovePropertySetValue(void* prop, unsigned long long keyhash);

// ---------------------- ARCHIVE 2 FUNCTION EXPORTS

// THIS CURRENT VERSION DOES NOT SUPPORT WRITING ARCHIVES. YOU WILL HAVE TO USE THE CXX API AND THEN WRITE YOUR OWN. ITS SIMPLE THOUGH, SEE TTARCHIVE2::CREATE

_TTToolLib_Exp TTArchive2* TelltaleToolLib_CreateArchive2Instance();

_TTToolLib_Exp void TelltaleToolLib_DeleteArchive2Instance(TTArchive2* pArchive);

_TTToolLib_Exp bool TelltaleToolLib_OpenArchive2(TTArchive2* pArchive, DataStream* pOwnershipGrantedStream);

_TTToolLib_Exp bool TelltaleToolLib_Archive2HasResource(TTArchive2* pArchive, unsigned long long hash);

//when done call deletestreaminstance
_TTToolLib_Exp DataStream* TelltaleToolLib_OpenArchive2Resource(TTArchive2* pArchive, unsigned long long hash);

_TTToolLib_Exp int TelltaleToolLib_NumArchive2Resources(TTArchive2* pArchive);

//returns 1024 fixed buffer where it will be stored.
_TTToolLib_Exp const char* TelltaleToolLib_GetArchive2ResourceName(TTArchive2* pArchive, unsigned long long hash);

//get the resource name at index index. Use NumResources and loop from 0 to num-1 to go through all resources.
_TTToolLib_Exp unsigned long long TelltaleToolLib_GetArchive2Resource(TTArchive2* pArchive, int index);

// ---------------------- UTIL FUNCTIONS: DATA STREAMS, CRC64, RENDER ADAPTERS.

//Reads all remaining bytes from the given data stream and returns a malloc buffer. Use dealloc exported function to delete the buffer once done
_TTToolLib_Exp void* TelltaleToolLib_ReadDataStream(DataStream* pReadStream, unsigned long* pOutSize);

_TTToolLib_Exp void TelltaleToolLib_WriteDataStream(DataStream* pOutStream, void* pBuffer, unsigned long size);

_TTToolLib_Exp unsigned long long TelltaleToolLib_CRC64CaseInsensitive(const char* pNulTermString, unsigned long long initCRC/*=0*/);

//See T3EffectUser.h. Does not deallocate any old ones
_TTToolLib_Exp void TelltaleToolLib_SetRenderAdapters(ToolLibRenderAdapters* pRenderAdapterOverloads);

/**
 * Gets the user render adapters struct. Defaults to a dead implementation.
 */
_TTToolLib_Exp ToolLibRenderAdapters* TelltaleToolLib_GetRenderAdaptersStruct();

/**
 * Call to initialize the render library which you can hook into your render API using the adapter (which will actually communicate with gpu).
 */
_TTToolLib_Exp void TelltaleToolLib_InitializeT3(ToolLibRenderAdapters* pOptionalRenderAdapterCustom);

/**
 * Gets the render configuration for T3.
 */
_TTToolLib_Exp RenderConfiguration* TelltaleToolLib_GetRenderConfiguration();

//see IntrinType. delete with deleteclassinstance.
_TTToolLib_Exp void* TelltaleToolLib_CreateIntrinsicInstance(int intrin, char data[16], MetaClassDescription** pOutTypeOptional);

/**
 * Perform a container operation for the given ContainerInterface (DCArray, Set, Map...). See ContainerOp
 */
_TTToolLib_Exp void* TelltaleToolLib_Container(int op, void* container, void* arg1, void* arg2, void* arg3);

/**
 * Peform a string operation of the given String type. See StringOp.
 */
_TTToolLib_Exp void* TelltaleToolLib_String(int op, void* stringInst, void* param);

/*
* Frees all non-static memory related to this library. 
*/
_TTToolLib_Exp void TelltaleToolLib_Free();

// ================ MEMORY HELPER EXPORTS

TTEXPORT void* TelltaleToolLib_Malloc(unsigned long size);
TTEXPORT void TelltaleToolLib_Dealloc(void*);

// ================ ERROR THROWING HELPERS AND LOGGING

/*
* Sets the error callback which will be called when any error occurs.
*/
_TTToolLib_Exp void TelltaleToolLib_SetErrorCallback(ErrorCallbackF _Func);

_TTToolLib_Exp void TelltaleToolLib_RaiseError(const char* _Msg, ErrorSeverity _S);

_TTToolLib_Exp const char* TelltaleToolLib_GetLastError();

_TTToolLib_Exp void TTL_Log(const char* const  _Fmt, ...);

_TTToolLib_Exp void TelltaleToolLib_SetLoggerHook(void (*func)(const char* const fmt, va_list args));

// ====================== UTILITY TYPES FROM TELLTALE

extern bool sInitialized;

struct Flags {
	u32 mFlags;

	Flags(u32 i) : mFlags(i) {}

	Flags() {}

	INLINE bool IsSet(u32 bitIndex){
		return (mFlags & (1u << bitIndex)) != 0;
	}

	INLINE Flags& operator=(u32 i) {
		mFlags = i;
		return *this;
	}
	INLINE Flags& operator|=(u32 i) {
		mFlags |= i;
		return *this;
	}
	INLINE Flags& operator&=(u32 i) {
		mFlags &= i;
		return *this;
	}
};

template<int Bits, typename T>
struct TTBitArray {
	enum NoInit {
		eNoInit = 0
	};

	T mBits[Bits / 8];

};

namespace Geo {

	struct GeoGuid {
		u64 _Id[2];
	};

}

INLINE bool starts_with(const char* pre, const char* str)
{
	return strncmp(pre, str, strlen(pre)) == 0;
}

INLINE bool ends_with(const std::string& value, std::string ending)
{
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

template< typename T >
std::string to_symbol(T i)
{
	std::stringstream stream;
	stream << "Symbol<"
		<< std::setfill('0') << std::setw(sizeof(T) * 2)
		<< std::hex << i << ">";
	return stream.str();
}


// ========================= BASIC BASIC PTR IMPLEMENTATION

template<typename T> void PtrModifyRefCount(T*, int delta) {}//Not actually a template in the engine but the classes using Ptr must have this overriden for their type in the global NS.

//THIS DOES NOT WORK! Its only designed to placehold. You can have ownership semantics but std::shared_ptr is used when available. Destructor does NOT all DeleteObject!
template<typename T>
class Ptr {
public:
	T* mpData;
public:

	Ptr() = default;

	Ptr(const T* pObj) : mpData(pObj) {}

	Ptr(const Ptr<T>& other) : mpData(other.mpData) {}

	~Ptr() {}

	Ptr<T>& operator=(const Ptr<T>& pObj) {
		mpData = pObj.mpData;
		return *this;
	}

	Ptr<T>& operator=(T* pOther) {
		mpData = pOther;
		return *this;
	}

	void Assign(const T* pObj) {
		mpData = pObj;
	}

	void DeleteObject() {
		if (mpData) {
			delete mpData;
		}
	}

};

#endif
