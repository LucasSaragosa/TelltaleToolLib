#ifndef _T3EFFECT_CACHE_H
#define _T3EFFECT_CACHE_H

#include "../LibraryConfig.h"
#include "../TelltaleToolLibrary.h"
#include "../Types/DCArray.h"
#include "../Types/SArray.h"
#include "../Types/List.h"
#include "../Types/ObjectSpaceIntrinsics.h"
#include "../Meta.hpp"
#include "LinearHeap.h"
#include "../LinkedList.h"
#include "T3EffectUser.h"
#include <vector>

// ====================================================================================== RENDER TYPE ENUMS =========================================================================================

enum T3RenderStateValue
{
	eRSZBias_Value = 0x0,
	eRSZEnable_Value = 0x1,
	eRSZWriteEnable_Value = 0x2,
	eRSZFunc_Value = 0x3,
	eRSCullMode_Value = 0x4,
	eRSInvertCullMode_Value = 0x5,
	eRSAlphaToCoverage_Value = 0x6,
	eRSAlphaBlendEnable_Value = 0x7,
	eRSBlendOp_Value = 0x8,
	eRSSrcBlend_Value = 0x9,
	eRSDstBlend_Value = 0xA,
	eRSColorWriteEnable_Value = 0xB,
	eRSSeparateBlendEnable_Value = 0xC,
	eRSBlendOpAlpha_Value = 0xD,
	eRSSrcBlendAlpha_Value = 0xE,
	eRSDstBlendAlpha_Value = 0xF,
	eRSStencilEnable_Value = 0x10,
	eRSStencilFail_Value = 0x11,
	eRSStencilZFail_Value = 0x12,
	eRSStencilPass_Value = 0x13,
	eRSStencilFunc_Value = 0x14,
	eRSStencilRef_Value = 0x15,
	eRSStencilMask_Value = 0x16,
	eRSStencilWriteMask_Value = 0x17,
	eRSFillMode_Value = 0x18,
	eRSDepthBoundsEnable_Value = 0x19,
	eRSZOffset_Value = 0x1A,
	eRSScissorEnable_Value = 0x1B,
	eRSZClip_Value = 0x1C,
	eRSZInvert_Value = 0x1D,
	eRS_Count = 0x1E,
};

enum T3MeshBatchType {
	eMeshBatchType_Default = 0,
	eMeshBatchType_Shadow = 1,
	eMeshBatchType_Count = 2,
};

enum RenderHDRType
{
	eRenderHDR_None = 0xFFFFFFFF,
	eRenderHDR_10 = 0x0,
	eRenderHDR_10_Linear = 0x1,
	eRenderHDR_MAX = 0x2,
};

enum RenderFeatureType
{
	eRenderFeature_Unknown = 0xFFFFFFFF,
	eRenderFeature_DepthOfField = 0x0,
	eRenderFeature_Glow = 0x1,
	eRenderFeature_MotionBlur = 0x2,
	eRenderFeature_Brush = 0x3,
	eRenderFeature_LowResolutionAlpha = 0x4,
	eRenderFeature_FinalResolve = 0x5,
	eRenderFeature_BakedLighting = 0x6,
	eRenderFeature_NPRLines = 0x7,
	eRenderFeature_ComputeCullCascadeShadows = 0x8,
	eRenderFeature_DepthCullCascadeShadows = 0x9,
	eRenderFeature_OptimizeSDSMShadows = 0xA,
	eRenderFeature_Bokeh = 0xB,
	eRenderFeature_Enlighten = 0xC,
	eRenderFeature_InvertDepth = 0xD,
	eRenderFeature_ForwardKeyShadows = 0xE,
	eRenderFeature_Occlusion = 0xF,
	eRenderFeature_ComputeLightClustering = 0x10,
	eRenderFeature_LowResolutionLighting = 0x11,
	eRenderFeature_HiStencilShadows = 0x12,
	eRenderFeature_LOD = 0x13,
	eRenderFeature_ComputeSkinning = 0x14,
	eRenderFeature_ForceLinearDepth = 0x15,
	eRenderFeature_Count = 0x16,
};

enum T3MeshEndianType {
	eMeshEndian_Default = 0,
	eMeshEndian_Swap = 1,
	eMeshEndian_SwapBytesAsWord = 2
};

struct T3MeshTexCoordTransform {

	Vector2 mScale, mOffset;
	
	inline T3MeshTexCoordTransform() {
		mScale.x = 1.0f;
		mScale.y = 1.0f;
		mOffset.x = 0.0f;
		mOffset.y = 0.0f;
	}

};

enum GFXPlatformVertexFrequency
{
	eGFXPlatformFrequency_PerVertex = 0x0,
	eGFXPlatformFrequency_PerInstance = 0x1,
	eGFXPlatformFrequency_Count = 0x2,
};

enum GFXPlatformBufferUsage : int {
	eGFXPlatformBuffer_None = 0,
	eGFXPlatformBuffer_Vertex = 1,
	eGFXPlatformBuffer_Index = 2,
	eGFXPlatformBuffer_Uniform = 4,
	eGFXPlatformBuffer_ShaderRead = 8,
	eGFXPlatformBuffer_ShaderWrite = 0x10,
	eGFXPlatformBuffer_ShaderReadWrite = 0x18,
	eGFXPlatformBuffer_ShaderRawAccess = 0x20,
	eGFXPlatformBuffer_ShaderReadRaw = 0x28,
	eGFXPlatformBuffer_ShaderWriteRaw = 0x30,
	eGFXPlatformBuffer_ShaderReadWriteRaw = 0x38,
	eGFXPlatformBuffer_DrawIndirectArgs = 0x40,
	eGFXPlatformBuffer_SingleValue = 0x80,
};

enum T3MeshMaterialFlags : u32 {
	eMeshMaterialFlag_Embedded = 1,
	eMeshMaterialFlag_LODGenerated = 2
};

enum T3CameraFacingType {
	eCameraFacing_None = 0,
	eCameraFacing_Enable = 1,
	eCameraFacing_EnableY = 2,
	eCameraFacing_EnableLocalY = 3
};

enum T3MeshTextureType {
	eMeshTexture_LightMap = 0,
	eMeshTexture_ShadowMap = 1,
	eMeshTexture_Count = 2,
	eMeshTexture_None = 0xFFFFFFFF
};


enum T3MeshBatchUsageFlag {
	eMeshBatchUsage_Deformable = 1,
	eMeshBatchUsage_DeformableSingle = 2,
	eMeshBatchUsage_DoubleSided = 4,
	eMeshBatchUsage_TriangleStrip = 8
};

enum T3MaterialTextureParamType {
	eMaterialTextureParam_Unused = 0,
	eMaterialTextureParam_Count = 1,
	eMaterialTextureParam_None = 0xFFFFFFFF,
};

enum T3MaterialQualityType {
	eMaterialQuality_High = 0,
	eMaterialQuality_Low = 1,
	eMaterialQuality_Lowest = 2,
	eMaterialQuality_Count = 3
};

enum PlatformType {
	ePlatform_None = 0x0,
	ePlatform_All = 0x1,
	ePlatform_PC = 0x2,
	ePlatform_Wii = 0x3,
	ePlatform_Xbox = 0x4,
	ePlatform_PS3 = 0x5,
	ePlatform_Mac = 0x6,
	ePlatform_iPhone = 0x7,
	ePlatform_Android = 0x8,
	ePlatform_Vita = 0x9,
	ePlatform_Linux = 0xA,
	ePlatform_PS4 = 0xB,
	ePlatform_XBOne = 0xC,
	ePlatform_WiiU = 0xD,
	ePlatform_Win10 = 0xE,
	ePlatform_NX = 0xF,
	ePlatform_Count = 0x10,

};

struct EnumPlatformType : EnumBase {
	PlatformType mVal;
};

enum T3SurfaceFormat {
	eSurface_Unknown = 0xFFFFFFFF,
	eSurface_ARGB8 = 0x0,
	eSurface_ARGB16 = 0x1,
	eSurface_RGB565 = 0x2,
	eSurface_ARGB1555 = 0x3,
	eSurface_ARGB4 = 0x4,
	eSurface_ARGB2101010 = 0x5,
	eSurface_R16 = 0x6,
	eSurface_RG16 = 0x7,
	eSurface_RGBA16 = 0x8,
	eSurface_RG8 = 0x9,
	eSurface_RGBA8 = 0xA,
	eSurface_R32 = 0xB,
	eSurface_RG32 = 0xC,
	eSurface_RGBA32 = 0xD,
	eSurface_RGBA8S = 0xF,
	eSurface_A8 = 0x10,
	eSurface_L8 = 0x11,
	eSurface_AL8 = 0x12,
	eSurface_L16 = 0x13,
	eSurface_RG16S = 0x14,
	eSurface_RGBA16S = 0x15,
	eSurface_R16UI = 0x16,
	eSurface_RG16UI = 0x17,
	eSurface_R16F = 0x20,
	eSurface_RG16F = 0x21,
	eSurface_RGBA16F = 0x22,
	eSurface_R32F = 0x23,
	eSurface_RG32F = 0x24,
	eSurface_RGBA32F = 0x25,
	eSurface_RGBA1010102F = 0x26,
	eSurface_RGB111110F = 0x27,
	eSurface_RGB9E5F = 0x28,
	eSurface_DepthPCF16 = 0x30,
	eSurface_DepthPCF24 = 0x31,
	eSurface_Depth16 = 0x32,
	eSurface_Depth24 = 0x33,
	eSurface_DepthStencil32 = 0x34,
	eSurface_Depth32F = 0x35,
	eSurface_Depth32F_Stencil8 = 0x36,
	eSurface_Depth24F_Stencil8 = 0x37,
	//eSurface_BC1 = 0x40,
	//eSurface_BC2 = 0x41
	eSurface_DXT1 = 64,//BC1
	eSurface_DXT3 = 65,//BC2
	eSurface_DXT5 = 66,//BC3
	eSurface_DXT5A = 67,//BC4
	eSurface_DXTN = 68,//Xbox360 Xenos specific - for normal maps. meant to be DXN. 4 bits per pixel (64 per block) see https://fileadmin.cs.lth.se/cs/Personal/Michael_Doggett/talks/unc-xenos-doggett.pdf
	eSurface_CTX1 = 69,//Xbox360 Xenos specific - for normal maps. (see link above). 8 bits per pixel (128 per block)
	eSurface_BC6 = 70,
	eSurface_BC7 = 71,
	eSurface_PVRTC2 = 80,
	eSurface_PVRTC4 = 81,
	eSurface_PVRTC2a = 82,
	eSurface_PVRTC4a = 83,
	eSurface_ATC_RGB = 96,
	eSurface_ATC_RGB1A = 97,
	eSurface_ATC_RGBA = 98,
	eSurface_ETC1_RGB = 112,
	eSurface_ETC2_RGB = 113,
	eSurface_ETC2_RGB1A = 114,
	eSurface_ETC2_RGBA = 115,
	eSurface_ETC2_R = 116,
	eSurface_ETC2_RG = 117,
	eSurface_ATSC_RGBA_4x4 = 128,
	eSurface_R8 = 14,
};

struct T3ToonGradientRegion {
	Color mColor;
	float mSize;
	float mGradientSize;
};

enum T3EffectCompilerFlags
{
	T3EffectCompilerFlag_None = 0xFFFFFFFF,
	T3EffectCompilerFlag_Indirect = 0x0,
	T3EffectCompilerFlag_Count = 0x1,
};

enum T3TextureLayout {
	eTextureLayout_2D = 0,//slices=1
	eTextureLayout_Cube = 1,//slices=1
	eTextureLayout_3D = 2,//slices=depth
	eTextureLayout_2DArray = 3,//slices=arraysize
	eTextureLayout_CubeArray = 4,//slices=arraysize
	eTextureLayout_Count = 5,
	eTextureLayout_Unknown = 0xFFFFFFFF
};

enum T3SurfaceGamma {
	eSurfaceGamma_Linear = 0,
	eSurfaceGamma_sRGB = 1,
	sSurfaceGamma_Unknown = 0xFFFFFFFF
};

enum T3SurfaceMultisample {
	eSurfaceMultisample_None = 0,
	eSurfaceMultisample_2x = 1,
	eSurfaceMultisample_4x = 2,
	eSurfaceMultisample_8x = 3,
	eSurfaceMultisample_16x = 4
};

enum T3ResourceUsage {
	eResourceUsage_Static = 0,
	eResourceUsage_Dynamic = 1,
	eResourceUsage_System = 2,
	eResourceUsage_RenderTarget = 3,
	eResourceUsage_ShaderWrite = 4
};

enum RenderSwizzleType : char {
	eRenderSwizzle_X = 0x0,
	eRenderSwizzle_Y = 0x1,
	eRenderSwizzle_Z = 0x2,
	eRenderSwizzle_W = 0x3,
	eRenderSwizzle_Zero = 0x4,
	eRenderSwizzle_One = 0x5,
	eRenderSwizzle_R = 0x0,
	eRenderSwizzle_G = 0x1,
	eRenderSwizzle_B = 0x2,
	eRenderSwizzle_A = 0x3,
};

enum T3MaterialDomainType
{
	eMaterialDomain_None = 0xFFFFFFFF,
	eMaterialDomain_Mesh = 0x0,
	eMaterialDomain_Particle = 0x1,
	eMaterialDomain_Decal = 0x2,
	eMaterialDomain_Post = 0x3,
	eMaterialDomain_ExportMeshShader = 0x4,
	eMaterialDomain_Count = 0x5,
};

enum T3EffectType : u32 {
	WDC_eEffect_Mesh = 0x0,
	WDC_eEffect_Particle = 0x1,
	WDC_eEffect_ParticleGlow = 0x2,
	WDC_eEffect_ParticleForceLinearDepth = 0x3,
	WDC_eEffect_Particle_Xbox_Instanced = 0x4,
	WDC_eEffect_DeferredDecalGBuffer = 0x5,
	WDC_eEffect_DeferredDecalEmissive = 0x6,
	WDC_eEffect_DeferredDecalGlow = 0x7,
	WDC_eEffect_SceneSimple = 0x8,
	WDC_eEffect_SceneSimple_Texture = 0x9,
	WDC_eEffect_SceneSimple_Cone = 0x0A,
	WDC_eEffect_SceneNPRLinesCS_Rasterize = 0x0B,
	WDC_eEffect_SceneNPRLinesCS_ForceLinearDepth = 0x0C,
	WDC_eEffect_ScenePreZ = 0x0D,
	WDC_eEffect_ScenePreZLines = 0x0E,
	WDC_eEffect_ForceLinearDepth = 0x0F,
	WDC_eEffect_SceneShadowMap = 0x10,
	WDC_eEffect_SceneGlow = 0x11,
	WDC_eEffect_SceneGBuffer = 0x12,
	WDC_eEffect_SceneGBufferLines = 0x13,
	WDC_eEffect_SceneToonOutline = 0x14,
	WDC_eEffect_SceneToonOutline2 = 0x15,
	WDC_eEffect_SceneToonOutline2_GBuffer = 0x16,
	WDC_eEffect_FX_GlowPrepare = 0x17,
	WDC_eEffect_FX_Glow = 0x18,
	WDC_eEffect_FX_DepthOfFieldPrepare = 0x19,
	WDC_eEffect_FX_GaussianBlur = 0x1A,
	WDC_eEffect_FX_GaussianBlur_Glow = 0x1B,
	WDC_eEffect_FX_GaussianBlur_Outline = 0x1C,
	WDC_eEffect_FX_BlurUp = 0x1D,
	WDC_eEffect_FX_MotionBlur = 0x1E,
	WDC_eEffect_FX_MergeColorDepth = 0x1F,
	WDC_eEffect_FX_PostColor = 0x20,
	WDC_eEffect_DynamicLightMap = 0x21,
	WDC_eEffect_FX_RadialBlur = 0x22,
	WDC_eEffect_FX_FXAA = 0x23,
	WDC_eEffect_FX_SMAA_EdgeDetect = 0x24,
	WDC_eEffect_FX_SMAA_BlendWeight = 0x25,
	WDC_eEffect_FX_SMAA_Final = 0x26,
	WDC_eEffect_FX_GBufferResolve = 0x27,
	WDC_eEffect_FX_GBufferDownSample = 0x28,
	WDC_eEffect_FX_LinearDepthDownSample = 0x29,
	WDC_eEffect_FX_DepthDownSample = 0x2A,
	WDC_eEffect_FX_StencilMaskRestore = 0x2B,
	WDC_eEffect_FX_OcclusionFilter = 0x2C,
	WDC_eEffect_FX_Null = 0x2D,
	WDC_eEffect_FX_Copy = 0x2E,
	WDC_eEffect_FX_CopyDepth = 0x2F,
	WDC_eEffect_FX_CompositeAlpha = 0x30,
	WDC_eEffect_FX_CompositeQuarter = 0x31,
	WDC_eEffect_FX_CreateOutlineSrc = 0x32,
	WDC_eEffect_FX_CreateOutline = 0x33,
	WDC_eEffect_FX_BrushOutline = 0x34,
	WDC_eEffect_FX_BrushOutline_Simple = 0x35,
	WDC_eEffect_FX_BrushOffsetApply = 0x36,
	WDC_eEffect_FX_BrushOutlineBlurPrepare = 0x37,
	WDC_eEffect_FX_BrushOutlineBlur = 0x38,
	WDC_eEffect_FX_ApplyModulatedShadow = 0x39,
	WDC_eEffect_FX_FinalResolve = 0x3A,
	WDC_eEffect_FX_FinalDebugOverlay = 0x3B,
	WDC_eEffect_SceneText = 0x3C,
	WDC_eEffect_SceneSimple_Texture_AlphaOnly = 0x3D,
	WDC_eEffect_SceneHLSMovieYUV = 0x3E,
	WDC_eEffect_SceneHLSMovieRGB = 0x3F,
	WDC_eEffect_DirectionalLightShadow = 0x40,
	WDC_eEffect_DirectionalLightShadow_High = 0x41,
	WDC_eEffect_ProjectedTextureShadow = 0x42,
	WDC_eEffect_ProjectedTextureColor = 0x43,
	WDC_eEffect_EnvLightShadowMark_Point = 0x44,
	WDC_eEffect_EnvLightShadowMark_Spot = 0x45,
	WDC_eEffect_EnvLightShadowApply_Point = 0x46,
	WDC_eEffect_EnvLightShadowApply_Spot = 0x47,
	WDC_eEffect_EnvLightShadowApply_Directional = 0x48,
	WDC_eEffect_EnvLightShadowApply_DirectionalFade = 0x49,
	WDC_eEffect_EnvLightShadowApply_DirectionalBlend = 0x4A,
	WDC_eEffect_EnvLightShadowApply_SDSM_Directional = 0x4B,
	WDC_eEffect_EnvLightShadowApply_SDSM_DirectionalFade = 0x4C,
	WDC_eEffect_ShadowMark_SDSM = 0x4D,
	WDC_eEffect_ShadowVolumeApply = 0x4E,
	WDC_eEffect_ShadowTrace = 0x4F,
	WDC_eEffect_ShadowTrace_HBAOLow = 0x50,
	WDC_eEffect_ShadowTrace_HBAOMid = 0x51,
	WDC_eEffect_ShadowTrace_HBAOHigh = 0x52,
	WDC_eEffect_ShadowTrace_MSAA = 0x53,
	WDC_eEffect_ShadowTrace_HBAOLow_MSAA = 0x54,
	WDC_eEffect_ShadowTrace_HBAOMid_MSAA = 0x55,
	WDC_eEffect_ShadowTrace_HBAOHigh_MSAA = 0x56,
	WDC_eEffect_FX_DeferredFilter_Light = 0x57,
	WDC_eEffect_FX_DeferredFilter_Shadow = 0x58,
	WDC_eEffect_FX_DeferredFilter_LightShadow = 0x59,
	WDC_eEffect_FX_CreateOutlineSrcQuarter = 0x5A,
	WDC_eEffect_FX_NewGlowH1x = 0x5B,
	WDC_eEffect_FX_NewGlowV1x = 0x6B,
	WDC_eEffect_FX_NewGlowPresent = 0x7B,
	WDC_eEffect_FX_NewDepthOfFieldPreparE, alf = 0x7C,
	WDC_eEffect_FX_DepthOfFieldBlurH1x = 0x7D,
	WDC_eEffect_FX_DepthOfFieldBlurV1x = 0x7E,
	WDC_eEffect_FX_DepthOfFieldBlurH = 0x7F,
	WDC_eEffect_FX_DepthOfFieldBlurV = 0x80,
	WDC_eEffect_FX_DepthOfFieldBlurHNear = 0x81,
	WDC_eEffect_FX_DepthOfFieldBlurHFar = 0x82,
	WDC_eEffect_FX_DepthOfFieldBlurVNear = 0x83,
	WDC_eEffect_FX_DepthOfFieldBlurVFar = 0x84,
	WDC_eEffect_FX_ShadowFilter_Pass0 = 0x85,
	WDC_eEffect_FX_ShadowFilter_Pass0_TAA = 0x86,
	WDC_eEffect_FX_ShadowFilter_Blur0 = 0x87,
	WDC_eEffect_FX_ShadowFilter_Blur1 = 0x88,
	WDC_eEffect_FX_HBAOLowestQuality = 0x89,
	WDC_eEffect_FX_HBAOLowQuality = 0x8A,
	WDC_eEffect_FX_HBAOMediumQuality = 0x8B,
	WDC_eEffect_FX_HBAOHighQuality = 0x8C,
	WDC_eEffect_FX_HBAOHighestQuality = 0x8D,
	WDC_eEffect_FX_HBAODeinterleaveDepth = 0x8E,
	WDC_eEffect_FX_HBAODeinterleaveNormals = 0x8F,
	WDC_eEffect_FX_HBAOReinterleave = 0x90,
	WDC_eEffect_FX_SSLines = 0x91,
	WDC_eEffect_FX_SSLinesResolve_1x_Pass0 = 0x92,
	WDC_eEffect_FX_SSLinesResolve_1x_Pass1 = 0x93,
	WDC_eEffect_FX_SSLinesResolve_2x_Pass0 = 0x94,
	WDC_eEffect_FX_SSLinesResolve_2x_Pass1 = 0x95,
	WDC_eEffect_FX_SSLinesResolve_3x_Pass0 = 0x96,
	WDC_eEffect_FX_SSLinesResolve_3x_Pass1 = 0x97,
	WDC_eEffect_FX_SSLinesResolve_4x_Pass0 = 0x98,
	WDC_eEffect_FX_SSLinesResolve_4x_Pass1 = 0x99,
	WDC_eEffect_FX_MSAAResolve_2x = 0x9A,
	WDC_eEffect_FX_MSAAResolve_4x = 0x9B,
	WDC_eEffect_FX_MSAAResolve_8x = 0x9C,
	WDC_eEffect_FX_MSAASceneResolve_2x = 0x9D,
	WDC_eEffect_FX_MSAASceneResolve_4x = 0x9E,
	WDC_eEffect_FX_MSAASceneResolve_8x = 0x9F,
	WDC_eEffect_FX_SceneResolve_TAA = 0x0A0,
	WDC_eEffect_FX_MSAASceneResolve_TAA_2x = 0x0A1,
	WDC_eEffect_FX_MSAASceneResolve_TAA_4x = 0x0A2,
	WDC_eEffect_FX_MSAASceneResolve_TAA_8x = 0x0A3,
	WDC_eEffect_FX_MSAAGBufferResolve_2x = 0x0A4,
	WDC_eEffect_FX_MSAAGBufferResolve_4x = 0x0A5,
	WDC_eEffect_FX_MSAAGBufferResolve_8x = 0x0A6,
	WDC_eEffect_MakeMipLevel = 0x0A7,
	WDC_eEffect_MakeMipLevel_BlurH = 0x0A8,
	WDC_eEffect_MakeMipLevel_BlurV = 0x0A9,
	WDC_eEffect_MakeMipLevel_Array = 0x0AA,
	WDC_eEffect_CS_ComputeTest = 0x0AB,
	WDC_eEffect_CS_ApplySkinning_0Normal = 0x0AC,
	WDC_eEffect_CS_ApplySkinning_1Normal = 0x0AD,
	WDC_eEffect_CS_ApplySkinning_2Normal = 0x0AE,
	WDC_eEffect_CS_ApplySkinning_3Normal = 0x0AF,
	WDC_eEffect_CS_ApplySkinning_Float3Position_0Normal = 0x0B0,
	WDC_eEffect_CS_ApplySkinning_Float3Position_1Normal = 0x0B1,
	WDC_eEffect_CS_ApplySkinning_Float3Position_2Normal = 0x0B2,
	WDC_eEffect_CS_ApplySkinning_Float3Position_3Normal = 0x0B3,
	WDC_eEffect_CS_LineGeneration = 0x0B4,
	WDC_eEffect_CS_ClearIndirectDraw = 0x0B5,
	WDC_eEffect_CS_LightClustering2D = 0x0B6,
	WDC_eEffect_CS_LightClustering3D = 0x0B7,
	WDC_eEffect_CS_ShadowCascadeDistances_Manual = 0x0B8,
	WDC_eEffect_CS_ShadowCascadeDistances_Auto = 0x0B9,
	WDC_eEffect_CS_ShadowCascadeBounds_Pass0 = 0x0BA,
	WDC_eEffect_CS_ShadowCascadeBounds_PassN = 0x0BB,
	WDC_eEffect_CS_ShadowCascades = 0x0BC,
	WDC_eEffect_CS_ShadowCascades_ComputedBounds = 0x0BD,
	WDC_eEffect_CS_CullShadowCasters = 0x0BE,
	WDC_eEffect_CS_EnvLightMakeEVSM_Linear_Blur0 = 0x0BF,
	WDC_eEffect_CS_EnvLightMakeEVSM_Linear_Blur3x3 = 0x0C0,
	WDC_eEffect_CS_EnvLightMakeEVSM_Linear_Blur9x9 = 0x0C1,
	WDC_eEffect_CS_EnvLightMakeEVSM_NonLinear_Blur0 = 0x0C2,
	WDC_eEffect_CS_EnvLightMakeEVSM_NonLinear_Blur3x3 = 0x0C3,
	WDC_eEffect_CS_EnvLightMakeEVSM_NonLinear_Blur9x9 = 0x0C4,
	WDC_eEffect_CS_DepthReduction_Pass0_8x8 = 0x0C5,
	WDC_eEffect_CS_DepthReduction_Pass0_16x16 = 0x0C6,
	WDC_eEffect_CS_DepthReduction_PassN = 0x0C7,
	WDC_eEffect_CS_DepthReduction_PassFinal = 0x0C8,
	WDC_eEffect_CS_BokehSprites = 0x0C9,
	WDC_eEffect_FX_RasterizeBokeh = 0x0CA,
	WDC_eEffect_CS_EnlightenMakeDusters_Point = 0x0CB,
	WDC_eEffect_CS_EnlightenMakeDusters_Spot = 0x0CC,
	WDC_eEffect_CS_EnlightenMakeDusters_Directional = 0x0CD,
	WDC_eEffect_CS_WaveformMonitor = 0x0CE,
	WDC_eEffect_CS_EnvLightApplyDeferred = 0x0CF,
	WDC_eEffect_CS_EnvLightApplyDeferred_LowRes = 0x0D0,
	WDC_eEffect_BakeObjectNormals_Tool = 0x0D1,
	WDC_eEffect_BakeObjectNormals_NormalMap_Tool = 0x0D2,
	WDC_eEffect_BakeTangentNormals_Tool = 0x0D3,
	WDC_eEffect_BakeLighting_Tool = 0x0D4,
	WDC_eEffect_CS_LightmapBlurH_Tool = 0x0D5,
	WDC_eEffect_CS_LightmapBlurV_Tool = 0x0D6,
	WDC_eEffect_CS_LightmapFilter_Tool = 0x0D7,
	WDC_eEffect_CS_LightmapConvert_Tool = 0x0D8,
	WDC_eEffect_CS_BakeShadowVolume_Tool = 0x0D9,
	WDC_eEffect_CS_ComputeAsyncTest = 0x0DA,
	WDC_eEffect_Count = 0x0DB,
	WDC_eEffect_None = 0x0FFFFFFFF,
};

enum T3MaterialPropertyType
{
	eMaterialProperty_None = 0xFFFFFFFF,
	eMaterialProperty_GlossExponent = 0x0,
	eMaterialProperty_GlowIntensity = 0x1,
	eMaterialProperty_SpecularPower = 0x2,
	eMaterialProperty_OutlineColor = 0x3,
	eMaterialProperty_OutlineInvertColor = 0x4,
	eMaterialProperty_OutlineZRange = 0x5,
	eMaterialProperty_ToonGradientCutoff = 0x6,
	eMaterialProperty_LineGenerateCreases = 0x7,
	eMaterialProperty_LineGenerateBoundaries = 0x8,
	eMaterialProperty_LineCreaseAngle = 0x9,
	eMaterialProperty_LineSmoothJaggedCreaseAngle = 0xA,
	eMaterialProperty_LineGenerateSmooth = 0xB,
	eMaterialProperty_LineGenerateJagged = 0xC,
	eMaterialProperty_LineMinWidth = 0xD,
	eMaterialProperty_LineMaxWidth = 0xE,
	eMaterialProperty_LineWidthFromLighting = 0xF,
	eMaterialProperty_LineLightingType = 0x10,
	eMaterialProperty_LineLightingId = 0x11,
	eMaterialProperty_LinePatternRepeat = 0x12,
	eMaterialProperty_LitLineBias = 0x13,
	eMaterialProperty_LitLineScale = 0x14,
	eMaterialProperty_ConformNormal = 0x15,
	eMaterialProperty_NPRLineFalloff = 0x16,
	eMaterialProperty_NPRLineAlphaFalloff = 0x17,
	eMaterialProperty_DrawHiddenLines = 0x18,
	eMaterialProperty_AlphaMeshCullsLines = 0x19,
	eMaterialProperty_UseArtistNormal = 0x1A,
	eMaterialProperty_HorizonFade = 0x1B,
	eMaterialProperty_HairTerms = 0x1C,
	eMaterialProperty_ClothOffsetScale = 0x1D,
	eMaterialProperty_LightWrap = 0x1E,
	eMaterialProperty_Count = 0x1F,
};

enum T3MaterialOptionalPropertyType
{
	eMaterialOptionalProperty_None = 0xFFFFFFFF,
	eMaterialOptionalProperty_ToonShades = 0x0,
	eMaterialOptionalProperty_Count = 0x1,
};

enum T3MaterialBlendModeType
{
	eMaterialBlendMode_Opaque = 0x0,
	eMaterialBlendMode_AlphaTest = 0x1,
	eMaterialBlendMode_AlphaBlend = 0x2,
	eMaterialBlendMode_AlphaTestBlend = 0x3,
	eMaterialBlendMode_Count = 0x4,
};

enum T3MaterialTextureLayout
{
	eMaterialTextureLayout_2D = 0x0,
	eMaterialTextureLayout_3D = 0x1,
	eMaterialTextureLayout_Cube = 0x2,
};

enum T3MaterialTextureType
{
	eMaterialTexture_None = 0xFFFFFFFF,
	eMaterialTexture_PrimaryColorMap = 0x0,
	eMaterialTexture_ColorMap = 0x1,
	eMaterialTexture_DetailColorMap = 0x2,
	eMaterialTexture_SharpDetailMap = 0x3,
	eMaterialTexture_NormalMap = 0x4,
	eMaterialTexture_MiscMap = 0x5,
	eMaterialTexture_Vector2Map = 0x6,
	eMaterialTexture_NormalDxDyMap = 0x7,
	eMaterialTexture_LinePatternMap = 0x8,
	eMaterialTexture_LineVisibilityMap = 0x9,
	eMaterialTexture_LineGenerationStyleMap = 0xA,
	eMaterialTexture_PackedDetailMap = 0xB,
	eMaterialTexture_SingleChannelDetailMap = 0xC,
	eMaterialTexture_NormalXYMap = 0xD,
	eMaterialTexture_CubeMap = 0xE,
	eMaterialTexture_VolumeMap = 0xF,
	eMaterialTexture_ObjectNormalMap = 0x10,
	eMaterialTexture_Count = 0x11,
};

enum T3MaterialValueType
{
	eMaterialValue_None = 0xFFFFFFFF,
	eMaterialValue_Float = 0x0,
	eMaterialValue_Float2 = 0x1,
	eMaterialValue_Float3 = 0x2,
	eMaterialValue_Float4 = 0x3,
	eMaterialValue_Channels = 0x4,
	eMaterialValue_Count = 0x5,
};

enum T3EffectParameterDefaultTextureType
{
	eEffectParameterDefaultTexture_Null = 0x0,
	eEffectParameterDefaultTexture_White = 0x1,
	eEffectParameterDefaultTexture_Black = 0x2,
	eEffectParameterDefaultTexture_Lightmap = 0x3,
	eEffectParameterDefaultTexture_BoneMatrix = 0x4,
	eEffectParameterDefaultTexture_Black3D = 0x5,
	eEffectParameterDefaultTexture_BlackCube = 0x6,
	eEffectParameterDefaultTexture_Count = 0x7
};

// Describes what an effect parameter stores in memory. Always 16 bytes raw.
enum T3EffectParameterStorage
{
	//Stores nothing, may hold invalid data.
	eEffectParameterStorage_Empty = 0x0,
	//First 8 bytes are pointer to T3EffectParameterBuffer. Second 8 bytes are a u64, the scalar offset in that buffer.
	eEffectParameterStorage_UniformBuffer = 0x1,
	//First 8 bytes is the CPU bound buffer data pointer (just a memory block), which size is the second 8 bytes u64 value.
	eEffectParameterStorage_UniformBufferData = 0x2,
	//First 8 bytes is the RAW T3TextureView (this struct is 8 bytes). Second 8 bytes is the pointer to the T3GFXBuffer.
	eEffectParameterStorage_GenericBuffer = 0x3,
	//First 8 bytes is the RAW T3TextureView (this struct is 8 bytes). Second 8 bytes is the pointer to the T3Texture.
	eEffectParameterStorage_Texture = 0x4,
	//First 8 bytes is the RAW T3TextureView (this struct is 8 bytes). Second 8 bytes is the render target ID.
	eEffectParameterStorage_RenderTarget = 0x5,
	//First 8 bytes is the RAW T3TextureView (this struct is 8 bytes). Second 8 bytes is the render target ID.
	eEffectParameterStorage_RenderTargetOutput = 0x6,
	eEffectParameterStorage_Count = 0x7,
};

enum T3EffectParameterDefaultBufferType
{
	eEffectParameterDefaultBuffer_Null = 0x0,
	eEffectParameterDefaultBuffer_EmptyRaw = 0x1,
	eEffectParameterDefaultBuffer_EmptyShadowCascades = 0x2,
	eEffectParameterDefaultBuffer_EmptyZBin = 0x3,
	eEffectParameterDefaultBuffer_EmptyBone = 0x4,
};

enum T3MaterialSceneTextureType
{
	eMaterialSceneTexture_Color = 0x0,
	eMaterialSceneTexture_Depth = 0x1,
	eMaterialSceneTexture_Count = 0x2,
};

enum T3EffectParameterClass
{
	eEffectParameterClass_None = 0xFFFFFFFF,
	eEffectParameterClass_UniformBuffer = 0x0,
	eEffectParameterClass_Sampler = 0x1,
	eEffectParameterClass_GenericBuffer = 0x2,
	eEffectParameterClass_Float1 = 0x3,
	eEffectParameterClass_Float2 = 0x4,
	eEffectParameterClass_Float3 = 0x5,
	eEffectParameterClass_Float4 = 0x6,
	eEffectParameterClass_Float4x4 = 0x7,
	eEffectParameterClass_Int1 = 0x8,
	eEffectParameterClass_Int2 = 0x9,
	eEffectParameterClass_Int3 = 0xA,
	eEffectParameterClass_Int4 = 0xB,
	eEffectParameterClass_UInt1 = 0xC,
	eEffectParameterClass_UInt2 = 0xD,
	eEffectParameterClass_UInt3 = 0xE,
	eEffectParameterClass_UInt4 = 0xF,
	eEffectParameterClass_Count = 0x10,
};

enum T3EffectSamplerFilterType
{
	eEffectSamplerFilter_Unknown = 0xFFFFFFFF,
	eEffectSamplerFilter_Linear = 0x0,
	eEffectSamplerFilter_Anisotropic = 0x1,
};

struct T3EffectParameterDesc {
	const char* mName;
	unsigned int mNameLength;
	T3EffectParameterClass mClass;
	T3EffectSamplerFilterType mSamplerFilter;
	unsigned int mScalarSize;
	unsigned int mParameterIndex;
	unsigned int mStateHash;

	union
	{
		const void* mpDefaultBuffer;
		T3EffectParameterDefaultTextureType mDefaultTextureType;
		T3EffectParameterDefaultBufferType mDefaultGenericBufferType;
	};

};

enum T3MaterialTexturePropertyType
{
	eMaterialTextureProperty_None = 0xFFFFFFFF,
	eMaterialTextureProperty_Toon = 0x0,
	eMaterialTextureProperty_Count = 0x1,
};

struct T3EffectParameterClassDesc {
	T3EffectParameterClass mBaseClass;
	int mScalarAlign, mScalarSize;
};

enum T3EffectQuality
{
	eEffectQuality_Default = 0xFFFFFFFF,
	WDC_eEffectQuality_High = 0x0,
	WDC_eEffectQuality_Medium = 0x1,
	WDC_eEffectQuality_LowPlus = 0x2,
	WDC_eEffectQuality_Low = 0x3,
	WDC_eEffectQuality_Lowest = 0x4,
	WDC_eEffectQuality_Count = 0x5,
};

struct T3EffectCachePackageHeader {

	u32 mMagic;//0x844FF83
	u32 mVersion;//on PC this is 157
	u32 mProgramCount;
	u32 mShaderCount;
	u32 mShaderDataOffset;
};

enum T3MaterialTransform2DComponent
{
	eMaterialTransform2D_MoveU = 0x0,
	eMaterialTransform2D_MoveV = 0x1,
	eMaterialTransform2D_ScaleU = 0x2,
	eMaterialTransform2D_ScaleV = 0x3,
	eMaterialTransform2D_ScaleOriginU = 0x4,
	eMaterialTransform2D_ScaleOriginV = 0x5,
	eMaterialTransform2D_ShearU = 0x6,
	eMaterialTransform2D_ShearV = 0x7,
	eMaterialTransform2D_ShearOriginU = 0x8,
	eMaterialTransform2D_ShearOriginV = 0x9,
	eMaterialTransform2D_RotateOriginU = 0xA,
	eMaterialTransform2D_RotateOriginV = 0xB,
	eMaterialTransform2D_Rotate = 0xC,
	eMaterialTransform2D_Count = 0xD,
};

enum T3EffectCacheProgramStatus
{
	eEffectProgramStatus_None = 0xFFFFFFFF,
	eEffectProgramStatus_NotCompiled = 0x0,
	eEffectProgramStatus_CreationFailed = 0x1,
	eEffectProgramStatus_CompileFailed = 0x2,
	eEffectProgramStatus_CompilePending = 0x3,
	eEffectProgramStatus_CreationPending = 0x4,
	eEffectProgramStatus_Compiled = 0x5,
	eEffectProgramStatus_Created = 0x6,
	eEffectProgramStatus_Count = 0x7,
};

enum T3EffectFeature
{
	eEffectFeature_None = 0xFFFFFFFF,
	WDC_eEffectFeature_FirstStatic = 0x0,
	WDC_eEffectFeature_Tool = 0x0,
	WDC_eEffectFeature_Deformable = 0x1,
	WDC_eEffectFeature_DeformableSingle = 0x2,
	WDC_eEffectFeature_DeformableCompute = 0x3,
	WDC_eEffectFeature_LightMap = 0x4,
	WDC_eEffectFeature_StaticShadowMap = 0x5,
	WDC_eEffectFeature_VertexColor = 0x6,
	WDC_eEffectFeature_SoftDepthTest = 0x7,
	WDC_eEffectFeature_SupportsHBAO = 0x8,
	WDC_eEffectFeature_ParticleFacing = 0x9,
	WDC_eEffectFeature_ParticleFacing3D = 0xA,
	WDC_eEffectFeature_ParticleSmoothAnimation = 0xB,
	WDC_eEffectFeature_ParticleRotation3D = 0xC,
	WDC_eEffectFeature_ParticleRotation3DTexture = 0xD,
	WDC_eEffectFeature_ParticleTexCoord = 0xE,
	WDC_eEffectFeature_PostLevels = 0xF,
	WDC_eEffectFeature_PostRadialBlur = 0x10,
	WDC_eEffectFeature_PostTonemap = 0x11,
	WDC_eEffectFeature_PostTonemapIntensity = 0x12,
	WDC_eEffectFeature_PostTonemapFilmic = 0x13,
	WDC_eEffectFeature_PostTonemapFilmicRGB = 0x14,
	WDC_eEffectFeature_PostTonemapDOF = 0x15,
	WDC_eEffectFeature_PostVignetteTint = 0x16,
	WDC_eEffectFeature_PostDepthOfField1x = 0x17,
	WDC_eEffectFeature_PostDepthOfField2x = 0x18,
	WDC_eEffectFeature_PostDepthOfField3x = 0x19,
	WDC_eEffectFeature_PostNewDepthOfField = 0x1A,
	WDC_eEffectFeature_PostNewDepthOfField_Low = 0x1B,
	WDC_eEffectFeature_PostNewDepthOfField_Medium = 0x1C,
	WDC_eEffectFeature_PostNewDepthOfField_High = 0x1D,
	WDC_eEffectFeature_PostNewDepthOfField_NoMRT = 0x1E,
	WDC_eEffectFeature_PostNewDepthOfFieldMCLegacy = 0x1F,
	WDC_eEffectFeature_PostBokehUsesGS = 0x20,
	WDC_eEffectFeature_PostBokehUsesAberration = 0x21,
	WDC_eEffectFeature_BokehZTest = 0x22,
	WDC_eEffectFeature_BokehScaleOccluded = 0x23,
	WDC_eEffectFeature_PostBokeh = 0x24,
	WDC_eEffectFeature_PostDebugHBAO = 0x25,
	WDC_eEffectFeature_PostConvert_sRGB = 0x26,
	WDC_eEffectFeature_PostHDR10 = 0x27,
	WDC_eEffectFeature_PostHDRMulti = 0x28,
	WDC_eEffectFeature_DepthOfFieldVignette = 0x29,
	WDC_eEffectFeature_PostBrushOutline = 0x2A,
	WDC_eEffectFeature_PostBrushDepthOfField = 0x2B,
	WDC_eEffectFeature_PostTextureArray = 0x2C,
	WDC_eEffectFeature_PostShadowInput2x = 0x2D,
	WDC_eEffectFeature_PostShadowSDSM = 0x2E,
	WDC_eEffectFeature_ShadowApplyTile = 0x2F,
	WDC_eEffectFeature_ShadowApplyModulated = 0x30,
	WDC_eEffectFeature_ShadowApplyGobo = 0x31,
	WDC_eEffectFeature_HBAODeinterleaved = 0x32,
	WDC_eEffectFeature_HBAODeinterleaved2x = 0x33,
	WDC_eEffectFeature_HBAODeinterleaved4x = 0x34,
	WDC_eEffectFeature_BakeFlatLighting = 0x35,
	WDC_eEffectFeature_LastStatic = 0x35,
	WDC_eEffectFeature_FirstDynamic = 0x36,
	WDC_eEffectFeature_RimLight = 0x36,
	WDC_eEffectFeature_SpecularEnvironment = 0x37,
	WDC_eEffectFeature_Shadow = 0x38,
	WDC_eEffectFeature_EnvLighting = 0x39,
	WDC_eEffectFeature_CinLighting = 0x3A,
	WDC_eEffectFeature_EnvKeyLight = 0x3B,
	WDC_eEffectFeature_EnvKeyShadow = 0x3C,
	WDC_eEffectFeature_EnvLight2x = 0x3D,
	WDC_eEffectFeature_EnvLight3x = 0x3E,
	WDC_eEffectFeature_EnlightenMap = 0x3F,
	WDC_eEffectFeature_GlowOutput = 0x40,
	WDC_eEffectFeature_GBufferOutput = 0x41,
	WDC_eEffectFeature_DecalApply = 0x42,
	WDC_eEffectFeature_ClipPlane = 0x43,
	WDC_eEffectFeature_CameraClipPlanes = 0x44,
	WDC_eEffectFeature_ShadowCascades = 0x45,
	WDC_eEffectFeature_ShadowDepth2 = 0x46,
	WDC_eEffectFeature_ShadowOutputColor = 0x47,
	WDC_eEffectFeature_AlphaTest = 0x48,
	WDC_eEffectFeature_MSAAAlphaTest = 0x49,
	WDC_eEffectFeature_Dissolve = 0x4A,
	WDC_eEffectFeature_SharpShadows = 0x4B,
	WDC_eEffectFeature_LinearDepthFetch = 0x4C,
	WDC_eEffectFeature_FrameBufferFetch = 0x4D,
	WDC_eEffectFeature_DepthBufferFetch = 0x4E,
	WDC_eEffectFeature_CreaseLines = 0x4F,
	WDC_eEffectFeature_BoundaryLines = 0x50,
	WDC_eEffectFeature_JaggedLines = 0x51,
	WDC_eEffectFeature_SmoothLines = 0x52,
	WDC_eEffectFeature_Debug = 0x53,
	WDC_eEffectFeature_LastDynamic = 0x53,
	WDC_eEffectFeature_Count = 0x54,
	WDC_eEffectFeature_StaticCount = 0x36,
	WDC_eEffectFeature_DynamicCount = 0x1E,
};

enum T3RenderTargetUsage
{
	eRenderTargetUsage_Unused = 0xFFFFFFFF,
	eRenderTargetUsage_Default = 0x0,
	eRenderTargetUsage_RGBA8 = 0x1,
	eRenderTargetUsage_RGBA8S = 0x2,
	eRenderTargetUsage_sRGBA8 = 0x3,
	eRenderTargetUsage_RGB565 = 0x4,
	eRenderTargetUsage_sRGB565 = 0x5,
	eRenderTargetUsage_RGBA1010102 = 0x6,
	eRenderTargetUsage_RGBA16F = 0x7,
	eRenderTargetUsage_RGBA32F = 0x8,
	eRenderTargetUsage_RGBA1010102F = 0x9,
	eRenderTargetUsage_RGB111110F = 0xA,
	eRenderTargetUsage_RGB9E5F = 0xB,
	eRenderTargetUsage_R8 = 0xC,
	eRenderTargetUsage_RG8 = 0xD,
	eRenderTargetUsage_RG16F = 0xE,
	eRenderTargetUsage_R16F = 0xF,
	eRenderTargetUsage_R32F = 0x10,
	eRenderTargetUsage_RG32F = 0x11,
	eRenderTargetUsage_RGBA16 = 0x12,
	eRenderTargetUsage_RG16 = 0x13,
	eRenderTargetUsage_RGBA16S = 0x14,
	eRenderTargetUsage_RG16S = 0x15,
	eRenderTargetUsage_R16 = 0x16,
	eRenderTargetUsage_R16UI = 0x17,
	eRenderTargetUsage_RG16UI = 0x18,
	eRenderTargetUsage_R32 = 0x19,
	eRenderTargetUsage_RG32 = 0x1A,
	eRenderTargetUsage_RGBA32 = 0x1B,
	eRenderTargetUsage_ShadowMap16 = 0x1C,
	eRenderTargetUsage_ShadowMap24 = 0x1D,
	eRenderTargetUsage_Depth16 = 0x1E,
	eRenderTargetUsage_Depth = 0x1F,
	eRenderTargetUsage_DepthStencil = 0x20,
	eRenderTargetUsage_Depth32F = 0x21,
	eRenderTargetUsage_LinearDepth = 0x22,
};

enum T3EffectParameterType : u32
{
	eEffectParameter_Unknown = 0xFFFFFFFF,
	eEffectParameter_FirstUniformBuffer = 0x0,
	eEffectParameter_UniformBufferCamera = 0x0,
	eEffectParameter_UniformBufferScene = 0x1,
	eEffectParameter_UniformBufferSceneTool = 0x2,
	eEffectParameter_UniformBufferObject = 0x3,
	eEffectParameter_UniformBufferInstance = 0x4,
	eEffectParameter_UniformBufferLights = 0x5,
	eEffectParameter_UniformBufferLightEnv = 0x6,
	eEffectParameter_UniformBufferLightCin = 0x7,
	eEffectParameter_UniformBufferSkinning = 0x8,
	eEffectParameter_UniformBufferShadow = 0x9,
	eEffectParameter_UniformBufferParticle = 0xA,
	eEffectParameter_UniformBufferDecal = 0xB,
	eEffectParameter_UniformBufferSimple = 0xC,
	eEffectParameter_UniformBufferGaussian = 0xD,
	eEffectParameter_UniformBufferPost = 0xE,
	eEffectParameter_UniformBufferBrush = 0xF,
	eEffectParameter_UniformBufferLightEnvData_High = 0x10,
	eEffectParameter_UniformBufferLightEnvData_Medium = 0x11,
	eEffectParameter_UniformBufferLightEnvData_Low = 0x12,
	eEffectParameter_UniformBufferLightEnvView = 0x13,
	eEffectParameter_UniformBufferLightAmbient = 0x14,
	eEffectParameter_UniformBufferShadowVolume = 0x15,
	eEffectParameter_UniformBufferMesh = 0x16,
	eEffectParameter_UniformBufferMeshBatch = 0x17,
	eEffectParameter_UniformBufferMeshDynamicBatch = 0x18,
	eEffectParameter_UniformBufferMeshDebugBatch = 0x19,
	eEffectParameter_UniformBufferHBAO = 0x1A,
	eEffectParameter_UniformBufferMaterialTool = 0x1B,
	eEffectParameter_UniformBufferMaterialBase = 0x1C,
	eEffectParameter_UniformBufferMaterialMain = 0x1D,
	eEffectParameter_LastUniformBuffer = 0x1D,
	eEffectParameter_FirstGenericBuffer = 0x1E,
	eEffectParameter_GenericBufferSkinning = 0x1E,
	eEffectParameter_GenericBuffer0VertexIn = 0x1F,
	eEffectParameter_GenericBuffer1VertexIn = 0x20,
	eEffectParameter_GenericBufferVertexOut = 0x21,
	eEffectParameter_GenericBufferMeshBounds = 0x22,
	eEffectParameter_GenericBufferIndices = 0x23,
	eEffectParameter_GenericBufferIndirectArgs = 0x24,
	eEffectParameter_GenericBufferStartInstance = 0x25,
	eEffectParameter_GenericBufferDepthRange = 0x26,
	eEffectParameter_GenericBufferPrevDepthRange = 0x27,
	eEffectParameter_GenericBufferLightGrid = 0x28,
	eEffectParameter_GenericBufferLightZBin = 0x29,
	eEffectParameter_GenericBufferLightGroupMask = 0x2A,
	eEffectParameter_GenericBufferShadowCascades = 0x2B,
	eEffectParameter_GenericBufferCinShadowData = 0x2C,
	eEffectParameter_GenericBufferDusterData = 0x2D,
	eEffectParameter_GenericBufferDusterVisibility = 0x2E,
	eEffectParameter_GenericBufferWaveformMonitor = 0x2F,
	eEffectParameter_GenericBufferInput0 = 0x30,
	eEffectParameter_GenericBufferInput1 = 0x31,
	eEffectParameter_GenericBufferInput2 = 0x32,
	eEffectParameter_GenericBufferInput3 = 0x33,
	eEffectParameter_GenericBufferInput4 = 0x34,
	eEffectParameter_GenericBufferInput5 = 0x35,
	eEffectParameter_GenericBufferInput6 = 0x36,
	eEffectParameter_GenericBufferInput7 = 0x37,
	eEffectParameter_GenericBufferOutput0 = 0x38,
	eEffectParameter_GenericBufferOutput1 = 0x39,
	eEffectParameter_GenericBufferOutput2 = 0x3A,
	eEffectParameter_GenericBufferOutput3 = 0x3B,
	eEffectParameter_GenericBufferOutput4 = 0x3C,
	eEffectParameter_LastGenericBuffer = 0x3C,
	eEffectParameter_FirstSampler = 0x3D,
	eEffectParameter_SamplerDiffuse = 0x3D,
	eEffectParameter_SamplerStaticShadowmap = 0x3E,
	eEffectParameter_SamplerShadowmap = 0x3F,
	eEffectParameter_SamplerProjected = 0x40,
	eEffectParameter_SamplerBrushNear = 0x41,
	eEffectParameter_SamplerBrushFar = 0x42,
	eEffectParameter_SamplerEnvironment = 0x43,
	eEffectParameter_SamplerBokehPattern = 0x44,
	eEffectParameter_SamplerNoiseLUT = 0x45,
	eEffectParameter_SamplerHLSMovieY = 0x46,
	eEffectParameter_SamplerHLSMovieC = 0x47,
	eEffectParameter_SamplerHLSMovieRGB = 0x48,
	eEffectParameter_SamplerBackbuffer = 0x49,
	eEffectParameter_SamplerBackbufferHDR = 0x4A,
	eEffectParameter_SamplerBackbufferHDRPrev = 0x4B,
	eEffectParameter_SamplerBackbufferHDRResolved = 0x4C,
	eEffectParameter_SamplerDepthbuffer = 0x4D,
	eEffectParameter_SamplerLinearDepth = 0x4E,
	eEffectParameter_SamplerLinearDepthPrev = 0x4F,
	eEffectParameter_SamplerAlphaMeshLinearDepth = 0x50,
	eEffectParameter_SamplerStencil = 0x51,
	eEffectParameter_SamplerGBuffer0 = 0x52,
	eEffectParameter_SamplerGBuffer1 = 0x53,
	eEffectParameter_SamplerDBuffer0 = 0x54,
	eEffectParameter_SamplerDBuffer1 = 0x55,
	eEffectParameter_SamplerDeferredShadows = 0x56,
	eEffectParameter_SamplerDeferredModulatedShadows = 0x57,
	eEffectParameter_SamplerDeferredShadowsPrev = 0x58,
	eEffectParameter_SamplerDeferredLight0 = 0x59,
	eEffectParameter_SamplerDeferredLight1 = 0x5A,
	eEffectParameter_SamplerDofBlur1x = 0x5B,
	eEffectParameter_SamplerDofBlur2x = 0x5C,
	eEffectParameter_SamplerDofBlur3x = 0x5D,
	eEffectParameter_NewDepthOfFieldHalf = 0x5E,
	eEffectParameter_NewDofNearH = 0x5F,
	eEffectParameter_NewDofNearV = 0x60,
	eEffectParameter_NewDofFarH1x = 0x61,
	eEffectParameter_NewDofFarV1x = 0x62,
	eEffectParameter_Bokeh = 0x63,
	eEffectParameter_SSLines = 0x64,
	eEffectParameter_SamplerLightEnvShadowGobo = 0x65,
	eEffectParameter_SamplerStaticShadowVolume = 0x66,
	eEffectParameter_FirstEVSMShadowSampler = 0x67,
	eEffectParameter_SamplerEVSMShadow0 = 0x67,
	eEffectParameter_SamplerEVSMShadow1 = 0x68,
	eEffectParameter_LastEVSMShadowSampler = 0x68,
	eEffectParameter_SamplerSMAAAreaLookup = 0x69,
	eEffectParameter_SamplerSMAASearchLookup = 0x6A,
	eEffectParameter_SamplerRandom = 0x6B,
	eEffectParameter_SamplerNoise = 0x6C,
	eEffectParameter_SamplerTetrahedralLookup = 0x6D,
	eEffectParameter_SamplerLightmap = 0x6E,
	eEffectParameter_SamplerLightmapFlat = 0x6F,
	eEffectParameter_SamplerBoneMatrices = 0x70,
	eEffectParameter_SamplerDebugOverlay = 0x71,
	eEffectParameter_SamplerSoftwareOcclusion = 0x72,
	eEffectParameter_FirstMaterialInputSampler = 0x73,
	eEffectParameter_SamplerMaterialInput0 = 0x73,
	eEffectParameter_SamplerMaterialInput1 = 0x74,
	eEffectParameter_SamplerMaterialInput2 = 0x75,
	eEffectParameter_SamplerMaterialInput3 = 0x76,
	eEffectParameter_SamplerMaterialInput4 = 0x77,
	eEffectParameter_SamplerMaterialInput5 = 0x78,
	eEffectParameter_SamplerMaterialInput6 = 0x79,
	eEffectParameter_SamplerMaterialInput7 = 0x7A,
	eEffectParameter_SamplerMaterialInput8 = 0x7B,
	eEffectParameter_SamplerMaterialInput9 = 0x7C,
	eEffectParameter_SamplerMaterialInput10 = 0x7D,
	eEffectParameter_SamplerMaterialInput11 = 0x7E,
	eEffectParameter_SamplerMaterialInput12 = 0x7F,
	eEffectParameter_SamplerMaterialInput13 = 0x80,
	eEffectParameter_SamplerMaterialInput14 = 0x81,
	eEffectParameter_SamplerMaterialInput15 = 0x82,
	eEffectParameter_LastMaterialInputSampler = 0x82,
	eEffectParameter_SamplerFxaaConsole360TexExpBiasNegOne = 0x83,
	eEffectParameter_SamplerFxaaConsole360TexExpBiasNegTwo = 0x84,
	eEffectParameter_FirstPostOutputSampler = 0x85,
	eEffectParameter_SamplerPostOutput0 = 0x85,
	eEffectParameter_SamplerPostOutput1 = 0x86,
	eEffectParameter_SamplerPostOutput2 = 0x87,
	eEffectParameter_SamplerPostOutput3 = 0x88,
	eEffectParameter_LastPostOutputSampler = 0x88,
	eEffectParameter_FirstPostInputSampler = 0x89,
	eEffectParameter_SamplerPostInput0 = 0x89,
	eEffectParameter_SamplerPostInput1 = 0x8A,
	eEffectParameter_SamplerPostInput2 = 0x8B,
	eEffectParameter_SamplerPostInput3 = 0x8C,
	eEffectParameter_SamplerPostInput4 = 0x8D,
	eEffectParameter_SamplerPostInput5 = 0x8E,
	eEffectParameter_SamplerPostInput6 = 0x8F,
	eEffectParameter_SamplerPostInput7 = 0x90,
	eEffectParameter_LastPostInputSampler = 0x90,
	eEffectParameter_FirstParticleSampler = 0x91,
	eEffectParameter_SamplerParticlePosition = 0x91,
	eEffectParameter_SamplerParticleOrientation = 0x92,
	eEffectParameter_SamplerParticleColor = 0x93,
	eEffectParameter_SamplerParticleRotation3D = 0x94,
	eEffectParameter_LastParticleSampler = 0x94,
	eEffectParameter_SamplerEnlighten = 0x95,
	eEffectParameter_LastSampler = 0x95,
	eEffectParameter_Count = 0x96,
	eEffectParameter_UniformBufferCount = 0x1E,
	eEffectParameter_GenericBufferCount = 0x1F,
	eEffectParameter_SamplerCount = 0x59,
	eEffectParameter_MaterialInputSamplerCount = 0x10,
	eEffectParameter_PostInputSamplerCount = 0x8,
	eEffectParameter_PostOutputSamplerCount = 0x4,
	eEffectParameter_ParticleSamplerCount = 0x4,
	eEffectParameter_EVSMShadowSamplerCount = 0x2,
};

enum T3EffectParameterField
{
	eEffectField_Unknown = 0xFFFFFFFF,
	eEffectField_LightMinDistance = 0x0,
	eEffectField_LightInvDeltaDistance = 0x1,
	eEffectField_LightWrapAroundA = 0x2,
	eEffectField_LightWrapAroundB = 0x3,
	eEffectField_LightRimWrapAround = 0x4,
	eEffectField_LightRimOcclusion = 0x5,
	eEffectField_LightAmbientOcclusion = 0x6,
	eEffectField_LightToonOpacity = 0x7,
	eEffectField_LightPositionX = 0x8,
	eEffectField_LightPositionY = 0x9,
	eEffectField_LightPositionZ = 0xA,
	eEffectField_LightColorR = 0xB,
	eEffectField_LightColorG = 0xC,
	eEffectField_LightColorB = 0xD,
	eEffectField_LightSpecularColorR = 0xE,
	eEffectField_LightSpecularColorG = 0xF,
	eEffectField_LightSpecularColorB = 0x10,
	eEffectField_LightRimColorR = 0x11,
	eEffectField_LightRimColorG = 0x12,
	eEffectField_LightRimColorB = 0x13,
	eEffectField_LightColorCorrection = 0x14,
	eEffectField_LightSH0 = 0x15,
	eEffectField_LightSH123 = 0x16,
	eEffectField_LightSH45678 = 0x17,
	eEffectField_LightSHMin = 0x18,
	eEffectField_LightmapSpecularPosition = 0x19,
	eEffectField_LightmapSpecularIntensity = 0x1A,
	eEffectField_LightReflectionSHParams = 0x1B,
	eEffectField_Light0Color = 0x1C,
	eEffectField_Light0SpecularColor = 0x1D,
	eEffectField_LightPrimaryRimDirection = 0x1E,
	eEffectField_LightPrimaryRimColor = 0x1F,
	eEffectField_LightPrimaryRimWrapAround = 0x20,
	eEffectField_LightPrimaryRimWrapOcclusion = 0x21,
	eEffectField_LightPrimaryShadowDirection = 0x22,
	eEffectField_LightEnabled = 0x23,
	eEffectField_LightEnvPositionData_High = 0x24,
	eEffectField_LightEnvDirectionData_High = 0x25,
	eEffectField_LightEnvColorData_High = 0x26,
	eEffectField_LightEnvShadowViewportData_High = 0x27,
	eEffectField_LightEnvShadowMatrixData_High = 0x28,
	eEffectField_LightEnvPositionData_Medium = 0x29,
	eEffectField_LightEnvDirectionData_Medium = 0x2A,
	eEffectField_LightEnvColorData_Medium = 0x2B,
	eEffectField_LightEnvShadowViewportData_Medium = 0x2C,
	eEffectField_LightEnvShadowMatrixData_Medium = 0x2D,
	eEffectField_LightEnvPositionData_Low = 0x2E,
	eEffectField_LightEnvDirectionData_Low = 0x2F,
	eEffectField_LightEnvAttenuationData_Low = 0x30,
	eEffectField_LightEnvShadowLayerData_Low = 0x31,
	eEffectField_LightEnvColorData_Low = 0x32,
	eEffectField_LightEnvCount = 0x33,
	eEffectField_LightEnvKeyColor = 0x34,
	eEffectField_LightEnvKeyDiffuseIntensity = 0x35,
	eEffectField_LightEnvKeySpecularIntensity = 0x36,
	eEffectField_LightEnvKeyDirection = 0x37,
	eEffectField_LightEnvKeyWrap = 0x38,
	eEffectField_LightEnvKeyOpacity = 0x39,
	eEffectField_LightEnvKeyShadowMatrices = 0x3A,
	eEffectField_LightEnvKeyShadowFadeScale = 0x3B,
	eEffectField_LightEnvKeyShadowFadeBias = 0x3C,
	eEffectField_LightEnvKeyShadowMaxDistance = 0x3D,
	eEffectField_LightEnvKeyShadowModulatedIntensity = 0x3E,
	eEffectField_LightEnvFogColor = 0x3F,
	eEffectField_LightEnvFogMaxOpacity = 0x40,
	eEffectField_LightEnvFogStartDistance = 0x41,
	eEffectField_LightEnvFogHeightFalloff = 0x42,
	eEffectField_LightEnvFogDensity = 0x43,
	eEffectField_LightEnvKeyHBAOParam = 0x44,
	eEffectField_LightEnvEnlightenIntensity = 0x45,
	eEffectField_LightCinColor0 = 0x46,
	eEffectField_LightCinColor1 = 0x47,
	eEffectField_LightCinColor2 = 0x48,
	eEffectField_LightCinDiffuseIntensity0 = 0x49,
	eEffectField_LightCinDiffuseIntensity1 = 0x4A,
	eEffectField_LightCinDiffuseIntensity2 = 0x4B,
	eEffectField_LightCinSpecularIntensity0 = 0x4C,
	eEffectField_LightCinSpecularIntensity1 = 0x4D,
	eEffectField_LightCinSpecularIntensity2 = 0x4E,
	eEffectField_LightCinEnable0 = 0x4F,
	eEffectField_LightCinEnable1 = 0x50,
	eEffectField_LightCinEnable2 = 0x51,
	eEffectField_LightCinDirection0 = 0x52,
	eEffectField_LightCinDirection1 = 0x53,
	eEffectField_LightCinDirection2 = 0x54,
	eEffectField_LightCinShadowIntensity0 = 0x55,
	eEffectField_LightCinShadowIntensity1 = 0x56,
	eEffectField_LightCinShadowIntensity2 = 0x57,
	eEffectField_LightCinWrap0 = 0x58,
	eEffectField_LightCinWrap1 = 0x59,
	eEffectField_LightCinWrap2 = 0x5A,
	eEffectField_LightCinAmbientOcclusion0 = 0x5B,
	eEffectField_LightCinAmbientOcclusion1 = 0x5C,
	eEffectField_LightCinAmbientOcclusion2 = 0x5D,
	eEffectField_LightCinLightEnvIntensity = 0x5E,
	eEffectField_LightCinEnlightenIntensity = 0x5F,
	eEffectField_LightCinEnlightenSaturation = 0x60,
	eEffectField_ShadowVolumeMatrixData = 0x61,
	eEffectField_ShadowVolumeTextureScale = 0x62,
	eEffectField_ShadowVolumeTextureBias = 0x63,
	eEffectField_ShadowVolumeCount = 0x64,
	eEffectField_ShadowLightIntensity = 0x65,
	eEffectField_ShadowLightColor = 0x66,
	eEffectField_ShadowPosition = 0x67,
	eEffectField_ShadowDirection = 0x68,
	eEffectField_ShadowProjectedDirection = 0x69,
	eEffectField_ShadowInvRadius = 0x6A,
	eEffectField_ShadowDistanceFalloff = 0x6B,
	eEffectField_ShadowCosConeAngle = 0x6C,
	eEffectField_ShadowInvDeltaCosConeAngle = 0x6D,
	eEffectField_ShadowColor = 0x6E,
	eEffectField_ShadowGutterColor = 0x6F,
	eEffectField_ShadowViewMatrix = 0x70,
	eEffectField_ShadowMatrix0 = 0x71,
	eEffectField_ShadowMatrix1 = 0x72,
	eEffectField_ShadowMatrix2 = 0x73,
	eEffectField_ShadowMatrix3 = 0x74,
	eEffectField_ShadowPCFKernel = 0x75,
	eEffectField_ShadowTextureMatrix0 = 0x76,
	eEffectField_ShadowTextureMatrix1 = 0x77,
	eEffectField_ShadowViewport = 0x78,
	eEffectField_ShadowFilterRadius = 0x79,
	eEffectField_ShadowDistanceScale = 0x7A,
	eEffectField_ShadowDistanceBias = 0x7B,
	eEffectField_ShadowSoftness = 0x7C,
	eEffectField_ShadowCascadeSplitLambda = 0x7D,
	eEffectField_ShadowCascadeCount = 0x7E,
	eEffectField_ShadowMapResolution = 0x7F,
	eEffectField_ShadowMapInvResolution = 0x80,
	eEffectField_ShadowMapResolution1 = 0x81,
	eEffectField_ShadowMapInvResolution1 = 0x82,
	eEffectField_ShadowMapTexelMax = 0x83,
	eEffectField_ShadowArrayIndex = 0x84,
	eEffectField_ShadowSDSMTexelScale = 0x85,
	eEffectField_ShadowSDSMTexelBias = 0x86,
	eEffectField_ShadowGoboIndex = 0x87,
	eEffectField_ShadowSceneMin = 0x88,
	eEffectField_ShadowSceneMax = 0x89,
	eEffectField_ShadowMinDepth = 0x8A,
	eEffectField_ShadowMaxDepth = 0x8B,
	eEffectField_ShadowMomentBias = 0x8C,
	eEffectField_ShadowGridCellSize = 0x8D,
	eEffectField_ShadowLightGroupMask = 0x8E,
	eEffectField_ShadowLightTileIndex = 0x8F,
	eEffectField_ShadowMSAACount = 0x90,
	eEffectField_WorldMatrix = 0x91,
	eEffectField_PrevWorldMatrix = 0x92,
	eEffectField_ViewMatrix = 0x93,
	eEffectField_InvViewMatrix = 0x94,
	eEffectField_ProjectionMatrix = 0x95,
	eEffectField_ViewProjectionMatrix = 0x96,
	eEffectField_InvViewProjectionMatrix = 0x97,
	eEffectField_PrevViewProjectionMatrix = 0x98,
	eEffectField_ViewToPrevViewMatrix = 0x99,
	eEffectField_ObjectScale = 0x9A,
	eEffectField_ObjectAnimatedPositionScale = 0x9B,
	eEffectField_ObjectAnimatedPositionBias = 0x9C,
	eEffectField_ObjectAnimatedPositionInvScale = 0x9D,
	eEffectField_ObjectAnimatedPositionInvBias = 0x9E,
	eEffectField_ObjectPrevAnimatedPositionScale = 0x9F,
	eEffectField_ObjectPrevAnimatedPositionBias = 0xA0,
	eEffectField_ObjectAlpha = 0xA1,
	eEffectField_ObjectRimBumpDepth = 0xA2,
	eEffectField_RecieveShadowsIntensity = 0xA3,
	eEffectField_ObjectReflectionColor = 0xA4,
	eEffectField_ObjectReflectionMipCount = 0xA5,
	eEffectField_ObjectReflectionIntensityShadow = 0xA6,
	eEffectField_ObjectReflectionMatrix0 = 0xA7,
	eEffectField_ObjectReflectionMatrix1 = 0xA8,
	eEffectField_ObjectReflectionMatrix2 = 0xA9,
	eEffectField_StencilMask = 0xAA,
	eEffectField_StencilRef = 0xAB,
	eEffectField_BatchLightmapIntensity = 0xAC,
	eEffectField_BatchAlphaRef = 0xAD,
	eEffectField_MeshPositionScale = 0xAE,
	eEffectField_MeshPositionWScale = 0xAF,
	eEffectField_MeshPositionOffset = 0xB0,
	eEffectField_MeshShadowDepthBias = 0xB1,
	eEffectField_MeshVertexCount = 0xB2,
	eEffectField_MeshDstVertexCapcity = 0xB3,
	eEffectField_MeshNormalCount = 0xB4,
	eEffectField_MeshSrcVertexStride = 0xB5,
	eEffectField_MeshSrcVertexUVStride = 0xB6,
	eEffectField_MeshDstVertexStride = 0xB7,
	eEffectField_MeshNormal0SrcOffset = 0xB8,
	eEffectField_MeshNormal1SrcOffset = 0xB9,
	eEffectField_MeshNormal2SrcOffset = 0xBA,
	eEffectField_MeshTexCoord0SrcOffset = 0xBB,
	eEffectField_MeshTexCoord1SrcOffset = 0xBC,
	eEffectField_MeshColor0SrcOffset = 0xBD,
	eEffectField_MeshColor1SrcOffset = 0xBE,
	eEffectField_MeshNormal0DstOffset = 0xBF,
	eEffectField_MeshNormal1DstOffset = 0xC0,
	eEffectField_MeshNormal2DstOffset = 0xC1,
	eEffectField_MeshTexCoord0DstOffset = 0xC2,
	eEffectField_MeshTexCoord1DstOffset = 0xC3,
	eEffectField_MeshColor0DstOffset = 0xC4,
	eEffectField_MeshColor1DstOffset = 0xC5,
	eEffectField_MeshBatchIndex = 0xC6,
	eEffectField_MeshBatchStartIndex = 0xC7,
	eEffectField_MeshBatchAdjacencyStartIndex = 0xC8,
	eEffectField_MeshBatchNumPrimitives = 0xC9,
	eEffectField_MeshBatchBaseIndex = 0xCA,
	eEffectField_MeshLineGenerationBatchIndex = 0xCB,
	eEffectField_MeshBatchVertexOffset = 0xCC,
	eEffectField_MeshBatchVertexCapacity = 0xCD,
	eEffectField_MeshBatchDebug = 0xCE,
	eEffectField_MaterialToolSelectedColor = 0xCF,
	eEffectField_TransposeViewZ = 0xD0,
	eEffectField_UnprojectDepthParams = 0xD1,
	eEffectField_TextureMatrix0 = 0xD2,
	eEffectField_TextureMatrix1 = 0xD3,
	eEffectField_LightmapTransform = 0xD4,
	eEffectField_LightmapPageIndex = 0xD5,
	eEffectField_TexCoordTransform0 = 0xD6,
	eEffectField_TexCoordTransform1 = 0xD7,
	eEffectField_TexCoordTransform2 = 0xD8,
	eEffectField_TexCoordTransform3 = 0xD9,
	eEffectField_CameraPosition = 0xDA,
	eEffectField_VisibilityThreshold = 0xDB,
	eEffectField_BackbufferSize = 0xDC,
	eEffectField_BackbufferInvSize = 0xDD,
	eEffectField_HBAOBufferParams = 0xDE,
	eEffectField_GameSize = 0xDF,
	eEffectField_CameraHFOVParam = 0xE0,
	eEffectField_CameraVFOVParam = 0xE1,
	eEffectField_CameraAspectRatio = 0xE2,
	eEffectField_CameraNear = 0xE3,
	eEffectField_CameraFar = 0xE4,
	eEffectField_CameraClipPlanes = 0xE5,
	eEffectField_CameraInvertedDepth = 0xE6,
	eEffectField_CameraPostEnabled = 0xE7,
	eEffectField_TargetSampleCount = 0xE8,
	eEffectField_Exposure = 0xE9,
	eEffectField_ExposedColorToBuffer = 0xEA,
	eEffectField_BufferToExposedColor = 0xEB,
	eEffectField_DecalFadeScale = 0xEC,
	eEffectField_DecalFadeBias = 0xED,
	eEffectField_Viewport = 0xEE,
	eEffectField_ViewportInt = 0xEF,
	eEffectField_FogParams = 0xF0,
	eEffectField_ClipPlane = 0xF1,
	eEffectField_CameraJitterOffset = 0xF2,
	eEffectField_SharpDetailFilter = 0xF3,
	eEffectField_ObjectBrushScale = 0xF4,
	eEffectField_ObjectBrushNearDetailBias = 0xF5,
	eEffectField_ObjectBrushFarDetailBias = 0xF6,
	eEffectField_ObjectBrushScaleByVertex = 0xF7,
	eEffectField_ObjectMaterialTime = 0xF8,
	eEffectField_ObjectMaterialPrevTime = 0xF9,
	eEffectField_ObjectDiffuseColor = 0xFA,
	eEffectField_TextureSheetParams = 0xFB,
	eEffectField_ParticlePivotParams = 0xFC,
	eEffectField_ParticleLinearDepthAlphaScaleBias = 0xFD,
	eEffectField_DecalWorldToObject0 = 0xFE,
	eEffectField_DecalWorldToObject1 = 0xFF,
	eEffectField_DecalWorldToObject2 = 0x100,
	eEffectField_DecalNormalThreshold = 0x101,
	eEffectField_DecalDoubleSided = 0x102,
	eEffectField_DecalNormalOpacity = 0x103,
	eEffectField_LightGridLog2TileSize = 0x104,
	eEffectField_LightGridRowPitch = 0x105,
	eEffectField_LightGridSlicePitch = 0x106,
	eEffectField_LightGridCellPitch = 0x107,
	eEffectField_LightGridDepthScale = 0x108,
	eEffectField_LightGridDepthBias = 0x109,
	eEffectField_LightGridDepthSliceMax = 0x10A,
	eEffectField_LightInvZBinCellSize = 0x10B,
	eEffectField_LightZBinGroupStride = 0x10C,
	eEffectField_LightComputeGroupToTileShift = 0x10D,
	eEffectField_DebugAlphaOverride = 0x10E,
	eEffectField_DebugRenderMode = 0x10F,
	eEffectField_DebugIntensityRange = 0x110,
	eEffectField_FogColor = 0x111,
	eEffectField_ColorPlaneParams = 0x112,
	eEffectField_HDRRange = 0x113,
	eEffectField_BrushMipBias = 0x114,
	eEffectField_TonemapParams = 0x115,
	eEffectField_TonemapShoulderCoeff = 0x116,
	eEffectField_TonemapToeCoeff = 0x117,
	eEffectField_TonemapCutoff = 0x118,
	eEffectField_TonemapFarShoulderCoeff = 0x119,
	eEffectField_TonemapFarToeCoeff = 0x11A,
	eEffectField_TonemapFarCutoff = 0x11B,
	eEffectField_ShadowMomentDepthBias = 0x11C,
	eEffectField_ShadowLightBleedReduction = 0x11D,
	eEffectField_ShadowTraceMinDistance = 0x11E,
	eEffectField_ShadowTraceInvDeltaDistance = 0x11F,
	eEffectField_ShadowAtlasResolution = 0x120,
	eEffectField_ShadowAtlasInvResolution = 0x121,
	eEffectField_ShadowInvCascadeCount = 0x122,
	eEffectField_TetrahedralMatrices = 0x123,
	eEffectField_LineSamplingOffsets = 0x124,
	eEffectField_MaterialSceneTime = 0x125,
	eEffectField_MaterialScenePrevTime = 0x126,
	eEffectField_MSAASampleCount = 0x127,
	eEffectField_NoiseScale = 0x128,
	eEffectField_NoiseFrameIndex0 = 0x129,
	eEffectField_NoiseFrameIndex1 = 0x12A,
	eEffectField_TAAWeight = 0x12B,
	eEffectField_GraphicBlackThreshold = 0x12C,
	eEffectField_GraphicBlackSoftness = 0x12D,
	eEffectField_GraphicBlackAlpha = 0x12E,
	eEffectField_GraphicBlackNear = 0x12F,
	eEffectField_GraphicBlackFar = 0x130,
	eEffectField_WindDirection = 0x131,
	eEffectField_WindSpeed = 0x132,
	eEffectField_WindIdleStrength = 0x133,
	eEffectField_WindIdleSpacialFrequency = 0x134,
	eEffectField_WindGustSpeed = 0x135,
	eEffectField_WindGustStrength = 0x136,
	eEffectField_WindGustSpacialFrequency = 0x137,
	eEffectField_WindGustIdleStrengthMultiplier = 0x138,
	eEffectField_WindGustSeparationExponent = 0x139,
	eEffectField_SimpleColor = 0x13A,
	eEffectField_Index = 0x13B,
	eEffectField_CopyOffset = 0x13C,
	eEffectField_CopySize = 0x13D,
	eEffectField_PrimitiveParams = 0x13E,
	eEffectField_BoundsMin = 0x13F,
	eEffectField_BoundsMax = 0x140,
	eEffectField_DepthOfFieldFar = 0x141,
	eEffectField_DepthOfFieldNear = 0x142,
	eEffectField_DepthOfFieldFarRamp = 0x143,
	eEffectField_DepthOfFieldNearRamp = 0x144,
	eEffectField_DepthOfFieldNearMax = 0x145,
	eEffectField_DepthOfFieldFarMax = 0x146,
	eEffectField_DepthOfFieldVignetteMax = 0x147,
	eEffectField_DofNearBlurRadiusPixels = 0x148,
	eEffectField_DofInvNearBlurRadiusPixels = 0x149,
	eEffectField_BokehBrightnessDeltaThreshold = 0x14A,
	eEffectField_BokehBrightnessThreshold = 0x14B,
	eEffectField_BokehBlurThreshold = 0x14C,
	eEffectField_BokehMinSize = 0x14D,
	eEffectField_BokehMaxSize = 0x14E,
	eEffectField_BokehFalloff = 0x14F,
	eEffectField_BokehDomainSizeX = 0x150,
	eEffectField_BokehDomainSizeY = 0x151,
	eEffectField_BokehAberrationOffsetsX = 0x152,
	eEffectField_BokehAberrationOffsetsY = 0x153,
	eEffectField_MaxBokehVertices = 0x154,
	eEffectField_ForceLinearDepthOffset = 0x155,
	eEffectField_UVToView = 0x156,
	eEffectField_BaseColor = 0x157,
	eEffectField_TargetScale = 0x158,
	eEffectField_FullRes_TexelSize = 0x159,
	eEffectField_LayerRes_TexelSize = 0x15A,
	eEffectField_Jitter = 0x15B,
	eEffectField_HBAOScreenParams = 0x15C,
	eEffectField_ProjectionParams = 0x15D,
	eEffectField_Deinterleaving_Offset00 = 0x15E,
	eEffectField_Deinterleaving_Offset10 = 0x15F,
	eEffectField_Deinterleaving_Offset01 = 0x160,
	eEffectField_Deinterleaving_Offset11 = 0x161,
	eEffectField_LayerOffset = 0x162,
	eEffectField_DistanceFalloff = 0x163,
	eEffectField_Radius = 0x164,
	eEffectField_MaxRadiusPixels = 0x165,
	eEffectField_HBAODepthRange = 0x166,
	eEffectField_NegInvRadius2 = 0x167,
	eEffectField_AngleBias = 0x168,
	eEffectField_AOmultiplier = 0x169,
	eEffectField_Intensity = 0x16A,
	eEffectField_NoiseTexSize = 0x16B,
	eEffectField_BlurSharpness = 0x16C,
	eEffectField_ColorBleedSaturation = 0x16D,
	eEffectField_AlbedoMultiplier = 0x16E,
	eEffectField_LuminanceInfluence = 0x16F,
	eEffectField_MaxDistance = 0x170,
	eEffectField_GaussianRow0 = 0x171,
	eEffectField_GaussianRow1 = 0x172,
	eEffectField_GaussianRow2 = 0x173,
	eEffectField_GaussianRow3 = 0x174,
	eEffectField_GaussianRow4 = 0x175,
	eEffectField_DofResolutionFraction = 0x176,
	eEffectField_DofFarResolutionFraction = 0x177,
	eEffectField_DofMaxCocRadiusPixels = 0x178,
	eEffectField_DofStrength = 0x179,
	eEffectField_DofFarRadiusRescale = 0x17A,
	eEffectField_DofCoverageBoost = 0x17B,
	eEffectField_DofFarBlurRadius = 0x17C,
	eEffectField_DofDebug = 0x17D,
	eEffectField_ZRange = 0x17E,
	eEffectField_ZNearClip = 0x17F,
	eEffectField_DofNearBlurryPlaneZ = 0x180,
	eEffectField_DofNearSharpPlaneZ = 0x181,
	eEffectField_DofFarSharpPlaneZ = 0x182,
	eEffectField_DofFarBlurryPlaneZ = 0x183,
	eEffectField_DofNearScale = 0x184,
	eEffectField_DofFarScale = 0x185,
	eEffectField_VignetteTint = 0x186,
	eEffectField_VignetteFalloff = 0x187,
	eEffectField_VignetteStart = 0x188,
	eEffectField_VignetteRange = 0x189,
	eEffectField_GlobalLevelsIntensity = 0x18A,
	eEffectField_GlobalLevelsScalar = 0x18B,
	eEffectField_GlobalLevelsAdder = 0x18C,
	eEffectField_LevelsIntensity = 0x18D,
	eEffectField_LevelsScalar = 0x18E,
	eEffectField_LevelsAdder = 0x18F,
	eEffectField_CameraFilterSize = 0x190,
	eEffectField_ScreenSpaceDepthScale = 0x191,
	eEffectField_ScreenSpaceDepthOffset = 0x192,
	eEffectField_BloomThreshold = 0x193,
	eEffectField_BloomIntensity = 0x194,
	eEffectField_MotionBlurIntensity = 0x195,
	eEffectField_FxaaConsoleRcpFrameOpt = 0x196,
	eEffectField_FxaaConsoleRcpFrameOpt2 = 0x197,
	eEffectField_FxaaConsole360RcpFrameOpt2 = 0x198,
	eEffectField_FxaaConsole360ConstDir = 0x199,
	eEffectField_RadialBlurTint = 0x19A,
	eEffectField_RadialBlurInRadius = 0x19B,
	eEffectField_RadialBlurInvDeltaRadius = 0x19C,
	eEffectField_RadialBlurIntensity = 0x19D,
	eEffectField_RadialBlurScale = 0x19E,
	eEffectField_BrushOutlineSize = 0x19F,
	eEffectField_BrushOutlineThreshold = 0x1A0,
	eEffectField_BrushOutlineColorThreshold = 0x1A1,
	eEffectField_BrushOutlineFalloff = 0x1A2,
	eEffectField_BrushOutlineScale = 0x1A3,
	eEffectField_BrushNearScale = 0x1A4,
	eEffectField_BrushNearDetail = 0x1A5,
	eEffectField_BrushFarScale = 0x1A6,
	eEffectField_BrushFarDetail = 0x1A7,
	eEffectField_BrushFar = 0x1A8,
	eEffectField_BrushInvFar = 0x1A9,
	eEffectField_BrushFarRamp = 0x1AA,
	eEffectField_BrushFarMaxScale = 0x1AB,
	eEffectField_BrushFarScaleBoost = 0x1AC,
	eEffectField_BrushFilterSize = 0x1AD,
	eEffectField_BrushFilterMax = 0x1AE,
	eEffectField_TonemapRGBShoulderCoeffs = 0x1AF,
	eEffectField_TonemapRGBToeCoeffs = 0x1B0,
	eEffectField_TonemapRGBCutoffs = 0x1B1,
	eEffectField_TonemapRGBFarShoulderCoeffs = 0x1B2,
	eEffectField_TonemapRGBFarToeCoeffs = 0x1B3,
	eEffectField_TonemapRGBFarCutoffs = 0x1B4,
	eEffectField_TonemapRGBWhitePoint = 0x1B5,
	eEffectField_TonemapRGBFarWhitePoint = 0x1B6,
	eEffectField_TonemapWhitePoint = 0x1B7,
	eEffectField_TonemapFarWhitePoint = 0x1B8,
	eEffectField_SSLineColor = 0x1B9,
	eEffectField_SSLineThickness = 0x1BA,
	eEffectField_SSLineDepthFadeNear = 0x1BB,
	eEffectField_SSLineDepthFadeFar = 0x1BC,
	eEffectField_SSLineDepthMagnitude = 0x1BD,
	eEffectField_SSLineDepthExponent = 0x1BE,
	eEffectField_SSLineLightDirection = 0x1BF,
	eEffectField_SSLineLightMagnitude = 0x1C0,
	eEffectField_SSLineLightExponent = 0x1C1,
	eEffectField_SSLineDebug = 0x1C2,
	eEffectField_HDRPaperWhiteNits = 0x1C3,
	eEffectField_CellColors0 = 0x1C4,
	eEffectField_CellColors1 = 0x1C5,
	eEffectField_CellColors2 = 0x1C6,
	eEffectField_CellColors3 = 0x1C7,
	eEffectField_CellLayerWeight = 0x1C8,
	eEffectField_CellBlendMode = 0x1C9,
	eEffectField_NPRLineFalloff = 0x1CA,
	eEffectField_NPRLineBias = 0x1CB,
	eEffectField_NPRLineAlphaFalloff = 0x1CC,
	eEffectField_NPRLineAlphaBias = 0x1CD,
	eEffectField_LightIntensity = 0x1CE,
	eEffectField_NPRLightBlendMask = 0x1CF,
	eEffectField_RelativePresentationScale = 0x1D0,
	eEffectField_NPRSpecularIntensity = 0x1D1,
	eEffectField_EnlightenUVTransform = 0x1D2,
	eEffectField_LightDiffuseSHR = 0x1D3,
	eEffectField_LightDiffuseSHG = 0x1D4,
	eEffectField_LightDiffuseSHB = 0x1D5,
	eEffectField_LightSpecularSHR = 0x1D6,
	eEffectField_LightSpecularSHG = 0x1D7,
	eEffectField_LightSpecularSHB = 0x1D8,
	eEffectField_BoneMatrixLocation = 0x1D9,
	eEffectField_BoneMatrixLocationf = 0x1DA,
	eEffectField_Count = 0x1DB,
};

enum T3MaterialChannelType
{
	eMaterialChannel_None = 0xFFFFFFFF,
	eMaterialChannel_SurfaceNormal = 0x0,
	eMaterialChannel_DiffuseColor = 0x1,
	eMaterialChannel_SpecularColor = 0x2,
	eMaterialChannel_EmissiveColor = 0x3,
	eMaterialChannel_VertexNormal = 0x4,
	eMaterialChannel_InvertShadow = 0x5,
	eMaterialChannel_Alpha_Legacy = 0x6,
	eMaterialChannel_Gloss = 0x7,
	eMaterialChannel_AmbientOcclusion = 0x8,
	eMaterialChannel_Glow = 0x9,
	eMaterialChannel_OutlineSize = 0xA,
	eMaterialChannel_VertexOffset = 0xB,
	eMaterialChannel_VertexColor0 = 0xC,
	eMaterialChannel_VertexColor1 = 0xD,
	eMaterialChannel_TexCoord0 = 0xE,
	eMaterialChannel_TexCoord1 = 0xF,
	eMaterialChannel_TexCoord2 = 0x10,
	eMaterialChannel_TexCoord3 = 0x11,
	eMaterialChannel_NPR_Hatching = 0x18,
	eMaterialChannel_LineColor = 0x19,
	eMaterialChannel_LineVisibility = 0x1A,
	eMaterialChannel_LineGenerationStyle = 0x1B,
	eMaterialChannel_LineWidth = 0x1C,
	eMaterialChannel_Reserved0 = 0x1C,
	eMaterialChannel_Reserved1 = 0x1D,
	eMaterialChannel_Reserved2 = 0x1E,
	eMaterialChannel_Reserved3 = 0x1F,
	eMaterialChannel_Reserved4 = 0x20,
	eMaterialChannel_DetailColor = 0x21,
	eMaterialChannel_DetailAlpha = 0x22,
	eMaterialChannel_DiffuseAlbedoColor = 0x23,
	eMaterialChannel_FinalColor = 0x24,
	eMaterialChannel_LineAlpha = 0x25,
	eMaterialChannel_Opacity = 0x26,
	eMaterialChannel_OpacityMask = 0x27,
	eMaterialChannel_SpecularAlbedoColor = 0x28,
	eMaterialChannel_Anisotropy = 0x29,
	eMaterialChannel_SmoothSurfaceNormal = 0x2A,
	eMaterialChannel_AnisotropyMask = 0x2B,
	eMaterialChannel_AnisotropyTangent = 0x2C,
	eMaterialChannel_SecondarySpecularAlbedoColor = 0x2D,
	eMaterialChannel_Count = 0x2E,
	eMaterialChannel_Custom = 0xFFFF,
	eMaterialChannel_VertexColorCount = 0x2,
	eMaterialChannel_TexCoordCount = 0x4,
};

enum T3MaterialShaderType
{
	eMaterialShader_None = 0xFFFFFFFF,
	eMaterialShader_Vertex = 0x0,
	eMaterialShader_Pixel = 0x1,
	eMaterialShader_Geometry = 0x2,
	eMaterialShader_Compute = 0x3,
	eMaterialShader_Count = 0x4,
};


enum GFXPlatformFastMemHeap
{
	eGFXPlatformFastMemHeap_None = 0xFFFFFFFF,
	eGFXPlatformFastMemHeap_ShadowMap = 0x0,
	eGFXPlatformFastMemHeap_Other = 0x1,
	eGFXPlatformFastMemHeap_Count = 0x2,
};

enum T3EffectCacheProgramFlags
{
	eEffectCacheProgramFlag_Preloaded = 0x1,
	eEffectCacheProgramFlag_HasShaderReferences = 0x2,
};

enum GFXPlatformVertexAttribute {
	eGFXPlatformAttribute_Position = 0,
	eGFXPlatformAttribute_Normal = 1,
	eGFXPlatformAttribute_Tangent = 2,
	eGFXPlatformAttribute_BlendWeight = 3,
	eGFXPlatformAttribute_BlendIndex = 4,
	eGFXPlatformAttribute_Color = 5,
	eGFXPlatformAttribute_TexCoord = 6,
	eGFXPlatformAttribute_Count = 7,
};

enum GFXPlatformResourceUsage
{
	eGFXPlatformUsage_Immutable = 0x0,
	eGFXPlatformUsage_Dynamic = 0x1,
	eGFXPlatformUsage_Streaming = 0x2,
	eGFXPlatformUsage_DynamicUnsynchronized = 0x3,
	eGFXPlatformUsage_GPUWritable = 0x4,
	eGFXPlatformUsage_CPUReadStaging = 0x5,
	eGFXPlatformUsage_CPUWriteStaging = 0x6,
	eGFXPlatformUsage_Count = 0x7,
};

enum GFXPlatformFormat {
	eGFXPlatformFormat_None = 0x0,
	eGFXPlatformFormat_F32 = 0x1,
	eGFXPlatformFormat_F32x2 = 0x2,
	eGFXPlatformFormat_F32x3 = 0x3,
	eGFXPlatformFormat_F32x4 = 0x4,
	eGFXPlatformFormat_F16x2 = 0x5,
	eGFXPlatformFormat_F16x4 = 0x6,
	eGFXPlatformFormat_S32 = 0x7,
	eGFXPlatformFormat_U32 = 0x8,
	eGFXPlatformFormat_S32x2 = 0x9,
	eGFXPlatformFormat_U32x2 = 0xA,
	eGFXPlatformFormat_S32x3 = 0xB,
	eGFXPlatformFormat_U32x3 = 0xC,
	eGFXPlatformFormat_S32x4 = 0xD,
	eGFXPlatformFormat_U32x4 = 0xE,
	eGFXPlatformFormat_S16 = 0xF,
	eGFXPlatformFormat_U16 = 0x10,
	eGFXPlatformFormat_S16x2 = 0x11,
	eGFXPlatformFormat_U16x2 = 0x12,
	eGFXPlatformFormat_S16x4 = 0x13,
	eGFXPlatformFormat_U16x4 = 0x14,
	/*N stands for normalised, so these are fractions of the bidwidth as a float, so take as int then divide by all 1s of the bigwidth as a float to get float val*/
	eGFXPlatformFormat_SN16 = 0x15,
	eGFXPlatformFormat_UN16 = 0x16,
	eGFXPlatformFormat_SN16x2 = 0x17,
	eGFXPlatformFormat_UN16x2 = 0x18,
	eGFXPlatformFormat_SN16x4 = 0x19,
	eGFXPlatformFormat_UN16x4 = 0x1A,
	eGFXPlatformFormat_S8 = 0x1B,
	eGFXPlatformFormat_U8 = 0x1C,
	eGFXPlatformFormat_S8x2 = 0x1D,
	eGFXPlatformFormat_U8x2 = 0x1E,
	eGFXPlatformFormat_S8x4 = 0x1F,
	eGFXPlatformFormat_U8x4 = 0x20,
	eGFXPlatformFormat_SN8 = 0x21,
	eGFXPlatformFormat_UN8 = 0x22,
	eGFXPlatformFormat_SN8x2 = 0x23,
	eGFXPlatformFormat_UN8x2 = 0x24,
	eGFXPlatformFormat_SN8x4 = 0x25,
	eGFXPlatformFormat_UN8x4 = 0x26,
	eGFXPlatformFormat_SN10_SN11_SN11 = 0x27,
	eGFXPlatformFormat_SN10x3_SN2 = 0x28,//?? lol
	eGFXPlatformFormat_UN10x3_UN2 = 0x29,
	eGFXPlatformFormat_D3DCOLOR = 0x2A,//DIRECT 3D colors (see d3d manual)
	eGFXPlatformFormat_Count = 0x2B,
};


enum T3RenderTargetConstantID
{
	eRenderTargetID_Unknown = 0xFFFFFFFF,
	eRenderTargetID_BackBuffer = 0x0,
	eRenderTargetID_BackBufferUnscaled = 0x1,
	eRenderTargetID_BackBufferHDR = 0x2,
	eRenderTargetID_BackBufferHDR_LinearDepth = 0x3,
	eRenderTargetID_BackBufferHDR_TAAOutput = 0x4,
	eRenderTargetID_BackBufferHDRResolved = 0x5,
	eRenderTargetID_DepthBuffer = 0x6,
	eRenderTargetID_DepthBufferUnscaled = 0x7,
	eRenderTargetID_BackBufferHDR_MSAA = 0x8,
	eRenderTargetID_DepthBuffer_MSAA = 0x9,
	eRenderTargetID_GBuffer0_MSAA = 0xA,
	eRenderTargetID_GBuffer1_MSAA = 0xB,
	eRenderTargetID_Velocity_MSAA = 0xC,
	eRenderTargetID_GBuffer0 = 0xD,
	eRenderTargetID_GBuffer0Quarter = 0xE,
	eRenderTargetID_GBuffer1 = 0xF,
	eRenderTargetID_DeferredDecalBuffer0 = 0x10,
	eRenderTargetID_DeferredDecalBuffer1 = 0x11,
	eRenderTargetID_Velocity = 0x12,
	eRenderTargetID_VelocityQuarter = 0x13,
	eRenderTargetID_GlowIntensity = 0x14,
	eRenderTargetID_LinearDepth = 0x15,
	eRenderTargetID_LinearDepthMax = 0x16,
	eRenderTargetID_AlphaMeshLinearDepth = 0x17,
	eRenderTargetID_DeferredLight0 = 0x18,
	eRenderTargetID_DeferredLight1 = 0x19,
	eRenderTargetID_DeferredLightQuarter0 = 0x1A,
	eRenderTargetID_DeferredLightQuarter1 = 0x1B,
	eRenderTargetID_DeferredShadows = 0x1C,
	eRenderTargetID_DeferredModulatedShadows = 0x1D,
	eRenderTargetID_DeferredShadowsQuarter = 0x1E,
	eRenderTargetID_DeferredModulatedShadowsQuarter = 0x1F,
	eRenderTargetID_DebugOverlayColor = 0x20,
	eRenderTargetID_DebugOverlayColorQuarter = 0x21,
	eRenderTargetID_BackBufferQuarter = 0x22,
	eRenderTargetID_DepthBufferQuarter = 0x23,
	eRenderTargetID_LinearDepthQuarter = 0x24,
	eRenderTargetID_OutlineSrc = 0x25,
	eRenderTargetID_OutlineSrcQuarter = 0x26,
	eRenderTargetID_Outline = 0x27,
	eRenderTargetID_OutlineBlur1x = 0x28,
	eRenderTargetID_OutlineBlur2x = 0x29,
	eRenderTargetID_OutlineBlur3x = 0x2A,
	eRenderTargetID_BrushOffset = 0x2B,
	eRenderTargetID_GlowBlur1x = 0x2C,
	eRenderTargetID_GlowBlur2x = 0x2D,
	eRenderTargetID_GlowBlur3x = 0x2E,
	eRenderTargetID_GlowBlur4x = 0x2F,
	eRenderTargetID_NewGlowBuffer = 0x30,
	eRenderTargetID_NewGlowBlur1x = 0x31,
	eRenderTargetID_NewGlowBlur2x = 0x32,
	eRenderTargetID_NewGlowBlur3x = 0x33,
	eRenderTargetID_DepthOfField = 0x34,
	eRenderTargetID_DofBlur1x = 0x35,
	eRenderTargetID_DofBlur2x = 0x36,
	eRenderTargetID_DofBlur3x = 0x37,
	eRenderTargetID_NewDepthOfFieldHalf = 0x38,
	eRenderTargetID_NewDofNearH = 0x39,
	eRenderTargetID_NewDofNearV = 0x3A,
	eRenderTargetID_NewDofFarH1x = 0x3B,
	eRenderTargetID_NewDofFarV1x = 0x3C,
	eRenderTargetID_NewDofFarH2x = 0x3D,
	eRenderTargetID_NewDofFarV2x = 0x3E,
	eRenderTargetID_Bokeh = 0x3F,
	eRenderTargetID_RadialBlur = 0x40,
	eRenderTargetID_SMAA_Edges = 0x41,
	eRenderTargetID_SMAA_Blend = 0x42,
	eRenderTargetID_SSLines = 0x43,
	eRenderTargetID_Count = 0x44,
	eRenderTargetID_FirstTemp = 0x44,
};


enum T3MaterialShaderInput
{
	eMaterialShaderInput_None = 0xFFFFFFFF,
	eMaterialShaderInput_VertexColor0 = 0x0,
	eMaterialShaderInput_VertexColor1 = 0x1,
	eMaterialShaderInput_TexCoord0 = 0x2,
	eMaterialShaderInput_TexCoord1 = 0x3,
	eMaterialShaderInput_TexCoord2 = 0x4,
	eMaterialShaderInput_TexCoord3 = 0x5,
	eMaterialShaderInput_Position = 0x6,
	eMaterialShaderInput_LinearDepth = 0x7,
	eMaterialShaderInput_SmoothNormal = 0x8,
	eMaterialShaderInput_VertexNormal = 0x9,
	eMaterialShaderInput_VertexTangent = 0xA,
	eMaterialShaderInput_VertexBinormal = 0xB,
	eMaterialShaderInput_Camera = 0xC,
	eMaterialShaderInput_SurfaceNormal = 0xD,
	eMaterialShaderInput_Reflection = 0xE,
	eMaterialShaderInput_SharpDetail = 0xF,
	eMaterialShaderInput_DstLinearDepth = 0x10,
	eMaterialShaderInput_Time = 0x11,
	eMaterialShaderInput_CameraFar = 0x12,
	eMaterialShaderInput_ObjectToWorld0 = 0x13,
	eMaterialShaderInput_ObjectToWorld1 = 0x14,
	eMaterialShaderInput_ObjectToWorld2 = 0x15,
	eMaterialShaderInput_ObjectPosition = 0x16,
	eMaterialShaderInput_FrontFace = 0x17,
	eMaterialShaderInput_LineTexCoord = 0x18,
	eMaterialShaderInput_LightIntensitySmooth = 0x19,
	eMaterialShaderInput_LightIntensityQuantized = 0x1A,
	eMaterialShaderInput_Shadow = 0x1B,
	eMaterialShaderInput_LightColorDiffuse = 0x1C,
	eMaterialShaderInput_LightColorSpecular = 0x1D,
	eMaterialShaderInput_LightCinKeyIntensity = 0x1E,
	eMaterialShaderInput_LightCinRimIntensity = 0x1F,
	eMaterialShaderInput_LightCinFillIntensity = 0x20,
	eMaterialShaderInput_CameraPosition = 0x21,
	eMaterialShaderInput_ScreenTexCoord = 0x22,
	eMaterialShaderInput_SceneTime = 0x23,
	eMaterialShaderInput_ObjectScale = 0x24,
	eMaterialShaderInput_HeightFogAmount = 0x25,
	eMaterialShaderInput_HeightFogColor = 0x26,
	eMaterialShaderInput_LightColor = 0x27,
	eMaterialShaderInput_LightColorEmissive = 0x28,
	eMaterialShaderInput_ColorOverlay = 0x29,
	eMaterialShaderInput_ColorOverlayAmount = 0x2A,
	eMaterialShaderInput_DepthFogAmount = 0x2B,
	eMaterialShaderInput_DepthFogColor = 0x2C,
	eMaterialShaderInput_GraphicBlackThreshold = 0x2D,
	eMaterialShaderInput_GraphicBlackAlpha = 0x2E,
	eMaterialShaderInput_GraphicBlackSoftness = 0x2F,
	eMaterialShaderInput_GraphicBlackNear = 0x30,
	eMaterialShaderInput_GraphicBlackFar = 0x31,
	eMaterialShaderInput_ParticleFrameTime = 0x32,
	eMaterialShaderInput_ParticleFrameRate = 0x33,
	eMaterialShaderInput_ParticleFrameIndex = 0x34,
	eMaterialShaderInput_ParticleLife = 0x35,
	eMaterialShaderInput_DecalSurfaceNormal = 0x36,
	eMaterialShaderInput_DecalSurfaceTranslucency = 0x37,
	eMaterialShaderInput_DecalTranslucency = 0x38,
	eMaterialShaderInput_DecalUnused2 = 0x39,
	eMaterialShaderInput_DecalUnused3 = 0x3A,
	eMaterialShaderInput_ObjectVertexNormal = 0x3B,
	eMaterialShaderInput_ViewVertexNormal = 0x3C,
	eMaterialShaderInput_WindDirection = 0x3D,
	eMaterialShaderInput_WindSpeed = 0x3E,
	eMaterialShaderInput_WindIdleStrength = 0x3F,
	eMaterialShaderInput_WindIdleSpacialFrequency = 0x40,
	eMaterialShaderInput_WindGustSpeed = 0x41,
	eMaterialShaderInput_WindGustStrength = 0x42,
	eMaterialShaderInput_WindGustSpacialFrequency = 0x43,
	eMaterialShaderInput_WindGustIdleStrengthMultiplier = 0x44,
	eMaterialShaderInput_DetailColor = 0x45,
	eMaterialShaderInput_DetailAlpha = 0x46,
	eMaterialShaderInput_WindGustSeparationExponent = 0x47,
	eMaterialShaderInput_ObjectDiffuseColor = 0x48,
	eMaterialShaderInput_Count = 0x49,
	eMaterialShaderInput_VertexColorCount = 0x2,
	eMaterialShaderInput_TexCoordCount = 0x4,
};

enum T3MaterialPrecisionType
{
	eMaterialPrecision_None = 0xFFFFFFFF,
	eMaterialPrecision_Low = 0x0,
	eMaterialPrecision_Medium = 0x1,
	eMaterialPrecision_High = 0x2,
	eMaterialPrecision_Count = 0x3,
};

enum T3MaterialPassType
{
	eMaterialPass_FirstMesh = 0x0,
	eMaterialPass_Main = 0x0,
	eMaterialPass_PreZ = 0x1,
	eMaterialPass_GBuffer = 0x2,
	eMaterialPass_GBuffer_Lines = 0x3,
	eMaterialPass_GBuffer_Outline = 0x4,
	eMaterialPass_Glow = 0x5,
	eMaterialPass_ShadowMap = 0x6,
	eMaterialPass_Outline = 0x7,
	eMaterialPass_LinearDepth = 0x8,
	eMaterialPass_LinesCS_Generate = 0x9,
	eMaterialPass_LinesCS_Rasterize = 0xA,
	eMaterialPass_LinesCS_ForceLinearDepth = 0xB,
	eMaterialPass_LastMesh = 0xB,
	eMaterialPass_FirstParticle = 0xC,
	eMaterialPass_ParticleMain = 0xC,
	eMaterialPass_ParticleGlow = 0xD,
	eMaterialPass_ParticleLinearDepth = 0xE,
	eMaterialPass_LastParticle = 0xE,
	eMaterialPass_FirstDecal = 0xF,
	eMaterialPass_DecalGBuffer = 0xF,
	eMaterialPass_DecalEmissive = 0x10,
	eMaterialPass_DecalGlow = 0x11,
	eMaterialPass_LastDecal = 0x11,
	eMaterialPass_Post = 0x12,
	eMaterialPass_MayaShader = 0x13,
	eMaterialPass_LightBake = 0x14,
	eMaterialPass_Count = 0x15,
	eMaterialPass_MeshCount = 0xC,
	eMaterialPass_ParticleCount = 0x3,
	eMaterialPass_DecalCount = 0x3,
};

enum T3RenderTargetProducerType
{
	eRenderTargetProducer_PostEffect = 0x0,
	eRenderTargetProducer_Scene = 0x1,
};

enum T3PostEffectType
{
	ePostEffect_Unknown = 0xFFFFFFFF,
	ePostEffect_ColorMain = 0x0,
	ePostEffect_MergeColorDepth = 0x1,
	ePostEffect_Glow = 0x2,
	ePostEffect_GlowPrepare = 0x3,
	ePostEffect_GlowBlur1x = 0x4,
	ePostEffect_GlowBlur2x = 0x5,
	ePostEffect_GlowBlur3x = 0x6,
	ePostEffect_GlowBlur4x = 0x7,
	ePostEffect_NewGlow = 0x8,
	ePostEffect_NewGlowPrepare = 0x9,
	ePostEffect_DofPrepare = 0xA,
	ePostEffect_DofPrepareQuarter = 0xB,
	ePostEffect_DofBlur1x = 0xC,
	ePostEffect_DofBlur1xQuarter = 0xD,
	ePostEffect_DofBlur2x = 0xE,
	ePostEffect_DofBlur3x = 0xF,
	ePostEffect_RadialBlur = 0x10,
	ePostEffect_MotionBlur = 0x11,
	ePostEffect_SMAA_EdgeDetect = 0x12,
	ePostEffect_SMAA_BlendWeight = 0x13,
	ePostEffect_SMAA_Final = 0x14,
	ePostEffect_FXAA = 0x15,
	ePostEffect_Copy = 0x16,
	ePostEffect_SceneResolve_TAA = 0x17,
	ePostEffect_Copy_TAA = 0x18,
	ePostEffect_MSAASceneResolve = 0x19,
	ePostEffect_MSAASceneResolve_TAA = 0x1A,
	ePostEffect_MSAAGBufferResolve = 0x1B,
	ePostEffect_ResolveBackbufferHDR_MSAA = 0x1C,
	ePostEffect_ResolveBackbufferHDR = 0x1D,
	ePostEffect_GBufferResolve = 0x1E,
	ePostEffect_GBufferDownSample = 0x1F,
	ePostEffect_OcclusionFilter = 0x20,
	ePostEffect_CreateOutlineSrc = 0x21,
	ePostEffect_CreateOutlineSrcQuarter = 0x22,
	ePostEffect_CreateOutlineSrcQuarter_Simple = 0x23,
	ePostEffect_CreateOutline = 0x24,
	ePostEffect_OutlineBlur1x = 0x25,
	ePostEffect_OutlineBlur2x = 0x26,
	ePostEffect_OutlineBlurUp1x = 0x27,
	ePostEffect_OutlineBlurUp2x = 0x28,
	ePostEffect_BrushOutline = 0x29,
	ePostEffect_BrushOutline_Simple = 0x2A,
	ePostEffect_BrushOffsetApply = 0x2B,
	ePostEffect_BrushOutlineBlurPrepare = 0x2C,
	ePostEffect_BrushOutlineBlur = 0x2D,
	ePostEffect_DepthDownSample = 0x2E,
	ePostEffect_StencilMaskRestore = 0x2F,
	ePostEffect_LinearDepthDownSample = 0x30,
	ePostEffect_CompositeQuarter = 0x31,
	ePostEffect_ApplyModulatedShadow = 0x32,
	ePostEffect_FinalResolve = 0x33,
	ePostEffect_FinalDebugOverlay = 0x34,
	ePostEffect_SSLines = 0x35,
	ePostEffect_WaveformMonitor = 0x36,
	ePostEffect_NewDofPrepareHalf = 0x37,
	ePostEffect_DofBlurH1x = 0x38,
	ePostEffect_DofBlurV1x = 0x39,
	ePostEffect_DofBlurH = 0x3A,
	ePostEffect_DofBlurV = 0x3B,
	ePostEffect_DofBlurHNear = 0x3C,
	ePostEffect_DofBlurHFar = 0x3D,
	ePostEffect_DofBlurVNear = 0x3E,
	ePostEffect_DofBlurVFar = 0x3F,
	ePostEffect_ComputeTest = 0x40,
	ePostEffect_Count = 0x41,
};

enum T3RenderHiStencilMode
{
	eRenderHiStencilMode_None = 0x0,
	eRenderHiStencilMode_Write = 0x1,
	eRenderHiStencilMode_Test = 0x2,
};

enum T3SurfaceAccess
{
	eSurface_ReadOnly = 0x0,
	eSurface_ReadWrite = 0x1,
	eSurface_WriteOnly = 0x2,
};

enum T3MaterialChannelFormatType
{
	eMaterialChannelFormat_None = 0xFFFFFFFF,
	eMaterialChannelFormat_Color = 0x0,
	eMaterialChannelFormat_Normal = 0x1,
	eMaterialChannelFormat_Scalar = 0x2,
	eMaterialChannelFormat_Vector = 0x3,
	eMaterialChannelFormat_Count = 0x4,
};

enum T3EffectParameterPrecision
{
	eEffectParameterPrecision_Low = 0x0,
	eEffectParameterPrecision_Medium = 0x1,
	eEffectParameterPrecision_High = 0x2,
	eEffectParameterPrecision_Count = 0x3,
};

enum T3EffectCategoryType
{
	eEffectCategory_None = 0xFFFFFFFF,
	eEffectCategory_Scene = 0x0,
	eEffectCategory_FirstNonMaterial = 0x1,
	eEffectCategory_Particle = 0x1,
	eEffectCategory_Deferred = 0x2,
	eEffectCategory_Post = 0x3,
	eEffectCategory_Utility = 0x4,
	eEffectCategory_Compute = 0x5,
	eEffectCategory_Count = 0x6,
};

// ====================================================================================== INTRINSIC RENDER TYPES =========================================================================================

struct T3RenderClear
{
	Color mClearColor;
	float mClearDepth;
	unsigned int mClearStencil;
	bool mbColor;
	bool mbDepth;
	bool mbStencil;

	inline T3RenderClear() : mClearColor(Color::Black), mClearStencil(0), mClearDepth(1.0f) {}

};

struct T3RenderStateBlock {

	struct RenderStateEntry {
		u32 mWord;
		u32 mShift;
		u32 mMask;
	};

	static RenderStateEntry smEntries[eRS_Count];

	SArray<u32, 3> mData;

	inline u32 GetRenderStateValue(T3RenderStateValue value){
		return (mData.mData[smEntries[value].mWord] & smEntries[value].mMask) >> smEntries[value].mShift;
	}

	inline void SetRenderStateValue(T3RenderStateValue value, u32 val){
		mData.mData[smEntries[value].mWord] &= ~smEntries[value].mMask;
		mData.mData[smEntries[value].mWord] |= ((val << smEntries[value].mShift) & smEntries[value].mMask);
	}

};

struct T3RenderTargetClear
{
	Color mClearColor;
	float mClearDepth;
	u32 mClearStencil;

	inline T3RenderTargetClear() : mClearColor(Color::Black), mClearStencil(0), mClearDepth(1.0f) {}

};

struct GFXPlatformAttributeParams
{
	GFXPlatformVertexAttribute mAttribute;
	GFXPlatformFormat mFormat;
	GFXPlatformVertexFrequency mFrequency;
	unsigned int mAttributeIndex;
	unsigned int mBufferIndex;
	unsigned int mBufferOffset;
};

struct T3GFXBuffer : T3GFXResource {

	GFXPlatformFormat mBufferFormat;
	GFXPlatformBufferUsage mBufferUsage;
	long mStride, mCount;
	GFXPlatformResourceUsage mResourceUsage;
	void* mpCPUBuffer;

	inline T3GFXBuffer() : T3GFXResource(eGFXPlatformResource_Buffer) {
		mpCPUBuffer = NULL;
	}

	inline ~T3GFXBuffer() {
		if (mpCPUBuffer)
			free(mpCPUBuffer);
		mpCPUBuffer = 0;
	}

	inline void AsyncRead(MetaStream* stream) {
		stream->BeginAsyncSection();
		if (mpCPUBuffer)
			free(mpCPUBuffer);
		mpCPUBuffer = malloc(mStride * mCount);
		stream->serialize_bytes(mpCPUBuffer, mStride * mCount);
		stream->EndAsyncSection();
	}

	inline static METAOP_FUNC_IMPL__(SerializeAsync) {
		CAST_METAOP(T3GFXBuffer, buf);
		MetaOpResult r = Meta::MetaOperation_SerializeAsync(pObj, pObjDescription, pContextDescription, pUserData);
		if (r == eMetaOp_Succeed && meta->IsWrite() && buf->mpCPUBuffer) {
			meta->BeginAsyncSection();
			meta->Key("T3 Graphics Buffer Binary Data");
			meta->serialize_bytes(buf->mpCPUBuffer, buf->mStride * buf->mCount);
			meta->EndAsyncSection();
		}
		return r;
	}

};

struct GFXPlatformVertexLayout {
	int mVertexBufferStrides[32];
	GFXPlatformAttributeParams mAttributes[32];
	GFXPlatformFormat mIndexFormat;
	int mVertexCountPerInstance, mVertexBufferCount, mAttributeCount;
};

namespace T3 {

	struct LayoutAttach {
		GFXPlatformVertexLayout mLayout;
		u64 mHash;

		inline LayoutAttach(GFXPlatformVertexLayout l, u64 h) : mHash(h), mLayout(l) {}

		LayoutAttach() = default;

	};

}

struct T3GFXVertexState : T3GFXResource {

	long mVertexCountPerInstance, mIndexBufferCount, mVertexBufferCount, mAttributeCount;
	GFXPlatformAttributeParams mAttributes[32];
	std::shared_ptr<T3GFXBuffer> mpIndexBuffer[4];
	std::shared_ptr<T3GFXBuffer> mpVertexBuffer[32];
	std::shared_ptr<T3::LayoutAttach> mpCachedVertexLayout[4]{};

	T3GFXVertexState(const T3GFXVertexState&) = default;

	inline T3GFXVertexState() : T3GFXResource(eGFXPlatformResource_VertexState) {
		for (int i = 0; i < 4; i++)
			mpIndexBuffer[i] = NULL;
		for (int i = 0; i < 32; i++)
			mpVertexBuffer[i] = NULL;
	}

	~T3GFXVertexState() = default;

	inline static METAOP_FUNC_IMPL__(SerializeAsync) {
		CAST_METAOP(T3GFXVertexState, state);
		int mich = TelltaleToolLib_GetGameKeyIndex("WDM");
		MetaOpResult r = Meta::MetaOperation_SerializeAsync(pObj, pObjDescription, pContextDescription, pUserData);
		if (r == eMetaOp_Succeed) {
			if (state->mAttributeCount > 32) {
				TelltaleToolLib_RaiseError("Too many attributes in vertex state!", ErrorSeverity::ERR);
				return eMetaOp_Fail;
			}
			if (state->mVertexBufferCount > 32) {
				TelltaleToolLib_RaiseError("Too many vertex buffers in vertex state!", ErrorSeverity::ERR);
				return eMetaOp_Fail;
			}
			if (state->mIndexBufferCount > 4) {
				TelltaleToolLib_RaiseError("Too many index buffers in vertex state!", ErrorSeverity::ERR);
				return eMetaOp_Fail;
			}
			if (state->mAttributeCount) {
				MetaClassDescription* mcd = ::GetMetaClassDescription<GFXPlatformAttributeParams>();
				if (!mcd) {
					TelltaleToolLib_RaiseError("Could not find GFX platform attributes metaclass", ErrorSeverity::ERR);
					return eMetaOp_Fail;
				}
				for (int i = 0; i < state->mAttributeCount; i++) {
					r = PerformMetaSerializeFull(meta, &state->mAttributes[i], mcd);
					if (r != eMetaOp_Succeed)
						return r;
				}
			}
			if (sSetKeyIndex > mich) {
				for (int i = 0; i < state->mIndexBufferCount; i++) {
					if (meta->IsRead()) {
						state->mpIndexBuffer[i] = std::make_shared<T3GFXBuffer>();
					}
					if (!state->mpIndexBuffer[i]) {
						TelltaleToolLib_RaiseError("Could not find or create index buffers", ErrorSeverity::ERR);
						return eMetaOp_OutOfMemory;
					}
					r = PerformMetaSerializeAsync<T3GFXBuffer>(meta, state->mpIndexBuffer[i].get());
					if (r != eMetaOp_Succeed)
						return r;
				}
			}
			else {
				state->mIndexBufferCount = 1;
				bool hasIndexBuffer = state->mpIndexBuffer[0] != NULL;
				meta->serialize_bool(&hasIndexBuffer);
				if (hasIndexBuffer) {
					if (meta->IsRead()) {
						state->mpIndexBuffer[0] = std::make_shared<T3GFXBuffer>();
					}
					else {
						if (!state->mpIndexBuffer[0])
							state->mpIndexBuffer[0] = std::make_shared<T3GFXBuffer>();
					}
					r = PerformMetaSerializeAsync<T3GFXBuffer>(meta, state->mpIndexBuffer[0].get());
					if (r != eMetaOp_Succeed)
						return r;
				}
			}
			for (int i = 0; i < state->mVertexBufferCount; i++) {
				if (meta->IsRead()) {
					state->mpVertexBuffer[i] = std::make_shared<T3GFXBuffer>();
				}
				if (!state->mpVertexBuffer[i]) {
					TelltaleToolLib_RaiseError("Could not find or create vertex buffers", ErrorSeverity::ERR);
					return eMetaOp_OutOfMemory;
				}
				r = PerformMetaSerializeAsync<T3GFXBuffer>(meta, state->mpVertexBuffer[i].get());
				if (r != eMetaOp_Succeed)
					return r;
			}
			if (meta->IsRead()) {
				for (int i = 0; i < state->mIndexBufferCount; i++) {
					T3GFXBuffer* buf = state->mpIndexBuffer[i].get();
					if (buf)
						buf->AsyncRead(meta);
				}
				for (int i = 0; i < state->mVertexBufferCount; i++) {
					T3GFXBuffer* buf = state->mpVertexBuffer[i].get();
					if (buf)
						buf->AsyncRead(meta);
				}
			}//written in perform meta serialize async
		}
		return r;
	}

};

// ====================================================================================== SAMPLER STATE BLOCK =========================================================================================

enum T3SamplerStateValue : unsigned int {
	eSamplerState_WrapU_Value = 0x0,// see TextureWrapMode.
	eSamplerState_WrapV_Value = 0x1,// see TextureWrapMode.
	eSamplerState_Filtered_Value = 0x2,//bool
	eSamplerState_BorderColor_Value = 0x3,//See TextureBorderColor 
	eSamplerState_GammaCorrect_Value = 0x4,//bool
	eSamplerState_MipBias_Value = 0x5,//uchar8bit
	eSamplerState_Count = 0x6,
};

enum TextureWrapMode {
	TEXTURE_WRAP_CLAMP = 0,
	TEXTURE_WRAP_WRAP = 1,
	TEXTURE_WRAP_BORDER = 2,
};

enum TextureBorderColor {
	TEXTURE_BORDER_COLOR_BLACK = 0,//Color(0,0,0,0)
	TEXTURE_BORDER_COLOR_WHITE = 1,//Color(1,1,1,1)
};

struct T3SamplerStateBlock;
extern T3SamplerStateBlock kDefault;

struct RenderFrameUpdateList;

/**
 * BINARY VARIABLE SIZE BITSET. FORMAT of the u32 word:
 * Bit Indices Inclusive | 0 THROUGH 3 | 4 THROUGH 7 | 8 THROUGH 8 | 9 THROUGH 12 | 13 THROUGH 13 | 14 THROUGH 21 |
 * Meaning               | TEX. WRAP U | TEX. WRAP V | bIsFiltered | BORDER COLOR | bGammaCorrect | MIP BIAS VAL. |

	WrapU and WrapV are enum values of TextureWrapMode.
	Filtered is a boolean flag.
	Border Color is also an enum. Value of 0 means black, 1 meaning white.
	Gamma correct is a boolean flag.
	mip bias is an unsigned char byte.

	The kDefault sampler has: wrap u and v both 1 (WRAP mode). filtered is true. border is black, (alpha 0). gamma correct false. mip bias 0.

 */
struct T3SamplerStateBlock {
	unsigned int mData;

	struct SamplerStateEntry {
		unsigned int mShift;
		unsigned int mMask;
	};

	static bool sInitialized;
	static SamplerStateEntry smEntries[6];

	static void Initialize() {
		if (sInitialized)return;
		smEntries[(unsigned int)T3SamplerStateValue::eSamplerState_WrapU_Value].mShift = 0x0;//0x14102C988
		smEntries[(unsigned int)T3SamplerStateValue::eSamplerState_WrapU_Value].mMask = 0xF;//0x14102C98C, 4 bits, so a nibble of data
		smEntries[(unsigned int)T3SamplerStateValue::eSamplerState_WrapV_Value].mShift = 0x4;//0x14102C990
		smEntries[(unsigned int)T3SamplerStateValue::eSamplerState_WrapV_Value].mMask = 0xF0;//0x14102C994, 4 bits, so a nibble of data
		smEntries[(unsigned int)T3SamplerStateValue::eSamplerState_Filtered_Value].mShift = 0x8;//0x14102C998
		smEntries[(unsigned int)T3SamplerStateValue::eSamplerState_Filtered_Value].mMask = 0x100;//0x14102C99C //1 bit, so a bool
		smEntries[(unsigned int)T3SamplerStateValue::eSamplerState_BorderColor_Value].mShift = 0x9;//0x14102C9A0
		smEntries[(unsigned int)T3SamplerStateValue::eSamplerState_BorderColor_Value].mMask = 0x1E00;//0x14102C9A4, 4 bits, so a nibble of data
		smEntries[(unsigned int)T3SamplerStateValue::eSamplerState_GammaCorrect_Value].mShift = 0xD;//0x14102C9A8
		smEntries[(unsigned int)T3SamplerStateValue::eSamplerState_GammaCorrect_Value].mMask = 0x2000;//0x14102C9AC, 1 bit so a bool
		smEntries[(unsigned int)T3SamplerStateValue::eSamplerState_MipBias_Value].mShift = 0xE;//0x14102C9B0
		smEntries[(unsigned int)T3SamplerStateValue::eSamplerState_MipBias_Value].mMask = 0x3FC000;//0x14102C9B4, 8 bits, so a byte of data
		kDefault.mData = (((kDefault.mData & 0xFFFFFFF0 | 1)
			& 0xFFFFFF0F | 0x10) & 0xFFFFFEFF | 0x100) & 0xFFC001FF;//273, filtered yes, wrap u and v are 1
		sInitialized = true;
	}

	inline T3SamplerStateBlock() : mData(0) { Initialize(); }

	inline void SetStateMask(T3SamplerStateValue state) {
		mData |= smEntries[state].mMask;
	}

	T3SamplerStateBlock Merge(T3SamplerStateBlock const& rhs, T3SamplerStateBlock const& mask) {
		T3SamplerStateBlock result{};
		result.mData = (mask.mData & rhs.mData) | (mData & ~mask.mData);
		return result;
	}

	u64 InternalGetSamplerState(T3SamplerStateValue state) {
		return (this->mData & smEntries[state].mMask) >> (smEntries[state].mShift);
	}

	void InternalSetSamplerState(T3SamplerStateValue state, unsigned int value) {
		mData &= ~smEntries[state].mMask;
		mData |= value << smEntries[state].mShift;
	}

	u64 DecrementMipBias(unsigned int steps) {
		float v2 = 8.0f;
		u64 result;
		unsigned int v3 = steps + (smEntries[5].mMask & mData) >> smEntries[5].mShift;
		if (v3 < v2)
			v2 = (float)v3;
		result = (unsigned int)floorf(v2);
		unsigned int v5 = mData & ~smEntries[5].mMask;
		mData = v5 | (result << smEntries[5].mShift);
		return result;
	}

};

// ====================================================================================== BITSET DECLARATIONS =========================================================================================

#define EFFECT_FEATURES_STATIC_MAX 54
#define EFFECT_FEATURES_DYNAMIC_MAX 30
#define EFFECT_FEATURES_MAX (EFFECT_FEATURES_STATIC_MAX+EFFECT_FEATURES_DYNAMIC_MAX)

struct T3EffectCacheVersionDatabaseStructHeader {
	u32 mStaticFeaturesCount = 0, mDynamicFeaturesCount = 0;
	u32 mFirstStaticFeature = 0, mFirstDynamicFeature = 0;
};

using DynamicFeatures = VBitSet<enum T3EffectFeature, EFFECT_FEATURES_DYNAMIC_MAX,
	offsetof(T3EffectCacheVersionDatabaseStructHeader, mFirstDynamicFeature), offsetof(T3EffectCacheVersionDatabaseStructHeader, mDynamicFeaturesCount)>;

using StaticFeatures = VBitSet<enum T3EffectFeature, EFFECT_FEATURES_STATIC_MAX,
	offsetof(T3EffectCacheVersionDatabaseStructHeader, mFirstStaticFeature), offsetof(T3EffectCacheVersionDatabaseStructHeader, mStaticFeaturesCount)>;

using EffectFeatures = BitSet<enum T3EffectFeature, EFFECT_FEATURES_MAX, 0>;

using EffectTypes = BitSet<enum  T3EffectType, 225, 0>;
using EffectQualities = BitSet<enum T3EffectQuality, 5, 0>;
using GamePlatforms = BitSet<enum PlatformType, 16, 0>;


// ====================================================================================== RENDER DESC STRUCTS =========================================================================================

#include "RenderTypeDescs.inl"

// ====================================================================================== RENDER TYPE DESC LIST DECLS =========================================================================================

extern QualityEntry sQualityEntry[eRenderQuality_Count];
extern T3PostEffectDesc sPostEffectDesc[65];

//These three are specific to each platform and game per engine version for the effect cache API to work. use the version database for these.

extern T3EffectFeatureDesc sFeatureDesc_WDC[84];
extern T3EffectDesc sEffectDesc_WDC[219];
//extern T3EffectQualityDesc sQualityDesc[5];

extern T3MaterialShaderDesc sShaderDesc[4];
extern T3MaterialBlendModeDesc sBlendModeDesc[4];
extern T3MaterialChannelDesc sChannelDesc[46];
extern T3MaterialChannelFormatDesc sChannelFormatDesc[4];
extern T3MaterialDomainDesc sDomainDesc[5];
extern T3MaterialLightModelDesc sLightModelDesc[14];
extern T3MaterialOptionalPropertyDesc sOptionalPropertyDesc[1];
extern T3MaterialPassDesc sPassDesc[21];
extern T3MaterialPropertyDesc sPropertyDesc[31];
extern T3MaterialSceneTextureDesc sSceneTextureDesc[2];
extern T3MaterialShaderInputDesc sInputDesc[73];
extern T3MaterialTextureDesc sTextureDesc[17];
extern T3MaterialTexturePropertyDesc sTexturePropertyDesc[1];
extern T3MaterialTransform2DComponentDesc sTransform2DDesc[13];
extern T3MaterialValueDesc sValueDesc[5];
extern T3MeshVertexStreamDesc sStreamDesc[32];
extern T3EffectParameterClassDesc sClassDesc[eEffectParameterClass_Count];
extern T3EffectParameterDesc sBufferDesc[eEffectParameter_Count];
extern T3EffectBufferFieldDesc sParameterDesc[475];
extern T3RenderTargetDesc sTargetDesc[eRenderTargetID_Count];

// ====================================================================================== EFFECT CACHE SHADER SYSTEM TYPES AND DECLS =========================================================================================

// -- SHADER PACKAGES

//.T3FXPROGRAM FILES (VIRTUAL)
struct T3EffectCachePackageProgram {
	T3EffectType mEffectType;
	EffectFeatures mEffectFeatures;
	u64 mMaterialCrc;
	T3EffectQuality mEffectQuality;
	u32 mPassCount, mSamplerCount;
};

//.T3FXSHADER FILES (VIRTUAL)
struct T3EffectCachePackageShader {
	//unsigned int mShaderType;
	T3MaterialShaderType mShaderType;
	BitSet<T3EffectParameterType, 150, 0> mNeededParameters;
	unsigned int mOffset;
	unsigned int mSize;
};

struct T3EffectCacheContext;

struct T3EffectCacheParams
{

	StaticFeatures mStaticFeatures;
	T3EffectQuality mQuality;
	u64 mMaterialCrc;

	inline T3EffectCacheParams(const T3EffectCacheContext& context);

	inline T3EffectCacheParams() {}

};

//OLD API. T3FXPACK FILES
struct T3EffectCachePackage {
	T3EffectCachePackageHeader mHeader;
	DCArray<T3EffectCachePackageProgram> mPrograms;
	DCArray<T3EffectCachePackageShader> mShaders;
};

//RENDER NAMESPACE.
namespace T3 {

	//CALL TO INIT RENDER TT API
	void Initialize();

	//CALL TO SHUTDOWN
	void Shutdown();

}

struct T3EffectCacheProgram;

struct T3EffectCacheProgramHashEntry
{
	T3EffectCacheProgramHashEntry* mpNext;
	T3EffectCacheProgram* mpProgram;
	unsigned int mKey;
};

struct T3EffectCacheSampler
{
	unsigned int mSamplerParameterIndex;
	T3SamplerStateBlock mSamplerState;
	T3SamplerStateBlock mSamplerStateMask;
};

struct T3EffectCachePassBase
{
	BitSet<enum T3EffectParameterType, 150, 0> mNeededParameters;

	T3EffectCachePassBase() = default;

};

struct T3EffectCacheShader : ListNode<T3EffectCacheShader>
{
	void* mpGFXShader;
	BitSet<enum T3EffectParameterType, 150, 0> mNeededParameters;
	u32 mShaderType;
	u32 mOffset;
	u32 mSize;
};

struct T3EffectCachePass_Draw : T3EffectCachePassBase
{
	T3EffectCacheShader* mpDrawShader[eGFXPlatformShader_Count];
	u32 mInitialShaderIndex[eGFXPlatformShader_Count];
	void* mpGFXProgram;
	T3RenderStateBlock mRenderStateBlock;
	T3RenderStateBlock mRenderStateMask;

	T3EffectCachePass_Draw() = default;

};

struct T3EffectCachePass_Compute : T3EffectCachePassBase
{
	T3EffectCacheShader* mpComputeShader;
	u32 mInitialShaderIndex;
	u32 mThreadGroupSizeX;
	u32 mThreadGroupSizeY;
	u32 mThreadGroupSizeZ;

	T3EffectCachePass_Compute() = default;

};

struct T3EffectCacheBucket;

//.T3FXPROGRAM FILES (INSTANCE VERSION)
struct T3EffectCacheProgram : ListNode<T3EffectCacheProgram>
{
	//mpStream is the actual t3fxpack. shaders offset and size. this is because different programs can come from different packs.
	std::shared_ptr<DataStream> mpStream;

	T3EffectCacheBucket* mpBucket;
	DynamicFeatures mFeatures;
	BitSet<enum T3EffectParameterType, 150, 0> mNeededParameters;
	union
	{
		T3EffectCachePass_Draw* mPassesDraw;
		T3EffectCachePass_Compute* mPassesCompute;
	};
	T3EffectCacheSampler* mSamplers;
	u16 mPreloadMarker;
	u16 mFlags;
	u8 mStatus;
	u8 mPassCount;
	u8 mSamplerCount;
	//u8 mNumFailedCreateAttempts;
};

struct T3EffectCacheBucket
{
	T3EffectCacheBucket* mpNext;
	T3EffectCacheParams mParams;
	u16 mEffectType;
	u16 mProgramHashSize;
	T3EffectCacheProgramHashEntry* mProgramHash[1];
};

struct T3EffectCacheRef
{
	T3EffectCacheBucket* mpValue;
};

struct T3EffectCacheVariantRef
{
	T3EffectCacheProgram* mpValue;

};

struct T3EffectCacheBucketHashEntry
{
	T3EffectCacheBucketHashEntry* mpNext;
	T3EffectCacheBucket* mpBucket;
	StaticFeatures mStaticFeatures;
};

struct T3EffectCacheZeroStrideBuffer
{
	std::shared_ptr<T3GFXBuffer> mpGFXBuffer;
	u64 mValueCRC;
	u32 mStride;
	u32 mCount;
};

struct T3EffectCacheVertexLayout
{
	u64 mHash;
	GFXPlatformVertexLayout* mpVertexLayout;
};

struct T3EffectCacheVertexState
{
	u64 mGFXProgramKey;
	std::shared_ptr<T3GFXVertexState> mpVertexState;
	u32 mGFXFrameUsed;
	u32 mKey;
	u32 mPacked_BaseIndex_IndexBufferIndex;
};

//.bin
struct T3EffectCacheVersionDatabase : T3EffectCacheVersionDatabaseStructHeader {

	String mGameID;
	LinearHeap mStringsHeap;
	std::vector<T3EffectQualityDesc> mQualities;
	std::vector<T3EffectDesc> mEffects;
	std::vector<T3EffectFeatureDesc> mFeatures;

};

#define EFFECT_CACHE_HASH_ENTRIES 64567

//GLOBAL SHADER CONTEXT
struct T3EffectCacheContext
{

	inline T3EffectCacheContext() : mHeap(0x40000) {
		_ResetInt();
	}

	LinearHeap mHeap;
	std::vector<T3EffectCacheVertexState> mVertexStates;
	std::vector<T3EffectCacheZeroStrideBuffer> mZeroStrideBuffers;
	std::vector<T3EffectCacheVertexLayout> mVertexLayouts;
	T3EffectCacheBucketHashEntry* mBucketHash[EFFECT_CACHE_HASH_ENTRIES];
	T3EffectCacheBucket* mpFirstBucket = 0;
	LinkedList<T3EffectCacheProgram> mPrograms[eEffectProgramStatus_Count];
	LinkedList<T3EffectCacheShader> mShaders;
	u32 mBucketCount = 0;
	u32 mBucketHashEntryCount = 0;
	u32 mVertexStateGarbageCollectionIndex = 0;
	u16 mCurrentPreloadMarker = 0;
	std::shared_ptr<DataStreamContainer> mpShaderBinary;
	PlatformType mLoadedPackagePlatform = ePlatform_PC;
	T3EffectCacheVersionDatabase* mpVersionDatabase;

	inline void SetGameID(const char* pGameID){
		mpVersionDatabase = TelltaleToolLib_GetEffectCacheDB(pGameID);
		if (mpVersionDatabase == 0)
			TTL_Log("WARN: Cannot find a loaded effect cache version database for %s, please ensure they are loaded to use T3EffectCache API!", pGameID);
	}

	inline bool _OK(){
		if (mpVersionDatabase == 0)
			TTL_Log("WARN: Call to T3EffectCache API was made, but ignored due to no version database being loaded");
		return mpVersionDatabase != 0;
	}

	//INTERNAL
	inline void _ResetInt(){
		memset(mBucketHash, 0, 8 * EFFECT_CACHE_HASH_ENTRIES);
		mCurrentPreloadMarker = 1;
		mVertexStateGarbageCollectionIndex = 0;
		mBucketCount = mBucketHashEntryCount = 0;
	}

	//Reset the context and unload everything.
	inline void ResetContext() {
		mVertexStates.clear();
		mZeroStrideBuffers.clear();
		mVertexLayouts.clear();
		mpShaderBinary.reset();
		mHeap.ReleaseAll();
		_ResetInt();
		mpVersionDatabase = 0;
	}
};

inline T3EffectCacheParams::T3EffectCacheParams(const T3EffectCacheContext& context){
	mStaticFeatures.SetTether((void*)context.mpVersionDatabase);
}

struct RenderThreadContext;

// ==================================================== RUNTIME LIBRARY VERSION OF TYPES FOR FXPACK SAVING ==============================================

struct LoadedShader {
	std::string mTypeName;
	BitSet<T3EffectParameterType, 150, 0> mNeededParameters;
	std::shared_ptr<DataStreamSubStream> mpShaderData;
	u32 mLoadedBinOff, mLoadedBinSize;
};

struct LoadedProgramPass {
	int mShader[3] = { -1,-1,-1 };
	u32 threadGroupSize[3] = { 1,1,1 };
	T3RenderStateBlock mRenderState, mRenderStateMask;
};

struct LoadedProgram {
	std::string mBuildFeatureList;
	DynamicFeatures mVariantFeatures{};
	bool mbCompute = false;
	std::vector<LoadedProgramPass> mPasses;
};

struct LoadedBucket {
	T3EffectCacheParams mParams;
	std::vector<LoadedProgram> mVariants;
	std::string mBuiltName;
	T3EffectType mEffect;
};

// ====================================================================================================================================================

//SHADER SYSTEM NAMESPACE
namespace T3EffectCache {

	static constexpr inline u32 kProgramHashSizes[5] = { 0x2, 0xB, 0x25, 0x43, 0x83 };

	inline void Initialize() {
		;
	}

	inline void Shutdown(){
		;
	}

	inline StaticFeatures GetValidStaticFeatures(T3EffectCacheContext& context, StaticFeatures staticFeatures, T3EffectQuality quality) {
		if (!context._OK())
			return StaticFeatures{};
		int _SCntCache = staticFeatures.GetCount();
		int _FCache = staticFeatures.GetFirst();
		for (T3EffectFeature i = (T3EffectFeature)_FCache; i <
			_FCache + _SCntCache; i = (T3EffectFeature)(i + 1)) {
			if (staticFeatures[i]) {
				T3EffectFeatureDesc& fdesc = context.mpVersionDatabase->mFeatures[i];
				for (int j = 0; j < _SCntCache; j++) {
					if (fdesc.mRequiredStaticFeatures[(T3EffectFeature)(j + _SCntCache)] &&
						!staticFeatures[(T3EffectFeature)(j + _FCache)]) {
						staticFeatures.Set(i, false);
						break;
					}
				}
				for (int j = 0; j < _SCntCache; j++) {
					if (fdesc.mIncompatibleStaticFeatures[(T3EffectFeature)(j + _FCache)]
						&& staticFeatures[(T3EffectFeature)(j + _FCache)]) {
						staticFeatures.Set(i, false);
						break;
					}
				}
			}
		}
		return staticFeatures;
	}

	inline StaticFeatures GetValidStaticFeatures(T3EffectCacheContext& context, T3EffectType effect, StaticFeatures staticFeatures, T3EffectQuality quality){
		if (!context._OK())
			return StaticFeatures{};
		staticFeatures.Mask(context.mpVersionDatabase->mEffects[effect].mFeatures[quality].mValidStaticFeatures);
		return GetValidStaticFeatures(context, staticFeatures, quality);
	}

	inline DynamicFeatures GetValidDynamicFeatures(T3EffectCacheContext& context, T3EffectQuality quality, PlatformType platform) {
		if (!context._OK())
			return DynamicFeatures{};
		DynamicFeatures ret{context.mpVersionDatabase};
		int _CntCache = ret.GetCount();
		int _FCache = ret.GetFirst();
		for (T3EffectFeature i = (T3EffectFeature)_FCache; i < _FCache + _CntCache; i = (T3EffectFeature)(i + 1)) {
			T3EffectFeatureDesc& fdesc = context.mpVersionDatabase->mFeatures[i];
			if (fdesc.mExcludePlatforms[platform])
				ret.Set(i, false);
			if (fdesc.mExcludeQuality[quality])
				ret.Set(i, false);
		}
		return ret;
	}

	inline DynamicFeatures GetValidDynamicFeatures(T3EffectCacheContext& context, T3EffectType effect, StaticFeatures staticFeatures, DynamicFeatures dynamicFeatures, T3EffectQuality quality, PlatformType platform){
		if (!context._OK())
			return DynamicFeatures{};
		T3EffectDesc& desc = context.mpVersionDatabase->mEffects[effect];
		DynamicFeatures ret = dynamicFeatures;
		int _SCntCache = staticFeatures.GetCount();
		int _SFCache = staticFeatures.GetFirst();
		int _DCntCache = dynamicFeatures.GetCount();
		int _DFCache = dynamicFeatures.GetFirst();
		for(int i = 0; i < _SCntCache; i++){
			if (staticFeatures[(T3EffectFeature)i])
				ret.Set(context.mpVersionDatabase->mFeatures[_SFCache + i].mIncludeDynamicFeatures);
		}
		for (int i = 0; i < _DCntCache; i++) {
			if (dynamicFeatures[(T3EffectFeature)(_DFCache + i)])
				ret.Set(context.mpVersionDatabase->mFeatures[_DFCache + i].mIncludeDynamicFeatures);
		}
		for(int i = 0; i < std::remove_reference_t<decltype(dynamicFeatures)>::NumWords; i++){
			ret.mWords[i] &= desc.mFeatures[quality].mValidDynamicFeatures.mWords[i];
		}
		for(int i = 0; i < _DCntCache; i++){
			if(ret[(T3EffectFeature)(_DFCache + i)]){
				T3EffectFeatureDesc& fdesc = context.mpVersionDatabase->mFeatures[_DFCache + i];
				T3EffectFeature feat = (T3EffectFeature)(_DFCache + i);
				if (fdesc.mExcludePlatforms[platform])
					ret.Set(feat, false);
				if (fdesc.mExcludeQuality[quality])
					ret.Set(feat, false);
				if (fdesc.mMinRequiredFeatureLevel < 1 && fdesc.mMaxSupportedFeatureLevel > 1)
					ret.Set(feat, false);
				for (int j = 0; j < _SCntCache; j++) {
					if(fdesc.mRequiredStaticFeatures[(T3EffectFeature)(j + _SFCache)] && !staticFeatures[(T3EffectFeature)(j + _SFCache)]){
						ret.Set(feat, false);
						break;
					}
				}
				for (int j = 0; j < _SCntCache; j++) {
					if (fdesc.mIncompatibleStaticFeatures[(T3EffectFeature)(j + _SFCache)] && staticFeatures[(T3EffectFeature)(j + _SFCache)]) {
						ret.Set(feat, false);
						break;
					}
				}
				// no features have dynamic features any
			}
		}
		return ret;
	}

	inline u32 GetValidDynamicFeatureCount(T3EffectCacheContext& context, T3EffectType effectType, StaticFeatures staticFeatures, T3EffectQuality quality, PlatformType currentPlatform) {
		if (!context._OK())
			return 0;
		u32 cnt = 0;
		DynamicFeatures toTest{context.mpVersionDatabase};
		toTest.SetAll(1);
		toTest = GetValidDynamicFeatures(context, effectType, staticFeatures, toTest, quality, currentPlatform);
		int _CntCache = toTest.GetCount();
		int _FCache = toTest.GetFirst();
		for(int i = 0; i < _CntCache; i++){
			if (toTest[(T3EffectFeature)(i + _FCache)])
				cnt++;
		}
		return cnt;
	}

	inline char* AllocateHeap(T3EffectCacheContext& context, u32 size, u32 align){
		if (!context._OK())
			return 0;
		return context.mHeap.Alloc(size, align);
	}

	inline void AllocateProgramMemory(T3EffectCacheContext& context, T3EffectCacheProgram& program){
		if (!context._OK())
			return;
		bool bCompute = context.mpVersionDatabase->mEffects[program.mpBucket->mEffectType].mCategory == eEffectCategory_Compute;
		void* pMemory = context.mHeap.Alloc((sizeof(T3EffectCacheSampler) * program.mSamplerCount) + ((bCompute ? sizeof(T3EffectCachePass_Compute) : sizeof(T3EffectCachePass_Draw)) * program.mPassCount), 16);
		program.mSamplers = (T3EffectCacheSampler*)pMemory;
		program.mPassesDraw = (T3EffectCachePass_Draw*)((T3EffectCacheSampler*)pMemory + program.mSamplerCount);
		memset(program.mPassesDraw, 0, bCompute ? sizeof(T3EffectCachePass_Compute) : sizeof(T3EffectCachePass_Draw));
	}

	inline void InsertProgramKey(T3EffectCacheContext& context, T3EffectCacheBucket& bucket, u32 key, T3EffectCacheProgram* pProgram){
		if (!context._OK())
			return;
		T3EffectCacheProgramHashEntry* pEntry = (T3EffectCacheProgramHashEntry*)AllocateHeap(context, sizeof(T3EffectCacheProgramHashEntry), 8);
		pEntry->mKey = key;
		pEntry->mpProgram = pProgram;
		pEntry->mpNext = bucket.mProgramHash[key % bucket.mProgramHashSize];
		bucket.mProgramHash[key % bucket.mProgramHashSize] = pEntry;
	}

	inline u32 GetProgramKey(DynamicFeatures features){
		return features.mWords[0];
	}

	inline void SetProgramStatus(T3EffectCacheContext & context, T3EffectCacheProgram & program, T3EffectCacheProgramStatus status) {
		if (!context._OK())
			return;
		if(program.mStatus != status){
			context.mPrograms[program.mStatus].remove(&program);
			context.mPrograms[program.mStatus = status].insert_tail(&program);
		}
	}

	inline T3EffectCacheProgramHashEntry* _FindProgramHashEntry(T3EffectCacheRef& effectRef, u32 k){
		return effectRef.mpValue->mProgramHash[k & effectRef.mpValue->mProgramHashSize];
	}

	inline T3EffectCacheProgram* _GetProgramInternal(T3EffectCacheProgramHashEntry* pEntry, u32 k){
		while (pEntry) {
			if (pEntry->mKey == k) {
				return pEntry->mpProgram;
			}
			pEntry = pEntry->mpNext;
		}
		return 0;
	}

	inline T3EffectCacheProgram* FindProgram(T3EffectCacheContext& context, T3EffectCacheBucket& bucket, u32 key){
		if (!context._OK())
			return 0;
		T3EffectCacheProgramHashEntry* pEntry = bucket.mProgramHash[key % bucket.mProgramHashSize];
		if (!pEntry)
			return 0;
		return _GetProgramInternal(pEntry, key);
	}

	inline T3EffectCacheProgram* GetProgram(T3EffectCacheContext& context, T3EffectCacheRef& effectRef, DynamicFeatures& effectFeatures, bool bMarkPreloaded, PlatformType currentPlatform){
		if (!context._OK())
			return 0;
		if (!effectRef.mpValue)
			return 0;

		//First test if with requested features the program is created
		u32 originalK = GetProgramKey(effectFeatures);
		T3EffectCacheProgramHashEntry* pProgramHash = _FindProgramHashEntry(effectRef, originalK);
		T3EffectCacheProgram* pProgram = 0;
		if (pProgramHash) {
			pProgram = _GetProgramInternal(pProgramHash, originalK);
			if (pProgram)
				return pProgram;
		}

		//Second test if with valid features masked into the features is created
		u32 validK = GetProgramKey(GetValidDynamicFeatures(context, (T3EffectType)effectRef.mpValue->mEffectType, effectRef.mpValue->mParams.mStaticFeatures, effectFeatures, effectRef.mpValue->mParams.mQuality, currentPlatform));
		pProgramHash = _FindProgramHashEntry(effectRef, validK);
		
		//ExplodeBucketEntry seemed to just print to somewhere stripped.
		/*if(!pProgramHash || !(pProgram=_GetProgramInternal(pProgramHash, validK))){
			//New program hash and programs for the requested feat. If program hash is 0, create the programs but return null.
			pProgramHash = _FindProgramHashEntry(effectRef, originalK);
			if (pProgramHash)
				_ExplodeBucketEntry(effectRef.mpValue, pProgramHash);
			else if(effectRef.mpValue->mProgramHashSize){
				//Explode all
				for(u16 i = 0; i < effectRef.mpValue->mProgramHashSize; i++){
					_ExplodeBucketEntry(effectRef.mpValue, effectRef.mpValue->mProgramHash[i]);
				}
			}
		}*/

		if (pProgramHash)
			pProgram = _GetProgramInternal(pProgramHash, validK);

		if(validK != originalK){
			//If they are different, set the program key for original features to map to the valid features program entry.
			if (!pProgram)
				return 0;
			InsertProgramKey(context, *effectRef.mpValue, originalK, pProgram);
		}
		return pProgram;
	}

	//Dont use this. Use CreateProgram
	inline void _CreateShader(T3EffectCacheContext& context, T3EffectCacheProgram& program, T3EffectCacheShader& shader){
		if (!context._OK())
			return;
		//set shader GFX shader to openGL shader
	}

	//Normally would do this async and use job system (SubmitCreationJob in internal effect cache).
	inline void CreateProgram(T3EffectCacheContext& context, T3EffectCacheProgram& program){
		if (!context._OK())
			return;
		bool bComputeProgram = context.mpVersionDatabase->mEffects[program.mpBucket->mEffectType].mCategory == eEffectCategory_Compute;
		for(int i = 0; i < program.mPassCount; i++){
			if (bComputeProgram) {
				if(program.mPassesCompute[i].mpComputeShader)
					_CreateShader(context, program, *program.mPassesCompute[i].mpComputeShader);
			}else{
				for(int j = 0; j < 3; j++){
					if (program.mPassesDraw[i].mpDrawShader[j])
						_CreateShader(context, program, *program.mPassesDraw[i].mpDrawShader[j]);
				}
			}
		}
		//use all GFX shader poitners in shaders, then link them and create openGL program.
	}

	//Fowler-Noll-Vo hash function, 32 bit. offset basis 84696351 and FNV prime (Default) 16777619. Same as mathematica default.
	inline u32 GetBucketKey(T3EffectType effectType, const T3EffectCacheParams& params){
		u32 hash = 84696351;//FNV offset basis
		hash = (hash * 16777619) ^ (u8)((effectType >> 0) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((effectType >> 8) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mQuality >> 0) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mMaterialCrc >> 56) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mMaterialCrc >> 48) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mMaterialCrc >> 40) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mMaterialCrc >> 32) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mMaterialCrc >> 24) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mMaterialCrc >> 16) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mMaterialCrc >> 8) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mMaterialCrc >> 0) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mStaticFeatures.mWords[0] >> 24) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mStaticFeatures.mWords[0] >> 16) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mStaticFeatures.mWords[0] >> 8) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mStaticFeatures.mWords[0] >> 0) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mStaticFeatures.mWords[1] >> 24) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mStaticFeatures.mWords[1] >> 16) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mStaticFeatures.mWords[1] >> 8) & 0xFF);
		hash = (hash * 16777619) ^ (u8)((params.mStaticFeatures.mWords[1] >> 0) & 0xFF);
		return hash;
	}
	
	inline void InsertBucketKey(T3EffectCacheContext& context, u32 key, StaticFeatures features, T3EffectCacheBucket* pBucket){
		if (!context._OK())
			return;
		T3EffectCacheBucketHashEntry* pEntry = (T3EffectCacheBucketHashEntry*)AllocateHeap(context, sizeof(T3EffectCacheBucketHashEntry), 8);
		pEntry->mpBucket = pBucket;
		pEntry->mStaticFeatures = features;
		pEntry->mpNext = context.mBucketHash[key % EFFECT_CACHE_HASH_ENTRIES];
		context.mBucketHash[key % EFFECT_CACHE_HASH_ENTRIES] = pEntry;
		context.mBucketHashEntryCount++;
	}

	inline T3EffectCacheBucket* FindBucket(T3EffectCacheContext& context, u32 key, T3EffectType effectType, const T3EffectCacheParams& params){
		if (!context._OK())
			return 0;
		T3EffectCacheBucketHashEntry* pEntry = context.mBucketHash[key % EFFECT_CACHE_HASH_ENTRIES];
		while(pEntry){
			if (pEntry->mStaticFeatures == params.mStaticFeatures && pEntry->mpBucket && pEntry->mpBucket->mEffectType == effectType && pEntry->mpBucket->mParams.mMaterialCrc == params.mMaterialCrc && pEntry->mpBucket->mParams.mQuality == params.mQuality)
				return pEntry->mpBucket;
			pEntry = pEntry->mpNext;
		}
		return 0;
	}

	inline T3EffectCacheBucket* GetBucket(T3EffectCacheContext& context, T3EffectType effectType, const T3EffectCacheParams& params){
		if (!context._OK())
			return 0;
		u32 key = GetBucketKey(effectType, params);
		T3EffectCacheBucket* pBucket = FindBucket(context, key, effectType, params);
		if(!pBucket){
			T3EffectCacheParams tryParams = params;
			tryParams.mStaticFeatures = GetValidStaticFeatures(context, effectType, params.mStaticFeatures, params.mQuality);
			pBucket = FindBucket(context, key, effectType, tryParams);
			if(!pBucket){			
				u32 hashEntriesIndex = 0;//number of bucket hash entries (already one is stored) index. DOES INCLUDE the [1] at the end of the struct
				u32 validDynamicFeatures = (1u << GetValidDynamicFeatureCount(context, effectType, params.mStaticFeatures, params.mQuality, context.mLoadedPackagePlatform)) >> 1u;//div 2, dynamic feature count can be 0 to 30, so fits in one u32. div 2, gets range 0 to 15 (ok for hashes 0 to 65657)
				do {
					if (validDynamicFeatures < kProgramHashSizes[hashEntriesIndex])
						break;
				} while (++hashEntriesIndex < 4);
				u32 allocSize = sizeof(T3EffectCacheBucket) + ((u32)(kProgramHashSizes[hashEntriesIndex] /*- 1*/) << 3u);
				pBucket = (T3EffectCacheBucket*)AllocateHeap(context, allocSize, 8);//-1 bc struct includes one anyway
				memset(pBucket, 0, allocSize);
				pBucket->mEffectType = effectType;
				pBucket->mParams = params;
				pBucket->mProgramHashSize = kProgramHashSizes[hashEntriesIndex];
				pBucket->mpNext = context.mpFirstBucket;
				context.mpFirstBucket = pBucket;
				context.mBucketCount++;
				//insert key
				InsertBucketKey(context, GetBucketKey(effectType, tryParams), tryParams.mStaticFeatures, pBucket);
			}
			//insert key for alias if needed
			if(params.mStaticFeatures != tryParams.mStaticFeatures)
				InsertBucketKey(context, key, params.mStaticFeatures, pBucket);
		}
		return pBucket;
	}

	inline u32 GetVersion(PlatformType platformType){
		if (platformType == ePlatform_PC)
			return 196;
		else if (platformType == ePlatform_Xbox)
			return 197;
		else if (platformType == ePlatform_Mac)
			return 201;
		else if (platformType == ePlatform_iPhone)
			return 222;
		else if (platformType == ePlatform_Android)
			return 220;
		else if (platformType == ePlatform_PS4)
			return 202;
		else if (platformType == ePlatform_XBOne)
			return 194;
		else if (platformType == ePlatform_NX)
			return 218;
		return 193;
	}

	inline u32 GetProgramBinaryVersion(PlatformType currentPlatform) {
		return GetVersion(currentPlatform) + 15;
	}

	/**
	 * Call this to load into the library shader packages. Pass in the render context and also the list of string shader pack names to use (.t3fxpack). 
	 * If any of the shader packs don't end in t3fxpack, assumed to be a folder and loaded from the folder.
	 */
	void LoadShaderPackages(T3EffectCacheContext& context, RenderThreadContext* pRenderThreadContext, const std::vector<String>& shaderPackNames);

	/**
	 * Loads a shader package. Use LoadShaderPackages.
	 */
	bool LoadPackage(T3EffectCacheContext& context, DataStream* pStream, int& outVersion);

	/**
	 * Save Package. Uses loaded stuff. See ShaderEditor.cpp. pPreviousPackage is from previous fxpack is one is present (to get shader bin) if not overriden by customs.
	 */
	bool SavePackage(std::shared_ptr<DataStreamContainer> pPreviousPackage, std::vector<LoadedShader>& shaders, std::vector<LoadedBucket> & buckets, DataStream* pOutStream, int version);

}

// ====================================================================================== [[LEGACY]] OLD SHADER READING (WDC PC) API =========================================================================================

//OLD API. Everyhing newer is in T3EffectCache including telltale T3EffectCacheInternal namespace.
namespace T3EffectCache_Legacy {

	bool LoadPackage(T3EffectCachePackage& package, DataStream& stream);

	bool LoadPackageHeader(T3EffectCachePackageHeader& p, DataStream& stream);

	bool LoadPackageShader(T3EffectCachePackageShader& p, DataStream& stream);

	bool LoadPackageProgram(T3EffectCachePackageProgram& p, DataStream& stream);


	/*
	* PAYLOAD FORMAT:
	* 4byte: num of params
	* 4byte: size of platform compiled shader
	* for num of params:
	* 7byte: param value (not sure on the actual data in it, most likely a bitset)
	* 
	* shader compiled
	*/
	bool LoadShaderPayload(const T3EffectCachePackage& p, const T3EffectCachePackageShader& shader, char* pDest, DataStream& stream);

	unsigned int GetShaderPayloadSize(const T3EffectCachePackageShader& shader);

	// Util functions

	const char* GetFeatureName(T3EffectFeature ft);

	T3EffectFeature GetFeatureEnum(const char*);

	const char* GetParameterTypeName(T3EffectParameterType ft);

	T3EffectParameterType GetParameterTypeEnum(const char*);

	const char* GetShaderTypeName(T3MaterialShaderType sh);

	T3MaterialShaderType GetShaderType(const char* n);

}

#endif