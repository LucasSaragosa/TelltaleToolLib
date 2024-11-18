// This file was written by Lucas Saragosa. The code derives from Telltale Games' Engine.
// I do not intend to take credit for it, however; Im the author of this interpretation of 
// the engine and require that if you use this code or library, you give credit to me and
// the amazing Telltale Games.

#ifndef _D3DMESH
#define _D3DMESH

#include "../Meta.hpp"
#include "DCArray.h"
#include "HandleObjectInfo.h"
#include "T3Texture.h"
#include "Map.h"
#include "ObjectSpaceIntrinsics.h"
#include "ToolProps.h"
#include "ParticleSprite.h"
#include "../T3/T3EffectCache.h"
#include "../Symbols.h"

#include <memory>
#include <cmath>

/* template for funcs
static METAOP_FUNC_IMPL__(SerializeAsync) {
		CAST_METAOP(T, occ);
		MetaOpResult r = Meta::MetaOperation_SerializeAsync(pObj, pObjDescription, pContextDescription, pUserData);
		if (r == eMetaOp_Succeed) {

		}
		return r;
	}
*/

struct MeshSceneEnlightenData {
	Symbol mSystemName;
	Vector4 mUVTransform;
	Flags mFlags;
};

struct MeshSceneLightmapData {

	struct Entry {
		Symbol mMeshName;
		unsigned long mLODIndex;
		long mLightQuality;
		Vector2 mTextureScale;
		Vector2 mTextureOffset;
		unsigned long mTexturePage;
	};

	DCArray<Entry> mEntries;
	DCArray<unsigned short> mStationaryLightIndices;
	Flags mFlags;

};

struct T3MaterialParameter {
	Symbol mName;
	T3MaterialPropertyType mPropertyType;
	T3MaterialValueType mValueType;
	//in WDM its mscalaroffset[2]
	long mFlags, mScalarOffset[4];//in GoG and above its just mScalarOffset, not an array
	long mPreShaderScalarOffset, mNestedMaterialIndex;
};

struct T3MaterialEnlightenPrecomputeParams {
	float mIndirectReflectivity;
	float mIndirectTransparency;
	T3MaterialEnlightenPrecomputeParams() : mIndirectReflectivity(1.0f) {}
};

struct T3MaterialTexture {
	Symbol mName, mTextureName, mTextureNameWithoutExtension;
	T3TextureLayout mLayout;
	T3MaterialPropertyType mPropertyType;//wd4+
	BitSetBase<1> mTextureTypes;
	long mFirstParamIndex, mParamCount, mTextureIndex, mNestedMaterialIndex;
};

struct T3MaterialTransform2D {
	Symbol mParameterPrefix;
	long mFlags, mScalarOffset0[4], mScalarOffset1[4], mPreShaderScalarOffset0, mPreShaderScalarOffset1;//not arrays in games newer GoG (inc)
	long mNestedMaterialIndex;//mflags only in GoG and above
};

struct T3MaterialNestedMaterial {
	Handle<PropertySet> mhMaterial;//material_x.prop
};

struct T3MaterialPreShader {
	T3MaterialValueType mValueType;
	long mFlags, mPreShaderOffset, mScalarParameterOffset[4];//flags newer and inc GoG
};

struct T3MaterialStaticParameter {
	Symbol mName;
	long mNestedMaterialIndex;
};

struct T3MaterialTextureParam {
	T3MaterialTextureParamType mParamType;
	T3MaterialValueType mValueType;
	long mFlags, mScalarOffset;
};

struct T3MaterialPassData {
	T3MaterialPassType mPassType;
	BlendMode mBlendMode;
	u64 mMaterialCrc;
};

struct T3MaterialRuntimeProperty {
	Symbol mName, mRuntimeName;
};

enum T3MaterialCompiledDataFlags
{
	eMaterialCompiledDataFlag_HasBufferMaterialBase = 0x1,
	eMaterialCompiledDataFlag_HasBufferMaterialMain = 0x2,
	eMaterialCompiledDataFlag_Optimized = 0x4,
};

struct T3MaterialCompiledData {
	DCArray<T3MaterialParameter> mParameters;
	DCArray<T3MaterialTexture> mTextures;
	DCArray<T3MaterialTransform2D> mTransforms;
	DCArray<T3MaterialNestedMaterial> mNestedMaterials;
	DCArray<T3MaterialPreShader> mPreShaders;
	DCArray<T3MaterialStaticParameter> mStaticParameters;
	DCArray<T3MaterialTextureParam> mTextureParams;//TODO CHECK WD3!
	DCArray<T3MaterialPassData> mPasses;
	T3MaterialQualityType mMaterialQuality;//>=wd4
	BitSetBase<1> mMaterialBlendModes, mMaterialPasses;
	BitSet<T3MaterialChannelType, 46, 0> mMaterialChannels;//>=wd3
	BitSet<T3MaterialChannelType, 32, 0> mMaterialChannels2;//WDM
	BitSetBase<3> mShaderInputs;//>=wd4
	BitSetBase<2> mShaderInputs2;//bat2>=
	BitSetBase<1> mShaderInputs3;//WDM
	BitSetBase<1> mSceneTextures, mOptionalPropertyTypes;//scene textures >=wd4
	BinaryBuffer mPreShaderBuffer;
	Flags mFlags;
	int mPropertyParameterIndex[31];
	int mTexturePropertyParameterIndex[1];
	unsigned int mParameterBufferScalarSize[4];//size 2 array in games newer and including GoGs GoG
	unsigned int mPreShaderParameterBufferScalarSize;

};

enum T3MaterialDataFlags
{
	eMaterialDataFlag_Compiled = 0x1,
	eMaterialDataFlag_DoubleSidedNormals = 0x2,
	eMaterialDataFlag_Flattened = 0x4,
};

struct T3MaterialData {
	Symbol mMaterialName, mRuntimePropertiesName, mLegacyRenderTextureProperty;
	Symbol mLegacyBlendModeRuntimeProperty;
	T3MaterialDomainType mDomain;//wd4+
	LightType mLightType;//bat2 and below
	DCArray<T3MaterialRuntimeProperty> mRuntimeProperties;
	Flags mFlags, mRuntimeFlags;//runtimeflags not serialized
	long mVersion;
	float mMaxDistance;//bat2 and below, not sure if its a float/int always 0
	DCArray<T3MaterialCompiledData> mCompiledData2;

	static METAOP_FUNC_IMPL__(SerializeAsync) {
		CAST_METAOP(T3MaterialData, data);
		static i32 wd4 = -1;
		MetaOpResult r = Meta::MetaOperation_SerializeAsync(pObj, pObjDescription, pContextDescription, pUserData);
		if (r != eMetaOp_Succeed)
			return r;
		if (wd4 == -1)
			wd4 = TelltaleToolLib_GetGameKeyIndex("WD4");
		if (sSetKeyIndex >= wd4) {
			//r = PerformMetaSerializeAsync<DCArray<T3MaterialCompiledData>>(meta, &data->mCompiledData2);
			//if (r != eMetaOp_Succeed)
			//	return r;
		}
		else {
			u32 num = data->mCompiledData2.GetSize();
			meta->serialize_uint32(&num);
			if (meta->IsRead()) {
				if (data->mCompiledData2.mpStorage)
					delete[] data->mCompiledData2.mpStorage;
				data->mCompiledData2.mpStorage = new T3MaterialCompiledData[num];
				data->mCompiledData2.mSize = num;
				data->mCompiledData2.mCapacity = num;
			}
			u32 index = 0;
			for (u32 i = 0; i < num; i++, index++) {
				meta->serialize_uint32(&index);
				if (index >= num) {
					TelltaleToolLib_RaiseError("Material index was larger than number of materials", ErrorSeverity::ERR);
					return eMetaOp_Fail;
				}
				r=PerformMetaSerializeAsync<T3MaterialCompiledData>(meta, data->mCompiledData2.mpStorage + index);
				if (r != eMetaOp_Succeed)
					return r;
			}
		}
		return r;
	}

};

struct T3MeshMaterial {
	Handle<PropertySet> mhMaterial;/*points to internal resource inside mesh data*/
	Symbol mBaseMaterialName, mLegacyRenderTextureProperty;//legacy >WDM excl
	BoundingBox mBoundingBox;
	Sphere mBoundingSphere;
	Flags mFlags;
};

struct T3MeshTextureIndices {

	int mIndex[eMeshBatchType_Count];

	inline T3MeshTextureIndices(){
		mIndex[eMeshBatchType_Default] = -1;
		mIndex[eMeshBatchType_Shadow] = -1;
	}

	static METAOP_FUNC_IMPL__(SerializeAsync) {
		CAST_METAOP(T3MeshTextureIndices, tex);
		if (meta->IsWrite()) {
			for (u32 i = 0; i < 2; i++) {
				u32 index = tex->mIndex[i];
				if ((index & 0x80000000) == 0) {
					meta->serialize_uint32(&i);
					meta->serialize_uint32(&index);
				}
			}
			u32 i = (u32)-1;
			meta->serialize_uint32(&i);
		}
		else {
			u32 x;
			u32 i = (u32)-1;
			meta->serialize_uint32(&i);
			while ((i & 0x80000000) == 0) {
				meta->serialize_uint32(&x);
				if (2 > i)
					tex->mIndex[i] = x;
				meta->serialize_uint32(&i);
			}
		}
		return eMetaOp_Succeed;
	}

};

struct T3MeshPropertyEntry {
	String mIncludeFilter, mExcludeFilter;
	Handle<PropertySet> mhProperties;
	long mPriority;
};

struct T3MeshBatch {
	BoundingBox mBoundingBox;
	Sphere mBoundingSphere;
	Flags mBatchUsage;
	long mMinVertIndex, mMaxVertIndex, mBaseIndex, mStartIndex, mNumPrimitives, mNumIndices;//base index and indices in GoG and above
	T3MeshTextureIndices mTextureIndices;
	long mMaterialIndex, mAdjacencyStartIndex;//adjancency games newer (excl) than WDM
	int mLocalTransformIndex, mBonePaletteIndex;//wd3 and below
};

struct T3MeshLOD {
	DCArray<T3MeshBatch> mBatches[eMeshBatchType_Count];//0 is default, 1 is shadow batch
	DCArray<T3MeshBatch> mBatchesM;//WDM
	BitSetBase<1> mVertexStreams;
	BoundingBox mBoundingBox;
	Sphere mBoundingSphere;
	Flags mFlags;//>=wd4
	//primitives,batches not in WDM
	//vertex state index, index into T3MeshData::mVertexStates (buffers)
	//vertex start,count, atlas width and height only in wd4 or higher
	long mVertexStateIndex, mNumPrimitives, mNumBatches, mVertexStart, mVertexCount, mTextureAtlasWidth, mTextureAtlasHeight;
	//distance only in wd4 or higher
	float mPixelSize, mDistance;
	DCArray<Symbol> mBones;
	//default serializer
};

struct T3MeshTexture {
	T3MeshTextureType mTextureType;
	Handle<T3Texture> mhTexture;
	Symbol mNameSymbol;
	BoundingBox mBoundingBox;
	Sphere mBoundingSphere;
	float mMaxObjAreaPerUVArea;
	float mAverageObjAreaPerUVArea;
};

struct T3MeshMaterialOverride {
	Handle<PropertySet> mhOverrideMaterial;
	long mMaterialIndex;
};

struct T3MeshBoneEntry {
	Symbol mBoneName;//use hash db
	BoundingBox mBoundingBox;
	Sphere mBoundingSphere;
	long mNumVerts;
};

struct T3MeshLocalTransformEntry {
	Transform mTransform;
	T3CameraFacingType mCameraFacingType;
};

struct T3MaterialRequirements {
	BitSetBase<1> mPasses;
	BitSet<T3MaterialChannelType, 46, 0> mChannels;
	BitSet<T3MaterialChannelType, 32, 0> mChannels2;//WDM>=
	BitSetBase<3> mInputs;//>=wd4
	BitSetBase<2> mInputs2;//BAT>=
	BitSetBase<1> mInputs3;//WDM
};

struct T3MeshEffectPreloadDynamicFeatures {
	BitSetBase<1> mDynamicFeatures;
	long mPriority;
};

struct T3MeshEffectPreloadEntry {
	T3EffectType mEffectType;
	BitSetBase<3> mStaticEffectFeatures;
	u64 mMaterialCRC;
	DCArray<T3MeshEffectPreloadDynamicFeatures> mDynamicEffectFeatures;
};

struct T3MeshEffectPreload {
	long mEffectQuality;
	DCArray<T3MeshEffectPreloadEntry> mEntries;
	long mTotalEffectCount;
};

struct T3MeshCPUSkinningData {
	GFXPlatformFormat mPositionFormat, mNormalFormat, mWeightFormat;
	BitSetBase<1> mVertexStreams;
	long mNormalCount, mWeightOffset, mVertexSize, mWeightSize;//weight off/size >= wd4
	BinaryBuffer mData;
};

struct T3MeshData {

	enum MeshFlags {
		eDeformable = 1,
		eHasLocalCameraFacing = 2,
		eHasLocalCameraFacingY = 4,
		eHasLocalCameraFacingLocalY = 8,
		eHasCPUSkinning = 0x10,
		eHasComputeSkinning = 0x20,
	};

	DCArray<T3MeshLOD> mLODs;
	DCArray<T3MeshTexture> mTextures;
	DCArray<T3MeshMaterial> mMaterials;
	DCArray<T3MeshMaterialOverride> mMaterialOverrides;
	DCArray<T3MeshBoneEntry> mBones;
	DCArray<T3MeshLocalTransformEntry> mLocalTransforms;
	DCArray<T3GFXVertexState*> mVertexStates;
	T3MaterialRequirements mMaterialRequirements;
	BitSetBase<1> mVertexStreams;
	T3MeshCPUSkinningData* mpCPUSkinningData;
	BoundingBox mBoundingBox;
	Sphere mBoundingSphere;
	T3MeshEndianType mEndianType;
	Vector3 mPositionScale, mPositionWScale, mPositionOffset;//all three in GoG and above
	float mLightmapTexelAreaPerSurfaceArea;
	Symbol mPropertyKeyBase;//all keys in internal resources concat onto this crc (crc with this as the start CRC64). very anoying.
	long mVertexCount;
	Flags mFlags;
	DCArray<T3MeshEffectPreload> mMeshPreload;//>=wd4
	T3MeshTexCoordTransform mTexCoordTransform[4];

	T3MeshData() {
		mpCPUSkinningData = NULL;
	}

	~T3MeshData() {
		for (int i = 0; i < mVertexStates.GetSize(); i++)
			delete mVertexStates[i];
		mVertexStates.ClearElements();
		if (mpCPUSkinningData)
			delete mpCPUSkinningData;
	}

	static METAOP_FUNC_IMPL__(SerializeAsync) {
		CAST_METAOP(T3MeshData,data);
		MetaOpResult r = Meta::MetaOperation_SerializeAsync(pObj, pObjDescription, pContextDescription, pUserData);
		if (r == eMetaOp_Succeed) {
			if (meta->IsWrite()) {
				u32 x[4];
				u32 z = 0;
				memset(x, 0, 0x10);
				for (int y = 0; y < 4; y++) {
					if (1.0f != data->mTexCoordTransform[y].mScale.x || 1.0f != data->mTexCoordTransform[y].mScale.y
						|| 0.0f != data->mTexCoordTransform[y].mOffset.x || 0.0f != data->mTexCoordTransform[y].mOffset.y) {
						x[z++] = y;
					}
				}
				MetaClassDescription* desc = NULL;
				meta->serialize_uint32(&z);
				for (int i = 0; i < z; i++) {
					if (!desc) {
						desc = ::GetMetaClassDescription<T3MeshTexCoordTransform>();
						if (!desc) {
							TelltaleToolLib_RaiseError("Cant find tex coordinate transform metaclass", ErrorSeverity::ERR);
							return eMetaOp_Fail;
						}
					}
					meta->serialize_uint32(&x[i]);
					r = PerformMetaSerializeFull(meta, &data->mTexCoordTransform[x[i]], desc);
					if (r != eMetaOp_Succeed)
						return r;
				}
			}
			else {
				MetaClassDescription* desc = ::GetMetaClassDescription<T3MeshTexCoordTransform>();
				if (!desc) {
					TelltaleToolLib_RaiseError("Cant find tex coordinate transform metaclass", ErrorSeverity::ERR);
					return eMetaOp_Fail;
				}
				u32 transforms;
				meta->serialize_uint32(&transforms);
				u32 v;
				for (int i = 0; i < transforms; i++) {
					meta->serialize_uint32(&v);
					r = PerformMetaSerializeFull(meta, &data->mTexCoordTransform[v], desc);
					if (r != eMetaOp_Succeed)
						return r;
				}
				if (data->mFlags.mFlags & MeshFlags::eHasCPUSkinning) {
					if (data->mpCPUSkinningData && meta->IsRead())
						delete data->mpCPUSkinningData;
					if (meta->IsRead())
						data->mpCPUSkinningData = new T3MeshCPUSkinningData;
					r = PerformMetaSerializeAsync<T3MeshCPUSkinningData>(meta, data->mpCPUSkinningData);
					if (r != eMetaOp_Succeed)
						return r;
				}
			}
			u32 v = data->mVertexStates.GetSize();
			meta->serialize_uint32(&v);
			T3GFXVertexState* state;
			for (int i = 0; i < v; i++) {
				if (meta->IsRead()) {
					state = new T3GFXVertexState;
					if (!state)
						return eMetaOp_OutOfMemory;
					data->mVertexStates.AddElement(0, NULL, &state);
				}
				else {
					state = data->mVertexStates[i];
				}
				r = PerformMetaSerializeAsync<T3GFXVertexState>(meta, state);
				if (r != eMetaOp_Succeed)
					return r;
			}
		}
		return r;
	}

};

struct T3VertexSampleDataBase {
	//mAllocator, mRefCount
	int mNumVerts;
	int mVertSize;
	char* mpData;

	INLINE T3VertexSampleDataBase() : mpData(NULL) {}

	INLINE ~T3VertexSampleDataBase() {
		if (mpData)
			free(mpData);
	}

	static METAOP_FUNC_DEF(SerializeAsync);

};

struct T3OcclusionMeshBatch {
	Flags mFlags;
	long mStartIndex, mNumTriangles;
};

struct T3OcclusionMeshData {
	BinaryBuffer mData;
	BoundingBox mBoundingBox;
	Sphere mBoundingSphere;
	DCArray<T3OcclusionMeshBatch> mBatches;
	long mVertexCount;

	static METAOP_FUNC_IMPL__(SerializeAsync) {
		CAST_METAOP(T3OcclusionMeshData, occ);
		MetaOpResult r = Meta::MetaOperation_SerializeAsync(pObj, pObjDescription, pContextDescription, pUserData);
		if (r == eMetaOp_Succeed) {
			//... not needed
		}
		return r;
	}

};

//.D3DMESH FILES
struct D3DMesh {

	String mName;
	long mVersion;
	T3MeshData mMeshData;
	DCArray<HandleUncached> mInternalResources;
	ToolProps mToolProps;
	//below is only in games newer than WD WDM (exclusive)
	EnumRenderLightmapUVGenerationType mLightmapUVGenerationType;//bat2 and below
	float mLightmapTexelAreaPerSurfaceArea;//in games older and including bat2
	float mLightmapGlobalScale;
	long mLightmapTexCoordVersion;
	//MeshDebugRenderType mType;//bat2 and below,99%sure
	long mLightmapTextureWidth, mLightmapTextureHeight;//bat2 and below
	u64 mLODParamCRC;//wd4+
	T3OcclusionMeshData* mpOcclusionMeshData;

	inline D3DMesh() {
		mpOcclusionMeshData = NULL;
		mLightmapGlobalScale = 1.0f;
		mLightmapTextureWidth = mLightmapTextureHeight = 0;
	}

	//DEPRECATED. THIS IS WRONG. THIS GETS THE RUNTIME CACHED PROP SET NOT THE SERIALIZED ONE
	inline PropertySet* GetMeshProps() {
		String propsResource = "\"";
		propsResource.append(mName);
		propsResource.append("\" Mesh Properties");
		Symbol sym(propsResource.c_str());
		for (int i = 0; i < mInternalResources.GetSize(); i++) {
			HandleUncached& handle = mInternalResources[i];
			if (handle.GetObjectName().operator==(sym)) {
				return handle.GetValue<PropertySet>();
			}
			if (handle.GetTypeDesc() == ::GetMetaClassDescription<PropertySet>())
				return (PropertySet*)handle.GetHandleObjectPointer();
		}
		return NULL;
	}

	inline PropertySet* GetResourceMaterial(Symbol hMaterial){
		for (int i = 0; i < mInternalResources.GetSize(); i++) {
			HandleUncached& handle = mInternalResources[i];
			if (handle.GetObjectName().operator==(hMaterial)) {
				return handle.GetValue<PropertySet>();
			}
		}
		return NULL;
	}

	T3MaterialData* GetMaterialData(PropertySet* pMeshProps) {
		if (!pMeshProps)
			return NULL;
		return pMeshProps->GetProperty<T3MaterialData>(
			T3MaterialInternal::kPropKeyMaterialData);
	}

	~D3DMesh() {
		if (mpOcclusionMeshData)
			delete mpOcclusionMeshData;
		//for (int i = 0; i < mInternalResources.GetSize(); i++) {
		//	delete mInternalResources[i];
		//}
		mInternalResources.ClearElements();
	}

	static MetaOpResult SerializeAsyncInternalResources(MetaStream* meta, D3DMesh* mesh) {
		u32 res = mesh->mInternalResources.GetSize();
		MetaOpResult r = eMetaOp_Succeed;
		meta->serialize_uint32(&res);
		if (meta->IsRead()) {
			Symbol name{};
			MetaClassDescription* mcd = NULL;
			u64 symbolCrc = 0;
			for (int i = 0; i < res; i++) {
				meta->serialize_Symbol(&name);
				meta->serialize_uint64(&symbolCrc);
				mcd = TelltaleToolLib_FindMetaClassDescription_ByHash(symbolCrc);
				meta->BeginBlock();
				if (!mcd) {
					//normally raise a warn (telltale just ignore the resource) if its an invalid type
#ifdef DEBUGMODE
					TTL_Log("Mesh internal resource type not found from symbol %llX\n", symbolCrc);
#endif
					TelltaleToolLib_RaiseError("D3DMesh internal resource type not found", ErrorSeverity::ERR);
					return eMetaOp_Fail;
				}
				void* inst = mcd->New();
				if (!inst)
					return eMetaOp_OutOfMemory;
				//HandleUncached* handle = new HandleUncached(mcd, inst);
				//if (!handle) {
				//	mcd->Delete(inst);
				//	return eMetaOp_OutOfMemory;
				//}
				HandleUncached handle{ mcd, inst };
				handle.SetObjectName(name);
				r=PerformMetaSerializeFull(meta, handle.GetHandleObjectPointer(), handle.GetTypeDesc());
				if (r != eMetaOp_Succeed) {
					//delete handle;
					return r;
				}
				mesh->mInternalResources.AddElementMove(0, NULL, &handle);
				meta->EndBlock();
			}
		}
		else {
			for (int i = 0; i < res; i++) {
				meta->serialize_Symbol(&mesh->mInternalResources[i].mHandleObjectInfo.mObjectName);
				meta->serialize_uint64(&mesh->mInternalResources[i].GetTypeDesc()->mHash);
				meta->BeginBlock();
				r=PerformMetaSerializeFull(meta, mesh->mInternalResources[i].GetHandleObjectPointer(), 
					mesh->mInternalResources[i].GetTypeDesc());
				if (r != eMetaOp_Succeed)
					return r;
				meta->EndBlock();
			}
		}
		return eMetaOp_Succeed;
	}

#define ENDBATFIX() if(batfix)sSetKeyIndex = TelltaleToolLib_GetGameKeyIndex("BAT");

	static METAOP_FUNC_IMPL__(SerializeAsync) {
		CAST_METAOP(D3DMesh, mesh);
		bool batfix = false;
		if (false && (sSetKeyIndex == TelltaleToolLib_GetGameKeyIndex("BAT"))) {
			batfix = true;
			sSetKeyIndex = TelltaleToolLib_GetGameKeyIndex("WD4");
		}
		MetaOpResult r = Meta::MetaOperation_SerializeAsync(pObj, pObjDescription, pContextDescription, pUserData);
		if (r == eMetaOp_Succeed) {
			if (batfix && mesh->mVersion != 46) {
				TelltaleToolLib_RaiseError("Cannot serialize D3DMesh: Only BAT Season 1's latest PC release is supported",ErrorSeverity::ERR);
				ENDBATFIX();
				return eMetaOp_Fail;
			}
			if (mesh->mVersion >= 22) {
				r = SerializeAsyncInternalResources(meta, mesh);
				if (r != eMetaOp_Succeed) {
					ENDBATFIX();
					return r;
				}
			}
			//MCSM
			if (false) {

			}else{ 
				if (19 > mesh->mVersion) {
					//TODO not supported in WDC
					TelltaleToolLib_RaiseError("Cannot serialize D3DMeshes from games older and including MC: Story Mode... with D3DMesh. See LegacyD3DMesh!", ErrorSeverity::ERR);
					ENDBATFIX();
					return eMetaOp_Fail;
				}
				u32 hasToolData = 0;
				meta->serialize_uint32(&hasToolData);
				if (hasToolData) {
					meta->BeginBlock();
					TelltaleToolLib_RaiseError("Mesh contains tool data; "
						"skipping block (safely)",
						ErrorSeverity::WARN);
					meta->SkipToEndOfCurrentBlock();
					meta->EndBlock();
				}
				if (mesh->mVersion >= 52) {
					bool hasOcclusionData = mesh->mpOcclusionMeshData != NULL;
					meta->serialize_bool(&hasOcclusionData);
					if (hasOcclusionData) {
						meta->BeginBlock();
						if (!mesh->mpOcclusionMeshData)
							mesh->mpOcclusionMeshData = new T3OcclusionMeshData;
						r = PerformMetaSerializeAsync<T3OcclusionMeshData>(meta, mesh->mpOcclusionMeshData);
						if (r != eMetaOp_Succeed) {
							ENDBATFIX();
							return r;
						}
						meta->EndBlock();
					}
				}
				meta->BeginBlock();
				r = PerformMetaSerializeAsync<T3MeshData>(meta, &mesh->mMeshData);
				if (r != eMetaOp_Succeed) {
					ENDBATFIX();
					return r;
				}
				meta->EndBlock();
			}
		}
		ENDBATFIX();
		return r;
	}

};

inline int decompose_type_size(int comtype) {
	if (comtype == 1)
		return 4;
	else if (comtype == 2 || comtype == 3)
		return 1;
	else if (comtype == 4 || comtype == 5)
		return 2;
	else if (comtype == 11)
		return 2;//half float 16bit
	else {
		TTL_Log("wtf unknown type again\n");
	}
}

inline float decompose_to_float(void* data, GFXPlatformFormat fmt, int typeoff) {
	if (fmt == eGFXPlatformFormat_F32x2 || fmt == eGFXPlatformFormat_F32x4 || fmt == eGFXPlatformFormat_F32x3) {
		return ((float*)data)[typeoff];
	}
	else if (fmt == eGFXPlatformFormat_SN16 || fmt == eGFXPlatformFormat_SN16x2 || fmt == eGFXPlatformFormat_SN16x4) {
		i16* raw = (i16*)data;
		i16 val = raw[typeoff];
		float asf = (float)val;
		return asf / 32767.f;
	}
	else if (fmt == eGFXPlatformFormat_UN16 || fmt == eGFXPlatformFormat_UN16x2 || fmt == eGFXPlatformFormat_UN16x4) {
		u16* raw = (u16*)data;
		u16 val = raw[typeoff];
		float asf = (float)val;
		return asf / 65536.f;
	}
	else if (fmt == eGFXPlatformFormat_SN8 || fmt == eGFXPlatformFormat_SN8x2 || fmt == eGFXPlatformFormat_SN8x4) {
		i8* raw = (i8*)data;
		i8 val = raw[typeoff];
		float asf = (float)val;
		return asf / 127.f;
	}
	else if (fmt == eGFXPlatformFormat_UN8 || fmt == eGFXPlatformFormat_UN8x2 || fmt == eGFXPlatformFormat_UN8x4) {
		u8* raw = (u8*)data;
		u8 val = raw[typeoff];
		float asf = (float)val;
		return asf / 255.f;
	}
	else {
		TTL_Log("ERROR: Not supported to decompose platform format to float, PLEASE CONTACT ME WITH THIS MESH NAME AND GAME\n");
		return 0.f;
	}
}

inline float decompose_new_UV(void* data, GFXPlatformFormat fmt, int typeoff, T3MeshTexCoordTransform transform) {
	float asf = decompose_to_float(data, fmt, typeoff);
	asf *= transform.mScale.array[typeoff];
	asf += transform.mOffset.array[typeoff];
	if (typeoff == 1)
		asf = 1 - asf;
	bool bNeg = asf <= 0.f;
	asf = bNeg ? -asf : asf;
	if (asf >= 1.f && asf <= 1.01f)
		asf = 1.0f;
	if (asf > 1.0f)
		asf = asf - (float)((int)asf);
	if (bNeg)
		asf = 0.f;
	return asf;
}


//  Produce value of bit n.  n must be less than 32.
#define Bit(n)  ((uint32_t) 1 << (n))

//  Create a mask of n bits in the low bits.  n must be less than 32.
#define _BMask(n) (Bit(n) - 1)


/*  Convert an IEEE-754 16-bit binary floating-point encoding to an IEEE-754
	32-bit binary floating-point encoding.

	This code has not been tested.
*/
inline uint32_t Float16ToFloat32(uint16_t x)
{
	/*  Separate the sign encoding (1 bit starting at bit 15), the exponent
		encoding (5 bits starting at bit 10), and the primary significand
		(fraction) encoding (10 bits starting at bit 0).
	*/
	uint32_t s = x >> 15;
	uint32_t e = x >> 10 & _BMask(5);
	uint32_t f = x & _BMask(10);

	//  Left-adjust the significand field.
	f <<= 23 - 10;

	//  Switch to handle subnormal numbers, normal numbers, and infinities/NaNs.
	switch (e)
	{
		//  Exponent code is subnormal.
	case 0:
		//  Zero does need any changes, but subnormals need normalization.
		if (f != 0)
		{
			/*  Set the 32-bit exponent code corresponding to the 16-bit
				subnormal exponent.
			*/
			e = 1 + (127 - 15);

			/*  Normalize the significand by shifting until its leading
				bit moves out of the field.  (This code could benefit from
				a find-first-set instruction or possibly using a conversion
				from integer to floating-point to do the normalization.)
			*/
			while (f < Bit(23))
			{
				f <<= 1;
				e -= 1;
			}

			//  Remove the leading bit.
			f &= _BMask(23);
		}
		break;

		// Exponent code is normal.
	default:
		e += 127 - 15;  //  Adjust from 16-bit bias to 32-bit bias.
		break;

		//  Exponent code indicates infinity or NaN.
	case 31:
		e = 255;        //  Set 32-bit exponent code for infinity or NaN.
		break;
	}

	//  Assemble and return the 32-bit encoding.
	return s << 31 | e << 23 | f;
}

inline float decompose_normal_int(void* data, int comtype, int typeoff, bool big_endian/*for type 11 half float*/, bool is_uv = false) {
	data = ((char*)data + typeoff * decompose_type_size(comtype));
	//type: 1 = float, 2= i8, 3= u8, 4 = i16 5 = u16
	if (comtype == 1)
		return *((float*)data);
	else if (comtype == 2)
		return ((float)(*(i8*)data)) / 127.f;
	else if (comtype == 3)
		return ((float)(*(u8*)data)) / 255.f;
	else if (comtype == 4) // MOST USED
		return ((float)(*(i16*)data)) / 32767.f;
	else if (comtype == 5)
		return ((float)(*(u16*)data)) / 65536.f;
	else if (comtype == 11) {
		/*fn convertTo32 input16 = (
		inputAsInt = input16
		sign = bit.get inputAsInt 16
		exponent = (bit.shift (bit.and inputAsInt (bit.hexasint "7C00")) -10) as integer - 16
		fraction = bit.and inputAsInt (bit.hexasint "03FF")
		if sign==true then sign = 1 else sign = 0
		exponentF = exponent + 127
		outputAsFloat = bit.or (bit.or (bit.shift fraction 13) (bit.shift exponentF 23)) (bit.shift sign 31)
		return bit.intasfloat outputasfloat
	)
	fn readHalfFloat fstream = (
	return convertTo32(Readshort fstream)
	)*/
		u16 lo = *((u8*)data);
		u16 hi = *((u8*)data + 1);
		u16 raw = big_endian ? ((lo << 8) | hi) : ((hi << 8) | lo);
		uint32_t compare = Float16ToFloat32(raw);
		u32 sign = (raw & 0x8000) ? 1 : 0;
		int exp = ((raw >> 10) & 0x1F) - 16;//-16 because 0-31 is two way: bottom are negative exp sign
		u32 mantissa = raw >> 6;
		exp += 127;
		u32 raw_float = (sign << 31) | ((u32)exp << 23) | (mantissa);
		float asf = *((float*)&compare);
		if (std::fabsf(asf) < 0.01f)
			asf = 0;//sometimes this fucks up
		if (false && is_uv) {
			asf *= 2;//some reason its halfed??
			if (asf >= 1.f && asf <= 1.01f)
				asf = 1.0f;
			if (typeoff == 1)//the V in UV
				asf = 1 - asf;
		}
		return asf;
	}
	else {
		TTL_Log("WARNING: UNKNOWN VERTEX COMPONENT TYPE FORMAT!! RETURNING 0 !! \n");
		return 0.f;
	}
}

inline float decompose_normal(void* data, int comtype, int typeoff, bool big_endian/*for type 11 half float*/, bool is_uv = false) {
	float asf = decompose_normal_int(data, comtype, typeoff, big_endian, is_uv);
	if (asf < 0.0f)
		asf = -asf;
	if (asf > 1.0f)
		asf = asf - (float)((int)asf);//WTF
	return asf;
}

inline GFXPlatformAttributeParams* get_params(T3GFXVertexState& state, GFXPlatformVertexAttribute attrib) {
	for (int i = 0; i < state.mAttributeCount; i++) {
		if (state.mAttributes[i].mAttribute == attrib)
			return state.mAttributes + i;
	}
	return 0;
}

#endif