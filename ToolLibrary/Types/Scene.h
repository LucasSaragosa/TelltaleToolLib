// This file was written by Lucas Saragosa. The code derives from Telltale Games' Engine.
// I do not intend to take credit for it, however; Im the author of this interpretation of 
// the engine and require that if you use this code or library, you give credit to me and
// the amazing Telltale Games.

#ifndef _SCENE
#define _SCENE

#include "PropertySet.h"
#include "../Meta.hpp"
#include "../TelltaleToolLibrary.h"
#include "HandleObjectInfo.h"
#include "List.h"
#include "DCArray.h"
#include "T3Texture.h"
#include "PreloadPackage.h"
#include "LightProbeData.h"
#include "../T3/Camera.h"
#include "../Symbols.h"

struct PreloadTimer
{
	float mTime;
	unsigned int mRef;
};

struct RenderWindParams
{
	Vector3 mDirection;
	float mSpeed;
	float mIdleStrength;
	float mIdleSpacialFrequency;
	float mGustSpeed;
	float mGustStrength;
	float mGustSpacialFrequency;
	float mGustIdleStrengthMultiplier;
	float mGustSeparationExponent;
};

struct RenderTonemapFilmicRGBParams
{
	Vector3 mBlackPoints;
	Vector3 mWhitePoints;
	Vector3 mPivots;
	Vector3 mShoulderIntensities;
	Vector3 mToeIntensities;
	bool mbInvertCurves[3];
};

struct RenderTonemapFilmicParams
{
	float mBlackPoint;
	float mWhitePoint;
	float mPivot;
	float mShoulderIntensity;
	float mToeIntensity;
	bool mbInvertCurve;
};

//.SCENE FILES
struct Scene {//only included whats serialized and needed. this is a very very big struct! (class in the engine)

	static constexpr Symbol kSceneLocationInfo{ 0x0CDAB21B527F20135 };//'Scene: Location Info'
	static constexpr Symbol kSceneStartPosKey{ 0x0A1FB3B4404D8C439 };
	static constexpr Symbol kSceneStartRotKey{ 0x53F29BE330D2D613 };
	static constexpr Symbol kSceneStartVisibilityKey{ 0x2AA2DE83FB51649C };
	static constexpr Symbol kSceneTransientKey{ 0x0C120B5E7442846DA };
	static constexpr Symbol kSceneAttachedKey{ 0x9ABEF281E8A46346 };
	static constexpr Symbol kSceneAttachedAgentKey{ 0x42BF5BF71E37A541 };
	static constexpr Symbol kSceneAttachedNodeKey{ 0x990221055FE50803 };
	static constexpr Symbol kSceneQualitySettingKey{ 0x349D77B9827E1962 };
	static constexpr Symbol kSceneUseDeprecatedChoreCameraLayers{ 0x6961EFFDCDAE4D59 };

	struct AgentInfo {

		void* mpAgent;//no need for agent
		String mAgentName;
		PropertySet mAgentSceneProps;
		//if in future i add a serialize async func then update scene serialize

		AgentInfo() {
			mpAgent = NULL;
			mAgentName = String("New Agent");
		}

	};

	struct AgentQualitySettings {
		Flags mFlags;//flag val 1 = exlude from low quality builds
	};

	//these 2 are not serialized
	float mTimeScale = 1.0f;
	bool mbActive = false;

	bool mbHidden = false;
	String mName;
	DCArray<HandleLock<Scene>> mReferencedScenes;
	//only add values to this using operator new! takes ownership
	List<AgentInfo*> mAgentList;

	//By the library. if auto focus is enabled, these are which you calculate the auto focus near and far, eg for a specific set of meshes.
	float mFXAutoFocus_Near = 0.f;
	float mFXAutoFocus_Far = 0.f;

	PreloadTimer mTimer;
	bool mbInputEnabled;
	bool mbShuttingDown;
	bool mbPreloadable;
	bool mbPreloadShaders;
	Flags mSceneFlags;
	Color mAmbient;
	Color mShadowColor;
	int mPriority;
	int mRenderLayer = 0;
	int mAgentPriority;
	//set to current camera in scene to render
	std::shared_ptr<Camera> mpViewCamera;
	Vector3 mPrevCameraPosition;
	Vector3 mPrevCameraDirection;
	Matrix4 mPrevCameraViewMatrix;
	Matrix4 mPrevCameraProjMatrix;
	u64 mCameraCutFrameIndex;
	int mShadowLayerBits;
	int mRenderObjectFeatureCount[3];
	Symbol mSceneAudioListenerAgentName;
	Symbol mSceneAudioPlayerOriginAgentName;
	std::weak_ptr<Agent> mpAudioListenerAgent;
	std::weak_ptr<Agent> mpAudioPlayerOriginAgent;
	//Ptr<ParticleManager> mpParticleManager;
	//Ptr<LightManager> mpLightManager;
	//Ptr<PhysicsManager> mpPhysicsManager;
	std::shared_ptr<PreloadPackage::RuntimeDataScene> mhPreloadPackage;
	bool mbAfterEffectEnabled;
	Color mGlowClearColor;
	float mGlowSigmaScale;
	bool mbGenerateNPRLines;
	float mNPRLineFalloff;
	float mNPRLineBias;
	float mNPRLineAlphaFalloff;
	float mNPRLineAlphaBias;
	//T3DepthReductionContext mDepthReductionContext;
	bool mbFXAntialias;
	bool mFXColorActive;
	Color mFXColor;
	float mFXColorOpacity;
	bool mCameraFXColorActive;
	Color mCameraFXColor;
	float mCameraFXColorOpacity;
	bool mFXSharpShadowsActive;
	bool mFXLevelsActive;
	float mFXLevelsBlack;
	float mFXLevelsWhite;
	float mFXLevelsIntensity;
	float mFXLevelsBlackHDR;
	float mFXLevelsWhiteHDR;
	float mFXLevelsIntensityHDR;
	TonemapType mFXTonemapType;
	bool mFXTonemapActive;
	bool mbFXTonemapDOFEnable;
	float mFXTonemapIntensity;
	RenderTonemapFilmicParams mFXTonemapFilmicParams;
	RenderTonemapFilmicParams mFXTonemapFilmicFarParams;
	bool mbFXTonemapFilmicRGBActive;
	bool mbFXTonemapFilmicRGBDOFEnable;
	RenderTonemapFilmicRGBParams mFXTonemapRGBParams;
	RenderTonemapFilmicRGBParams mFXTonemapRGBFarParams;
	float mFXBloomThreshold;
	float mFXBloomIntensity;
	bool mFXAmbientOcclusionActive;
	float mFXAmbientOcclusionIntensity;
	float mFXAmbientOcclusionFalloff;
	float mFXAmbientOcclusionRadius;
	float mFXAmbientOcclusionLightmap;
	bool mHDRLightmaps;
	float mHDRLightmapIntensity;
	bool mbFXBrushDOFEnable;
	bool mbFXBrushOutlineEnable;
	bool mbFXBrushOutlineFilterEnable;
	float mFXBrushOutlineSize;
	float mFXBrushOutlineThreshold;
	float mFXBrushOutlineColorThreshold;
	float mFXBrushOutlineFalloff;
	float mFXBrushOutlineScale;
	float mFXBrushFar;
	float mFXBrushFarFalloff;
	float mFXBrushFarMaxScale;
	float mFXBrushNearScale;
	float mFXBrushNearDetail;
	float mFXBrushFarScale;
	float mFXBrushFarDetail;
	float mFXBrushFarScaleBoost;
	std::shared_ptr<T3Texture> mhBrushNearTexture;
	std::shared_ptr<T3Texture> mhBrushFarTexture;
	bool mFXDOFEnabled;
	bool mbFXDOFFieldOfViewAdjustEnabled;
	bool mFXDOFAutoFocusEnabled;
	float mFXDOFNear;
	float mFXDOFNearFalloff;
	float mFXDOFFar;
	float mFXDOFFarFalloff;
	float mFXDOFNearMax;
	float mFXDOFFarMax;
	float mFXForceLinearDepthOffset;
	float mFXDOFDebug;
	float mFXDOFCoverageBoost;
	float mFXDOFVignetteMax;
	bool mFXVignetteTintEnabled;
	bool mFXVignetteDOFEnabled;
	Color mFXVignetteTint;
	float mFXVignetteFallOff;
	float mFXVignetteStart;
	float mFXVignetteEnd;
	float mFXTAAWeight;
	float mFXNoiseScale;
	bool mbHBAOEnabled;
	bool mbHBAODebug;
	float mHBAORadius;
	float mHBAOMaxRadiusPercent;
	float mHBAOHemisphereBias;
	float mHBAOIntensity;
	float mHBAOOcclusionScale;
	float mHBAOLuminanceScale;
	float mHBAOMaxDistance;
	float mHBAODistanceFalloff;
	float mHBAOBlurSharpness;
	bool mbSSLinesEnabled;
	Color mSSLineColor;
	float mSSLineThickness;
	float mSSLineDepthFadeNear;
	float mSSLineDepthFadeFar;
	float mSSLineDepthMagnitude;
	float mSSLineDepthExponent;
	Vector3 mSSLineLightDirection;
	float mSSLineLightMagnitude;
	float mSSLineLightExponent;
	int mSSLineDebug;
	bool mbLightEnvBakeEnabled;
	bool mbLightEnvReflectionEnabled;
	bool mbLightEnvEnabled;
	std::shared_ptr<T3Texture> mhLightEnvReflectionTexture;
	std::shared_ptr<LightProbeData> mhEnvLightProbeData;
	Color mLightEnvReflectionTint;
	float mLightEnvReflectionIntensity;
	Color mLightEnvTint;
	float mLightEnvIntensity;
	float mLightEnvSaturation;
	Color mLightEnvBackgroundColor;
	float mLightEnvProbeDensityX;
	float mLightEnvProbeDensityY;
	float mLightEnvShadowMomentBias;
	float mLightEnvShadowDepthBias;
	float mLightEnvShadowPositionOffsetBias;
	float mLightEnvShadowLightBleedReduction;
	float mLightEnvShadowMinDistance;
	float mLightEnvShadowMaxDistance;
	float mLightEnvDynamicShadowMaxDistance;
	float mLightEnvShadowCascadeLambda;
	float mLightEnvReflectionIntensityShadow;
	float mLightShadowTraceMaxDistance;
	int mLightEnvShadowMaxUpdates;
	bool mbLightEnvShadowAutoDepthBounds;
	Vector3 mLightEnvStaticShadowMin;
	Vector3 mLightEnvStaticShadowMax;
	std::shared_ptr<T3Texture> mhEnvLightShadowGoboTexture;
	bool mFogEnabled;
	Color mFogColor;
	float mFogAlpha;
	float mFogMinDistance;
	float mFogMaxDistance;
	float mGraphicBlackThreshold;
	float mGraphicBlackSoftness;
	float mGraphicBlackAlpha;
	float mGraphicBlackNear;
	float mGraphicBlackFar;
	float mHDR_PaperWhiteNits;
	RenderWindParams mWindParams;
	float mCameraCutPositionThreshold;
	float mCameraCutRotationThreshold;
	bool mSpecMultEnabled;
	float mSpecMultColor;
	float mSpecMultIntensity;
	float mSpecMultExponent;
	bool mOverrideFrameBufferScaleFactor;
	float mFrameBufferScaleFactor;
	unsigned int mRenderDirty;
	bool mbShadowLayerDirty;
	float mMaterialTime;
	float mMaterialPrevTime;
	float mViewportScissorLeft;
	float mViewportScissorTop;
	float mViewportScissorRight;
	float mViewportScissorBottom;
	Symbol mAudioAmbienceAgentName;
	Symbol mAudioMusicAgentName;
	Symbol mAudioReverbAgentName;
	Symbol mAudioListenerAgentName;
	Symbol mAudioSfxAgentName;
	Symbol mAudioEventPreloadAgentName;

	Scene() = default;

	Scene(const Scene& o) = delete;//NO COPIES!
	Scene& operator=(const Scene&) = delete;

	Scene(Scene&&) = delete;
	Scene& operator=(Scene&&) = delete;

	~Scene() {
		for (auto it = mAgentList.begin(); it != mAgentList.end(); it++) {
			delete *it;
		}
	}

	static MetaOpResult MetaOperation_SerializeAsync(void* pObj, MetaClassDescription* pObjDesc, 
		MetaMemberDescription* c, void* pUserData) {
		MetaOpResult result = Meta::MetaOperation_SerializeAsync(pObj, pObjDesc, c, pUserData);
		if (result == MetaOpResult::eMetaOp_Succeed) {
			MetaStream* stream = static_cast<MetaStream*>(pUserData);
			Scene* scene = static_cast<Scene*>(pObj);
			stream->BeginBlock();
			u32 num = scene->mAgentList.size();
			stream->serialize_uint32(&num);
			MetaClassDescription* inf_mcd = GetMetaClassDescription(typeid(AgentInfo).name());
			if (!inf_mcd)return MetaOpResult::eMetaOp_Fail;//!!
			if (stream->mMode == MetaStreamMode::eMetaStream_Read) {
				for (int i = 0; i < num; i++) {
					AgentInfo *info = new AgentInfo;
					result = Meta::MetaOperation_SerializeAsync(info, inf_mcd, c, pUserData);
					if (result != eMetaOp_Succeed)break;
					scene->mAgentList.push_back(info);
				}
			}
			else {
				for (auto it = scene->mAgentList.begin(); it != scene->mAgentList.end(); it++) {
					result = Meta::MetaOperation_SerializeAsync(*it, inf_mcd, c, pUserData);
					if (result != eMetaOp_Succeed)break;
				}
			}
			stream->EndBlock();
		}
		return result;
	}

	AgentInfo* GetAgent(const char* name){
		for (auto it = mAgentList.begin(); it != mAgentList.end(); it++) {
			if ((*it)->mAgentName == name)
				return *it;
		}
		return 0;
	}

	AgentInfo* CreateAgent(String pAgentName) {
		AgentInfo* info = new AgentInfo;
		info->mAgentName = pAgentName;
		mAgentList.push_back(info);
		return info;
	}

	//wont bother with serialize main :P

};

struct ClipResourceFilter {
	Set<Symbol> mResources;
	bool mbExclusiveMode;
};

struct PerAgentClipResourceFilter {
	Map<String, ClipResourceFilter, StringCompareCaseInsensitive> mIncludedAgents;
	Set<String, StringCompareCaseInsensitive> mExcludedAgents;
	bool mbExclusiveMode;
};

struct T3LightSceneInternalData {

	struct LightmapPage {
		Handle<T3Texture> mhTextureAtlas;
		Flags mFlags;
	};

	struct QualityEntry {
		DCArray<LightmapPage> mLightmapPages;
		Handle<T3Texture> mhStaticShadowVolumeTexture;
	};

	QualityEntry mEntryForQuality[4];//last quality entry (index 3) in WD4 and above
	unsigned int mStationaryLightCount;
	unsigned int mBakeVersion;

};

#endif