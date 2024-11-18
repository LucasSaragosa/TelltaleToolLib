#include "T3EffectCache.h"
#include "../TelltaleToolLibrary.h"
#include <filesystem>
#include "../TTArchive2.hpp"

static std::vector<T3EffectCacheVersionDatabase*> sLoadedDatabases{};

static u32 _ReadInt(DataStream& stream){
	char buf[4];
	stream.Serialize(buf, 4);
	return *((u32*)buf);
}

static u16 _ReadInt2(DataStream& stream) {
	char buf[2];
	stream.Serialize(buf, 2);
	return *((u16*)buf);
}

static u8 _ReadInt1(DataStream& stream) {
	char buf[1];
	stream.Serialize(buf, 1);
	return *((u8*)buf);
}

static const char* _ReadString(T3EffectCacheVersionDatabase& database, DataStream& stream){
	u32 len = _ReadInt(stream);
	if (len > 1000)
		return "<SerializationError>";
	char* buf = database.mStringsHeap.Alloc(len + 1, 1);
	stream.Serialize(buf, len);
	buf[len] = 0;
	return buf;
}

//load the .BIN
static bool LoadEffectVersionDB(const char* gameID,T3EffectCacheVersionDatabase& database, DataStream& db)
{
	u32 magic;
	if((magic=_ReadInt(db)) != 0x01020304){
		TTL_Log("Effect cache database magic is invalid: 0x%X", magic);
		return false;
	}
	u32 numWrittenQualities{}, numQualitiesStored{}, numQualityEnums{}, numEffects{}, numFeats{};


	//tried to fix then made a mistake. cba to fix all the generators and stuff for prev files/games so just accept this ugly stuff for older games.
	//qualities stored is used in the loop for each effect type, num written is the number of qualities written in the first loop below, num enums how many in the exe
	numQualityEnums = _ReadInt2(db);
	numQualitiesStored = _ReadInt2(db);
	if (numQualitiesStored == 0)
		numQualitiesStored = numQualityEnums;//fix
	if(numQualitiesStored > 0x8000){
		//patch for newer DBs
		numQualitiesStored &= 0x7FFF;
		numWrittenQualities = numQualitiesStored;
	}else{
		//older DBs (newer games)
		numWrittenQualities = numQualityEnums;//should always be 5
	}


	numEffects = _ReadInt(db);
	numFeats = _ReadInt(db);
	database.mGameID = gameID;
	database.mFirstStaticFeature = 0;
	database.mFirstDynamicFeature = _ReadInt(db);
	database.mStaticFeaturesCount = database.mFirstDynamicFeature;
	database.mDynamicFeaturesCount = numFeats - database.mFirstDynamicFeature;
	database.mQualities.reserve(numQualityEnums);
	database.mEffects.reserve(numEffects);
	database.mFeatures.reserve(numFeats);
	for(u32 i = 0; i < numWrittenQualities; i++){
		T3EffectQualityDesc& desc = database.mQualities.emplace_back();
		desc.mName = _ReadString(database,db);
		desc.mSuffix = _ReadString(database, db);
	}
	for(u32 i = 0; i < numEffects; i++){
		T3EffectDesc& desc = database.mEffects.emplace_back();
		desc.mValidFeatures.SetTether(&database);
		desc.mName = _ReadString(database, db);
		desc.mFXFileName = _ReadString(database, db);
		desc.mMacroList = _ReadString(database, db);
		std::string mac = desc.mMacroList;
		if (mac.length() > 0)
			desc.mFullName = std::string(desc.mName) + " [" + mac + "]";
		else
			desc.mFullName = desc.mName;
 		desc.mCompilerFlags.mWords[0] = _ReadInt1(db);
		desc.mbToolOnlyEffect = _ReadInt1(db);
		desc.mCategory = (T3EffectCategoryType)_ReadInt1(db);
		desc.mbRequiresMaterial = _ReadInt1(db);
		desc.mExcludedQuality.mWords[0] = (u32)_ReadInt1(db);
		desc.mExcludedPlatforms.mWords[0] = (u32)_ReadInt2(db);
		desc.mStateHash = _ReadInt(db);
 		for (int x = 0; x < 3; x++)
			desc.mValidFeatures.mWords[x] = _ReadInt(db);
		for (int x = 0; x < numQualitiesStored; x++) {
			desc.mFeatures[x].mValidDynamicFeatures.SetTether(&database);
			desc.mFeatures[x].mValidStaticFeatures.SetTether(&database);
			desc.mFeatures[x].mValidFeaturesForQuality.SetTether(&database);
			desc.mFeatures[x].mValidFeaturesForQuality.mWords[0] = _ReadInt(db);
			desc.mFeatures[x].mValidFeaturesForQuality.mWords[1] = _ReadInt(db);
			desc.mFeatures[x].mValidFeaturesForQuality.mWords[2] = _ReadInt(db);
			desc.mFeatures[x].mValidStaticFeatures.mWords[0] = _ReadInt(db);
			desc.mFeatures[x].mValidStaticFeatures.mWords[1] = _ReadInt(db);
			desc.mFeatures[x].mValidDynamicFeatures.mWords[0] = _ReadInt(db);
		}
		//rem
		for(u32 x = numQualitiesStored; x < numQualityEnums; x++){
			if (x)
				desc.mFeatures[x] = desc.mFeatures[numQualitiesStored - 1];//any not stored just use lowest quality
		}
	}
	for(u32 i = 0; i < numFeats; i++){
		T3EffectFeatureDesc& desc = database.mFeatures.emplace_back();
		desc.mName = _ReadString(database, db);
		desc.mTag = _ReadString(database, db);
		desc.mSuffix = _ReadString(database, db);
		desc.mbDynamic = _ReadInt1(db);
		desc.mbToolOnly = _ReadInt1(db);
		desc.mExcludeQuality.mWords[0] = _ReadInt1(db);
		desc.mExcludePlatforms.mWords[0] = _ReadInt2(db);
		desc.mRequiredStaticFeatures.mWords[0] = _ReadInt(db);
		desc.mRequiredStaticFeatures.mWords[1] = _ReadInt(db);

		desc.mRequiredStaticFeatures.SetTether(&database);
		desc.mIncompatibleStaticFeatures.SetTether(&database);
		desc.mIncludeDynamicFeatures.SetTether(&database);
		desc.mRequiredDynamicFeaturesAny.SetTether(&database);

		desc.mIncompatibleStaticFeatures.mWords[0] = _ReadInt(db);
		desc.mIncompatibleStaticFeatures.mWords[1] = _ReadInt(db);
		desc.mIncludeDynamicFeatures.mWords[0] = _ReadInt(db);
		desc.mRequiredDynamicFeaturesAny.mWords[0] = _ReadInt(db);
	}
	return true;
}

void _T3EffectCache_Free(){
	for (auto& it : sLoadedDatabases)
		delete it;
	sLoadedDatabases.clear();
}

_TTToolLib_Exp void TelltaleToolLib_LoadEffectCaches(const char* pFolder) {
	if (!pFolder || !(*pFolder))
		return;
	std::filesystem::path folderPath{ pFolder };
	std::string f = folderPath.string();
	if (!(f.at(f.length() - 1) == '/' || f.at(f.length() - 1) == '\\'))
		folderPath += "/";
	for (auto& it : std::filesystem::directory_iterator{ folderPath }) {
		if (it.is_regular_file() && starts_with("T3EffectCache_",it.path().filename().string().c_str())) {
			size_t lastindex = it.path().filename().string().find_last_of(".");
			std::string rawname = it.path().filename().string().substr(14, lastindex-14);
			int index = TelltaleToolLib_GetGameKeyIndex(rawname.c_str());
			if (index == -1) {
				TTL_Log("Found an effect cache bin with a name that doesnt belong to a valid game ID! Ignoring: %s", rawname.c_str());
				continue;
			}
			else {
				int x = 0;
				for (auto& pDB : sLoadedDatabases) {
					if (pDB->mGameID == rawname){
						delete pDB;
						auto it = sLoadedDatabases.begin();
						std::advance(it, x);
						sLoadedDatabases.erase(it);
						break;
					}
					x++;
				}
				DataStreamFile_PlatformSpecific in = _OpenDataStreamFromDisc_(it.path().string().c_str(), READ);
				T3EffectCacheVersionDatabase* pDB = new T3EffectCacheVersionDatabase();
				sLoadedDatabases.push_back(pDB);
				if(LoadEffectVersionDB(rawname.c_str(),*pDB, in))
					TTL_Log("Loaded effect cache bin cache %s", rawname.c_str());
				else{
					TTL_Log("Failed to load effect cache bin cache %s", rawname.c_str());
					delete pDB;
					sLoadedDatabases.pop_back();
				}
			}
				
		}
	}
}

/**
 * Loads the effect cache enumerations BIN files from the given archive
 */
_TTToolLib_Exp void TelltaleToolLib_LoadEffectCachesFromArchive(TTArchive2& archive) {
	if (&archive == 0)
		return;
	for (int i = 0; i < KEY_COUNT; i++) {
		Symbol versName = "T3EffectCache_";
		versName += sBlowfishKeys[i].game_id;
		versName += ".bin";
		TTArchive2::ResourceEntry* pEntry = archive._FindResource(versName);
		if (pEntry) {
			int x = 0;
			for (auto& pDB : sLoadedDatabases) {
				if (pDB->mGameID == sBlowfishKeys[i].game_id) {
					delete pDB;
					auto it = sLoadedDatabases.begin();
					std::advance(it, x);
					sLoadedDatabases.erase(it);
					break;
				}
				x++;
			}
			int index = i;
			DataStream* pStream = archive.GetResourceStream(pEntry);
			if (!pStream || pStream->IsInvalid()) {
				TTL_Log("WARN: Could not obtain stream for effect cache DB: T3EffectCache_%s.bin", sBlowfishKeys[i].game_id);
			}
			else {
				T3EffectCacheVersionDatabase* pDB = new T3EffectCacheVersionDatabase();
				sLoadedDatabases.push_back(pDB);
				if (!LoadEffectVersionDB(sBlowfishKeys[i].game_id, *pDB, *pStream)) {
					TTL_Log("WARN: Could not read in effect database: T3EffectCache_%s.bin! Contact me", sBlowfishKeys[i].game_id);
					delete pDB;
					sLoadedDatabases.pop_back();
				}
				else {
					TTL_Log("Loaded effect database: T3EffectCache_%s.bin! Contact me", sBlowfishKeys[i].game_id);
				}
			}
		}
	}
}

_TTToolLib_Exp T3EffectCacheVersionDatabase* TelltaleToolLib_GetEffectCacheDB(const char* pGameID) {
	String id = pGameID;
	for(auto& pDB : sLoadedDatabases){
		if (pDB->mGameID == id)
			return pDB;
	}
	return 0;
}