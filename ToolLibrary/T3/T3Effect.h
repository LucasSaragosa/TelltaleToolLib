#pragma once
#include "../LibraryConfig.h"
#include "T3EffectCache.h"
#include "../Types/D3DMesh.h"
#include "../LinkedList.h"
#include <atomic>
#include <vector>
#include <math.h>

enum RenderScopeGPU
{
	eRenderScopeGPU_None = 0xFFFFFFFF,
	eRenderScopeGPU_Frame = 0x0,
	eRenderScopeGPU_Main = 0x1,
	eRenderScopeGPU_Alpha = 0x2,
	eRenderScopeGPU_EVSM = 0x3,
	eRenderScopeGPU_Cascade = 0x4,
	eRenderScopeGPU_Ray = 0x5,
	eRenderScopeGPU_MAX = 0x6,
	eRenderScopeGPU_FirstScope = 0x1,
};

enum T3LightCinematicType
{
	eLightCinematicType_Key = 0x0,
	eLightCinematicType_Fill = 0x1,
	eLightCinematicType_Rim = 0x2,
	eLightCinematicType_Count = 0x3,
};

enum RenderDepthFetchMode
{
	eRenderDepthFetch_None = 0x0,
	eRenderDepthFetch_LinearDepth = 0x1,
	eRenderDepthFetch_DepthBuffer = 0x2,
	eRenderDepthFetch_FrameBuffer = 0x3,
};

enum T3LightEnvFlags
{
	eLightEnvFlag_Enabled = 0x1,
	eLightEnvFlag_TileEnabled = 0x2,
	eLightEnvFlag_GroupEnabled = 0x4,
	eLightEnvFlag_LODOnHigh = 0x10000,
	eLightEnvFlag_LODOnMedium = 0x20000,
	eLightEnvFlag_LODOnLow = 0x40000,
	eLightEnvFlag_EnabledFlags = 0x7,
	eLightEnvFlag_LODFlags = 0x70000,
};

enum T3LightEnvType
{
	eLightEnvType_Point = 0x0,
	eLightEnvType_Spot = 0x1,
	eLightEnvType_DirectionalKey = 0x2,
	eLightEnvType_Ambient = 0x3,
	eLightEnvType_DirectionalAmbient = 0x4,
	eLightEnvType_Count = 0x5,
};

struct T3SurfaceFormatDesc
{
	i32 mBlockWidth;
	i32 mBlockHeight;
	i32 mBitsPerBlock;
	i32 mBitsPerPixel;
	i32 mMinBytesPerSurface;
};

enum T3LightCinematicRigFlags
{
	eLightCinematicRigFlag_SelfShadowing = 0x1,
	eLightCinematicRigFlag_LODIntensityOnHigh = 0x10000,
	eLightCinematicRigFlag_LODIntensityOnMedium = 0x20000,
	eLightCinematicRigFlag_LODIntensityOnLow = 0x40000,
	eLightCinematicRigFlag_LODIntensityFlags = 0x70000,
};

enum T3LightCinematicFlags
{
	eLightCinematicFlag_Enabled = 0x1,
};

struct T3LightCinematicRigParams
{
	float mLightCinIntensity;
	float mLightEnvIntensity;
	float mEnlightenIntensity;
	float mEnlightenSaturation;
	Flags mFlags;
	EnumHBAOParticipationType mHBAOParticipationType;
};

struct T3LightCinematicParams
{
	Vector3 mDirection;
	Color mColor;
	float mIntensity;
	float mDimmer;
	float mDiffuseIntensity;
	float mSpecularIntensity;
	float mNPRBanding;
	Vector3 mNPRBandThresholds;
	float mShadowIntensity;
	float mShadowTraceLength;
	float mOpacity;
	float mWrap;
	Flags mFlags;
	EnumHBAOParticipationType mHBAOParticipationType;
};

struct GFXPlatformScissorRect
{
	u16 X;
	u16 Y;
	u16 W;
	u16 H;
};

enum T3TransparencyMode
{
	eTransparencyMode_Opaque = 0x0,
	eTransparencyMode_OpaqueGlow = 0x1,
	eTransparencyMode_AlphaTest = 0x2,
	eTransparencyMode_AlphaBlend = 0x3,
};

enum GFXPlatformPrimitiveType
{
	eGFXPlatformPrimitive_None = 0x0,
	eGFXPlatformPrimitive_Points = 0x1,
	eGFXPlatformPrimitive_Lines = 0x2,
	eGFXPlatformPrimitive_LineStrip = 0x3,
	eGFXPlatformPrimitive_Triangles = 0x4,
	eGFXPlatformPrimitive_TriangleStrip = 0x5,
	eGFXPlatformPrimitive_TriangleFan = 0x6,
	eGFXPlatformPrimitive_Quads = 0x7,
	eGFXPlatformPrimitive_TriangleAdjacency = 0x8,
	eGFXPlatformPrimitive_Count = 0x9,
};

enum RenderViewPassType
{
	eRenderViewPass_Normal = 0x0,
	eRenderViewPass_SubView = 0x1,
};

enum T3RenderPassType
{
	eRenderPassType_None = 0xFFFFFFFF,
	eRenderPassType_GBuffer = 0x0,
	eRenderPassType_Scene = 0x1,
	eRenderPassType_Post = 0x2,
};

enum T3RenderPass
{
	eRenderPass_FirstScene = 0x0,
	eRenderPass_Clear = 0x0,
	eRenderPass_GBuffer = 0x1,
	eRenderPass_GBuffer_Simple = 0x2,
	eRenderPass_DynamicLightMap = 0x3,
	eRenderPass_DeferredDecal_GBuffer = 0x4,
	eRenderPass_FirstDefault = 0x5,
	eRenderPass_Main_Compute = 0x5,
	eRenderPass_Main_Opaque = 0x6,
	eRenderPass_Main_Alpha_Antialiased = 0x7,
	eRenderPass_DeferredDecal_Emissive = 0x8,
	eRenderPass_Main_Alpha_PreResolve = 0x9,
	eRenderPass_Main_Alpha = 0xA,
	eRenderPass_Main_AfterPostEffects = 0xB,
	eRenderPass_Main_AfterAntialiasing = 0xC,
	eRenderPass_Main_AfterSSLines = 0xD,
	eRenderPass_LastDefault = 0xD,
	eRenderPass_Downsample_Alpha = 0xE,
	eRenderPass_ShadowMap = 0xF,
	eRenderPass_ForceLinearDepth = 0x10,
	eRenderPass_Glow = 0x11,
	eRenderPass_Lines = 0x12,
	eRenderPass_LinesCS_GBuffer = 0x13,
	eRenderPass_LinesCS_LinearDepth = 0x14,
	eRenderPass_LastScene = 0x14,
	eRenderPass_Post = 0x15,
	eRenderPass_Count = 0x16,
	eRenderPass_DefaultCount = 0x9,
	eRenderPass_SceneCount = 0x15,
};

enum T3GFXDynamicResourceType
{
	eT3GFXDynamicResource_GenericBuffer = 0x0,
	eT3GFXDynamicResource_UniformBuffer = 0x1,
	eT3GFXDynamicResource_Texture = 0x2,
	eT3GFXDynamicResource_Count = 0x3,
};

enum RenderViewType
{
	eRenderView_Main = 0x0,
	eRenderView_Shadow = 0x1,
	eRenderView_ShadowCascade = 0x2,
	eRenderView_ShadowEVSM = 0x3,
	eRenderView_ShadowCascadeEVSM = 0x4,
	eRenderView_RenderTexture = 0x5,
	eRenderView_RenderViewport = 0x6,
	eRenderView_LightBake = 0x7,
	eRenderView_Misc = 0x8,
};

enum RenderDebugMode
{
	eRenderDebugMode_Normal = 0x0,
	eRenderDebugMode_LightMapOnly = 0x1,
	eRenderDebugMode_IndirectOnly = 0x2,
	eRenderDebugMode_BaseTextureOnly = 0x3,
	eRenderDebugMode_NormalsOnly = 0x4,
	eRenderDebugMode_DiffuseLightOnly = 0x5,
	eRenderDebugMode_SpecularLightOnly = 0x6,
	eRenderDebugMode_LightOnly = 0x7,
	eRenderDebugMode_Depth = 0x8,
	eRenderDebugMode_LightCountPerTile = 0x9,
	eRenderDebugMode_ShadowCascades = 0xA,
	eRenderDebugMode_DebugMipUsage = 0xB,
	eRenderDebugMode_Alpha = 0xC,
	eRenderDebugMode_DoubleSided = 0xD,
	eRenderDebugMode_LOD = 0xE,
	eRenderDebugMode_LODCount = 0xF,
	eRenderDebugMode_ScreenArea = 0x10,
	eRenderDebugMode_DrawCalls = 0x11,
	eRenderDebugMode_ColorRange = 0x12,
	eRenderDebugMode_WaveformMonitor = 0x13,
	eRenderDebugMode_Count = 0x14,
};

enum TextureStreamingPriority
{
	eTextureStreaming_Requested = 0x0,
	eTextureStreaming_Required = 0x1,
	eTextureStreaming_RequireFullTexture = 0x2,
};

enum T3GFXUniformBufferUsage
{
	eGFXUniformBufferUsage_Immutable = 0x0,
	eGFXUniformBufferUsage_Dynamic = 0x1,
	eGFXUniformBufferUsage_Streaming = 0x2,
};

enum T3MeshVertexStream
{
	eMeshVertexStream_None = 0xFFFFFFFF,
	eMeshVertexStream_Position = 0x0,
	eMeshVertexStream_BlendWeight = 0x1,
	eMeshVertexStream_BlendIndex = 0x2,
	eMeshVertexStream_Normal = 0x3,
	eMeshVertexStream_Tangent = 0x4,
	eMeshVertexStream_SmoothNormal = 0x5,
	eMeshVertexStream_LightmapTexCoord = 0x6,
	eMeshVertexStream_ShadowmapTexCoord = 0x7,
	eMeshVertexStream_ScaledTexCoord = 0x8,
	eMeshVertexStream_EnlightentTexCoord = 0x9,
	eMeshVertexStream_Color0 = 0x14,
	eMeshVertexStream_Color1 = 0x15,
	eMeshVertexStream_TexCoord0 = 0x1C,
	eMeshVertexStream_TexCoord1 = 0x1D,
	eMeshVertexStream_TexCoord2 = 0x1E,
	eMeshVertexStream_TexCoord3 = 0x1F,
	eMeshVertexStream_Count = 0x20,
	eMeshVertexStream_FirstColor = 0x14,
	eMeshVertexStream_LastColor = 0x15,
	eMeshVertexStream_FirstTexCoord = 0x1C,
	eMeshVertexStream_LastTexCoord = 0x1F,
	eMeshVertexStream_ColorCount = 0x2,
	eMeshVertexStream_TexCoordCount = 0x4,
};

enum RenderDirtyType
{
	eRenderDirty_None = 0x0,
	eRenderDirty_Object = 0x1,
	eRenderDirty_Batch = 0x2,
	eRenderDirty_Texture = 0x4,
	eRenderDirty_Lighting = 0x8,
	eRenderDirty_State = 0x10,
	eRenderDirty_Effect = 0x20,
	eRenderDirty_LightGroups = 0x40,
	eRenderDirty_LightProbe = 0x80,
	eRenderDirty_MaterialOverride = 0x100,
	eRenderDirty_All = 0x1FF,
};

struct MipSizeData
{

	struct MipSize
	{
		unsigned __int16 mWidthPix;
		unsigned __int16 mHeightPix;
		unsigned __int16 mWidthBlocks;
		unsigned __int16 mHeightBlocks;
		unsigned __int16 mPitchBytes;
		unsigned __int16 mMipLevel;
		unsigned int mMipOffsetBytes;
		unsigned int mblockAlignment;
	};

	T3SurfaceFormatDesc mFormatDesc;
	MipSize mSizes[16];
	unsigned int mNumMips;
	unsigned int mTotalSizeBytes;
};

enum RenderPrimitiveStyle
{
	eRenderStyle_Filled = 0x0,
	eRenderStyle_Wire = 0x1,
};

struct RenderVertex_Position3
{
	Vector3 mPosition;
};

struct RenderVertex_Position2
{
	Vector2 mPosition;
};

struct ColorRGBA8
{
	char r;
	char g;
	char b;
	char a;
};

struct RenderVertex_Color
{
	ColorRGBA8 mColor;
};

struct RenderVertex_Texture2
{
	Vector2 mTexCoord;
};

struct RenderVertex_EnlightenUV
{
	__int16 mTexCoordU;
	__int16 mTexCoordV;
};

struct RenderVertex_OutlineSize
{
	ColorRGBA8 mColor;
};

enum RenderVertexFormat
{
	eVertexFormat_Position3 = 0x0,
	eVertexFormat_Position2 = 0x1,
	eVertexFormat_Color = 0x2,
	eVertexFormat_Texture2 = 0x3,
	eVertexFormat_OutlineSize = 0x4,
	eVertexFormat_BlendWeight = 0x5,
	eVertexFormat_Position3Color = 0x6,
	eVertexFormat_Position2Color = 0x7,
	eVertexFormat_Position2Texture2 = 0x8,
	eVertexFormat_Position3Texture2 = 0x9,
	eVertexFormat_Position3Texture3 = 0xA,
	eVertexFormat_Position3Texture2Color = 0xB,
	eVertexFormat_Position2Texture2Color = 0xC,
	eVertexFormat_EnlightenUV = 0xD,
};

struct T3MeshBufferFormatEntry
{
	GFXPlatformFormat mFormat = eGFXPlatformFormat_None;
	GFXPlatformVertexAttribute mAttribute = (GFXPlatformVertexAttribute)-1;
	unsigned int mAttributeIndex = 0;
};

//index into T3::spDefaultVertexStates
enum VertexArrayType
{
	eVertexArray_DefaultColor = 0x0,
	eVertexArray_DefaultOutline = 0x1,
	eVertexArray_DefaultBlendWeight = 0x2,
	eVertexArray_DefaultEnlighten = 0x3,
	eVertexArray_Corners = 0x4,
	eVertexArray_Billboard = 0x5,//positions 0 to 1
	eVertexArray_Billboard3D = 0x6,//positions -0.5 to 0.5
	eVertexArray_Fullscreen = 0x7,
	eVertexArray_BoxBase = 0x8,//vertices (No indices) for box vertex arrays, eg filled box etc
	eVertexArray_WireBox = 0x9,
	eVertexArray_FilledBox = 0xA,
	eVertexArray_WireSphere = 0xB,
	eVertexArray_FilledSphere = 0xC,
	eVertexArray_FilledCone = 0xD,
	eVertexArray_Arrow = 0xE,
	eVertexArray_Axis = 0xF,
	eVertexArray_WireCircle = 0x10,
	eVertexArray_FilledCircle = 0x11,
	eVertexArray_WireCylinder = 0x12,
	eVertexArray_Count = 0x13,
};

enum IndexBufferType
{
	eIndexBuffer_DefaultTextBackground = 0x0,
	eIndexBuffer_DefaultQuadList = 0x1,
	eIndexBuffer_Count = 0x2,
};

enum GenericBufferType
{
	eGenericBuffer_EmptyRaw = 0x0,
	eGenericBuffer_EmptyShadowCascade = 0x1,
	eGenericBuffer_EmptyZBin = 0x2,
	eGenericBuffer_EmptyBone = 0x3,
	eGenericBuffer_Count = 0x4,
};

struct T3RenderTargetID
{
	T3RenderTargetConstantID mValue;

	inline bool IsTemporary() {
		return (int)mValue >= eRenderTargetID_Count;
	}

	inline bool IsConstant(){
		return !IsTemporary();
	}

};

enum T3RenderTargetAccess
{
	eRenderTargetAccess_Input = 0x0,
	eRenderTargetAccess_Output = 0x1,
};

struct RenderFrameStats
{
	u32 mDrawCalls;
	u32 mDispatchCalls;
	u32 mProgramBinds;
	u32 mUniformBufferBinds;
	u32 mBufferBinds;
	u32 mTextureBinds;
	u32 mPrimitiveIndexCount;
	u32 mVertexCount;
	u32 mBufferUploadCount;
	u32 mBufferUploadBytes;
	u32 mUniformBufferUploadCount;
	u32 mUniformBufferUploadBytes;
	u32 mTextureUploadCount;
	u32 mTextureUploadBytes;
};

enum DefaultTextureType {
	eDefaultTextureType_White = 0,
	eDefaultTextureType_Black = 1,
	eDefaultTextureType_BlackArray = 2,
	eDefaultTextureType_Black3D = 3,
	eDefaultTextureType_BlackCubeMap = 4,
	eDefaultTextureType_Random = 5,
	eDefaultTextureType_Tetrahedral = 6,
	eDefaultTextureType_SMAAArea = 7,
	eDefaultTextureType_SMAASearch = 8,
	eDefaultTextureType_Noise = 9,
	eDefaultTextureType_DefaultLightmap = 10,
	eDefaultTextureType_DefaultEnlighten = 11,
	eDefaultTextureType_DefaultBoneMatrix = 12,
	eDefaultTextureType_HBAONoiseLookup = 13,
	eDefaultTextureType_Count = 14,
};

struct RenderClipPlanes
{
	Plane mPlanes[4];
	u32 mCount;
};

struct T3RenderViewport
{
	int mWidth;
	int mHeight;
	int mOriginX;
	int mOriginY;
	float mMinDepth;
	float mMaxDepth;

	inline T3RenderViewport() : mWidth(0), mHeight(0), mOriginX(0), mOriginY(0), mMinDepth(0.f), mMaxDepth(1.0f) {}

	inline bool operator==(const T3RenderViewport& rhs) const {
		return mWidth == rhs.mWidth && mHeight == rhs.mHeight && mOriginX == rhs.mOriginX &&
			mOriginY == rhs.mOriginY && mMinDepth == rhs.mMinDepth && mMaxDepth == rhs.mMinDepth;
	}

};

struct T3RenderRelativeViewport
{
	float mWidth;
	float mHeight;
	float mOriginX;
	float mOriginY;
	float mMinDepth;
	float mMaxDepth;

	inline T3RenderRelativeViewport() : mWidth(0), mHeight(0), mOriginX(0), mOriginY(0), mMinDepth(0.f), mMaxDepth(1.0f) {}

};

struct DefaultTexture {
	std::shared_ptr<T3Texture> mpTexture;
	const char* mName;
};

struct RenderConfiguration {

	Color mClearColor = Color::Black;
	RenderQualityType mQuality = eRenderQuality_Mid;
	RenderDebugMode mDebugMode = eRenderDebugMode_Normal;
	float mHDRColorBufferScale = 1.0f;
	float mRenderScale = 1.0f;
	float mIntensity = 1.0f;
	float mTAAFrameTimeThreshold = 0.1f;
	HBAOResolution mHBAOResolution = eHBAOResolutionFull;
	RenderAntialiasType mAliasingType = eRenderAntialias_None;
	RenderHDRType mHDRType = eRenderHDR_10;
	DOFQualityLevel mDOFQuality = eDofQualityLevelDisabled;
	float mHDRLevelWhite = 1.f;
	float mHDRLevelBlack = 0.f;

	bool mbGlowEnabled = true;
	bool mbAllowWrapShadowedLights = true;
	bool mbGammaCorrect = true;

	static const char* GetDebugModeName(RenderDebugMode mode)
	{
		const char* result;
		switch (mode)
		{
		case 0:
			result = "Standard";
			break;
		case 1:
			result = "Lightmap Only";
			break;
		case 2:
			result = "Indirect (Enlighten) Only";
			break;
		case 3:
			result = "Base Texture";
			break;
		case 4:
			result = "Normals";
			break;
		case 5:
			result = "Diffuse Only";
			break;
		case 6:
			result = "Specular Only";
			break;
		case 7:
			result = "Light Only";
			break;
		case 8:
			result = "Depth";
			break;
		case 9:
			result = "Light Count";
			break;
		case 10:
			result = "Shadow Cascades";
			break;
		case 11:
			result = "Mip Usage";
			break;
		case 12:
			result = "Alpha";
			break;
		case 13:
			result = "Double Sided";
			break;
		case 14:
			result = "LOD";
			break;
		case 15:
			result = "LOD Count";
			break;
		case 17:
			result = "Draw Calls";
			break;
		case 16:
			result = "Screen Area";
			break;
		case 18:
			result = "Color Range";
			break;
		case 19:
			result = "Waveform Monitor";
			break;
		default:
			result = "Unknown";
			break;
		}
		return result;
	}

};

extern RenderConfiguration sGlobalRenderConfig;

namespace T3 {

	extern std::vector<std::shared_ptr<LayoutAttach>> sCachedLayouts;
	extern std::shared_ptr<T3GFXVertexState> spDefaultVertexStates[eVertexArray_Count];
	extern std::shared_ptr<T3GFXBuffer> spDefaultGenericBuffer[eGenericBuffer_Count];
	extern std::shared_ptr<T3GFXBuffer> spDefaultIndexBuffer[eIndexBuffer_Count];
	extern DefaultTexture spDefaultTexture[eDefaultTextureType_Count];

	inline void CopyBuffer(T3GFXBuffer& src, T3GFXBuffer& dst){
		if (dst.mpCPUBuffer)
			free(dst.mpCPUBuffer);
		dst.mpCPUBuffer = malloc(src.mStride * src.mCount);
		memcpy(dst.mpCPUBuffer, src.mpCPUBuffer, src.mStride * src.mCount);
		dst.mCount = src.mCount;
		dst.mStride = src.mStride;
		dst.mBufferFormat = src.mBufferFormat;
		dst.mBufferUsage = src.mBufferUsage;
		dst.mResourceUsage = src.mResourceUsage;
	}

	inline u64 ComputeVertexLayoutHash(GFXPlatformVertexLayout& layout, u64 hash0 = 0){
		hash0 = CRC64(hash0, (const char*)&layout.mIndexFormat, 4u);
		hash0 = CRC64(hash0, (const char*)&layout.mAttributeCount, 4u);
		hash0 = CRC64(hash0, (const char*)&layout.mVertexBufferCount, 4u);
		hash0 = CRC64(hash0, (const char*)&layout.mVertexCountPerInstance, 4u);
		hash0 = CRC64(hash0, (const char*)&layout.mVertexBufferStrides, 4u * layout.mVertexBufferCount);
		hash0 = CRC64(hash0, (const char*)&layout.mAttributes, sizeof(GFXPlatformAttributeParams) * layout.mAttributeCount);
		return hash0;
	}
	
	inline std::shared_ptr<LayoutAttach> AddCachedVertexLayout(GFXPlatformVertexLayout& layout){
		u64 hash = ComputeVertexLayoutHash(layout, 0);
		for (auto& it : sCachedLayouts)
			if (it->mHash == hash)
				return it;
		auto& ptr = sCachedLayouts.emplace_back();
		ptr = std::make_shared<LayoutAttach>(layout, hash);
		return ptr;
	}

	inline void UpdateCachedVertexLayout(T3GFXVertexState& state){
		state.mpCachedVertexLayout[0] = 0;
		state.mpCachedVertexLayout[1] = 0;
		state.mpCachedVertexLayout[2] = 0;
		state.mpCachedVertexLayout[3] = 0;
		GFXPlatformVertexLayout Dst{};
		for(int i = 0; i < state.mIndexBufferCount; i++){
			if (state.mpIndexBuffer[i])
				Dst.mIndexFormat = state.mpIndexBuffer[i]->mBufferFormat;
			Dst.mVertexCountPerInstance = state.mVertexCountPerInstance;
			Dst.mVertexBufferCount = state.mVertexBufferCount;
			Dst.mAttributeCount = state.mAttributeCount;
			int vertIndex = 0;
			if (Dst.mVertexBufferCount) {
				for (int j = Dst.mVertexBufferCount - 1; j >= 0; j--) {
					if (state.mpVertexBuffer[vertIndex]) {
						int stride = state.mpVertexBuffer[vertIndex]->mBufferUsage & eGFXPlatformBuffer_SingleValue ? 0 : state.mpVertexBuffer[vertIndex]->mStride;
						Dst.mVertexBufferStrides[vertIndex] = stride;
					}
					vertIndex++;
				}
			}
		//	if (state.mAttributeCount)
		//		memmove(Dst.mAttributes, state.mAttributes, 0);
			state.mpCachedVertexLayout[i] = AddCachedVertexLayout(Dst);
		}
	}

	inline void ClearUnusedBuffers(T3GFXVertexState& state){
		T3GFXVertexState* vState=0; // rbx@1
		unsigned int v2 = 0; // eax@1
		unsigned int* v3 = 0; // rcx@2
		__int64 v4 = 0; // rdx@2
		__int64 v5 = 0; // rax@3
		unsigned int verButCount = 0; // eax@4
		unsigned int v7 = 0; // esi@4
		unsigned int incrementIfUSED = 0; // er13@4
		unsigned int alwaysIncrements = 0; // edi@4
		std::shared_ptr<T3GFXBuffer> currentvertbuf{}; // rbp@6
		int v11 = 0; // er15@6
		__int64 currentCounterRep = 0; // rax@6
		signed __int64 v13 = 0; // r12@6
		unsigned int v15 = 0; // edi@12
		__int64 v17 = 0; // rax@17
		int Dst[32];
		int v19[32];

		vState = &state;
		memset(Dst, 0, 0x80u);
		memset(v19, 0, 0x80u);
		v2 = vState->mAttributeCount;
		if (v2)
		{
			v3 = &vState->mAttributes[0].mBufferIndex;
			v4 = v2;
			do
			{
				v5 = *v3;
				v3 += 6;
				++Dst[v5];
				--v4;
			} while (v4);
		}
		verButCount = vState->mVertexBufferCount;
		v7 = 0;
		incrementIfUSED = 0;
		alwaysIncrements = 0;
		if (verButCount)
		{
			do
			{
				if ((unsigned int)Dst[(unsigned __int64)alwaysIncrements] <= 0)
				{
					v11 = -1;
				}
				else
				{
					currentvertbuf = vState->mpVertexBuffer[alwaysIncrements];
					v11 = incrementIfUSED;
					currentCounterRep = incrementIfUSED++;
					v13 = (i64)vState + 8 * currentCounterRep;
					vState->mpVertexBuffer[currentCounterRep] = std::move(currentvertbuf);
				}
				v19[(u64)alwaysIncrements++] = v11;
				verButCount = vState->mVertexBufferCount;
			} while (alwaysIncrements < verButCount);
		}
		v15 = incrementIfUSED;
		if (incrementIfUSED < verButCount)
		{
			do
			{
				vState->mpVertexBuffer[v15].reset();
				++v15;
			} while (v15 < vState->mVertexBufferCount);
		}
		vState->mVertexBufferCount = incrementIfUSED;
		if (vState->mAttributeCount > 0)
		{
			do
			{
				v17 = v7++;
				vState->mAttributes[v17].mBufferIndex = v19[vState->mAttributes[v17].mBufferIndex];
			} while (v7 < vState->mAttributeCount);
		}
	}

	inline void DuplicateAttribute(T3GFXVertexState& state, GFXPlatformVertexAttribute srcAttrib, u32 srcAttribIndex, GFXPlatformVertexAttribute dstAttrib, u32 dstAtribIndex){
		int attribSrc = 0;
		for(int i = 0; i < state.mAttributeCount; i++){
			if (state.mAttributes[i].mAttributeIndex == srcAttribIndex && state.mAttributes[i].mAttribute == srcAttrib)
				break;
			attribSrc++;
		}
		bool broke = false;
		if (attribSrc < state.mAttributeCount && state.mAttributeCount){
			int index = 0;
			while(state.mAttributes[index].mAttribute != dstAttrib || state.mAttributes[index].mAttributeIndex != dstAtribIndex){
				index++;
				if(index >= state.mAttributeCount){
					broke = true;
					break;
				}
			}
			if(broke||index<0){
				state.mAttributeCount++;
				state.mAttributes[state.mAttributeCount - 1] = state.mAttributes[attribSrc];
				state.mAttributes[state.mAttributeCount - 1].mAttributeIndex = dstAtribIndex;
				state.mAttributes[state.mAttributeCount - 1].mAttribute = dstAttrib;
				UpdateCachedVertexLayout(state);
			}
		}
	}

	inline void MergeVertexState(T3GFXVertexState& src, T3GFXVertexState& dst){
		int Dst[32]{ 0 };
		for (int i = 0; i < src.mVertexBufferCount; i++) {
			int j = 0;
			bool broken = false;
			while (dst.mpVertexBuffer[j].get() != src.mpVertexBuffer[i].get()) {
				if (++j >= dst.mVertexBufferCount) {
					broken = true;
					break;
				}
			}
			if (broken || j < 0) {
				dst.mVertexBufferCount++;
				dst.mpVertexBuffer[dst.mVertexBufferCount - 1] = src.mpVertexBuffer[i];
				Dst[i] = j;
			}
		}
		for(int i = 0; i < src.mAttributeCount;){
			int j = 0;
			bool broken = false;
			while(dst.mAttributes[j].mAttribute != src.mAttributes[i].mAttribute || dst.mAttributes[j].mFrequency != src.mAttributes[i].mFrequency){
				if (++j >= dst.mAttributeCount) {
					broken = true;
					break;
				}
			}
			if (broken || (j & 0x80000000) != 0) {
				j = dst.mAttributeCount;
				dst.mAttributeCount++;
				dst.mAttributes[j].mAttribute = src.mAttributes[i].mAttribute;
				dst.mAttributes[j].mFrequency = src.mAttributes[i].mFrequency;
			}
			dst.mAttributes[j].mFormat = src.mAttributes[i].mFormat;
			dst.mAttributes[j].mBufferIndex = src.mAttributes[i].mBufferIndex;
			dst.mAttributes[j].mAttributeIndex = src.mAttributes[i].mAttributeIndex;
			++i;
		}
		int indexBuffers = src.mIndexBufferCount;
		if (dst.mIndexBufferCount > indexBuffers)
			indexBuffers = dst.mIndexBufferCount;
		for(int i = 0; i < indexBuffers; i++){
			if (src.mpIndexBuffer[i])
				dst.mpIndexBuffer[i] = src.mpIndexBuffer[i];
		}
		UpdateCachedVertexLayout(dst);
	}

	inline T3SurfaceFormatDesc T3SurfaceFormat_GetDesc(T3SurfaceFormat format){
		T3SurfaceFormatDesc result{}; // r8@1
		int v3; // eax@14
		signed int v4; // ecx@14
		int v5; // eax@17

		*(u64*)&result.mBlockWidth = 0ll;
		*(u64*)&result.mBitsPerBlock = 0ll;
		result.mMinBytesPerSurface = 0;
		switch (format)
		{
			case 0xEu:
			case 0x10u:
			case 0x11u:
				result.mBitsPerPixel = 8;
				break;
			case 0xDu:
			case 0x25u:
				result.mBitsPerPixel = 128;
				break;
			case 2u:
			case 3u:
			case 4u:
			case 6u:
			case 9u:
			case 0x12u:
			case 0x13u:
			case 0x16u:
			case 0x20u:
			case 0x30u:
			case 0x32u:
				result.mBitsPerPixel = 16;
				break;
			case 1u:
			case 8u:
			case 0xCu:
			case 0x15u:
			case 0x22u:
			case 0x24u:
			case 0x36u:
				result.mBitsPerPixel = 64;
				break;
			case 0x40u:
			case 0x43u:
			case 0x45u:
			case 0x70u:
			case 0x71u:
			case 0x72u:
			case 0x74u:
				result.mBitsPerPixel = 4;
				result.mBlockWidth = 4;
				result.mBlockHeight = 4;
				result.mBitsPerBlock = 64;
				break;
			case 0x50u:
			case 0x52u:
				result.mBitsPerPixel = 2;
				result.mBlockHeight = 8;
				goto LABEL_8;
			case 0x51u:
			case 0x53u:
				result.mBitsPerPixel = 4;
				result.mBlockHeight = 4;
			LABEL_8:
				result.mBlockWidth = 4;
				result.mBitsPerBlock = 64;
				result.mMinBytesPerSurface = 32;
				break;
			case 0x60u:
				result.mBitsPerPixel = 4;
				result.mBlockWidth = 4;
				result.mBlockHeight = 4;
				result.mBitsPerBlock = 64;
				result.mMinBytesPerSurface = 8;
				break;
			case 0x61u:
			case 0x62u:
				result.mBitsPerPixel = 8;
				result.mBlockWidth = 4;
				result.mBlockHeight = 4;
				result.mBitsPerBlock = 128;
				result.mMinBytesPerSurface = 16;
				break;
			case 0x41u:
			case 0x42u:
			case 0x44u:
			case 0x46u:
			case 0x47u:
			case 0x73u:
			case 0x75u:
			case 0x80u:
				result.mBitsPerPixel = 8;
				result.mBlockWidth = 4;
				result.mBlockHeight = 4;
				result.mBitsPerBlock = 128;
				break;
			case 0u:
			case 5u:
			case 7u:
			case 0xAu:
			case 0xBu:
			case 0xFu:
			case 0x14u:
			case 0x17u:
			case 0x21u:
			case 0x23u:
			case 0x26u:
			case 0x31u:
			case 0x33u:
			case 0x34u:
			case 0x35u:
			case 0x37u:
			case 0x90u:
				result.mBitsPerPixel = 32;
				break;
			default:
				break;
		}
		v3 = result.mBitsPerBlock;
		v4 = 1;
		if (!v3)
		{
			v3 = result.mBitsPerPixel;
			result.mBitsPerBlock = v3;
			result.mBlockWidth = 1;
			result.mBlockHeight = 1;
		}
		if (!result.mMinBytesPerSurface)
		{
			v5 = (v3 + 7) / 8;
			if (v5 > 1)
				v4 = v5;
			result.mMinBytesPerSurface = v4;
		}
		return result;
	}

	inline i64 GetNumMipLevelsNeeded(T3SurfaceFormat format, int w, int h)
	{
		int v3; // ebx@1
		int v4; // edi@1
		signed int v5; // er8@1
		int v6; // er9@1
		int i; // ecx@1
		T3SurfaceFormatDesc result; // [sp+20h] [bp-28h]@1

		v3 = w;
		v4 = h;
		result = T3SurfaceFormat_GetDesc(format);
		v5 = 1;
		v6 = v3 / 2;
		for (i = v4 / 2; v6 >= result.mBlockWidth; i /= 2)
		{
			if (i < result.mBlockHeight)
				break;
			++v5;
			v6 /= 2;
		}
		return (unsigned int)v5;
	}

	inline void GetMipSize(int baseWidth, int baseHeight, __int64 miplevel, int* outWidth, int* outHeight)
	{
		signed int v5; // er10@1
		int v6; // eax@3
		signed int v7; // eax@3
		signed int v8; // eax@5
		int v9; // edx@5
		int v10; // eax@5

		v5 = baseHeight;
		if ((signed int)miplevel <= 0)
		{
			*outWidth = baseWidth;
			*outHeight = baseHeight;
		}
		else
		{
			miplevel = (unsigned int)miplevel;
			do
			{
				v6 = baseWidth;
				baseWidth = 1;
				v7 = v6 / 2;
				if (v7 > 1)
					baseWidth = v7;
				v8 = v5;
				v9 = (unsigned __int64)v5 >> 32;
				v5 = 1;
				v10 = (v8 - v9) >> 1;
				if (v10 > 1)
					v5 = v10;
				--miplevel;
			} while (miplevel);
			*outWidth = baseWidth;
			*outHeight = v5;
		}
	}

	inline void ConvertDXT5ToDXT5A(void* pDst, const void* pSrc, unsigned int width, unsigned int height)
	{
		char* v4; // rbx@1
		unsigned int v5; // er11@1
		unsigned int v6; // er10@1
		unsigned int v7; // er9@1
		char* v8; // rdi@1
		__int64 v9; // r8@3
		__int64 v10; // rax@3
		char* v11; // rdx@3
		char* v12; // rax@3
		__int64 v13; // rcx@4

		v4 = (char*)pSrc;
		v5 = (height + 3) >> 2;
		v6 = (width + 3) >> 2;
		v7 = 0;
		v8 = (char*)pDst;
		if (v5)
		{
			do
			{
				if (v6)
				{
					v9 = v6;
					v10 = v6 * v7;
					v11 = &v8[8 * v10];
					v12 = &v4[16 * v10];
					do
					{
						v13 = *(u64*)v12;
						v12 += 16;
						*(u64*)v11 = v13;
						v11 += 8;
						--v9;
					} while (v9);
				}
				++v7;
			} while (v7 < v5);
		}
	}

	inline void ConvertDXT5AToDXT5(void* pDst, const void* pSrc, unsigned int width, unsigned int height)
	{
		unsigned int v4; // er11@1
		unsigned int v5; // er10@1
		char* v6; // rbx@1
		char* v7; // rdi@1
		unsigned int v8; // er9@1
		__int64 v9; // r8@3
		__int64 v10; // rax@3
		char* v11; // rdx@3
		signed __int64 v12; // rax@3
		__int64 v13; // rcx@4

		v4 = (height + 3) >> 2;
		v5 = (width + 3) >> 2;
		v6 = (char*)pSrc;
		v7 = (char*)pDst;
		v8 = 0;
		if (v4)
		{
			do
			{
				if (v5)
				{
					v9 = v5;
					v10 = v5 * v8;
					v11 = &v6[8 * v10];
					v12 = (signed __int64)&v7[16 * v10];
					do
					{
						v13 = *(u64*)v11;
						v11 += 8;
						*(u64*)v12 = v13;
						*(u64*)(v12 + 8) = 0ll;
						v12 += 16ll;
						--v9;
					} while (v9);
				}
				++v8;
			} while (v8 < v4);
		}
	}

	//number of bytes for given texture info
	inline u32 T3SurfaceFormat_GetSurfaceBytes(T3TextureLayout layout, T3SurfaceFormat format, int width, int height, int depth, int arraySize, int numMipLevels)
	{
		int v7;
		int v8;
		int v9;
		T3SurfaceFormat v10;
		T3TextureLayout v11;
		T3SurfaceFormatDesc v12;
		int v13;
		int v14;
		int v15;
		int i;
		__int64 v17;
		signed int v18;
		int v19;
		int v20;
		int v21;
		int v22;
		int v23;
		int v24;
		int v25;

		v7 = numMipLevels;
		v8 = height;
		v9 = width;
		v10 = format;
		v11 = layout;
		if (!numMipLevels)
			v7 = GetNumMipLevelsNeeded(format, width, height);
		v12 = T3SurfaceFormat_GetDesc((T3SurfaceFormat)v10);
		v13 = depth;
		v14 = 0;
		v15 = 0;
		for (i = v12.mBitsPerPixel; v15 < v7; ++v15)
		{
			v17 = i * v9 * v8 * v13 + 7;
			v14 += (((v17 >> 24) & 7) + (signed int)v17) >> 3;
			if (v9 == 1 && v8 == 1 && v13 == 1)
				break;
			v18 = v9;
			v9 = 1;
			v19 = v18 / 2;
			if (v19 > 1)
				v9 = v19;
			v20 = v8;
			v21 = (unsigned __int64)v8 >> 32;
			v8 = 1;
			v22 = (v20 - v21) >> 1;
			if (v22 > 1)
				v8 = v22;
			v23 = v13;
			v24 = (unsigned __int64)v13 >> 32;
			v13 = 1;
			v25 = (v23 - v24) >> 1;
			if (v25 > 1)
				v13 = v25;
		}
		if (v11 == 1)
			v14 *= 6;
		return (unsigned int)(arraySize * v14);
	}

	inline bool T3SurfaceFormat_IsStencilFormat(T3SurfaceFormat format)
	{
		return format == 52 || format - 54 <= 1;
	}

	inline bool ComputeMipsData(MipSizeData* pMipSizeData, T3SurfaceFormatDesc* formatDesc, unsigned int width, unsigned int height, unsigned int maxMips, bool bLinear, unsigned int blockAlignment)
	{
		signed int v7; // ebx@1
		signed int v8; // edi@1
		T3SurfaceFormatDesc* v9; // r14@1
		__int64 v10; // rsi@3
		signed int v11; // er12@4
		unsigned int v12; // er15@7
		int v13; // er10@8
		signed int v14; // er8@8
		int v15; // edx@8
		unsigned int v16; // er9@10
		unsigned int v17; // er11@10
		unsigned int v18; // er10@10
		bool result; // al@19

		v7 = height;
		v8 = width;
		v9 = formatDesc;
		if (width && height)
		{
			v10 = 0ll;
			*(i64*)&pMipSizeData->mNumMips = 0llu;
			*(u64*)&pMipSizeData->mFormatDesc.mBlockWidth = *(u64*)&formatDesc->mBlockWidth;
			pMipSizeData->mFormatDesc.mMinBytesPerSurface = formatDesc->mMinBytesPerSurface;
			if (bLinear)
				v11 = (signed int)(formatDesc->mBlockWidth + width - 1) / formatDesc->mBlockWidth;
			else
				v11 = 1;
			if (maxMips)
			{
				v12 = ~(blockAlignment - 1);
				while (1)
				{
					v13 = v9->mBitsPerBlock;
					v14 = v9->mBlockHeight;
					v15 = v11;
					if (((v8 + v9->mBlockWidth - 1) / v9->mBlockWidth) > v11)
						v15 = (v8 + v9->mBlockWidth - 1) / v9->mBlockWidth;
					v16 = v12 & (v15 + blockAlignment - 1);
					pMipSizeData->mSizes[v10].mWidthBlocks = v16;
					v17 = v12 & ((v7 + v14 - 1) / v14 + blockAlignment - 1);
					v18 = (v16 * v13 + 7) >> 3;
					pMipSizeData->mSizes[pMipSizeData->mNumMips].mHeightBlocks = v17;
					*((u16*)&pMipSizeData->mFormatDesc.mBlockWidth + 10 * (pMipSizeData->mNumMips + 1)) = v8;
					pMipSizeData->mSizes[pMipSizeData->mNumMips].mHeightPix = v7;
					pMipSizeData->mSizes[pMipSizeData->mNumMips].mPitchBytes = v18;
					pMipSizeData->mSizes[pMipSizeData->mNumMips].mMipLevel = pMipSizeData->mNumMips;
					pMipSizeData->mSizes[pMipSizeData->mNumMips].mMipOffsetBytes = pMipSizeData->mTotalSizeBytes;
					pMipSizeData->mTotalSizeBytes += v17 * v18;
					if (v8 == 1 || v7 == 1)
						break;
					v8 /= 2;
					if (v8 < 1)
						v8 = 1;
					v7 /= 2;
					if (v7 < 1)
						v7 = 1;
					v10 = pMipSizeData->mNumMips + 1;
					pMipSizeData->mNumMips = v10;
					if ((unsigned int)v10 >= maxMips)
						goto LABEL_19;
				}
				*((u32*)&v10) = ++pMipSizeData->mNumMips;
			}
		LABEL_19:
			result = (u32)v10 != 0;
		}
		else
		{
			result = 0;
		}
		return result;
	}

	inline const char* GetTextureTypeName(T3Texture::TextureType type)
	{
		const char* result; // rax@2

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

	inline const char* T3TextureLayout_GetName(T3TextureLayout v)
	{
		unsigned __int32 v1; // ecx@2
		int v2; // ecx@3
		int v3; // ecx@4
		const char* result; // rax@6

		if (v)
		{
			v1 = v - 1;
			if (v1)
			{
				v2 = v1 - 1;
				if (v2)
				{
					v3 = v2 - 1;
					if (v3)
					{
						if (v3 == 1)
							result = "CubeArray";
						else
							result = "Unknown";
					}
					else
					{
						result = "2DArray";
					}
				}
				else
				{
					result = "3D";
				}
			}
			else
			{
				result = "Cube";
			}
		}
		else
		{
			result = "2D";
		}
		return result;
	}

	inline GFXPlatformAttributeParams& AddAttribute(T3GFXVertexState& state, GFXPlatformVertexAttribute attribute, unsigned int attributeIndex)
	{
		for(int i = 0; i < state.mAttributeCount; i++){
			if (state.mAttributes[i].mAttributeIndex == attributeIndex && state.mAttributes[i].mAttribute == attribute)
				return state.mAttributes[i];
		}
		state.mAttributes[state.mAttributeCount].mAttribute = attribute;
		state.mAttributes[state.mAttributeCount].mAttributeIndex = attributeIndex;
		state.mAttributeCount++;
		return state.mAttributes[state.mAttributeCount - 1];
	}

	inline void AddIndexBuffer(T3GFXVertexState& state, std::shared_ptr<T3GFXBuffer> pBuffer){
		if (state.mIndexBufferCount == 4)
			return;
		for (int i = 0; i < state.mIndexBufferCount; i++)
			if (state.mpIndexBuffer[i] == pBuffer)
				return;
		state.mpIndexBuffer[state.mIndexBufferCount++] = std::move(pBuffer);
	}

	inline void AddVertexBuffer(T3GFXVertexState& state, std::shared_ptr<T3GFXBuffer> pBuffer) {
		if (state.mIndexBufferCount == 32)
			return;
		for (int i = 0; i < state.mVertexBufferCount; i++)
			if (state.mpVertexBuffer[i] == pBuffer)
				return;
		state.mpVertexBuffer[state.mIndexBufferCount++] = std::move(pBuffer);
	}

	inline int GetVertexBufferIndex(T3GFXVertexState& state, std::shared_ptr<T3GFXBuffer> pBuffer) {
		for (int i = 0; i < state.mVertexBufferCount; i++)
			if (state.mpVertexBuffer[i] == pBuffer)
				return i;
		return -1;
	}

	inline int GetAttributeIndex(T3GFXVertexState& state, GFXPlatformVertexAttribute attribute, unsigned int attributeIndex) {
		for (int i = 0; i < state.mAttributeCount; i++)
			if (state.mAttributes[i].mAttributeIndex == attributeIndex && state.mAttributes[i].mAttribute == attribute)
				return i;
		return -1;
	}

	inline int GetIndexBufferIndex(T3GFXVertexState& state, std::shared_ptr<T3GFXBuffer> pBuffer) {
		for (int i = 0; i < state.mIndexBufferCount; i++)
			if (state.mpIndexBuffer[i] == pBuffer)
				return i;
		return -1;
	}

	inline float HalfToFloat(unsigned __int16 x)
	{
		u32 u = Float16ToFloat32(x);
		return *((float*)&u);
	}

	/*inline float TT_Broken_HalfToFloat(unsigned __int16 v)
	{
		unsigned int v2; // edx@1
		int v3; // ecx@1
		int v4; // eax@2
		__int64 result; // rax@10
		int v7; // [sp+8h] [bp+8h]@10

		v2 = v;
		v3 = v & 0x3FF;
		if ((v2 & 0x7C00) == 31744)
		{
			v4 = 143;
		}
		else if (v2 & 0x7C00)
		{
			v4 = (v2 >> 10) & 0x1F;
		}
		else if (v3)
		{
			v4 = 1;
			do
			{
				--v4;
				v3 *= 2;
			} while (!_bittest(&v3, 0xAu));
			v3 &= 0x3FFu;
		}
		else
		{
			v4 = -112;
		}
		result = (unsigned int)((v4 + 112) << 23);
		v7 = (v3 << 13) | result | (v2 << 16) & 0x80000000;
		__asm { movss   xmm0, [rsp + arg_0] }
		return result;
	}*/

	inline u16 FloatToHalf(float v)
	{
		int v1; // edx@1
		int v2; // er8@1
		signed __int16 v3; // ax@2
		unsigned __int16 result; // ax@4
		unsigned int v5; // edx@6

		v1 = *(u32*)&v & 0x7FFFFFFF;
		v2 = (*(u32*)&v >> 16) & 0x8000;
		if ((*(u32*)&v & 0x7FFFFFFFu) <= 0x477FE000)
		{
			if ((unsigned int)v1 >= 0x38800000)
				v5 = v1 - 939524096;
			else
				v5 = (*(u32*)&v & 0x7FFFFF | 0x800000u) >> (113 - ((unsigned int)v1 >> 23));
			result = v2 | ((((v5 >> 13) & 1) + v5 + 4095) >> 13) & 0x7FFF;
		}
		else
		{
			v3 = 0x7FFF;
			if ((*(u32*)&v & 0x7F800000) != 2139095040 || (*(u32*)&v & 0x7FFFFF) == 0)
				v3 = 31744;
			result = v2 | v3;
		}
		return result;
	}

	inline Vector4 ConvertToFloat(void* pSrc, GFXPlatformFormat format)
	{
		Vector4 dst{};
		void* v5; // rdi@1
		float v6; // xmm2_4@4
		signed int v7; // eax@4
		v5 = pSrc;
		if (format == eGFXPlatformFormat_F16x4)
		{
			dst.x = HalfToFloat(*(u16*)pSrc);
			dst.y = HalfToFloat(*((u16*)v5 + 1));
			dst.z = HalfToFloat(*((u16*)v5 + 2));
			dst.w = HalfToFloat(*((u16*)v5 + 3));
		}
		else
		{
			if (format == eGFXPlatformFormat_UN16x4)
			{
				v6 = 0.000015259022f;//1/65535
				dst.x = (float)(*(u16*)pSrc) * 0.000015259022f;
				dst.y = (float)*((u16*)pSrc + 1) * 0.000015259022f;
				dst.z = (float)*((u16*)pSrc + 2) * 0.000015259022f;
				v7 = *((u16*)pSrc + 3);
			}
			else
			{
				if (format != eGFXPlatformFormat_U8x4)
					return Vector4();
				v6 = 0.0039215689f;//1/255
				dst.x = (float)*(u8*)pSrc * 0.0039215689f;
				dst.y = (float)*((u8*)pSrc + 1) * 0.0039215689f;
				dst.z = (float)*((u8*)pSrc + 2) * 0.0039215689f;
				v7 = *((u8*)pSrc + 3);
			}
			dst.w = (float)v7 * v6;
		}
		return dst;
	}

	inline void  ConvertFromFloat(void* pDst, Vector4 psrc, GFXPlatformFormat format)
	{
		Vector4* v3; // rbx@1
		u16* v4; // rdi@1
		float v5; // xmm0_4@8
		float v6; // xmm1_4@8
		float v7; // xmm2_4@10
		float v8; // xmm1_4@12
		float v9; // xmm2_4@14
		float v10; // xmm1_4@15
		float v11; // xmm2_4@17
		float v12; // xmm1_4@19
		float v13; // xmm2_4@20
		float v14; // xmm1_4@20
		float v15; // xmm0_4@20
		float v16; // xmm3_4@24
		float v17; // xmm2_4@28
		float v18; // xmm3_4@32
		float v19; // xmm2_4@38
		float v20; // xmm3_4@42
		float v21; // xmm2_4@46
		float v22; // xmm1_4@51
		float v23; // xmm3_4@51
		float v24; // xmm0_4@51
		float v25; // xmm2_4@55
		float v26; // xmm0_4@60
		float v27; // xmm1_4@62
		float v28; // xmm2_4@64
		float v29; // xmm2_4@66
		float v30; // xmm1_4@66
		float v31; // xmm0_4@66
		float v32; // xmm3_4@70
		float v33; // xmm2_4@74
		float v34; // xmm3_4@78
		float v35; // xmm0_4@84
		float v36; // xmm1_4@84
		float v37; // xmm2_4@86
		float v38; // xmm1_4@88
		float v39; // xmm2_4@90
		float v40; // xmm1_4@93
		float v41; // xmm1_4@98
		float v42; // xmm2_4@100
		float v43; // xmm2_4@102
		float v44; // xmm3_4@106
		float v45; // xmm2_4@110
		float v46; // xmm1_4@115
		float v47; // xmm2_4@117
		float v48; // xmm1_4@119
		float v49; // xmm2_4@121
		float v50; // xmm1_4@123
		float v51; // xmm4_4@123
		float v52; // xmm2_4@123
		float v53; // xmm0_4@123
		signed int v54; // edx@137
		int v55; // edx@139
		signed int v56; // ecx@140
		int v57; // ecx@142
		float v58; // xmm4_4@145
		float v59; // xmm5_4@145
		float v60; // xmm3_4@145
		float v61; // xmm2_4@145
		float v62; // xmm1_4@145
		float v63; // xmm0_4@162
		float v64; // xmm1_4@164
		float v65; // xmm0_4@170
		float v66; // xmm1_4@173
		float v67; // xmm2_4@175
		float v68; // xmm1_4@178
		float v69; // xmm2_4@181

		v3 = &psrc;
		Vector4* src = &psrc;
		v4 = (u16*)pDst;
		switch (format)
		{
		case 1:
			*(float*)pDst = src->x;
			return;
		case 2:
			*(float*)pDst = src->x;
			*((u32*)pDst + 1) = (src->y);
			return;
		case 3:
			*(float*)pDst = src->x;
			*((u32*)pDst + 1) = (src->y);
			*((u32*)pDst + 2) = (src->z);
			return;
		case 4:
			*(float*)pDst = src->x;
			*((u32*)pDst + 1) = (src->y);
			*((u32*)pDst + 2) = (src->z);
			*((u32*)pDst + 3) = (src->w);
			return;
		case 5:
			*(u16*)pDst = FloatToHalf(src->x);
			v4[1] = FloatToHalf(v3->y);
			return;
		case 6:
			*(u16*)pDst = FloatToHalf(src->x);
			v4[1] = FloatToHalf(v3->y);
			v4[2] = FloatToHalf(v3->z);
			v4[3] = FloatToHalf(v3->w);
			return;
		case 32:
			v5 = 255.f;
			v6 = fmaxf(src->x, 0.0f);
			if (v6 >= 255.0f)
				v6 = 255.f;
			*(u8*)pDst = (signed int)std::floorf(v6);
			v7 = fmaxf(src->y, 0.0f);
			if (v7 >= 255.0f)
				v7 = 255.f;
			*((u8*)pDst + 1) = (signed int)std::floorf(v7);
			v8 = fmaxf(src->z, 0.0f);
			if (v8 >= 255.0f)
				v8 = 255.f;
			*((u8*)pDst + 2) = (signed int)std::floorf(v8);
			v9 = src->w;
			goto LABEL_181;
		case 38:
			v5 = 255.f;
			v10 = fmaxf(src->x * 255.0f, 0.0f);
			if (v10 >= 255.0f)
				v10 = 255.f;
			*(u8*)pDst = (signed int)std::floorf(v10);
			v11 = fmaxf(src->y * 255.0f, 0.0f);
			if (v11 >= 255.0f)
				v11 = 255.f;
			*((u8*)pDst + 1) = (signed int)std::floorf(v11);
			v12 = src->z;
			goto LABEL_178;
		case 31:
			v13 = src->x;
			v14 = -128.f;
			v15 = 127.f;
			if (src->x < -128.0f)
			{
				v13 = -128.f;
			}
			else if (v13 >= 127.0f)
			{
				v13 = 127.f;
			}
			*(u8*)pDst = (signed int)std::floorf(v13);
			v16 = src->y;
			if (v16 < -128.0f)
			{
				v16 = -128.f;
			}
			else if (v16 >= 127.0f)
			{
				v16 = 127.f;
			}
			*((u8*)pDst + 1) = (signed int)std::floorf(v16);
			v17 = src->z;
			if (v17 < -128.0f)
			{
				v17 = -128.f;
			}
			else if (v17 >= 127.0f)
			{
				v17 = 127.f;
			}
			*((u8*)pDst + 2) = (signed int)std::floorf(v17);
			v18 = src->w;
			goto LABEL_33;
		case 37:
			v15 = 127.f;
			v14 = -128.f;
			v19 = src->x * 127.0f;
			if (v19 < -128.0f)
			{
				v19 = -128.f;
			}
			else if (v19 >= 127.0f)
			{
				v19 = 127.f;
			}
			*(u8*)pDst = (signed int)std::floorf(v19);
			v20 = src->y * 127.0f;
			if (v20 < -128.0f)
			{
				v20 = -128.f;
			}
			else if (v20 >= 127.0f)
			{
				v20 = 127.f;
			}
			*((u8*)pDst + 1) = (signed int)std::floorf(v20);
			v21 = src->z * 127.0f;
			if (v21 < -128.0f)
			{
				v21 = -128.f;
			}
			else if (v21 >= 127.0f)
			{
				v21 = 127.f;
			}
			*((u8*)pDst + 2) = (signed int)std::floorf(v21);
			v18 = src->w * 127.0f;
		LABEL_33:
			if (v18 < v14)
			{
				v18 = v14;
			}
			else if (v18 >= v15)
			{
				*((u8*)pDst + 3) = (signed int)std::floorf(v15);
				return;
			}
			*((u8*)pDst + 3) = (signed int)std::floorf(v18);
			return;
		case 17:
			v22 = src->x;
			v23 = -32768.f;
			v24 = 32767.f;
			if (src->x < -32768.0f)
			{
				v22 = -32768.f;
			}
			else if (v22 >= 32767.0f)
			{
				v22 = 32767.f;
			}
			*(u16*)pDst = (signed int)std::floorf(v22);
			v25 = src->y;
			goto LABEL_56;
		case 18:
			v26 = 65535.f;
			v27 = fmaxf(src->x, 0.0f);
			if (v27 >= 65535.0f)
				v27 = 65535.f;
			*(u16*)pDst = (signed int)std::floorf(v27);
			v28 = fmaxf(src->y, 0.0f);
			if (v28 >= 65535.0f)
				goto LABEL_61;
			*((u16*)pDst + 1) = (signed int)std::floorf(v28);
			return;
		case 19:
			v29 = src->x;
			v30 = -32768.f;
			v31 = 32767.f;
			if (src->x < -32768.0f)
			{
				v29 = -32768.f;
			}
			else if (v29 >= 32767.0f)
			{
				v29 = 32767.f;
			}
			*(u16*)pDst = (signed int)std::floorf(v29);
			v32 = src->y;
			if (v32 < -32768.0f)
			{
				v32 = -32768.f;
			}
			else if (v32 >= 32767.0f)
			{
				v32 = 32767.f;
			}
			*((u16*)pDst + 1) = (signed int)std::floorf(v32);
			v33 = src->z;
			if (v33 < -32768.0f)
			{
				v33 = -32768.f;
			}
			else if (v33 >= 32767.0f)
			{
				v33 = 32767.f;
			}
			*((u16*)pDst + 2) = (signed int)std::floorf(v33);
			v34 = src->w;
			goto LABEL_79;
		case 20:
			v35 = 65535.f;
			v36 = fmaxf(src->x, 0.0f);
			if (v36 >= 65535.0f)
				v36 = 65535.f;
			*(u16*)pDst = (signed int)std::floorf(v36);
			v37 = fmaxf(src->y, 0.0f);
			if (v37 >= 65535.0f)
				v37 = 65535.f;
			*((u16*)pDst + 1) = (signed int)std::floorf(v37);
			v38 = fmaxf(src->z, 0.0f);
			if (v38 >= 65535.0f)
				v38 = 65535.f;
			*((u16*)pDst + 2) = (signed int)std::floorf(v38);
			v39 = fmaxf(src->w, 0.0f);
			if (v39 < 65535.0f)
				v35 = v39;
			goto LABEL_92;
		case 23:
			v24 = 32767.f;
			v23 = -32768.f;
			v40 = src->x * 32767.f;
			if (v40 < -32768.0f)
			{
				v40 = -32768.f;
			}
			else if (v40 >= 32767.0f)
			{
				v40 = 32767.f;
			}
			*(u16*)pDst = (signed int)std::floorf(v40);
			v25 = src->y * 32767.f;
		LABEL_56:
			if (v25 < v23)
			{
				v25 = v23;
			}
			else if (v25 >= v24)
			{
				*((u16*)pDst + 1) = (signed int)std::floorf(v24);
				return;
			}
			v26 = v25;
		LABEL_61:
			*((u16*)pDst + 1) = (signed int)std::floorf(v26);
			return;
		case 24:
			v26 = 65535.f;
			v41 = fmaxf(src->x * 65535.0f, 0.0f);
			if (v41 >= 65535.0f)
				v41 = 65535.f;
			*(u16*)pDst = (signed int)std::floorf(v41);
			v42 = fmaxf(src->y * 65535.0f, 0.0f);
			if (v42 >= 65535.0f)
				goto LABEL_61;
			*((u16*)pDst + 1) = (signed int)std::floorf(v42);
			return;
		case 25:
			v31 = 32767.f;
			v30 = -32768.f;
			v43 = src->x * 32767.f;
			if (v43 < -32768.0f)
			{
				v43 = -32768.f;
			}
			else if (v43 >= 32767.0f)
			{
				v43 = 32767.f;
			}
			*(u16*)pDst = (signed int)std::floorf(v43);
			v44 = src->y * 32767.f;
			if (v44 < -32768.0f)
			{
				v44 = -32768.f;
			}
			else if (v44 >= 32767.0f)
			{
				v44 = 32767.f;
			}
			*((u16*)pDst + 1) = (signed int)std::floorf(v44);
			v45 = src->z * 32767.f;
			if (v45 < -32768.0f)
			{
				v45 = -32768.f;
			}
			else if (v45 >= 32767.0f)
			{
				v45 = 32767.f;
			}
			*((u16*)pDst + 2) = (signed int)std::floorf(v45);
			v34 = src->w * 32767.f;
		LABEL_79:
			if (v34 < v30)
			{
				v34 = v30;
			}
			else if (v34 >= v31)
			{
				*((u16*)pDst + 3) = (signed int)std::floorf(v31);
				return;
			}
			*((u16*)pDst + 3) = (signed int)std::floorf(v34);
			return;
		case 26:
			v35 = 65535.f;
			v46 = fmaxf(src->x * 65535.f, 0.0f);
			if (v46 >= 65535.0f)
				v46 = 65535.f;
			*(u16*)pDst = (signed int)std::floorf(v46);
			v47 = fmaxf(src->y * 65535.f, 0.0f);
			if (v47 >= 65535.0f)
				v47 = 65535.f;
			*((u16*)pDst + 1) = (signed int)std::floorf(v47);
			v48 = fmaxf(src->z * 65535.f, 0.0f);
			if (v48 >= 65535.0f)
				v48 = 65535.f;
			*((u16*)pDst + 2) = (signed int)std::floorf(v48);
			v49 = fmaxf(src->w * 65535.f, 0.0f);
			if (v49 >= 65535.0f)
				LABEL_92:
			*((u16*)pDst + 3) = (signed int)std::floorf(v35);
			else
				*((u16*)pDst + 3) = (signed int)std::floorf(v49);
			return;
		case 39:
			v50 = src->x;
			v51 = src->z;
			v52 = src->y;
			v53 = 1.0f;
			if (src->x < -1.0f)
			{
				v50 = -1.0f;
			}
			else if (v50 >= 1.0f)
			{
				v50 = -1.0f;
			}
			if (v52 < -1.0f)
			{
				v52 = -1.0f;
			}
			else if (v52 >= 1.0f)
			{
				v52 = -1.0f;
			}
			if (v51 < -1.0f)
			{
				v51 = -1.0f;
			}
			else if (v51 >= 1.0f)
			{
				goto LABEL_136;
			}
			v53 = v51;
		LABEL_136:
			if (v50 == 0.0f)
				(v54) = ~(signed int)std::floorf(v50 * -1024.0f) + 1;
			else
				v54 = (signed int)std::floorf(v50 * 1023.0f);
			v55 = v54 & 0x7FF;
			if (v52 <= 0.0f)
				(v56) = ~(signed int)std::floorf(v52 * -1024.0f) + 1;
			else
				v56 = (signed int)std::floorf(v52 * 1023.0f);
			v57 = v56 & 0x7FF;
			if (v53 <= 0.0f)
				*(u32*)v4 = v55 | ((v57 | -2048 * (signed int)std::floorf(v53 * -512.0f)) << 11);
			else
				*(u32*)v4 = v55 | ((v57 | ((signed int)std::floorf(v53 * 511.0f) << 11)) << 11);
			return;
		case 40:
			v58 = src->x;
			v59 = src->w;
			v60 = src->z;
			v61 = src->y;
			v62 = 1.0f;
			if (src->x < -1.0f)
			{
				v58 = -1.0f;
			}
			else if (v58 >= 1.0f)
			{
				v58 = 1.0f;
			}
			if (v61 < -1.0f)
			{
				v61 = -1.0f;
			}
			else if (v61 >= 1.0f)
			{
				v61 = 1.0f;
			}
			if (v60 < -1.0f)
			{
				v60 = -1.0f;
			}
			else if (v60 >= 1.0f)
			{
				v60 = 1.0f;
			}
			if (v59 < -1.0f)
			{
				v59 = -1.0f;
			}
			else if (v59 >= 1.0f)
			{
				goto LABEL_162;
			}
			v62 = v59;
		LABEL_162:
			v63 = 511.0f;
		LABEL_163:
			*(u32*)pDst = (signed int)std::floorf(v58 * v63) & 0x3FF | (((signed int)std::floorf(v61 * v63) & 0x3FF | ((((signed int)std::floorf(v62) << 10) | (signed int)std::floorf(v60 * v63) & 0x3FF) << 10)) << 10);
			return;
		case 41:
			v64 = 1.0f;
			v58 = fmaxf(src->x, 0.0f);
			if (v58 >= 1.0f)
				v58 = 1.0f;
			v61 = fmaxf(src->y, 0.0f);
			if (v61 >= 1.0f)
				v61 = 1.0f;
			v60 = fmaxf(src->z, 0.0f);
			if (v60 >= 1.0f)
				v60 = 1.0f;
			v65 = fmaxf(src->w, 0.0f);
			if (v65 < 1.0f)
				v64 = v65;
			v63 = 1023.f;
			v62 = v64 * 3.f;
			goto LABEL_163;
		case 42:
			v5 = 255.f;
			v66 = fmaxf(src->z * 255.f, 0.0f);
			if (v66 >= 255.0f)
				v66 = 255.f;
			*(u8*)pDst = (signed int)std::floorf(v66);
			v67 = fmaxf(src->y * 255.f, 0.0f);
			if (v67 >= 255.0f)
				v67 = 255.f;
			*((u8*)pDst + 1) = (signed int)std::floorf(v67);
			v12 = src->x;
		LABEL_178:
			v68 = fmaxf(v12 * v5, 0.0f);
			if (v68 >= v5)
				v68 = v5;
			*((u8*)pDst + 2) = (signed int)std::floorf(v68);
			v9 = src->w * v5;
		LABEL_181:
			v69 = fmaxf(v9, 0.0f);
			if (v69 < v5)
				v5 = v69;
			*((u8*)pDst + 3) = (signed int)std::floorf(v5);
			return;
		default:
			return;
		}
	}

	inline u32 GetStride(GFXPlatformFormat fmt){
		//specials (dxgi see that)
		if (fmt == 0x27||fmt==0x28||fmt==0x29)
			return 4;
		if (fmt == 0x2a)
			return 16;//guessing 16.
		if (fmt == 1 || fmt == 7 || fmt == 8 || fmt == 5 || fmt == 0x11 || fmt == 0x12 || fmt == 0x17 || fmt == 0x18 || fmt == 0x1f || fmt == 0x20 || fmt == 0x25 || fmt == 0x26)
			return 4;//all 4 byte types
		if (fmt == 2 || fmt == 6 || fmt == 9 || fmt == 10 || fmt == 0x13 || fmt == 0x14 || fmt == 0x19 || fmt == 0x1a)
			return 8;//all 8 byte types
		if (fmt == 15 || fmt == 16 || fmt == 0x1d || fmt == 0x1e || fmt == 0x16 || fmt == 0x15 || fmt == 0x23 || fmt == 0x24)
			return 2;//all 2 byte types
		if (fmt == 0x1b/*omg lua*/ || fmt == 0x1c || fmt == 0x21 || fmt == 0x22)
			return 1;//1 byte
		if (fmt == 3 || fmt == 0xb || fmt == 0xc)
			return 12;//12 bytes
		if (fmt == 4 || fmt == 0xd || fmt == 0xe)
			return 16;//16
		return 0;//!!
	}

	inline std::shared_ptr<T3GFXBuffer> AllocateCPUBuffer(u32 count ,GFXPlatformBufferUsage usage, GFXPlatformFormat fmt){
		auto ptr = std::make_shared<T3GFXBuffer>();
		ptr->mBufferFormat = fmt;
		ptr->mBufferUsage = usage;
		ptr->mCount = count;
		ptr->mStride = GetStride(fmt);
		ptr->mResourceUsage = GFXPlatformResourceUsage::eGFXPlatformUsage_Immutable;
		ptr->mpCPUBuffer = calloc(1, ptr->mCount * ptr->mStride);
		return ptr;
	}

	inline void* AllocateBuffer(std::shared_ptr<T3GFXBuffer>& pBuffer){
		if (pBuffer->mpCPUBuffer)
			free(pBuffer->mpCPUBuffer);
		pBuffer->mStride = GetStride(pBuffer->mBufferFormat);
		pBuffer->mpCPUBuffer = calloc(1, pBuffer->mCount * pBuffer->mStride);
		return pBuffer->mpCPUBuffer;
	}

	//Returns number of entries.
	inline u32 SetVertexFormat(RenderVertexFormat format, T3MeshBufferFormatEntry* entries) {
		u32 ret = 1;
		if (format == eVertexFormat_Position3) {
			entries->mAttributeIndex = 0;
			entries->mFormat = eGFXPlatformFormat_F32x3;
			entries->mAttribute = eGFXPlatformAttribute_Position;
		}
		else if(format == eVertexFormat_Position2){
			entries->mAttributeIndex = 0;
			entries->mFormat = eGFXPlatformFormat_F32x2;
			entries->mAttribute = eGFXPlatformAttribute_Position;
		}
		else if (format == eVertexFormat_Color) {
			entries->mAttributeIndex = 0;
			entries->mFormat = eGFXPlatformFormat_UN8x4;
			entries->mAttribute = eGFXPlatformAttribute_Color;
		}
		else if (format == eVertexFormat_Texture2) {
			entries->mAttributeIndex = 0;
			entries->mFormat = eGFXPlatformFormat_F32x2;
			entries->mAttribute = eGFXPlatformAttribute_TexCoord;
		}
		else if (format == eVertexFormat_OutlineSize) {
			entries->mAttributeIndex = 1;
			entries->mFormat = eGFXPlatformFormat_UN8x4;
			entries->mAttribute = eGFXPlatformAttribute_Color;
		}
		else if (format == eVertexFormat_BlendWeight) {
			entries->mAttributeIndex = 0;
			entries->mFormat = eGFXPlatformFormat_F32;
			entries->mAttribute = eGFXPlatformAttribute_BlendWeight;
		}
		else if (format == eVertexFormat_Position3Color) {
			//POS3
			entries->mAttributeIndex = 0;
			entries->mFormat = eGFXPlatformFormat_F32x3;
			entries->mAttribute = eGFXPlatformAttribute_Position;
			//COLOR
			entries[1].mAttributeIndex = 0;
			entries[1].mFormat = eGFXPlatformFormat_UN8x4;
			entries[1].mAttribute = eGFXPlatformAttribute_Color;

			ret = 2;
		}
		else if (format == eVertexFormat_Position2Color) {
			//POS
			entries->mAttributeIndex = 0;
			entries->mFormat = eGFXPlatformFormat_F32x2;
			entries->mAttribute = eGFXPlatformAttribute_Position;
			//COLOR
			entries[1].mAttributeIndex = 0;
			entries[1].mFormat = eGFXPlatformFormat_UN8x4;
			entries[1].mAttribute = eGFXPlatformAttribute_Color;

			ret = 2;
		}
		else if (format == eVertexFormat_Position2Texture2) {
			//POS
			entries->mAttributeIndex = 0;
			entries->mFormat = eGFXPlatformFormat_F32x2;
			entries->mAttribute = eGFXPlatformAttribute_Position;
			//TEX
			entries[1].mAttributeIndex = 0;
			entries[1].mFormat = eGFXPlatformFormat_F32x2;
			entries[1].mAttribute = eGFXPlatformAttribute_TexCoord;

			ret = 2;
		}
		else if (format == eVertexFormat_Position3Texture2) {
			//POS
			entries->mAttributeIndex = 0;
			entries->mFormat = eGFXPlatformFormat_F32x3;
			entries->mAttribute = eGFXPlatformAttribute_Position;
			//TEX
			entries[1].mAttributeIndex = 0;
			entries[1].mFormat = eGFXPlatformFormat_F32x2;
			entries[1].mAttribute = eGFXPlatformAttribute_TexCoord;

			ret = 2;
		}
		else if (format == eVertexFormat_Position3Texture3) {
			//POS
			entries->mAttributeIndex = 0;
			entries->mFormat = eGFXPlatformFormat_F32x3;
			entries->mAttribute = eGFXPlatformAttribute_Position;
			//TEX
			entries[1].mAttributeIndex = 0;
			entries[1].mFormat = eGFXPlatformFormat_F32x3;
			entries[1].mAttribute = eGFXPlatformAttribute_TexCoord;

			ret = 2;
		}
		else if (format == eVertexFormat_Position2Texture2Color) {
			//POS
			entries->mAttributeIndex = 0;
			entries->mFormat = eGFXPlatformFormat_F32x2;
			entries->mAttribute = eGFXPlatformAttribute_Position;
			//TEX
			entries[1].mAttributeIndex = 0;
			entries[1].mFormat = eGFXPlatformFormat_F32x2;
			entries[1].mAttribute = eGFXPlatformAttribute_TexCoord;
			//COL
			entries[2].mAttributeIndex = 0;
			entries[2].mFormat = eGFXPlatformFormat_UN8x4;
			entries[2].mAttribute = eGFXPlatformAttribute_Color;

			ret = 3;
		}
		else if (format == eVertexFormat_Position3Texture2Color) {
			//POS
			entries->mAttributeIndex = 0;
			entries->mFormat = eGFXPlatformFormat_F32x3;
			entries->mAttribute = eGFXPlatformAttribute_Position;
			//TEX
			entries[1].mAttributeIndex = 0;
			entries[1].mFormat = eGFXPlatformFormat_F32x2;
			entries[1].mAttribute = eGFXPlatformAttribute_TexCoord;
			//COL
			entries[2].mAttributeIndex = 0;
			entries[2].mFormat = eGFXPlatformFormat_UN8x4;
			entries[2].mAttribute = eGFXPlatformAttribute_Color;

			ret = 3;
		}
		else if (format == eVertexFormat_EnlightenUV) {
			entries->mAttributeIndex = 4;
			entries->mFormat = eGFXPlatformFormat_SN16x2;
			entries->mAttribute = eGFXPlatformAttribute_TexCoord;
		}
		return ret;
	}

	inline std::shared_ptr<T3GFXBuffer> CreateConstantVertexBuffer(GFXPlatformFormat format, Vector4 value)
	{
		std::shared_ptr<T3GFXBuffer> pBuffer = std::make_shared<T3GFXBuffer>();
		pBuffer->mResourceUsage = eGFXPlatformUsage_Immutable;
		pBuffer->mBufferUsage = (GFXPlatformBufferUsage)(eGFXPlatformBuffer_SingleValue | eGFXPlatformBuffer_Vertex);
		pBuffer->mBufferFormat = format;
		pBuffer->mCount = 1;
		pBuffer->mStride = GetStride(format);
		pBuffer->mpCPUBuffer = calloc(1, pBuffer->mStride * 1);
		ConvertFromFloat(pBuffer->mpCPUBuffer, value, format);
		return pBuffer;
	}

	inline std::shared_ptr<T3GFXBuffer> CreateConstantVertexBuffer(T3GFXVertexState& state, T3MeshBufferFormatEntry& entry, Vector4 value)
	{
		std::shared_ptr<T3GFXBuffer> pBuffer = CreateConstantVertexBuffer(entry.mFormat, value);
		GFXPlatformAttributeParams& attrib = AddAttribute(state, entry.mAttribute, entry.mAttributeIndex);
		attrib.mFormat = entry.mFormat;
		attrib.mFrequency = eGFXPlatformFrequency_PerVertex;
		attrib.mBufferOffset = 0;
		attrib.mBufferIndex = state.mVertexBufferCount;
		AddVertexBuffer(state, pBuffer);
		UpdateCachedVertexLayout(state);
		return pBuffer;
	}

	inline void CreateConstantStaticVertices(T3GFXVertexState& state, RenderVertexFormat format, Vector4 value)
	{
		T3MeshBufferFormatEntry pEntries[1]{};
		SetVertexFormat(format, pEntries);
		CreateConstantVertexBuffer(state, pEntries[0], value);
	}

	inline std::shared_ptr<T3GFXBuffer> CreateTypedBuffer(unsigned int bufferUsage, unsigned int count, GFXPlatformFormat format, GFXPlatformResourceUsage usage)
	{
		std::shared_ptr<T3GFXBuffer> pBuffer = std::make_shared<T3GFXBuffer>();
		pBuffer->mBufferUsage = (GFXPlatformBufferUsage)bufferUsage;
		pBuffer->mCount = count;
		pBuffer->mStride = GetStride(format);
		pBuffer->mBufferFormat = format;
		pBuffer->mResourceUsage = usage;
		return pBuffer;
	}

	inline std::shared_ptr<T3GFXBuffer> CreateIndexBuffer(unsigned int count, GFXPlatformFormat format, GFXPlatformResourceUsage usage)
	{
		return CreateTypedBuffer(GFXPlatformBufferUsage::eGFXPlatformBuffer_Index, count, format, usage);
	}

	inline std::shared_ptr<T3GFXBuffer> CreateIndirectDrawBuffer(unsigned int drawCount)
	{
		return CreateTypedBuffer(eGFXPlatformBuffer_DrawIndirectArgs | eGFXPlatformBuffer_ShaderWrite, 4 * drawCount, eGFXPlatformFormat_U32, eGFXPlatformUsage_GPUWritable);
	}

	inline std::shared_ptr<T3GFXBuffer> CreateRawBuffer(unsigned int bufferUsage, unsigned int size)
	{
		return CreateTypedBuffer(bufferUsage | eGFXPlatformBuffer_ShaderRawAccess, (size+3)>>2, eGFXPlatformFormat_U32, eGFXPlatformUsage_GPUWritable);
	}

	inline std::shared_ptr<T3GFXBuffer> CreateStructuredBuffer(unsigned int bufferUsage, unsigned int count, unsigned int stride, GFXPlatformResourceUsage usage)
	{
		std::shared_ptr<T3GFXBuffer> pBuffer = std::make_shared<T3GFXBuffer>();
		pBuffer->mBufferUsage = (GFXPlatformBufferUsage)bufferUsage;
		pBuffer->mCount = count;
		pBuffer->mStride = stride;
		pBuffer->mBufferFormat = eGFXPlatformFormat_None;
		pBuffer->mResourceUsage = usage;
		return pBuffer;
	}

	inline std::shared_ptr<T3GFXBuffer> CreateVertexBuffer(unsigned int count, unsigned int vertSize, GFXPlatformResourceUsage usage, unsigned int bufferUsage)
	{
		std::shared_ptr<T3GFXBuffer> pBuffer = std::make_shared<T3GFXBuffer>();
		pBuffer->mBufferUsage = (GFXPlatformBufferUsage)(bufferUsage | GFXPlatformBufferUsage::eGFXPlatformBuffer_Vertex);
		pBuffer->mCount = count;
		pBuffer->mStride = vertSize;
		pBuffer->mBufferFormat = eGFXPlatformFormat_None;
		pBuffer->mResourceUsage = usage;
		return pBuffer;
	}

	inline std::shared_ptr<T3GFXBuffer> CreateVertexBuffer(T3GFXVertexState& state, unsigned int count,
		T3MeshBufferFormatEntry* pEntries, unsigned int entryCount, GFXPlatformResourceUsage usage, GFXPlatformVertexFrequency freq, unsigned int bufferUsage, int* pOutEntryAttrIndex){
		int totalBufSize = 0;
		for(int i = 0; i < entryCount; i++){
			GFXPlatformAttributeParams& params = AddAttribute(state, pEntries[i].mAttribute, pEntries[i].mAttributeIndex);
			params.mBufferIndex = state.mVertexBufferCount;//will be incremented
			params.mBufferOffset = totalBufSize;
			params.mFrequency = freq;
			params.mFormat = pEntries[i].mFormat;
			totalBufSize = (totalBufSize + GetStride(params.mFormat) + 3) & ~0x3u;//ensure alignment of 8 (log2_8 is 3)
			if (pOutEntryAttrIndex)
				*pOutEntryAttrIndex = i;
		}
		std::shared_ptr<T3GFXBuffer> pBuffer = CreateVertexBuffer(count, totalBufSize, usage, bufferUsage);
		if (entryCount == 1 && !(bufferUsage & eGFXPlatformBuffer_ShaderRawAccess))
			pBuffer->mBufferFormat = pEntries[0].mFormat;
		state.mpVertexBuffer[state.mVertexBufferCount++] = pBuffer;
		UpdateCachedVertexLayout(state);
		return pBuffer;
	}

	//pBuffer should be ref to one of mpIndexBuffer
	inline u16* BeginStaticIndices(std::shared_ptr<T3GFXBuffer>& pBuffer, int numIndices){
		if(!pBuffer)
			pBuffer = CreateIndexBuffer(numIndices, eGFXPlatformFormat_U16, eGFXPlatformUsage_Immutable);
		u16* cpuBuffer = (u16*)calloc(1, 2 * numIndices);
		if (pBuffer->mpCPUBuffer)
			free(pBuffer->mpCPUBuffer);
		pBuffer->mpCPUBuffer = cpuBuffer;
		return cpuBuffer;
	}

	inline u16* BeginStaticIndices(T3GFXVertexState& state, int numIndices){
		state.mIndexBufferCount = 1;
		return BeginStaticIndices(state.mpIndexBuffer[0], numIndices);
	}

	inline std::shared_ptr<T3GFXBuffer> CreateVertexBuffer(T3GFXVertexState& state, unsigned int count, RenderVertexFormat format, GFXPlatformResourceUsage usage){
		T3MeshBufferFormatEntry pEntries[32]{};
		u32 num = SetVertexFormat(format, pEntries);
		return num ? CreateVertexBuffer(state, count, pEntries, num, usage, eGFXPlatformFrequency_PerVertex, 0, 0) : 0;
	}

	inline void* BeginStaticVertices(T3GFXVertexState& state, RenderVertexFormat format, unsigned int numVerts)
	{
		auto buf = CreateVertexBuffer(state, numVerts, format, eGFXPlatformUsage_Immutable);
		buf->mpCPUBuffer = calloc(buf->mCount, buf->mStride);
		return buf->mpCPUBuffer;
	}

	inline void CopyVertexState(T3GFXVertexState& src, T3GFXVertexState& dst){
		dst.mIndexBufferCount = src.mIndexBufferCount;
		dst.mAttributeCount = src.mAttributeCount;
		dst.mVertexBufferCount = src.mVertexBufferCount;
		for(int i = 0; i < src.mAttributeCount; i++){
			dst.mAttributes[i] = src.mAttributes[i];
		}
		for(int i = 0; i < src.mIndexBufferCount; i++){
			dst.mpIndexBuffer[i] = src.mpIndexBuffer[i];//add ref count
			dst.mpCachedVertexLayout[i] = src.mpCachedVertexLayout[i];
		}
		for(int i = 0; i < src.mVertexBufferCount; i++){
			dst.mpVertexBuffer[i] = src.mpVertexBuffer[i];
		}
	}

	//In Degrees. pVerts has min 33 elements for wire, 66 for filled 
	inline void SetArcVerts(RenderVertex_Position3* pVerts, RenderPrimitiveStyle style, float angleStart, float angleEnd){
		float until = angleEnd - 0.001f;
		float increment = (angleEnd - angleStart) * 0.03125f;//1/32: 32 frames
		float counter = angleStart;
		do {
			float fracSin = sinf(/*360/pi (angle to radians)*/0.0087266462f * counter);
			float fracCos = cosf(/*360/pi (angle to radians)*/0.0087266462f * counter);
			Vector3 tmpV = Vector3::Down * fracSin;
			float mag = Vector4(tmpV, fracCos).Magnitude();
			tmpV = tmpV * mag;
			Quaternion tmpQ{ tmpV.x, tmpV.y, tmpV.z, mag == 0.f ? 1.f : mag * fracCos };
			*((Vector3*)pVerts++) = Vector3::Forward * tmpQ;
			if (style == eRenderStyle_Filled)
				*((Vector3*)pVerts++) = Vector3(0.f, 0.f, 0.f);
			counter += increment;
		} while (counter < until);
		Quaternion tmpQ{ Vector3::Down, angleEnd * 0.01745292f/*to rads*/ };
		*((Vector3*)pVerts++) = Vector3::Forward * tmpQ;
		if (style == eRenderStyle_Filled)
			*((Vector3*)pVerts++) = Vector3(0.f, 0.f, 0.f);
	}

	inline std::string BuildSuffix(T3EffectCacheContext& context, const EffectFeatures& feat){
		std::string res{};
		if (!context._OK())
			return res;
		for (int i = 0; i < context.mpVersionDatabase->mFeatures.size(); i++) {
			if (feat[(T3EffectFeature)i])
				res += context.mpVersionDatabase->mFeatures[i].mSuffix;
		}
		return res;
	};

	inline std::string BuildQualityList(T3EffectCacheContext& context, const EffectQualities& validQuality) {
		std::string res{};
		if (!context._OK())
			return res;
		for (int i = 0; i < context.mpVersionDatabase->mQualities.size(); i++) {
			if (validQuality[(T3EffectQuality)i])
				res += context.mpVersionDatabase->mQualities[i].mSuffix;
		}
		return res;
	};

	inline std::string BuildName(T3EffectCacheContext& context, T3EffectType effectType, const EffectFeatures& feat, const EffectQualities& validQuality, u64 materialCrc,
		bool bDebug, const char* szExt){
		std::string res{};
		char tmp[64];
		T3EffectDesc desc = context.mpVersionDatabase->mEffects[(int)effectType];
		if (desc.mName)
			res = desc.mName;
		res += BuildSuffix(context, feat);
		if(materialCrc){
			sprintf_s(tmp, "_M%llx", materialCrc);
			res += tmp;
		}
		res += BuildQualityList(context, validQuality);
		if (bDebug)
			res += "_DEBUG";
		//res += "_V[RenderDevice::GetFeatureLevel()]";//not impl
		if (szExt)
			res += szExt;
		return res;
	};

	/*

	inline BitSet<enum T3EffectQuality, 5, 0> GetAllEffectQuality(){
		BitSet<enum T3EffectQuality, 5, 0> set{};
		for (int i = 0; i < eEffectQuality_Count; i++)
			set.Set((T3EffectQuality)i, true);
		return set;//or set.mwords[0]=31;
	};

	inline BitSet<enum T3EffectFeature, 84, 0> GetStaticFeatures()
	{
		BitSet<enum T3EffectFeature, 84, 0> set{};
		set.mWords[0] = 0xFFFF'FFFF;
		set.mWords[1] = 0x3F'FFFF;
		set.mWords[2] = 0;
 		return set;
	}

	inline BitSet<enum T3EffectFeature, 30, 54> GetAllDynamicFeatures()
	{
		BitSet<enum T3EffectFeature, 30, 54> set{};
		set.mWords[0] = 0x3FFF'FFFF;
		return set;
	}

	inline BitSet<enum T3EffectFeature, 84, 0> GetDynamicFeatures()
	{
		BitSet<enum T3EffectFeature, 84, 0> set{};
		set.mWords[0] = 0;
		set.mWords[1] = 0xFFC0'0000;
		set.mWords[2] = 0xF'FFFF;
		return set;
	}

	inline BitSet<enum T3EffectFeature, 54, 0> GetAllStaticFeatures()
	{
		BitSet<enum T3EffectFeature, 54, 0> set{};
		set.mWords[0] = 0xFFFF'FFFF;
		set.mWords[1] = 0x3F'FFFF;
		return set;
	}

	inline T3EffectFeature GetDynamicFeatureFromTagStr(String tagName){
		for(int i = eEffectFeature_FirstDynamic; i < eEffectFeature_LastDynamic; i++){
			if (tagName == sFeatureDesc[i].mTag)
				return (T3EffectFeature)i;
		}
		return eEffectFeature_None;
	}

	inline BitSet<enum T3EffectFeature, 84, 0> GetDynamicFeatures(BitSet<enum T3EffectFeature, 84, 0> features)
	{
		BitSet<enum T3EffectFeature, 84, 0> set{};
		set.mWords[0] = 0 | features.mWords[0];
		set.mWords[1] = 0xFFC0'0000 | features.mWords[1];
		set.mWords[2] = 0xF'FFFF | features.mWords[2];
		return set;
	}

	inline BitSet<enum T3EffectFeature, 54, 0> GetValidStaticFeatures(BitSet<enum T3EffectFeature, 54, 0>& features, T3EffectQuality quality){
		BitSet<enum T3EffectFeature, 54, 0> result{};
		for (int i = eEffectFeature_FirstStatic; i < eEffectFeature_LastStatic; i++) {
			if(sFeatureDesc[i].mExcludeQuality[quality])
				continue;
			result.Set((T3EffectFeature)i, 1);
		}
		return result;
	};

	inline BitSet<enum T3EffectFeature, 54, 0> GetValidStaticFeatures(T3EffectType effect, BitSet<enum T3EffectFeature, 54, 0>& features, T3EffectQuality quality) {
		BitSet<enum T3EffectFeature, 54, 0> result = GetValidStaticFeatures(features, quality);
		result.Set(sEffectDesc[effect].mFeatures[quality].mValidStaticFeatures);
		return result;
	};

	*/

	inline bool IsGFXBufferReallocation(GFXPlatformResourceUsage usage, unsigned int bufferCount, unsigned int updateCount)
	{
		return !(usage & (~GFXPlatformResourceUsage::eGFXPlatformUsage_GPUWritable)) || updateCount > bufferCount;
	}

	inline bool TestCap(GFXPlatformCapability cap) {
		return TelltaleToolLib_GetRenderAdaptersStruct()->TestCap(cap);
	}

	inline u64 GFXPlatformPrimitiveType_GetNumVerts(GFXPlatformPrimitiveType type, u32 numPrimitives)
	{
		u64 result; // rax@2

		switch (type)
		{
		case 1:
			result = numPrimitives;
			break;
		case 2:
			result = 2 * numPrimitives;
			break;
		case 3:
			result = numPrimitives + 1;
			break;
		case 4:
			result = 3 * numPrimitives;
			break;
		case 5:
		case 6:
			result = numPrimitives + 2;
			break;
		case 7:
			result = 4 * numPrimitives;
			break;
		case 8:
			result = 6 * numPrimitives;
			break;
		default:
			result = 0;
			break;
		}
		return result;
	}

	inline T3EffectQuality GetEffectQuality(RenderQualityType renderQuality) {
		if (renderQuality == eRenderQuality_High)
			return WDC_eEffectQuality_High;
		if (renderQuality == eRenderQuality_Mid)
			return WDC_eEffectQuality_Medium;
		if (renderQuality == eRenderQuality_LowPlus)
			return WDC_eEffectQuality_LowPlus;
		if (renderQuality == eRenderQuality_Low)
			return WDC_eEffectQuality_Low;
		return WDC_eEffectQuality_Lowest;
	}

}