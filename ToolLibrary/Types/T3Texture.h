// This file was written by Lucas Saragosa. The code derives from Telltale Games' Engine.
// I do not intend to take credit for it, however; Im the author of this interpretation of 
// the engine and require that if you use this code or library, you give credit to me and
// the amazing Telltale Games.

#ifndef _D3DTX
#define _D3DTX

#include "ToolProps.h"
#include "../TelltaleToolLibrary.h"
#include "List.h"
#include "../Meta.hpp"
#include "ObjectSpaceIntrinsics.h"
#include "DCArray.h"
#include "../DataStream/DataStream.h"
#include "SArray.h"
#include "../T3/T3EffectCache.h"

struct RenderSwizzleParams {
	RenderSwizzleType mSwizzle[4];

	static METAOP_FUNC_IMPL__(SerializeAsync) {
		static int _ID = TelltaleToolLib_GetGameKeyIndex("BAT");
		RenderSwizzleParams* swiz = static_cast<RenderSwizzleParams*>(pObj);
		MetaStream* meta = static_cast<MetaStream*>(pUserData);
		u32 blockSize = 8;
		if (sSetKeyIndex >= _ID) {
			meta->serialize_uint32(&blockSize);
			if (blockSize != 8) {
				TelltaleToolLib_RaiseError("Swizzle block not correct size", ErrorSeverity::ERR);
				return eMetaOp_Fail;
			}
		}
		meta->serialize_int8((char*)&swiz->mSwizzle[0]);
		meta->serialize_int8((char*)&swiz->mSwizzle[1]);
		meta->serialize_int8((char*)&swiz->mSwizzle[2]);
		meta->serialize_int8((char*)&swiz->mSwizzle[3]);
		return eMetaOp_Succeed;
	}

};

//.D3DTX FILES
struct T3Texture : T3GFXResource {

	struct CreateParams
	{
		GFXPlatformAllocationType mAllocationType;
		T3TextureLayout mLayout;
		T3SurfaceFormat mFormat;
		T3SurfaceGamma mGamma;
		T3SurfaceMultisample mMultisample;
		T3ResourceUsage mUsage;
		T3SurfaceAccess mAccess;
		u16 mWidth;
		u16 mHeight;
		u16 mDepth;
		u16 mArraySize;
		u16 mNumMipLevels;
		u16 mbPrimaryDisplaySurface;
		bool mbVideoTexture;
		bool mbCPUAccessible;
		GFXPlatformFastMemHeap mFastMemHeap;
		Symbol mTag;
	};

	enum TextureType {
		eTxUnknown = 0x0,
		eTxLightmap_V0 = 0x1,
		eTxBumpmap = 0x2,
		eTxNormalMap = 0x3,
		eTxUNUSED1 = 0x4,
		eTxUNUSED0 = 0x5,
		eTxSubsurfaceScatteringMap_V0 = 0x6,
		eTxSubsurfaceScatteringMap = 0x7,
		eTxDetailMap = 0x8,
		eTxStaticShadowMap = 0x9,
		eTxLightmapHDR = 0xA,
		eTxSDFDetailMap = 0xB,
		eTxEnvMap = 0xC,
		eTxSpecularColor = 0xD,
		eTxToonLookup = 0xE,
		eTxStandard = 0xF,
		eTxOutlineDiscontinuity = 0x10,
		eTxLightmapHDRScaled = 0x11, 
		eTxEmissiveMap = 0x12, 
		eTxParticleProperties = 0x13,
		eTxBrushNormalMap = 0x14,
		eTxUNUSED2 = 0x15,
		eTxNormalGlossMap = 0x16,
		eTxLookup = 0x17,
		eTxAmbientOcclusion = 0x18,
		eTxPrefilteredEnvCubeMapHDR = 0x19,
		eTxBrushLookupMap = 0x1A,
		eTxVector2Map = 0x1B, 
		eTxNormalDxDyMap = 0x1C,
		eTxPackedSDFDetailMap = 0x1D,
		eTxSingleChannelSDFDetailMap = 0x1E,
		eTxLightmapDirection = 0x1F,
		eTxLightmapStaticShadows = 0x20,
		eTxLightStaticShadowMapAtlas = 0x21,
		eTxLightStaticShadowMap = 0x22,
		eTxPrefilteredEnvCubeMapHDRScaled = 0x23, 
		eTxLightStaticShadowVolume = 0x24,
		eTxLightmapAtlas = 0x25,
		eTxNormalXYMap = 0x26,
		eTxLightmapFlatAtlas = 0x27,
		eTxLookupXY = 0x28,
		eTxObjectNormalMap = 0x29
	};

	enum ColorMode {
		eTxColorFull = 0,
		eTxColorBlack = 1,
		eTxColorGrayscale = 2,
		eTxColorGrayscaleAlpha = 3,
		eTxColorUnknown = 0xFFFFFFFF
	};

	struct AuxilaryData {
		Symbol mType;
		BinaryBuffer mData;
	};

	struct RegionStreamHeader {
		int mFaceIndex;
		int mMipIndex;
		int mMipCount;
		int mDataSize;
		int mPitch;
		int mSlicePitch;
		DataStream* mpAsyncStream;//moved here, so its easier to set data

		RegionStreamHeader() : mpAsyncStream(NULL) {}

		~RegionStreamHeader() {
			if (mpAsyncStream)
				delete mpAsyncStream;
			mpAsyncStream = 0;
		}

	};

	enum AlphaMode {
		eTxNoAlpha = 0,
		eTxAlphaTest = 1,
		eTxAlphaBlend = 2,
		eTxAlphaUnkown = 0xFFFFFFFF//telltale spelt it wrong not me
	};

	long mVersion;
	T3SamplerStateBlock mSamplerState;
	EnumPlatformType mPlatform;
	String mName;
	String mImportName;
	float mImportScale;
	float mImportSpecularPower;//old games
	ToolProps mToolProps;
	long mNumMipLevels;
	long mWidth;
	long mHeight;
	long mDepth;
	long mArraySize;
	T3SurfaceFormat mSurfaceFormat;
	T3TextureLayout mTextureLayout;
	T3SurfaceGamma mSurfaceGamma;
	T3SurfaceMultisample mSurfaceMultisample;
	T3SurfaceAccess mSurfaceAccess;
	T3ResourceUsage mResourceUsage;
	TextureType mType;
	int mNormalMapFormat;//MCSM and below
	RenderSwizzleParams mSwizzle;//WDM and above
	float mSpecularGlossExponent;
	float mHDRLightmapScale;
	float mToonGradientCutoff;
	AlphaMode mAlphaMode;
	ColorMode mColorMode;
	Vector2 mUVOffset;
	Vector2 mUVScale;
	//next 4 are not serialized (has serialize disable set)
	int mNumMipLevelsAllocated;
	int mNumSurfacesRequested;
	int mNumSurfacesRequired;
	int mNumSurfacesLoaded;
	DCArray<Symbol> mArrayFrameNames;
	DCArray<T3ToonGradientRegion> mToonRegions;

	u64 mLastUsedFrame = 0;

	List<AuxilaryData*> mAuxilaryData;//should be linkedlist which uses pointers

	inline bool IsValidImmediate(){
		return mbResourceAssigned;
	}

	inline T3Texture() : T3GFXResource(eGFXPlatformResource_Texture) {
		mImportScale = 1.0f;
		mUVScale.x = 1.0f;
		mUVScale.x = 1.0f;
		mArraySize = 1;
		mDepth = 1;
		mTextureLayout = eTextureLayout_2D;
		mSurfaceFormat = T3SurfaceFormat::eSurface_Unknown;
		mSurfaceMultisample = T3SurfaceMultisample::eSurfaceMultisample_None;
		mSurfaceGamma = T3SurfaceGamma::sSurfaceGamma_Unknown;
		mUVOffset.x = 0;
		mUVOffset.y = 0;
		mToonGradientCutoff = -1.0f;
		mSpecularGlossExponent = 8.0f;
		mHDRLightmapScale = 6.0f;
		mSwizzle.mSwizzle[0] = RenderSwizzleType::eRenderSwizzle_R;
		mSwizzle.mSwizzle[1] = RenderSwizzleType::eRenderSwizzle_G;
		mSwizzle.mSwizzle[2] = RenderSwizzleType::eRenderSwizzle_B;
		mSwizzle.mSwizzle[3] = RenderSwizzleType::eRenderSwizzle_A;
		mAlphaMode = AlphaMode::eTxAlphaUnkown;
		mPlatform.mVal = PlatformType::ePlatform_PC;
		mType = TextureType::eTxStandard;
		mNumSurfacesLoaded = 0;
		mNumSurfacesRequested = 0;
		mNumSurfacesRequired = 0;
		mNumMipLevelsAllocated = 0;
		mWidth = mHeight = 1;
	}

	struct LockContext {
		//Put in the data yourself here
		u8* mpTextureData;
		//Size of pTextureData
		u32 mDataSize;
		//Pitch of this
		u32 mPitch;
		//Current mip
		u32 mMipIndex;
		//Current face
		u32 mFace;
		//Current mip width
		u32 mMipWidth;
		//Current mip height
		u32 mMipHeight;
	};

	//Lock the texture data and then unlock it. Ues this to upload data to the texture directly to GPU on the unlock call through the
	//render adapters struct. This is for static uploads (not streamed or anything - not using update lists).
	const LockContext Lock(u32 mip, u32 face);

	void Unlock(const LockContext& context);

	void CreateSolidTexture(Color color, T3TextureLayout layout);

	void CreateHorizontalGradientTexture(Color startColor, Color endColor, int numSteps);

	bool CreateRenderTarget(u32 width, u32 height, T3SurfaceFormat format, bool bPrimaryDisplaySurface, bool bVideoTexture);

	//Returns number of surfaces in texture
	u32 CreateTexture(CreateParams& params);

	void SetUsedOnFrame(RenderFrameUpdateList& updateList);

	bool IsCompressed() {
		int v1 = (int)mSurfaceFormat;
		return (unsigned int)(v1 - 64) <= 5
			|| v1 >= 64 && (v1 <= 68 || (unsigned int)(v1 - 70) <= 1)
			|| (unsigned int)(v1 - 80) <= 3
			|| v1 == 128;
	}

	inline int GetNumSlices() {
		int result = 0;
		if (mTextureLayout == T3TextureLayout::eTextureLayout_3D) {
			result = mDepth;
		}
		else if ((unsigned int)(mTextureLayout - 3) > 1) {
			result = 1;
		}
		else {
			result = mArraySize;
		}
		return result;
	}

	inline int GetNumFaces() {
		int result = 0;
		if (mTextureLayout == T3TextureLayout::eTextureLayout_CubeArray) {
			result = 6 * mArraySize;
		}
		else if (mTextureLayout == T3TextureLayout::eTextureLayout_2DArray) {
			result = mArraySize * 1;
		}
		else {
			result = mTextureLayout == T3TextureLayout::eTextureLayout_Cube ? 6 : 1;
		}
		return result;
	}

	/// <summary>
	/// Gets texture data from this texture. The mip index is index of the mipmap texture data to retrieve (between 0 and mNumMipLevels-1)
	/// ArrayIndex is the index of the region in the mipmap of texture data to get. In a cube map, this is an index between 0 and 5.
	/// The mArraySize member variable tells you how many regions each mipmap has (in a cube map, its 6, in a standard its 1).
	/// This will return NULL if the indicies passed are invalid or there is no loaded texture data.
	/// </summary>
	DataStream* GetTxData(int pMipIndex, int pArrayIndex = 0) {
		if (pMipIndex >= mNumMipLevels || pArrayIndex >= mArraySize)
			return NULL;
		for (int i = 0; i < mRegionHeaders.GetSize(); i++) {
			RegionStreamHeader* header = mRegionHeaders.mpStorage + i;
			if (header->mMipIndex == pMipIndex && header->mFaceIndex == pArrayIndex) {
				return header->mpAsyncStream;
			}
		}
		return NULL;
	}

	int mLoadPendingRefCount = 0;
	int mStreamingRefCount = 0;

	inline void ModifyStreamingRefCount(int v) {
		mStreamingRefCount += v;
	}

	inline void ModifyLoadPendingRefCount(int v){
		mLoadPendingRefCount += v;
	}

	inline bool SetTxData(DataStream* stream, int pMipIndex, int pArrayIndex = 0) {
		if (pMipIndex >= mNumMipLevels || pArrayIndex >= mArraySize)
			return false;
		for (int i = 0; i < mRegionHeaders.GetSize(); i++) {
			RegionStreamHeader* header = mRegionHeaders.mpStorage + i;
			if (header->mMipIndex == pMipIndex && header->mFaceIndex == pArrayIndex) {
				if (header->mpAsyncStream && header->mpAsyncStream != stream)
					delete header->mpAsyncStream;
				header->mpAsyncStream = stream;
				return true;
			}
		}
		return false;
	}

	inline int _GetNumSurfacesToLoad(){
		return mTextureLayout == eTextureLayout_CubeArray ? mNumMipLevels * mArraySize * 6 : mTextureLayout == eTextureLayout_2DArray ? mNumMipLevels * mArraySize : mTextureLayout == eTextureLayout_Cube ? 6 : mNumMipLevels;
	}

	inline bool IsEmpty()
	{
		return (mWidth==0 && mHeight==0) || !mNumMipLevels;
	}

	inline bool IsFullyLoaded()
	{
		return !mRegionHeaders.mSize || mNumSurfacesLoaded == _GetNumSurfacesToLoad();
	}

	static const char* GetTextureTypeName(TextureType type) {
		const char* result; //RAX REGISTER
		switch (type)
		{
		case 0:
			result = "Unknown";
			break;
		case 1:
			result = "LightMapV0";
			break;
		case 2:
			result = "BumpMap";
			break;
		case 3:
			result = "NormalMap";
			break;
		case 4:
			result = "UNUSED0";
			break;
		case 5:
			result = "UNUSED1";
			break;
		case 6:
			result = "SubsurfaceScatteringMapV0";
			break;
		case 7:
			result = "SubsurfaceScatteringMap";
			break;
		case 8:
			result = "DetailMap";
			break;
		case 9:
			result = "StaticShadowMap";
			break;
		case 10:
			result = "LightmapHDR";
			break;
		case 11:
			result = "SharpDetailMap";
			break;
		case 12:
			result = "EnvMap";
			break;
		case 13:
			result = "SpecularColorMap";
			break;
		case 14:
			result = "ToonLookupMap";
			break;
		case 15:
			result = "DiffuseColorMap";
			break;
		case 16:
			result = "OutlineMap";
			break;
		case 17:
			result = "LightmapHDRScaled";
			break;
		case 18:
			result = "EmissiveMap";
			break;
		case 19:
			result = "ParticleProperties";
			break;
		case 20:
			result = "BrushNormalMap";
			break;
		case 21:
			result = "UNUSED2";
			break;
		case 22:
			result = "NormalGlossMap";
			break;
		case 23:
			result = "LookupMap";
			break;
		case 40:
			result = "LookupXYMap";
			break;
		case 24:
			result = "AmbientOcclusionMap";
			break;
		case 25:
			result = "PrefilteredEnvCubeMapHDR";
			break;
		case 35:
			result = "PrefilteredEnvCubeMapHDRScaled";
			break;
		case 26:
			result = "BrushLookupMap";
			break;
		case 27:
			result = "Vector2Map";
			break;
		case 28:
			result = "NormalDxDyMap";
			break;
		case 29:
			result = "PackedSDFMap";
			break;
		case 30:
			result = "SingleChannelSDFMap";
			break;
		case 31:
			result = "LightmapDirection";
			break;
		case 32:
			result = "LightmapStaticShadows";
			break;
		case 33:
			result = "LightStaticShadowMapAtlas";
			break;
		case 34:
			result = "LightStaticShadowMap";
			break;
		case 38:
			result = "NormalXYMap";
			break;
		case 41:
			result = "ObjectNormalMap";
			break;
		default:
			result = "InvalidMap";
			break;
		}
		return result;
	}

	static T3SurfaceGamma GetDefaultGammaForType(TextureType type) {
		return type >= TextureType::eTxLightmapFlatAtlas ? T3SurfaceGamma::eSurfaceGamma_sRGB: T3SurfaceGamma::eSurfaceGamma_Linear;
	}

	struct StreamHeader {
		int mRegionCount;
		int mAuxDataCount;
		int mTotalDataSize;
	};

	DCArray<RegionStreamHeader> mRegionHeaders;

	~T3Texture();

	static MetaOpResult MetaOperation_SerializeAsync(void* pObj, 
		MetaClassDescription* pObjDesc, MetaMemberDescription* pCtx, void* pUserData) {
		MetaStream* stream = static_cast<MetaStream*>(pUserData);
		T3Texture* tex = static_cast<T3Texture*>(pObj);
		MetaOpResult result = Meta::MetaOperation_SerializeAsync(pObj, pObjDesc, pCtx, pUserData);
		if (result == eMetaOp_Succeed) {
			int arraysize = 0;
			if (tex->mTextureLayout == eTextureLayout_CubeArray)
				arraysize = 6 * tex->mArraySize;
			else if (tex->mTextureLayout == eTextureLayout_2DArray)
				arraysize = tex->mArraySize;
			else {
				arraysize = tex->mTextureLayout == eTextureLayout_Cube ? 6 : 1;
			}
			tex->mArraySize = arraysize;
			StreamHeader header;
			if (stream->mMode == MetaStreamMode::eMetaStream_Read) {
				PerformMetaSerializeAsync(stream, &header);
				if (header.mRegionCount > tex->mRegionHeaders.mCapacity)
					tex->mRegionHeaders.ReserveAndResize(header.mRegionCount - tex->mRegionHeaders.mCapacity);
				else 
					tex->mRegionHeaders.mSize = header.mRegionCount;
				for (int i = 0; i < header.mRegionCount; i++) {
					RegionStreamHeader* ptr = tex->mRegionHeaders.mpStorage + i;
					PerformMetaSerializeAsync(stream, ptr);
					if (!ptr->mMipCount)
						ptr->mMipCount = 1;
					if (!ptr->mSlicePitch)
						ptr->mSlicePitch = ptr->mDataSize;
				}
				if (header.mAuxDataCount) {
					stream->BeginBlock();
					for (int i = 0; i < header.mAuxDataCount; i++) {
						AuxilaryData* data = new AuxilaryData;
						PerformMetaSerializeAsync(stream, data);
						tex->mAuxilaryData.push_back(data);
					}
					stream->EndBlock();
				}
				stream->BeginAsyncSection();
				if (header.mTotalDataSize > 0) {
					for (int i = 0; i < header.mRegionCount; i++) {
						RegionStreamHeader* region = tex->mRegionHeaders.mpStorage + i;
						if (region->mpAsyncStream)
							delete region->mpAsyncStream;
						DataStreamMemory* mem  = new DataStreamMemory(region->mDataSize, 0x1000);
						region->mpAsyncStream = mem;
						stream->serialize_bytes(mem->mMemoryBuffer, region->mDataSize);
						mem->SetMode(DataStreamMode::eMode_Read);
					}
				}
				stream->EndAsyncSection();
			}
			else {
				int regions = arraysize * tex->mNumMipLevels;//v13
				//arraysize = v12, v43 = temp
				//v81 = arraysize
				header.mRegionCount = regions;
				int tot = 0;
				for (int i = 0; i < tex->mRegionHeaders.GetSize(); i++) {
					RegionStreamHeader* header = tex->mRegionHeaders.mpStorage + i;
					if (!header->mpAsyncStream) {
						TelltaleToolLib_RaiseError("Could not finish writing texture: No texture data found in region", ErrorSeverity::WARN);
						return eMetaOp_Fail;
					}
					if (header->mpAsyncStream->GetSize() != header->mDataSize)
						header->mDataSize = header->mpAsyncStream->GetSize();
					tot += header->mDataSize;
				}
				header.mTotalDataSize = tot;
				header.mAuxDataCount = tex->mAuxilaryData.size();
				PerformMetaSerializeAsync(stream, &header);
				for (int i = 0; i < tex->mRegionHeaders.GetSize(); i++) {
					PerformMetaSerializeAsync(stream, tex->mRegionHeaders.mpStorage + i);
				}
				if (header.mAuxDataCount) {
					stream->BeginBlock();
					for (int i = 0; i < header.mAuxDataCount; i++) {
						PerformMetaSerializeAsync(stream, tex->mAuxilaryData[i]);
					}
					stream->EndBlock();
				}
				stream->BeginAsyncSection();
				char buf[40];
				for (int i = 0; i < tex->mRegionHeaders.GetSize(); i++) {
					RegionStreamHeader* header = tex->mRegionHeaders.mpStorage + i;
					header->mpAsyncStream->SetMode(DataStreamMode::eMode_Read);
					header->mpAsyncStream->SetPosition(0,DataStreamSeekType::eSeekType_Begin);
					char* tmp = new char[header->mpAsyncStream->GetSize()];
					header->mpAsyncStream->Serialize(tmp, header->mpAsyncStream->GetSize());
					sprintf(buf, "Mip %d", header->mMipIndex);
					stream->Key(buf);
					stream->serialize_bytes(tmp, header->mpAsyncStream->GetSize());
					delete[] tmp;
				}
				stream->EndAsyncSection();
			}
		}
		return result;
	}

};

#endif