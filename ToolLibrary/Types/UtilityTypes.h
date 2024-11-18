#pragma once

#include "../TelltaleToolLibrary.h"

struct EnumBase {};

struct Guide {
	float m_Time;
	long m_Bitfield;
	long m_AutoActRole;
};

struct ParticleLODKey
{
	float mCountScale;
	float mStrideScale;
	float mDivisionScale;
	float mLifeScale;
};

struct T3LightCinematicRigLOD
{
	Flags mFlags;
};

enum LightCellBlendMode
{
	eLightCellBlendMode_Normal = 0x0,
	eLightCellBlendMode_Dodge = 0x1,
	eLightCellBlendMode_Multiply = 0x2,
	eLightCellBlendMode_Screen = 0x3,
	eLightCellBlendMode_Overlay = 0x4,
};

enum T3NPRSpecularType
{
	T3NPRSpecularType_None = 0x0,
	T3NPRSpecularType_Isotropic = 0x1,
	T3NPRSpecularType_Anisotropic = 0x2,
};

enum ParticlePropDriver
{
	ePartPropDriver_EmitterSpeed = 0x1,
	ePartPropDriver_DistanceToTarget = 0x2,
	ePartPropDriver_BurstTime = 0x3,
	ePartPropDriver_CameraDot = 0x4,
	ePartPropDriver_KeyControl01 = 0x5,
	ePartPropDriver_KeyControl02 = 0x6,
	ePartPropDriver_KeyControl03 = 0x7,
	ePartPropDriver_KeyControl04 = 0x8,
	ePartPropDriver_DistanceToCamera = 0x9,
};

enum T3DetailShadingType
{
	T3DetailShadingType_No_Detail_Map = 0x0,
	T3DetailShadingType_Old_Toon = 0x1,
	T3DetailShadingType_Sharp_Detail = 0x2,
	T3DetailShadingType_Packed_Detail_And_Tiled_Packed_Detail = 0x3,
	T3DetailShadingType_Packed_Detail = 0x4,
	T3DetailShadingType_Single_Channel_Detail = 0x5,
	T3DetailShadingType_Animated_Detail = 0x6,
};

enum ParticlePropModifier
{
	ePartPropModifier_Constraint_Length = 0x0,
	ePartPropModifier_Effect_Scale = 0x1,
	ePartPropModifier_Geometry_Turbulence = 0x2,
	ePartPropModifier_Global_Alpha = 0x3,
	ePartPropModifier_Global_Acceleration = 0x4,
	ePartPropModifier_Max_Particles = 0x5,
	ePartPropModifier_PP_Scale = 0x6,
	ePartPropModifier_PP_Lifespan = 0x7,
	ePartPropModifier_PP_Rotation = 0x8,
	ePartPropModifier_PP_RotationSpeed = 0x9,
	ePartPropModifier_PP_Speed = 0xA,
	ePartPropModifier_PP_Intensity = 0xB,
	ePartPropModifier_Time_Scale = 0xC,
	ePartPropModifier_Sprite_Animation_Rate = 0xD,
	ePartPropModifier_Sprite_Animation_Cycles = 0xE,
	ePartPropModifier_Spawn_Angle = 0xF,
	ePartPropModifier_Spawn_Volume_Sweep = 0x10,
	ePartPropModifier_Spawn_Volume_Sweep_Offset = 0x11,
	ePartPropModifier_Target_Render_Lerp = 0x12,
	ePartPropModifier_Velocity_Turbulence_Force = 0x13,
	ePartPropModifier_Velocity_Turbulence_Speed = 0x14,
	ePartPropModifier_Velocity_Timescale_Modifier = 0x15,
	ePartPropModifier_KeyControl01 = 0x16,
	ePartPropModifier_KeyControl02 = 0x17,
	ePartPropModifier_KeyControl03 = 0x18,
	ePartPropModifier_KeyControl04 = 0x19,
	ePartPropModifier_PP_Alpha = 0x1A,
	ePartPropModifier_Geometry_Scale = 0x1B,
	ePartPropModifier_Enable = 0x1C,
	ePartPropModifier_Count = 0x1D,
};

struct  EnumT3DetailShadingType : EnumBase
{
	T3DetailShadingType mVal;
};

/* 25792 */
struct  EnumT3NPRSpecularType : EnumBase
{
	T3NPRSpecularType mVal;
};

struct  EnumParticlePropModifier : EnumBase
{
	ParticlePropModifier mVal;
};

struct  EnumParticlePropDriver : EnumBase
{
	ParticlePropDriver mVal;
};

enum RenderTextureResolution
{
	eRenderTextureResolution_Small = 0x1,
	eRenderTextureResolution_Medium = 0x2,
	eRenderTextureResolution_Default = 0x3,
	eRenderTextureResolution_Ultra = 0x4,
};

struct EnumRenderTextureResolution : EnumBase {
	RenderTextureResolution mVal;
};

struct FilterArea {
	String mText;
};

struct MovieCaptureInfo {

	enum CompressorType {
		IV50 = 5,
		MSVC = 4,
		IV32 = 3,
		CVID = 2,
		Uncompressed = 1
	};

	struct EnumCompressorType : EnumBase {
		CompressorType mVal;
	};

	int mFPS;
	EnumCompressorType mCType;

};

enum TextOrientationType
{
	eTextOrientation_Screen = 0x0,
	eTextOrientation_WorldZ = 0x1,
	eTextOrientation_WorldXYZ = 0x2,
};

enum DepthOfFieldType
{
	eDepthOfFieldType_Default = 0x1,
	eDepthOfFieldType_Brush = 0x2,
};

enum T3LightEnvBakeOnStatic
{
	eLightEnvBakeOnStatic_Default = 0x0,
	eLightEnvBakeOnStatic_AlwaysAllow = 0x1,
	eLightEnvBakeOnStatic_NeverAllow = 0x2,
};

enum T3LightEnvLODBehavior
{
	eLightEnvLOD_Disable = 0x0,
	eLightEnvLOD_BakeOnly = 0x1,
};

enum T3LightEnvEnlightenBakeBehavior
{
	eLightEnvEnlightenBake_Auto = 0x0,
	eLightEnvEnlightenBake_Enable = 0x1,
	eLightEnvEnlightenBake_Disable = 0x2,
};

enum BokehOcclusionType
{
	eBokehOcclusionDisabled = 0x0,
	eBokehOcclusionZTestAndScaleOccluded = 0x1,
	eBokehOcclusionScaleOccluded = 0x2,
	eBokehOcclusionZTest = 0x3,
};

struct  EnumBokehOcclusionType : EnumBase
{

	EnumBokehOcclusionType(int val = 0) {
		mVal = (BokehOcclusionType)val;
	}

	BokehOcclusionType mVal;
};

enum GlowQualityLevel
{
	eGlowQualityLevelOld = 0x0,
	eGlowQualityLevelLow = 0x1,
	eGlowQualityLevelMedium = 0x2,
	eGlowQualityLevelHigh = 0x3,
};

struct  EnumGlowQualityLevel : EnumBase
{
	GlowQualityLevel mVal;
};

enum BokehQualityLevel
{
	eBokehQualityLevelDisabled = 0x0,
	eBokehQualityLevelLow = 0x1,
	eBokehQualityLevelMedium = 0x2,
	eBokehQualityLevelHigh = 0x3,
};

struct  EnumBokehQualityLevel : EnumBase
{
	BokehQualityLevel mVal;
};

enum DOFQualityLevel
{
	eDofQualityLevelDisabled = 0x0,
	eDOFQualityLevelLow = 0x1,
	eDOFQualityLevelMedium = 0x2,
	eDOFQualityLevelHigh = 0x3,
};

struct  EnumDOFQualityLevel : EnumBase
{
	DOFQualityLevel mVal;
};

enum RenderTAAJitterType
{
	eRenderTAAJitter_None = 0x1,
	eRenderTAAJitter_Uniform2x = 0x2,
	eRenderTAAJitter_Hammersley4x = 0x3,
	eRenderTAAJitter_Hammersley8x = 0x4,
};

struct  EnumRenderTAAJitterType : EnumBase
{
	RenderTAAJitterType mVal;
};

enum RenderAntialiasType
{
	eRenderAntialias_None = 0x0,
	eRenderAntialias_FXAA = 0x1,
	eRenderAntialias_SMAA = 0x2,
	eRenderAntialias_MSAA_2x = 0x3,
	eRenderAntialias_MSAA_4x = 0x4,
	eRenderAntialias_MSAA_8x = 0x5,
	eRenderAntialias_TAA_MSAA_2x = 0x6,
	eRenderAntialias_TAA_MSAA_4x = 0x7,
	eRenderAntialias_TAA_MSAA_8x = 0x8,
	eRenderAntialias_TAA = 0x9,
};

struct  EnumRenderAntialiasType : EnumBase
{
	RenderAntialiasType mVal;
};

enum HBAOResolution
{
	eHBAOResolutionFull = 0x0,
	eHBAOResolutionHalf = 0x1,
	eHBAOResolutionQuarter = 0x2,
};

struct  EnumHBAOResolution : EnumBase
{
	HBAOResolution mVal;
};

enum HBAOQualityLevel
{
	eHBAOQualityLevelLowest = 0x0,
	eHBAOQualityLevelLow = 0x1,
	eHBAOQualityLevelMedium = 0x2,
	eHBAOQualityLevelHigh = 0x3,
	eHBAOQualityLevelHighest = 0x4,
};
struct  EnumHBAOQualityLevel : EnumBase
{
	HBAOQualityLevel mVal;
};

enum HBAOPerPixelNormals
{
	eHBAOPerPixelNormalsGBuffer = 0x0,
	eHBAOPerPixelNormalsReconstructed = 0x1,
};

struct  EnumHBAOPerPixelNormals : EnumBase
{
	HBAOPerPixelNormals mVal;
};

enum HBAODeinterleaving
{
	eHBAODeinterleaving_Disabled = 0x0,
	eHBAODeinterleaving_2x = 0x1,
	eHBAODeinterleaving_4x = 0x2,
};

struct  EnumHBAODeinterleaving : EnumBase
{
	HBAODeinterleaving mVal;
};

enum HBAOPreset
{
	eHBAOPresetFromTool = 0x0,
	eHBAOPresetXBone = 0x1,
	eHBAOPresetPS4 = 0x2,
	eHBAOPresetDisabled = 0x3,
	eHBAOPresetLow = 0x4,
	eHBAOPresetMedium = 0x5,
	eHBAOPresetHigh = 0x6,
	eHBAOPresetUltra = 0x7,
};

struct  EnumHBAOPreset : EnumBase
{
	HBAOPreset mVal;
};

enum HBAOBlurQuality
{
	eHBAOBlurQualityNone = 0x0,
	eHBAOBlurQualityNarrow = 0x1,
	eHBAOBlurQualityMedium = 0x2,
	eHBAOBlurQualityWide = 0x3,
	eHBAOBlurQualityExtraWide = 0x4,
};


struct  EnumHBAOBlurQuality : EnumBase
{
	HBAOBlurQuality mVal;
};


struct  EnumT3LightEnvBakeOnStatic : EnumBase
{
	T3LightEnvBakeOnStatic mVal;
};

struct  EnumDepthOfFieldType : EnumBase
{
	DepthOfFieldType mVal;
};

struct  EnumT3LightEnvLODBehavior : EnumBase
{
	T3LightEnvLODBehavior mVal;
};

struct  EnumT3LightEnvEnlightenBakeBehavior : EnumBase
{
	T3LightEnvEnlightenBakeBehavior mVal;
};

struct  EnumTextOrientationType : EnumBase
{
	TextOrientationType mVal;
};


struct  EnumLightCellBlendMode : EnumBase
{
	LightCellBlendMode mVal;
};

enum T3MaterialNormalSpaceType
{
	eMaterialNormalSpace_Tangent = 0x0,
	eMaterialNormalSpace_World = 0x1,
	eMaterialNormalSpace_Count = 0x2,
};

enum T3MaterialLODFullyRough
{
	eMaterialFullyRough_Never = 0x0,
	eMaterialFullyRough_OnLowestQuality = 0x1,
	eMaterialFullyRough_OnLowQuality = 0x2,
	eMaterialFullyRough_Always = 0x3,
};

struct  EnumT3MaterialNormalSpaceType : EnumBase
{
	T3MaterialNormalSpaceType mVal;
};

struct  EnumT3MaterialLODFullyRough : EnumBase
{
	T3MaterialLODFullyRough mVal;
};

/* 3514 */
struct T3LightEnvInternalData
{

	struct QualityEntry
	{
		unsigned int mShadowLayer;
		Flags mFlags;
	};

	T3LightEnvInternalData::QualityEntry mEntryForQuality[4];//last quality entry (index 3) in WD4 and above
	int mStationaryLightIndex;
};

/* 974 */
enum EmitterColorType
{
	eEmitterColorType_Constant = 0x1,
	eEmitterColorType_Random_Lerp = 0x2,
	eEmitterColorType_Random_Discrete = 0x3,
	eEmitterColorType_BurstLife_Lerp = 0x4,
	eEmitterColorType_Index_Lerp = 0x5,
	eEmitterColorType_KeyControl01 = 0x6,
};

/* 975 */
enum EmitterSpriteAnimationType
{
	eEmitterSpriteAnimationType_Linear = 0x1,
	eEmitterSpriteAnimationType_Random = 0x2,
};

/* 976 */
enum EmitterSpriteAnimationSelection
{
	eEmitterSpriteAnimationSelection_Random = 0x1,
	eEmitterSpriteAnimationSelection_LinearLoop = 0x2,
	eEmitterSpriteAnimationSelection_LinearStretch = 0x3,
	eEmitterSpriteAnimationSelection_KeyControl01 = 0x4,
};

enum ParticleSortMode
{
	eParticleSortMode_None = 0x1,
	eParticleSortMode_ByDistance = 0x2,
	eParticleSortMode_YoungestFirst = 0x3,
	eParticleSortMode_OldestFirst = 0x4,
};

enum EmitterSpawnShape
{
	eEmitterSpawn_Box = 0x1,
	eEmitterSpawn_Sphere = 0x2,
	eEmitterSpawn_Cylinder = 0x3,
	eEmitterSpawn_ToTarget = 0x4,
	eEmitterSpawn_Particle = 0x5,
	eEmitterSpawn_ParticleInterpolate = 0x6,
	eEmitterSpawn_Bones = 0x7,
	eEmitterSpawn_BoneBoxes = 0x8,
};

struct PointOfInterestBlocking {};

/* 993 */
enum EmitterBoneSelection
{
	eEmitterBoneSelection_All = 0x1,
	eEmitterBoneSelection_Children = 0x2,
};

enum EmitterParticleCountType
{
	eEmitterParticleCountType_Count = 0x1,
	eEmitterParticleCountType_SpawnVolParticleCount = 0x2,
};

enum RenderQualityType {
	eRenderQuality_High = 0,
	eRenderQuality_Mid = 1,
	eRenderQuality_LowPlus = 2,
	eRenderQuality_Mid2_Legacy = 2,
	eRenderQuality_Low = 3,
	eRenderQuality_Low_Legacy = 3,
	eRenderQuality_Lowest = 4,
	eRenderQuality_Count = 5,
	eRenderQuality_Default = 0xFFFFFFFE,
	eRenderQuality_None = 0xFFFFFFFF,
};

enum EmitterConstraintType
{
	eEmitterConstraintType_None = 0x1,
	eEmitterConstraintType_0Point = 0x2,
	eEmitterConstraintType_1Point = 0x3,
	eEmitterConstraintType_2Point = 0x4,
};

struct  EnumParticleSortMode : EnumBase
{
	ParticleSortMode mVal;
};

struct  EnumEmitterSpawnShape : EnumBase
{
	EmitterSpawnShape mVal;
};

struct  EnumEmitterBoneSelection : EnumBase
{
	EmitterBoneSelection mVal;
};

/* 13215 */
struct  EnumEmitterColorType : EnumBase
{
	EmitterColorType mVal;
};

/* 13217 */
struct  EnumEmitterSpriteAnimationType : EnumBase
{
	EmitterSpriteAnimationType mVal;
};

/* 13219 */
struct  EnumEmitterSpriteAnimationSelection : EnumBase
{
	EmitterSpriteAnimationSelection mVal;
};

struct EnumEmitterConstraintType : EnumBase
{
	EmitterConstraintType mVal;
};

struct EnumEmitterParticleCountType : EnumBase
{
	EmitterParticleCountType mVal;
};

struct MetaClassDescription;
struct MetaMemberDescription;

enum ConstraintReferenceFrame : int {
	World = 0,
	Root = 1,
	Parent = 2,
};

struct AnimationConstraint {
	enum LockMode : int {
		eTrafoMode = 0,/*pos and rot*/
		ePosMode = 1,/*pos*/
		eOriMode = 2,/*rot*/
	};
	enum Node : int {
		eWorld = 0x0,
		eRoot = 0x1,
		eSpine1 = 0x2,
		eSpine2 = 0x3,
		eSpine3 = 0x4,
		eSpine4 = 0x5,
		eShoulder_L = 0x6,
		eArm_L = 0x7,
		eElbow_L = 0x8,
		eWrist_L = 0x9,
		eMiddleF1_L = 0xA,
		eMiddleF2_L = 0xB,
		eMiddleF3_L = 0xC,
		eIndexF1_L = 0xD,
		eIndexF2_L = 0xE,
		eIndexF3_L = 0xF,
		eThumb1_L = 0x10,
		eThumb2_L = 0x11,
		eThumb3_L = 0x12,
		eRingF1_L = 0x13,
		eRingF2_L = 0x14,
		eRingF3_L = 0x15,
		ePinkyF1_L = 0x16,
		ePinkyF2_L = 0x17,
		ePinkyF3_L = 0x18,
		eShoulder_R = 0x19,
		eArm_R = 0x1A,
		eElbow_R = 0x1B,
		eWrist_R = 0x1C,
		ePinkyF1_R = 0x1D,
		ePinkyF2_R = 0x1E,
		ePinkyF3_R = 0x1F,
		eMiddleF1_R = 0x20,
		eMiddleF2_R = 0x21,
		eMiddleF3_R = 0x22,
		eIndexF1_R = 0x23,
		eIndexF2_R = 0x24,
		eIndexF3_R = 0x25,
		eThumb1_R = 0x26,
		eThumb2_R = 0x27,
		eThumb3_R = 0x28,
		eRingF1_R = 0x29,
		eRingF2_R = 0x2A,
		eRingF3_R = 0x2B,
		eNeck = 0x2C,
		eNeck2 = 0x2D,
		eHead = 0x2E,
		eEye_L = 0x2F,
		eEye_R = 0x30,
		ePelvis = 0x31,
		eLeg_L = 0x32,
		eKnee_L = 0x33,
		eAnkle_L = 0x34,
		eLeg_R = 0x35,
		eKnee_R = 0x36,
		eAnkle_R = 0x37,
		eNumNodes = 0x38,
	};
};

enum T3LightEnvMobility : int {
	eLightEnvMobility_Static = 0,
	eLightEnvMobility_Stationary = 1,
	eLightEnvMobility_Moveable = 2,
};

enum T3LightEnvShadowType : int {
	eLightEnvShadowType_None = 0,
	eLightEnvShadowType_PerLight = 2,
	eLightEnvShadowType_Modulated = 3,
};

enum T3LightEnvShadowQuality : int {
	eLightEnvShadowQuality_Low = 0,
	eLightEnvShadowQuality_Medium = 1,
	eLightEnvShadowQuality_High = 2,
};

enum HBAOParticipationType : int {
	eHBAOParticipationTypeAuto = 0,
	eHBAOParticipationTypeForceOn = 1,
	eHBAOParticipationTypeForceOff = 2,
};

struct EnumT3LightEnvMobility : EnumBase {
	T3LightEnvMobility mVal;
};

struct EnumT3LightEnvShadowType : EnumBase {
	T3LightEnvShadowType mVal;
};

struct EnumT3LightEnvShadowQuality : EnumBase {
	T3LightEnvShadowQuality mVal;
};

struct EnumHBAOParticipationType : EnumBase {
	HBAOParticipationType mVal;
};

enum T3LightEnvLODFlags {
	eLODHigh = 0x10000,
	eLODMedium = 0x20000,
	eLODLow = 0x40000,
};

struct T3LightEnvLOD {
	Flags mFlags;
};

struct RootKey {
	bool mTranslationConstraint__Enabled;
	float mTranslationConstraint_MaxVelocity;
	float mTranslationConstraint_MaxAcceleration;
	bool mRotationConstraint__Enabled;
	float mRotationConstraint_MaxBendAngularVelocity;
	float mRotationConstraint_MaxBendAngularAcceleration;
	float mRotationConstraint_MaxTwistAngularVelocity;
	float mRotationConstraint_MaxTwistAngularAcceleration;
};

struct PivotJointKey {
	bool mBoneLengthConstraint__Enabled;
	bool mBoneLengthConstraint_RestRelative;
	float mBoneLengthConstraint_NodeMobility;
	float mBoneLengthConstraint_ParentMobility;
	bool mAngleConstraint__Enabled;
	float mAngleConstraint_MinHorizontalBendAngle;
	float mAngleConstraint_MaxHorizontalBendAngle;
	float mAngleConstraint_MinVerticalBendAngle;
	float mAngleConstraint_MaxVerticalBendAngle;
	bool mTranslationConstraint__Enabled;
	ConstraintReferenceFrame mTranslationConstraint_ReferenceFrame;
	float mTranslationConstraint_MaxVelocity;
	float mTranslationConstraint_MaxAcceleration;
	bool mRotationConstraint__Enabled;
	ConstraintReferenceFrame mRotationConstraint_ReferenceFrame;
	float mRotationConstraint_MaxBendAngularVelocity;
	float mRotationConstraint_MaxBendAngularAcceleration;
};

struct BallJointKey {
	bool mBoneLengthConstraint__Enabled;
	bool mBoneLengthConstraint_RestRelative;
	float mBoneLengthConstraint_NodeMobility;
	float mBoneLengthConstraint_ParentMobility;
	bool mAngleConstraint__Enabled;
	float mAngleConstraint_MaxBendAngle;
	bool mTranslationConstraint__Enabled;
	ConstraintReferenceFrame mTranslationConstraint_ReferenceFrame;
	float mTranslationConstraint_MaxVelocity;
	float mTranslationConstraint_MaxAcceleration;
	bool mRotationConstraint__Enabled;
	ConstraintReferenceFrame mRotationConstraint_ReferenceFrame;
	float mRotationConstraint_MaxBendAngularVelocity;
	float mRotationConstraint_MaxBendAngularAcceleration;
};

struct HingeJointKey {
	bool mBoneLengthConstraint__Enabled;
	bool mBoneLengthConstraint_RestRelative;
	float mBoneLengthConstraint_NodeMobility;
	float mBoneLengthConstraint_ParentMobility;
	bool mAngleConstraint__Enabled;
	float mAngleConstraint_MinBendAngle;
	float mAngleConstraint_MaxBendAngle;
	bool mTranslationConstraint__Enabled;
	ConstraintReferenceFrame mTranslationConstraint_ReferenceFrame;
	float mTranslationConstraint_MaxVelocity;
	float mTranslationConstraint_MaxAcceleration;
	bool mRotationConstraint__Enabled;
	ConstraintReferenceFrame mRotationConstraint_ReferenceFrame;
	float mRotationConstraint_MaxBendAngularVelocity;
	float mRotationConstraint_MaxBendAngularAcceleration;
};


struct BallTwistJointKey {

	bool mBoneLengthConstraint__Enabled;
	bool mBoneLengthConstraint_RestRelative;
	float mBoneLengthConstraint_NodeMobility;
	float mBoneLengthConstraint_ParentMobility;
	bool mAngleConstraint__Enabled;
	float mAngleConstraint_MaxBendAngle;
	float mAngleConstraint_MinTwistAngle;
	float mAngleConstraint_MaxTwistAngle;
	bool mTranslationConstraint__Enabled;
	ConstraintReferenceFrame mTranslationConstraint_ReferenceFrame;
	float mTranslationConstraint_MaxVelocity;
	float mTranslationConstraint_MaxAcceleration;
	bool mRotationConstraint__Enabled;
	ConstraintReferenceFrame mRotationConstraint_ReferenceFrame;
	float mRotationConstraint_MaxBendAngularVelocity;
	float mRotationConstraint_MaxBendAngularAcceleration;
	float mRotationConstraint_MaxTwistAngularVelocity;
	float mRotationConstraint_MaxTwistAngularAcceleration;

};

struct PlaceableBallTwistJointKey {
	bool mBoneLengthConstraint__Enabled;
	bool mBoneLengthConstraint_RestRelative;
	float mBoneLengthConstraint_NodeMobility;
	float mBoneLengthConstraint_ParentMobility;
	bool mAngleConstraint__Enabled;
	float mAngleConstraint_MaxBendAngle;
	float mAngleConstraint_MinTwistAngle;
	float mAngleConstraint_MaxTwistAngle;
	bool mPlacementLockToAnimConstraint__Enabled;
	AnimationConstraint::LockMode mPlacementLockToAnimConstraint_LockMode;
	bool mPlacementLockToNodeConstraint__Enabled;
	AnimationConstraint::Node mPlacementLockToNodeConstraint_Node;
	AnimationConstraint::LockMode mPlacementLockToNodeConstraint_LockMode;
	bool mTranslationConstraint__Enabled;
	ConstraintReferenceFrame mTranslationConstraint_ReferenceFrame;
	float mTranslationConstraint_MaxVelocity;
	float mTranslationConstraint_MaxAcceleration;
	bool mRotationConstraint__Enabled;
	ConstraintReferenceFrame mRotationConstraint_ReferenceFrame;
	float mRotationConstraint_MaxBendAngularVelocity;
	float mRotationConstraint_MaxBendAngularAcceleration;
	float mRotationConstraint_MaxTwistAngularVelocity;
	float mRotationConstraint_MaxTwistAngularAcceleration;
};

struct LinkedBallTwistJointKey {

	bool mBoneLengthConstraint__Enabled;
	bool mBoneLengthConstraint_RestRelative;
	float mBoneLengthConstraint_NodeMobility;
	float mBoneLengthConstraint_ParentMobility;
	bool mAngleConstraint__Enabled;
	float mAngleConstraint_MaxBendAngle;
	float mAngleConstraint_MinTwistAngle;
	float mAngleConstraint_MaxTwistAngle;
	bool mTranslationConstraint__Enabled;
	ConstraintReferenceFrame mTranslationConstraint_ReferenceFrame;
	float mTranslationConstraint_MaxVelocity;
	float mTranslationConstraint_MaxAcceleration;
	bool mRotationConstraint__Enabled;
	ConstraintReferenceFrame mRotationConstraint_ReferenceFrame;
	float mRotationConstraint_MaxBendAngularVelocity;
	float mRotationConstraint_MaxBendAngularAcceleration;
	float mRotationConstraint_MaxTwistAngularVelocity;
	float mRotationConstraint_MaxTwistAngularAcceleration;
	bool mLinkBendConstraint__Enabled;
	float mLinkBendConstraint_Strength;
	float mLinkBendConstraint_EchoDelay;
	bool mLinkTwistConstraint__Enabled;
	float mLinkTwistConstraint_Strength;
	float mLinkTwistConstraint_EchoDelay;

};

enum EmitterTrigglerEnable {
	eEmitterTriggerEnable_None = 1,
	eEmitterTriggerEnable_AgentVis = 2,
	eEmitterTriggerEnable_AgentVisInvert = 3,
	eEmitterTriggerEnable_Default = 4,
};

enum EmittersEnableType {
	eEmittersEnableType_All = 1,
	eEmittersEnableType_Random = 2,
	eEmittersEnableType_Sequential = 3,
};

struct EnumEmitterTriggerEnable : EnumBase {
	EmitterTrigglerEnable mVal;
};

struct EnumEmittersEnableType : EnumBase {
	EmittersEnableType mVal;
};

enum ParticleAffectorType {
	eParticleAffectorType_Force = 1,
	eParticleAffectorType_Attractor = 2,
	eParticleAffectorType_KillPlane = 3,
	eParticleAffectorType_KillBox = 4,
	eParticleAffectorType_CollisionPlane = 5,
	eParticleAffectorType_CollisionSphere = 6,
	eParticleAffectorType_CollisionBox = 7,
	eParticleAffectorType_CollisionCylinder = 8,
};

enum ParticleGeometryType {
	eParticleGeometry_Sprite = 1,
	eParticleGeometry_Quad = 2,
	eParticleGeometry_Streak = 3,
	eParticleGeometry_Strip = 4,
	eParticleGeometry_StripFacing = 5,
	eParticleGeometry_None = 6
};

struct EnumParticleAffectorType : EnumBase {
	ParticleAffectorType mVal;
};

struct EnumParticleGeometryType : EnumBase {
	ParticleGeometryType mVal;
};

struct PhysicsObject {

	enum ePhysicsCollisionType {
		PhysicsCollisionType_InterAgent = 0,
		PhysicsCollisionType_Raycast = 1,
	};

	enum ePhysicsBoundingVolumeType {
		PhysicsBoundingVolumeType_Cylinder = 0,
		PhysicsBoundingVolumeType_Box = 1,
		PhysicsBoundingVolumeType_Sphere = 2,
	};

	struct EnumePhysicsBoundingVolumeType : EnumBase {
		ePhysicsBoundingVolumeType mVal;
	};

	struct EnumePhysicsCollisionType : EnumBase {
		ePhysicsCollisionType mVal;
	};

	bool mbEnabledPropertyOn;
	float mfBoundingVolumeScalingFactor;
	PhysicsObject::ePhysicsBoundingVolumeType mBoundingVolumeType;
	PhysicsObject::ePhysicsCollisionType mCollisionType;

};

struct CameraFacingTypes {
	int mCameraFacingType;

	enum cFacingTypes {
		eFacing = 0,
		eFacingY = 1,
		eFacingLocalY = 2,
	};

};

namespace UID {

	typedef int idT;

	struct __declspec(align(8)) Generator {
		static constexpr idT msUninitID = -1;
		idT miNextUniqueID;

		Generator() : miNextUniqueID(1) {}

		static idT UninitID() {
			return 0xFFFFFFFF;
		}

		inline void Reset() {
			miNextUniqueID = 1;
		}

		inline bool ValidID(idT id) {
			return id >= 1 && id < this->miNextUniqueID;
		}

		inline idT GetNextUniqueID(bool bIncrement) {
			idT _t = miNextUniqueID;
			if (miNextUniqueID == 0xFFFFFFFF) {
				miNextUniqueID = 1;
				_t = 1;
			}
			else if (bIncrement)
				miNextUniqueID++;
			return _t;
		}
	};

	struct __declspec(align(8)) Owner {
		idT miUniqueID;

		Owner() : miUniqueID(-1) {}

		Owner(Generator& gen) {
			miUniqueID = gen.GetNextUniqueID(true);
		}

	};

};

struct ZTestFunction {
	enum zFuncTypes {
		eNever = 0x1, eLess = 0x2, eEqual = 0x3, eLessEqual = 0x4, eGreater = 0x5, eNotEqual = 0x6, eGreaterEqual = 0x7, eAlways = 0x8,
	};
	zFuncTypes mZTestType;
};


enum HTextAlignmentType {
	eHTextAlignment_None = 0,
	eHTextAlignment_LeftJustified = 1,
	eHTextAlignment_Centered = 2,
	eHTextAlignment_RightJustified = 3,
};

enum VTextAlignmentType {
	eVTextAlignment_None = 0,
	eVTextAlignment_Top = 1,
	eVTextAlignment_Middle = 2,
	eVTextAlignment_Bottom = 3,
};

enum TonemapType {
	eTonemapType_Default = 1,
	eTonemapType_Filmic = 2,
};

struct EnumTonemapType {
	TonemapType mVal;
};

struct EnumHTextAlignmentType {
	HTextAlignmentType mVal;
};

enum T3MaterialSwizzleType
{
	eMaterialSwizzle_None = 0x0,
	eMaterialSwizzle_X = 0x1,
	eMaterialSwizzle_Y = 0x2,
	eMaterialSwizzle_Z = 0x3,
	eMaterialSwizzle_W = 0x4,
	eMaterialSwizzle_Zero = 0x5,
	eMaterialSwizzle_One = 0x6,
	eMaterialSwizzle_FirstComponent = 0x1,
	eMaterialSwizzle_FirstNumeric = 0x5,
};

//.TRANS FILES
struct StyleIdleTransitionsRes {
	String mOwningAgent, mGuideName;
};

struct EnumT3MaterialSwizzleType : EnumBase {
	T3MaterialSwizzleType mVal;
};

struct T3MaterialSwizzleParams
{
	union {
		EnumT3MaterialSwizzleType mSwizzle[4];
		u32 __reserve;
	};
};

struct EnumVTextAlignmentType {
	VTextAlignmentType mVal;
};

struct TextAlignmentType
{
	int mAlignmentType;
};

struct IdleTransitionSettings
{
	float mTransitionTime;
	int mTransitionStyleDummy;
};

struct IdleSlotDefaults
{
	IdleTransitionSettings mTransitionInDefault;
	IdleTransitionSettings mTransitionDefault;
	IdleTransitionSettings mTransitionOutDefault;
	int mBasePriority;
	u64 mSlotFriendlyName;
};


struct FileNameBase {
	u64 mFileName;
	//MetaClassDescription* mpMetaClassDescription;
};

template<typename T>
struct FileName : FileNameBase {

	inline bool operator<(const FileName& rhs) const {
		return mFileName < rhs.mFileName;
	}

};


struct LightType {
	int mLightType;
};

enum RenderLightmapUVGenerationType {
	eRenderLightmapUVGeneration_Default = 0,
	eRenderLightmapUVGeneration_Auto = 1,
	eRenderLightmapUVGeneration_UV0 = 2,
	eRenderLightmapUVGeneration_UV1 = 3,
	eRenderLightmapUVGeneration_UV2 = 4,
	eRenderLightmapUVGeneration_UV3 = 5,
};

struct EnumRenderLightmapUVGenerationType {
	RenderLightmapUVGenerationType mVal;
};

namespace RecordingUtils {

	enum ScriptType {
		STScript = 1,
		STActor = 2,
		STEngineer = 3,
		STTakeSheet = 4
	};

	enum RecordingStatus {
		eRecordingStatus_None = 0,
		eRecordingStatus_AtStudio = 1,
		eRecordingStatus_Recorded = 2,
		eRecordingStatus_Delivered = 3
	};

	struct EnumRecordingStatus : public EnumBase {
		RecordingStatus mVal;
	};

	struct EnumScriptType : public EnumBase {
		ScriptType mVal;
	};

}


enum T3MaterialLightModelType {
	eMaterialLightModel_Unlit = 0,
	eMaterialLightModel_VertexDiffuse = 1,
	eMaterialLightModel_Diffuse = 2,
	eMaterialLightModel_Phong = 3,
	eMaterialLightModel_PhongGloss = 4,
	eMaterialLightModel_Toon = 5,
	eMaterialLightModel_NPR_Depreceated = 6,
	eMaterialLightModel_PBS = 7,
	eMaterialLightModel_Cloth = 8,
	eMaterialLightModel_Hair = 9,
	eMaterialLightModel_Skin = 0xA,
	eMaterialLightModel_HybridCloth = 0xB,
	eMaterialLightModel_DiffuseNoDirection = 0xC,
	eMaterialLightModel_HybridHair = 0xD,
	eMaterialLightModel_Count = 0xE,
	eMaterialLightModel_Default = 0xFFFFFFFF
};

struct EnumT3MaterialLightModelType : EnumBase {
	T3MaterialLightModelType mVal;
};

struct BlendCameraResource {};