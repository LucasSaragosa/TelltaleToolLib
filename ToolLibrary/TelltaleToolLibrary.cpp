// This file was written by Lucas Saragosa. If you use this code or library,
// I ask you to you give credit to me and the amazing Telltale Games.

#include "TelltaleToolLibrary.h"
#include "Meta.hpp"
#include "MetaInitialize.h"
#include "HashDB/HashDB.h"
#include "VersDB.h"
#include "DataStream/DataStream.h"
#include "TTArchive2.hpp"
#include <vector>
#include <filesystem>
#include <mutex>

#include "T3/T3EffectCache.h"
#include "T3/T3Effect.h"
#include "T3/Render.hpp"

struct _LoadedLibrary {
    LibraryHandle hLibrary;
    const char* pName;
};

static TelltaleVersionDatabase* sVersionDBs[KEY_COUNT]{ 0 };
static bool sbUsingStates = false;
static ProxyMetaState sProxyState[KEY_COUNT]{ 0 };
static std::mutex log_guard{};
static ToolLibRenderAdapters* spRenderAdapter = 0;
static ToolLibRenderAdapters spDefaultAdapter{};
static std::vector<_LoadedLibrary> loadedLibraries{};

void _InitializeDescs();

void _T3EffectCache_Free();

void T3GFXResource::OnDestroyed(){
    if (spRenderAdapter)
        spRenderAdapter->OnResourceDestroy(this);
}

_TTToolLib_Exp RenderConfiguration* TelltaleToolLib_GetRenderConfiguration(){
    return &sGlobalRenderConfig;
}

void T3GFXResource::OnCreate() {
    if (spRenderAdapter)
        spRenderAdapter->OnResourceCreation(this);
}

_TTToolLib_Exp void TelltaleToolLib_SetRenderAdapters(ToolLibRenderAdapters* pRenderAdapterOverloads){
    spRenderAdapter = pRenderAdapterOverloads;
    if (spRenderAdapter == 0)
        spRenderAdapter = &spDefaultAdapter;
}

_TTToolLib_Exp ToolLibRenderAdapters* TelltaleToolLib_GetRenderAdaptersStruct(){
    return spRenderAdapter;
}

_TTToolLib_Exp void TelltaleToolLib_InitializeT3(ToolLibRenderAdapters* pOptionalRenderAdapterCustom) {
	TelltaleToolLib_SetRenderAdapters(pOptionalRenderAdapterCustom);
	T3::Initialize();
}

_TTToolLib_Exp bool TelltaleToolLib_ProxyClassesAvail(const char* pGameID)
{
    int nMinGame = TelltaleToolLib_GetGameKeyIndex(MINIMUM_VERSDB_GAME);
    int nGameIndex = TelltaleToolLib_GetGameKeyIndex(pGameID);
    if (nGameIndex == -1)
        return false;
    if (nMinGame > nGameIndex)
        return false;
    return sVersionDBs[nGameIndex] != nullptr;
}

_TTToolLib_Exp MetaClassDescription* TelltaleToolLib_GenerateMetaClassDescriptionProxy(ProxyMetaState* pState, u64 typeSymbolCrc, bool bGetOnly /*= false*/)
{
    if (!pState || !typeSymbolCrc || !pState->mpStateGameDB)
        return NULL;
    MetaClassDescription* pProxyClass = pState->mpProxyClassList;
    MetaClassDescription* pTail = nullptr;
    while (pProxyClass) {
        if (pProxyClass->mHash == typeSymbolCrc)
            return pProxyClass;
        pTail = pProxyClass;
        pProxyClass = pProxyClass->pNextMetaClassDescription;
    }
    if (bGetOnly)
        return NULL;

    TelltaleVersionDatabase::ClassEntry* pSerializedClass = pState->mpStateGameDB->FindClass(typeSymbolCrc);
    if (!pSerializedClass)
        return NULL;

    pProxyClass = new MetaClassDescription;
    pProxyClass->mpExt = (const char*)pSerializedClass->mpExt;
    pProxyClass->mClassAlign = 8;
    pProxyClass->mClassSize = pSerializedClass->mClassSize;
    pProxyClass->mFlags = pSerializedClass->mFlags;
    pProxyClass->mpTypeInfoExternalName = (const char*)pSerializedClass->mpTypeInfoName;
    pProxyClass->mHash = pSerializedClass->mHash;
    bool p = sbUsingStates;
    sbUsingStates = false;
    MetaClassDescription* ac = TelltaleToolLib_FindMetaClassDescription_ByHash(typeSymbolCrc);
    if (ac)
        memcpy(pProxyClass->mpVTable, ac->mpVTable, sizeof(ac->mpVTable));
    else
        memset(pProxyClass->mpVTable, 0, sizeof(pProxyClass->mpVTable));
    sbUsingStates = p;
    pProxyClass->mMetaOperationsList = NULL;
    MetaMemberDescription* pPrev = NULL;
    TelltaleVersionDatabase::MemberEntry* pMember{};
    do {
        if (pMember)
            pMember = pState->mpStateGameDB->mMembers.mpStorage + pMember->mpNextMember;
        else
            pMember = pState->mpStateGameDB->mMembers.mpStorage + pSerializedClass->mpFirstMember;
        if (pMember) {
            MetaMemberDescription* pNewMember = new MetaMemberDescription;
            pNewMember->mFlags = pMember->mFlags;
            pNewMember->mOffset = pMember->mOffset;
            //TODO
        }
    } while (pMember != nullptr);

    if (pTail == nullptr)
        pState->mpProxyClassList = pProxyClass;
    else
        pTail->pNextMetaClassDescription = pProxyClass;
    return pProxyClass;
}

_TTToolLib_Exp void TelltaleToolLib_ResetProxyMetaState(ProxyMetaState* pState)
{
    if (pState) {
        MetaClassDescription* pProxy = pState->mpProxyClassList;
        //TODO
    }
}

_TTToolLib_Exp bool TelltaleToolLib_SetUsingProxyMetaStates(bool bOnOff)
{
    bool bState = sbUsingStates;
    sbUsingStates = bOnOff;
    return bState;
}

_TTToolLib_Exp ProxyMetaState* TelltaleToolLib_GetProxyMetaState(const char* pGameID)
{
    int index = TelltaleToolLib_GetGameKeyIndex(pGameID);
    if (index == -1)
        return NULL;
    if (!TelltaleToolLib_ProxyClassesAvail(pGameID))
        return NULL;
    return &sProxyState[index];
}

_TTToolLib_Exp int TelltaleToolLib_SetProxyVersionDatabasesFromArchive(TTArchive2& archive){
    if (&archive == 0)
        return 0;
    int nNum = 0;
    for(int i = 0; i < KEY_COUNT; i++){
        Symbol versName = sBlowfishKeys[i].game_id;
        versName += ".VersDB";
        TTArchive2::ResourceEntry* pEntry = archive._FindResource(versName);
        if(pEntry){
            int index = i;
			if (sVersionDBs[index] != nullptr)
				delete sVersionDBs[index];
			sVersionDBs[index] = new TelltaleVersionDatabase;
            DataStream* pStream = archive.GetResourceStream(pEntry);
            if(!pStream || pStream->IsInvalid()){
                TTL_Log("WARN: Could not obtain stream for VersDB: %s.VersDB", sBlowfishKeys[i].game_id);
            }else{
                if(!sVersionDBs[index]->SerializeIn(pStream)){
                    TTL_Log("WARN: Could not read in VersDB: %s.VersDB! Contact me", sBlowfishKeys[i].game_id);
                }else{
					sProxyState[index].mpStateGameDB = sVersionDBs[index];
					nNum++;
                }
            }
        }
    }
    return nNum;
}

_TTToolLib_Exp int TelltaleToolLib_SetProxyVersionDatabases(const char* pFolder)
{
    if (!pFolder || !(*pFolder))
        return 0;
    int nNum = 0;
    std::filesystem::path folderPath{ pFolder };
    std::string f = folderPath.string();
    if (!(f.at(f.length() - 1) == '/' || f.at(f.length() - 1) == '\\'))
        folderPath += "/";
    for (auto& it : std::filesystem::directory_iterator{ folderPath }) {
        if (it.is_regular_file() && it.path().extension().string() == ".VersDB") {
			size_t lastindex = it.path().filename().string().find_last_of(".");
			std::string rawname = it.path().filename().string().substr(0, lastindex);
            int index = TelltaleToolLib_GetGameKeyIndex(rawname.c_str());
            if (index == -1) {
                TTL_Log("Found VersDB with a name that doesnt belong to a valid game ID! Ignoring: %s", rawname.c_str());
                continue;
            }
            if (sVersionDBs[index] != nullptr)
                delete sVersionDBs[index];
            sVersionDBs[index] = new TelltaleVersionDatabase;
            DataStreamFile_PlatformSpecific in = _OpenDataStreamFromDisc_(it.path().string().c_str(), READ);
            if (in.IsInvalid() || !sVersionDBs[index]->SerializeIn(&in)) {
				TelltaleToolLib_RaiseError("Could not open or load a VersDB file", ErrorSeverity::ERR);
                return -1;
            }
            sProxyState[index].mpStateGameDB = sVersionDBs[index];
            nNum++;
        }
    }
    return nNum;
}

_TTToolLib_Exp i32 TelltaleToolLib_GetGameKeyIndex(const char* pGameID) {
    if (pGameID) {
        for (int i = 0; i < KEY_COUNT; i++) {
            if (!_stricmp(sBlowfishKeys[i].game_id, pGameID)) {
                return i;
            }
        }
    }
    return -1;
}

/*
MetaOpResult TelltaleToolLib_PerformMetaSerialize(MetaClassDescription* pObjectDescription, void* pObject, MetaStream* pUserData) {
    if (!pObjectDescription || !pObject || !pUserData)
        return eMetaOp_Fail;
    return PerformMetaSerializeFull(pUserData, pObject, pObjectDescription);
}
*/

HashDatabase* sgHashDB = NULL;
bool sInitialized = false;
const char* s_lastError = nullptr;
void (*printf_hook)(const char* const fmt, va_list args) = NULL;

void _DefaultCallback(const char* msg, ErrorSeverity e) {
#ifdef DEBUGMODE
    TTL_Log("ERROR: %s: [%s]\n",msg, e == ErrorSeverity::CRITICAL_ERROR ? "CRITICAL" : e == ErrorSeverity::NOTIFY ? "NOTIFY" 
    : e == ErrorSeverity::WARN ? "WARNING" : "ERR");
#endif
    if (e == ErrorSeverity::CRITICAL_ERROR)exit(-1);
}

ErrorCallbackF sDefaultErrorCallback = _DefaultCallback;

_TTToolLib_Exp void TelltaleToolLib_SetErrorCallback(ErrorCallbackF _Func) {
    sDefaultErrorCallback = _Func;
}

_TTToolLib_Exp void TelltaleToolLib_RaiseError(const char* _Msg, ErrorSeverity _S) {
    sDefaultErrorCallback(_Msg, _S);
    s_lastError = _Msg;
}

_TTToolLib_Exp const char* TelltaleToolLib_GetLastError() {
    return s_lastError;
}

_TTToolLib_Exp void TTL_Log(const char* const  _Fmt, ...){
    std::lock_guard _g(log_guard);
    va_list va{};
    va_start(va, _Fmt);
#ifdef _DEBUG
    vprintf_s(_Fmt, va); //print as normal
#endif
    if (printf_hook != NULL)
        printf_hook(_Fmt, va);
    va_end(va);
}

_TTToolLib_Exp void TelltaleToolLib_SetLoggerHook(void (*func)(const char* const fmt, va_list args)){
    printf_hook = func;
}

TTEXPORT void* TelltaleToolLib_Malloc(unsigned long size) {
    return malloc(size);
}

TTEXPORT void TelltaleToolLib_Dealloc(void* ptr) {
    if(ptr)
        return free(ptr);
}

_TTToolLib_Exp void TelltaleToolLib_DeleteClassInstance(MetaClassDescription* pClass, void* inst) {
    pClass->Delete(inst);
}

_TTToolLib_Exp void* TelltaleToolLib_CreateClassInstance(MetaClassDescription* pClass) {
    if (pClass) {
        return pClass->New();
    }
    return nullptr;
}

_TTToolLib_Exp LibraryHandle TelltaleToolLib_GetLibrary(const char* pName) {
    char buf[256];
    sprintf_s(buf, "./LibBin/%s64." PLATFORM_DYLIB_EXT, pName);
    pName = buf;
    for(auto& it : loadedLibraries){
        if (!memcmp(pName, it.pName, strlen(pName)))
            return it.hLibrary;
    }
    LibraryHandle h = PlatformLoadLibrary(pName);
    if (h == EMPTY_LIBRARY_HANDLE)
        return EMPTY_LIBRARY_HANDLE;
    loadedLibraries.push_back({ h,pName });
    return h;
}

_TTToolLib_Exp void TelltaleToolLib_Free() {
    T3::Shutdown();
    if (sgHashDB) {
        delete sgHashDB;
        sgHashDB = NULL;
    }
    sDefaultErrorCallback = _DefaultCallback;
    for (int i = 0; i < KEY_COUNT; i++) {
        TelltaleToolLib_ResetProxyMetaState(&sProxyState[i]);
        if (sVersionDBs[i])
            delete sVersionDBs[i];
    }
    for(auto& val : loadedLibraries){
        PlatformFreeLibrary(val.hLibrary);
    }
    loadedLibraries.clear();
    _T3EffectCache_Free();
    printf_hook = NULL;
    spRenderAdapter = 0;
    sSetKeyIndex = DEFAULT_BLOWFISH_GAME_KEY;
    memset(sVersionDBs, 0, sizeof(TelltaleVersionDatabase*) * KEY_COUNT);
    memset(sProxyState, 0, sizeof(ProxyMetaState) * KEY_COUNT);
    __ReleaseSVI_Internal();
}

_TTToolLib_Exp void TelltaleToolLib_SetGlobalHashDatabaseFromStream(DataStream* stream) {
    if(stream)
        TelltaleToolLib_SetGlobalHashDatabase(new HashDatabase(stream));
}

_TTToolLib_Exp void TelltaleToolLib_SetGlobalHashDatabase(HashDatabase* db) {
    if (sgHashDB)
        delete sgHashDB;
    sgHashDB = db;
    if(db)
        db->Open();
}

_TTToolLib_Exp HashDatabase* TelltaleToolLib_GetGlobalHashDatabase() {
    return sgHashDB;
}

_TTToolLib_Exp u8* TelltaleToolLib_EncryptLencScript(u8* data, u32 size, u32* outsize) {
    u8* ret = (u8*)malloc(size);
    memcpy(ret, data, size);
    if(outsize)
     *outsize = size;
    TelltaleToolLib_BlowfishEncrypt(ret, size, sBlowfishKeys[sSetKeyIndex].isNewEncryption, (unsigned char*)sBlowfishKeys[sSetKeyIndex].game_key);
    return ret;
}

_TTToolLib_Exp u8* TelltaleToolLib_DecryptLencScript(u8* data, u32 size, u32* outsize) {
    u8* ret = (u8*)malloc(size);
    memcpy(ret, data, size);
	if (outsize)
		*outsize = size;
    TelltaleToolLib_BlowfishDecrypt(ret, size, sBlowfishKeys[sSetKeyIndex].isNewEncryption, (unsigned char*)sBlowfishKeys[sSetKeyIndex].game_key);
    return ret;
}

_TTToolLib_Exp u8* TelltaleToolLib_EncryptScript(u8* data, u32 size, u32 *outsize) {
    if (!data || 8 > size)return NULL;
    if (*(int*)data == *(const int*)"\x1BLua") {
        u8* ret = (u8*)malloc(size);
        memcpy(ret, data, size);
		if (outsize)
			*outsize = size;
        *(int*)ret = 1850035227;//LEn
        TelltaleToolLib_BlowfishEncrypt(ret + 4, size - 4, sBlowfishKeys[sSetKeyIndex].isNewEncryption, (unsigned char*)sBlowfishKeys[sSetKeyIndex].game_key);
        return ret;
    }
    else if (*(int*)data != *(const int*)"\x1BLEo" && *(int*)data != *(const int*)"\x1BLEn") {
        u8* ret = (u8*)malloc(size + 4);
        memcpy(ret + 4, data, size);
        if (outsize)
            *outsize = size + 4;
        *(int*)ret = 1866812443;//LEo
        TelltaleToolLib_BlowfishEncrypt(ret + 4, size, sBlowfishKeys[sSetKeyIndex].isNewEncryption, (unsigned char*)sBlowfishKeys[sSetKeyIndex].game_key);
        return ret;
    }
    u8* ret = (u8*)malloc(size);
    memcpy(ret, data, size);
	if (outsize)
		*outsize = size;
    return ret;
}

_TTToolLib_Exp u8* TelltaleToolLib_DecryptScript(u8* data, u32 size, u32* outsize) {
    if (8 >= size || !data)return NULL;
    if (*(char*)data != '\x1B') {
        //plain old text version workaround
        u8* ret = (u8*)malloc(size);
        memcpy(ret, data, size);
        *outsize = size;
        return ret;
    }
    if (*(int*)data == *(const int*)"\x1BLEo") {
        u8* ret = (u8*)malloc(size - 4);
        memcpy(ret, data + 4, size - 4);
        *outsize = size - 4;
        TelltaleToolLib_BlowfishDecrypt(ret, size - 4, sBlowfishKeys[sSetKeyIndex].isNewEncryption, (unsigned char*)sBlowfishKeys[sSetKeyIndex].game_key);
        return ret;
    }
    u8* ret = (u8*)malloc(size);
    memcpy(ret, data, size);
    if (*(int*)data == *(const int*)"\x1BLEn")
        *(int*)ret = 1635077147;
    TelltaleToolLib_BlowfishDecrypt(ret + 4, size - 4, sBlowfishKeys[sSetKeyIndex].isNewEncryption, (unsigned char*)sBlowfishKeys[sSetKeyIndex].game_key);
    *outsize = size;
    return ret;
}

_TTToolLib_Exp const char* TelltaleToolLib_GetMetaClassDescriptionName(MetaClassDescription* pObjDesc) {
    return pObjDesc->mpTypeInfoName;
}

_TTToolLib_Exp DataStream* TelltaleToolLib_CreateDataStream(const char* fp, DataStreamMode mode) {
    DataStreamFileDisc* pDisk =  new DataStreamFileDisc((FileHandle)PlatformSpecOpenFile(fp, mode), mode);
    if(pDisk->IsInvalid()){
        delete pDisk;
        return nullptr;
    }
    return pDisk;
}

_TTToolLib_Exp void TelltaleToolLib_DeleteDataStream(DataStream* stream) {
    delete stream;//calls destructor
}

_TTToolLib_Exp char* TelltaleToolLib_Alloc_GetFixed1024ByteStringBuffer() {
    static char buf[1024];
    return &buf[0];
}

_TTToolLib_Exp char* TelltaleToolLib_Alloc_GetFixed8BytePointerBuffer() {
    static char buf[8];
    return &buf[0];
}

_TTToolLib_Exp void TelltaleToolLib_GetNextMetaClassDescription(MetaClassDescription** pObjDescPtr) {
    if (pObjDescPtr && *pObjDescPtr) *pObjDescPtr = (*pObjDescPtr)->pNextMetaClassDescription;
}

_TTToolLib_Exp void TelltaleToolLib_GetNextMetaMemberDescription(MetaMemberDescription** pMemberDescPtr) {
    if (pMemberDescPtr && *pMemberDescPtr)*pMemberDescPtr = (*pMemberDescPtr)->mpNextMember;
}

_TTToolLib_Exp MetaClassDescription* TelltaleToolLib_FindMetaClassDescription_ByHash(u64 pHash) {
    for (MetaClassDescription* i = TelltaleToolLib_GetFirstMetaClassDescription(); i;) {
        if (i->mHash == pHash)
            return i;
        TelltaleToolLib_GetNextMetaClassDescription(&i);
    }
    return NULL;
}

_TTToolLib_Exp MetaClassDescription* TelltaleToolLib_FindMetaClassDescription(const char* pStr, bool pIsName) {
    if (pIsName) {
        u64 crc = CRC64_CaseInsensitive(0, pStr);
        for (MetaClassDescription* i = TelltaleToolLib_GetFirstMetaClassDescription(); i;) {          
            if (i->mHash == crc)
                return i;
            TelltaleToolLib_GetNextMetaClassDescription(&i);
        }
    }
    else {
        for (MetaClassDescription* i = TelltaleToolLib_GetFirstMetaClassDescription(); i;) {
            if (!i->mpExt) {//stfu:(cba
                TelltaleToolLib_GetNextMetaClassDescription(&i);
                continue;
            }
            if (!_stricmp(pStr,i->mpExt))
                return i;
            TelltaleToolLib_GetNextMetaClassDescription(&i);
        }
    }
    return NULL;
}

_TTToolLib_Exp void TelltaleToolLib_GetMetaMemberDescriptionInfo(MetaMemberDescription* pMemberDesc, void* pDest, MetaMemberDescriptionParam param) {
    switch (param) {
    case MetaMemberDescriptionParam::eMMDP_Name:
        *static_cast<const void**>(pDest) = pMemberDesc->mpName;
        break;
    case MetaMemberDescriptionParam::eMMDP_Offset:
        *static_cast<u64*>(pDest) = pMemberDesc->mOffset;
        break;
    case MetaMemberDescriptionParam::eMMDP_Flags:
        *static_cast<u32*>(pDest) = pMemberDesc->mFlags;
        break;
    case MetaMemberDescriptionParam::eMMDP_HostClass:
        *static_cast<MetaClassDescription**>(pDest) = pMemberDesc->mpHostClass;
        break;
    case MetaMemberDescriptionParam::eMMDP_MemberClassDesc:
        *static_cast<MetaClassDescription**>(pDest) = pMemberDesc->mpMemberDesc;
        break;
    case MetaMemberDescriptionParam::eMMDP_NextMember:
        *static_cast<MetaMemberDescription**>(pDest) = pMemberDesc->mpNextMember;
        break;
    case MetaMemberDescriptionParam::eMMDP_GameVersionRange:
        *static_cast<u32*>((void*)((char*)pDest + 0)) = pMemberDesc->mSkipVersion;
        *static_cast<u32*>((void*)((char*)pDest + 4)) = pMemberDesc->mGameIndexVersionRange.min;
        *static_cast<u32*>((void*)((char*)pDest + 8)) = pMemberDesc->mGameIndexVersionRange.max;
        break;
    default:
        return;
    }
}

_TTToolLib_Exp void TelltaleToolLib_GetMetaClassDescriptionInfo(MetaClassDescription* pObj, void* pDest, MetaClassDescriptionParam param) {
    switch (param) {
    case MetaClassDescriptionParam::eMCDP_Extension:
        *static_cast<const char**>(pDest) = pObj->mpExt;
        break;
    case MetaClassDescriptionParam::eMCDP_Name:
        *static_cast<const char**>(pDest) = pObj->mpTypeInfoName;
        break;
    case MetaClassDescriptionParam::eMCDP_Hash:
        *static_cast<u64*>(pDest) = pObj->mHash;
        break;
    case MetaClassDescriptionParam::eMCDP_Flags:
        *static_cast<u32*>(pDest) = pObj->mFlags.mFlags;
        break;
    case MetaClassDescriptionParam::eMCDP_ClassSize:
        *static_cast<u32*>(pDest) = pObj->mClassSize;
        break;
    case MetaClassDescriptionParam::eMCDP_FirstMemberPtr:
        *static_cast<MetaMemberDescription**>(pDest) = pObj->mpFirstMember;
        break;
    case MetaClassDescriptionParam::eMCDP_OperationsList:
        *static_cast<MetaOperationDescription**>(pDest) = pObj->mMetaOperationsList;
        break;
    case MetaClassDescriptionParam::eMCDP_VTable:
        *static_cast<void***>(pDest) = &pObj->mpVTable[0];
        break;
    case MetaClassDescriptionParam::eMCDP_SerializeAccel:
        *static_cast<MetaSerializeAccel**>(pDest) = pObj->mpSerializeAccel;
        break;
    default:
        return;
    }
}

_TTToolLib_Exp MetaClassDescription* TelltaleToolLib_GetFirstMetaClassDescription() {
    return spFirstMetaClassDescription;
}

_TTToolLib_Exp const char* TelltaleToolLib_GetVersion() {
	return _VERSION;
}

_TTToolLib_Exp i32 TelltaleToolLib_GetMetaTypesCount() {
    return sMetaTypesCount;
}

_TTToolLib_Exp bool TelltaleToolLib_Initialized() {
    return sInitialized;
}

_TTToolLib_Exp bool TelltaleToolLib_ReadMetaStream(DataStream* pIn, MetaClassDescription* pClass, void* pDestInstance){
    if(pIn && pClass && pDestInstance){
        MetaStream ms{};
        ms.Open(pIn, MetaStreamMode::eMetaStream_Read, {});
        ms.mbDontDeleteStream = true;
        if(ms.mbErrored){
            TelltaleToolLib_RaiseError("RMS: not a meta stream file", ErrorSeverity::ERR);
        }else{
            if(ms.mVersionInfo.size() > 0 && ms.mVersionInfo[0].mTypeSymbolCrc != pClass->mHash){
                TelltaleToolLib_RaiseError("RMS: the type class trying to be read was not the type stored in the file. The extension may be invalid.", ErrorSeverity::ERR);
                return false;
            }
            bool ok = PerformMetaSerializeFull(&ms, pDestInstance, pClass) == eMetaOp_Succeed;
            if(!ok)
                TelltaleToolLib_RaiseError("RMS: invalid type stored inside meta stream: could not serialize in", ErrorSeverity::ERR);
            return ok;
        }
    }
    else
        TelltaleToolLib_RaiseError("RMS: invalid parameters", ErrorSeverity::ERR);
}

_TTToolLib_Exp void TelltaleToolLib_RemovePropertySetValue(void* prop, unsigned long long keyhash){
	 PropertySet* p = (PropertySet*)prop;
     if (p == nullptr)
         return;
     p->RemoveProperty(keyhash);
 }

_TTToolLib_Exp void* TelltaleToolLib_GetPropertySetValue(void* prop, unsigned long long keyhash, MetaClassDescription** out) {
    PropertySet* p = (PropertySet*)prop;
    if (p == nullptr)
        return nullptr;
	for (int i = 0; i < p->mKeyMap.GetSize(); i++)
        if (p->mKeyMap[i].mKeyName == keyhash) {
            *out = p->mKeyMap[i].mpValue->mpDataDescription;
            return p->mKeyMap[i].mpValue->mpValue;
        }
    return nullptr;
}

_TTToolLib_Exp bool TelltaleToolLib_SetPropertySetValue(void* prop, unsigned long long kh, MetaClassDescription* pType, void* pInst){
	PropertySet* p = (PropertySet*)prop;
    if (p == nullptr || pType == nullptr || pInst == nullptr)
        return false;
	for (int i = 0; i < p->mKeyMap.GetSize(); i++)
		if (p->mKeyMap[i].mKeyName == kh) {
            p->mKeyMap[i].mpValue->SetData(pInst, pType);
            return true;
		}
    p->AddProperty(kh, pType, pInst);
    return true;
}

inline bool operator==(int i, IntrinType t) {
	return i == (int)t;
}

inline bool operator==(int i, ContainerOp t) {
	return i == (int)t;
}

inline bool operator==(int i, StringOp t) {
	return i == (int)t;
}

_TTToolLib_Exp void* TelltaleToolLib_CreateIntrinsicInstance(int intrin, char data[16], MetaClassDescription** pOutTypeOptional){
    unsigned long long rawvalue = *((unsigned long long*)data);
    MetaClassDescription* mcd = 0;
    void* v = 0;
    if(intrin == IntrinType::I8){
        mcd = TelltaleToolLib_FindMetaClassDescription("int8", 1);
        v = TelltaleToolLib_CreateClassInstance(mcd);
        *((signed char*)v) = rawvalue & 0xFF;
	}
	else if (intrin == IntrinType::U8) {
		mcd = TelltaleToolLib_FindMetaClassDescription("uint8", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		*((unsigned char*)v) = rawvalue & 0xFF;
	}
	else if (intrin == IntrinType::I16) {
		mcd = TelltaleToolLib_FindMetaClassDescription("uint16", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		*((unsigned short*)v) = rawvalue & 0xFFFF;
	}
	else if (intrin == IntrinType::U16) {
		mcd = TelltaleToolLib_FindMetaClassDescription("int16", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		*((signed short*)v) = rawvalue & 0xFFFF;
	}
	else if (intrin == IntrinType::I32) {
		mcd = TelltaleToolLib_FindMetaClassDescription("int32", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		*((long*)v) = rawvalue & 0xFFFFFFFF;
	}else if (intrin == IntrinType::U32) {
		mcd = TelltaleToolLib_FindMetaClassDescription("uint32", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		*((unsigned long*)v) = rawvalue & 0xFFFFFFFF;
	}
	else if (intrin == IntrinType::I64) {
		mcd = TelltaleToolLib_FindMetaClassDescription("int64", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
        *((signed long long*)v) = rawvalue;
	}
	else if (intrin == IntrinType::U64) {
		mcd = TelltaleToolLib_FindMetaClassDescription("uint64", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		*((unsigned long long*)v) = rawvalue & 0xFF;
	}
	else if (intrin == IntrinType::SYM) {
		mcd = TelltaleToolLib_FindMetaClassDescription("Symbol", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
        ((Symbol*)v)->SetCRC(rawvalue);
	}
	else if (intrin == IntrinType::STR) {
		mcd = TelltaleToolLib_FindMetaClassDescription("String", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
        ((String*)v)->assign((const char*)rawvalue);
	}
	else if (intrin == IntrinType::FLT) {
		mcd = TelltaleToolLib_FindMetaClassDescription("float", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
        *((unsigned long*)v) = rawvalue & 0xFFFFFFFF;
	}
	else if (intrin == IntrinType::DBL) {
		mcd = TelltaleToolLib_FindMetaClassDescription("double", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		*((unsigned long long*)v) = rawvalue;
	}
	else if (intrin == IntrinType::FLG) {
		mcd = TelltaleToolLib_FindMetaClassDescription("Flags", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
        ((Flags*)v)->mFlags = (u32)rawvalue;
	}
	else if (intrin == IntrinType::BLN) {
		mcd = TelltaleToolLib_FindMetaClassDescription("bool", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
        *((unsigned char*)v) = rawvalue != 0;
	}
	else if (intrin == IntrinType::VC4) {
		mcd = TelltaleToolLib_FindMetaClassDescription("Vector4", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
        ((Vector4*)v)->x = *((float*)(data + 0));
        ((Vector4*)v)->y = *((float*)(data + 4));
        ((Vector4*)v)->z = *((float*)(data + 8));
        ((Vector4*)v)->w = *((float*)(data + 12));
	}
	else if (intrin == IntrinType::VC3) {
		mcd = TelltaleToolLib_FindMetaClassDescription("Vector3", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		((Vector3*)v)->x = *((float*)(data + 0));
		((Vector3*)v)->y = *((float*)(data + 4));
		((Vector3*)v)->z = *((float*)(data + 8));
	}
	else if (intrin == IntrinType::VC2) {
		mcd = TelltaleToolLib_FindMetaClassDescription("Vector2", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		((Vector2*)v)->x = *((float*)(data + 0));
		((Vector2*)v)->y = *((float*)(data + 4));
	}
	else if (intrin == IntrinType::QUA) {
		mcd = TelltaleToolLib_FindMetaClassDescription("Quaternion", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		((Quaternion*)v)->x = *((float*)(data + 0));
		((Quaternion*)v)->y = *((float*)(data + 4));
		((Quaternion*)v)->z = *((float*)(data + 8));
		((Quaternion*)v)->w = *((float*)(data + 12));
	}
	else if (intrin == IntrinType::COL) {
		mcd = TelltaleToolLib_FindMetaClassDescription("Color", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		((Color*)v)->r = *((float*)(data + 0));
		((Color*)v)->g = *((float*)(data + 4));
		((Color*)v)->b = *((float*)(data + 8));
		((Color*)v)->a = *((float*)(data + 12));
	}
	else if (intrin == IntrinType::RGI) {
		mcd = TelltaleToolLib_FindMetaClassDescription("TRange<int>", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		((TRange<int>*)v)->min = *((int*)(data + 0));
        ((TRange<int>*)v)->max = *((int*)(data + 4));
	}
	else if (intrin == IntrinType::RGF) {
		mcd = TelltaleToolLib_FindMetaClassDescription("TRange<float>", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		((TRange<float>*)v)->min = *((float*)(data + 0));
		((TRange<float>*)v)->max = *((float*)(data + 4));
	}
	else if (intrin == IntrinType::RCF) {
		mcd = TelltaleToolLib_FindMetaClassDescription("TRect<float>", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		((TRect<float>*)v)->bottom = *((float*)(data + 0));
		((TRect<float>*)v)->top = *((float*)(data + 4));
		((TRect<float>*)v)->right = *((float*)(data + 8));
		((TRect<float>*)v)->left = *((float*)(data + 12));
	}
	else if (intrin == IntrinType::SPR) {
		mcd = TelltaleToolLib_FindMetaClassDescription("Sphere", 1);
		v = TelltaleToolLib_CreateClassInstance(mcd);
		((Sphere*)v)->mCenter.x = *((float*)(data + 0));
		((Sphere*)v)->mCenter.y = *((float*)(data + 4));
		((Sphere*)v)->mCenter.z = *((float*)(data + 8));
		((Sphere*)v)->mRadius = *((float*)(data + 12));
	}else{
        TelltaleToolLib_RaiseError("Invalid intrinsic type number passed into CreateIntrinsicInstance", ERR);
        return nullptr;
    }
}

_TTToolLib_Exp void* TelltaleToolLib_Container(int op, void* container, void* arg1, void* arg2, void* arg3){
    if(op == ContainerOp::CHK){
        if (container == nullptr)
            return 0;
        MetaClassDescription* clazz = (MetaClassDescription*)arg1;
        return (void*)((clazz->mpFirstMember != nullptr && !_stricmp(clazz->mpFirstMember->mpName,"Baseclass_ContainerInterface")) ? 1llu : 0llu);
    }
    ContainerInterface* pInterface = (ContainerInterface*)container;
    if (pInterface == nullptr)
        return nullptr;
    if(op == ContainerOp::GET){
        return pInterface->GetElement((int)((uintptr_t)arg1));
    }else if(op == ContainerOp::ADD){
        pInterface->AddElement((int)((uintptr_t)arg1), arg2, arg3);
        return nullptr;
	}
	else if (op == ContainerOp::REM) {
        pInterface->RemoveElement((int)((uintptr_t)arg1));
		return nullptr;
	}
    else if (op == ContainerOp::CNT) {
        return (void*)pInterface->GetSize();
	}
    else if (op == ContainerOp::KTY) {
        return (void*)pInterface->GetContainerKeyClassDescription();
	}else if(op == ContainerOp::CLR){
        pInterface->ClearElements();
    }
	else if (op == ContainerOp::VTY) {
        return (void*)pInterface->GetContainerDataClassDescription();
    }else{
        TelltaleToolLib_RaiseError("Invalid operation number for Container operation", ERR);
        return nullptr;
    }
};

_TTToolLib_Exp void* TelltaleToolLib_String(int op, void* stringInst, void* param){
    if (stringInst == nullptr)
        return nullptr;
    if(op == StringOp::GET){
        return (void*)((String*)stringInst)->c_str();
    }else if(op == StringOp::SET){
        ((String*)stringInst)->assign((const char*)param);
    }
    else if (op == StringOp::CLR)
        ((String*)stringInst)->clear();
    return nullptr;
}

_TTToolLib_Exp MetaMemberDescription* TelltaleToolLib_FindMember(MetaClassDescription* clazz, const char* memberVarName){
     if (clazz == nullptr || memberVarName == nullptr)
         return nullptr;
     MetaMemberDescription* pMem = clazz->mpFirstMember;
     while(pMem){
         if (!_stricmp(memberVarName, pMem->mpName))
             return pMem;
         pMem = pMem->mpNextMember;
     }
     return 0;
}

_TTToolLib_Exp bool TelltaleToolLib_WriteMetaStream(DataStream* pOut, MetaClassDescription* pClass, void* pInstance) {
	if (pOut && pClass && pInstance) {
		MetaStream ms{};
		ms.Open(pOut, MetaStreamMode::eMetaStream_Write, {});
		ms.mbDontDeleteStream = true;
		if (ms.mbErrored) {
			TelltaleToolLib_RaiseError("WMS: error initialising stream", ErrorSeverity::ERR);
		}
		else {
			bool ok = PerformMetaSerializeFull(&ms, pInstance, pClass) == eMetaOp_Succeed;
            if (!ok)
                TelltaleToolLib_RaiseError("WMS: could not serialize out!", ErrorSeverity::ERR);
            else
                ms.Close();
			return ok;
		}
	}
	else
		TelltaleToolLib_RaiseError("WMS: invalid parameters", ErrorSeverity::ERR);
}

_TTToolLib_Exp bool TelltaleToolLib_SetBlowfishKey(const char* game_id) {
    if (game_id) {
        for (int i = 0; i < KEY_COUNT; i++) {
            if (!_stricmp(sBlowfishKeys[i].game_id, game_id)) {
                sSetKeyIndex = i;
                return true;
                break;
            }
        }
    }
    else sSetKeyIndex = DEFAULT_BLOWFISH_GAME_KEY;
    return false;
    //if (spBlowfish) {
    //    spBlowfish->Init(sBlowfishKeys[sSetKeyIndex].game_key);
    //}
}

_TTToolLib_Exp const char* TelltaleToolLib_GetBlowfishKey() {
    return sBlowfishKeys[sSetKeyIndex].game_id;
}

_TTToolLib_Exp bool TelltaleToolLib_Initialize(const char* game_id) {
    if (sInitialized)return false;
    if (game_id) {
        const BlowfishKey* k = NULL;
        for (int i = 0; i < KEY_COUNT; i++) {
            if (!_stricmp(sBlowfishKeys[i].game_id, game_id)) {
                k = &sBlowfishKeys[i];
                sSetKeyIndex = i;
                break;
            }
        }
        if (k == NULL)return false;
       // Blowfish::Initialize(k);
    }
  //  else Blowfish::Initialize(&sBlowfishKeys[sSetKeyIndex]);
    MetaInit::Initialize();//init all types
    memset(sVersionDBs, 0, sizeof(TelltaleVersionDatabase*) * KEY_COUNT);
    memset(sProxyState, 0, sizeof(ProxyMetaState) * KEY_COUNT);
    for (int i = 0; i < KEY_COUNT; i++) {
        sProxyState[i].mpStateGameID = sBlowfishKeys[i].game_id;
    }
    ShadowUtil::Initialize();
    _InitializeDescs();
    sbUsingStates = false;
    sInitialized = true;
    spRenderAdapter = &spDefaultAdapter;
    return true;
}

_TTToolLib_Exp bool TelltaleToolLib_IsGameIDOld(const char* game_id)
{
    int id = TelltaleToolLib_GetGameKeyIndex(game_id);
    return id <= 51;
}

//I KNOW ITS BAD FOR ALLOCATIONS BUT IM SORRY I CANT BE ASKED TO CHANGE IT D:
_TTToolLib_Exp void TelltaleToolLib_MakeInternalTypeName(char** _StringPtr) {
    static std::string _sRepl_A = "struct ";
    static std::string _sRepl_B = "class ";
    static std::string _sRepl_C = "enum ";
    static std::string _sRepl_D = "std::";
    static std::string _sReplLast = "basic_string<char,char_traits<char>,allocator<char>>";
    static std::string _sRepl_E = " ";
    static std::string _sNothing = "";
	std::string ReplStr(*_StringPtr);
    size_t start_pos = 0;
    while ((start_pos = ReplStr.find(_sRepl_A, start_pos)) != std::string::npos) {
        ReplStr.replace(start_pos, _sRepl_A.length(), _sNothing);
    }
    start_pos = 0;
    while ((start_pos = ReplStr.find(_sRepl_B, start_pos)) != std::string::npos) {
        ReplStr.replace(start_pos, _sRepl_B.length(), _sNothing);
    }
    start_pos = 0;
    while ((start_pos = ReplStr.find(_sRepl_C, start_pos)) != std::string::npos) {
        ReplStr.replace(start_pos, _sRepl_C.length(), _sNothing);
    }
    start_pos = 0;
    while ((start_pos = ReplStr.find(_sRepl_D, start_pos)) != std::string::npos) {
        ReplStr.replace(start_pos, _sRepl_D.length(), _sNothing);
    }
	start_pos = 0;
	while ((start_pos = ReplStr.find(_sRepl_E, start_pos)) != std::string::npos) {
		ReplStr.replace(start_pos, _sRepl_E.length(), _sNothing);
	}
	start_pos = 0;
	while ((start_pos = ReplStr.find(_sReplLast, start_pos)) != std::string::npos) {
		ReplStr.replace(start_pos, _sReplLast.length(), "String");
	}
    u32 slen = ReplStr.length();
    char* nbuf = (char*)malloc(slen+1);
    memcpy(nbuf, ReplStr.c_str(), slen);
    //for (int i = 0; i < slen; i++) {
    //    nbuf[i] |= 0b100000;
    //}
    nbuf[slen] = 0i8;
    free(*_StringPtr);
    *_StringPtr = nbuf;
}

void _PrintfDumper(const char* const _Fmt, const char* _ParamA, const char* _ParamB) {
    if (_ParamA && _ParamB) {
        printf(_Fmt, _ParamA, _ParamB);
    }
    else if (_ParamA) {
        printf(_Fmt, _ParamA);
    }
    else {
        printf(_Fmt);
    }
}

void printMembers(int tabs, MetaMemberDescription* mem, DumpClassInfoF _Dumper) {
    while (mem) {
        for (int i = 0; i < tabs; i++)
            _Dumper("\t", NULL, NULL);
        _Dumper("[%s] %s\n", mem->mpMemberDesc->mpTypeInfoName, mem->mpName);
        if (mem->mpMemberDesc->mpFirstMember)
            printMembers(tabs + 1, mem->mpMemberDesc->mpFirstMember,_Dumper);
        mem = mem->mpNextMember;
    }
}

_TTToolLib_Exp void TelltaleToolLib_DumpClassInfo(DumpClassInfoF _Dumper) {
    MetaClassDescription* clazz = TelltaleToolLib_GetFirstMetaClassDescription();
    for (int i = 0; i < TelltaleToolLib_GetMetaTypesCount(); i++) {
        _Dumper("Class: %s\n", clazz->mpTypeInfoName,NULL);
        if (clazz->mpFirstMember) {
            printMembers(1, clazz->mpFirstMember,_Dumper);
        }
        TelltaleToolLib_GetNextMetaClassDescription(&clazz);
        _Dumper("\n\n",NULL,NULL);
    }
}

_TTToolLib_Exp TTArchive2* TelltaleToolLib_CreateArchive2Instance(){
    return new TTArchive2;
}

_TTToolLib_Exp void TelltaleToolLib_DeleteArchive2Instance(TTArchive2* pArchive){
    delete pArchive;
}

_TTToolLib_Exp bool TelltaleToolLib_OpenArchive2(TTArchive2* pArchive, DataStream* pOwnershipGrantedStream){
    if (pArchive&&pOwnershipGrantedStream) {
        pArchive->Activate(pOwnershipGrantedStream);
        return pArchive->mbActive;
    }
    else return false;
}

_TTToolLib_Exp bool TelltaleToolLib_Archive2HasResource(TTArchive2* pArchive, unsigned long long hash){
    return pArchive->HasResource(hash);
}

_TTToolLib_Exp DataStream* TelltaleToolLib_OpenArchive2Resource(TTArchive2* pArchive, unsigned long long hash){
    if (!pArchive)
        return 0;
    for(auto it = pArchive->mResources.begin(); it != pArchive->mResources.end(); it++){
        if (it->mNameCRC == hash)
            return pArchive->GetResourceStream(&(*it));
    }
    TelltaleToolLib_RaiseError("At OpenArchive2Resource could not find file by given hash in archive", ERR);
    return 0;
}

_TTToolLib_Exp int TelltaleToolLib_NumArchive2Resources(TTArchive2* pArchive){
    return pArchive ?(int) pArchive->mResources.size() : 0;
}

_TTToolLib_Exp const char* TelltaleToolLib_GetArchive2ResourceName(TTArchive2* pArchive, unsigned long long hash){
    String name;
    if (!pArchive)
        return 0;
    name = pArchive->GetResourceName(hash);
    memcpy(TelltaleToolLib_Alloc_GetFixed1024ByteStringBuffer(), name.c_str(), name.length() + 1);
    return TelltaleToolLib_Alloc_GetFixed1024ByteStringBuffer();
}

_TTToolLib_Exp unsigned long long TelltaleToolLib_GetArchive2Resource(TTArchive2* pArchive, int index){
    return pArchive->mResources[index].mNameCRC;
}

_TTToolLib_Exp void* TelltaleToolLib_ReadDataStream(DataStream* pReadStream, unsigned long* p){
    if (pReadStream) {
        if(!pReadStream->IsRead()){
			TelltaleToolLib_RaiseError("Cannot read data stream: data stream is not a readable stream", ERR);
			return nullptr;
        }
        unsigned long long rem = pReadStream->GetSize() - pReadStream->GetPosition();
        if(rem > 0xFFFFFFFFL){
            TelltaleToolLib_RaiseError("Cannot read data stream: remaining bytes is larger than 2GB (and larger than uint32 max)", ERR);
            return nullptr;
        }
        void* pBuf = TelltaleToolLib_Malloc((unsigned long)rem);
        if(!pReadStream->Serialize((char*)pBuf, rem)){
			TelltaleToolLib_RaiseError("Cannot read data stream: data stream serialize failed", ERR);
            TelltaleToolLib_Dealloc(pBuf);
			return nullptr;
        }
        *p = (unsigned long)rem;
        return pBuf;
    }
    else return nullptr;
}

_TTToolLib_Exp void TelltaleToolLib_WriteDataStream(DataStream* pOutStream, void* pBuffer, unsigned long size){
    if (pOutStream&&pBuffer&&size&&pOutStream->IsWrite()) {
        pOutStream->Serialize((char*)pBuffer, size);
    }else{
		TelltaleToolLib_RaiseError("Cannot write data stream: invalid parameters or stream not writable", ERR);
    }
}

_TTToolLib_Exp unsigned long long TelltaleToolLib_CRC64CaseInsensitive(const char* pBuf, unsigned long long initialCRC){
    return CRC64_CaseInsensitive(initialCRC, pBuf);
}

#define __T3EFFECTUSER_IMPL_DEFAULTS
#include "T3/Render.hpp"
#include "T3/T3EffectUser.h"