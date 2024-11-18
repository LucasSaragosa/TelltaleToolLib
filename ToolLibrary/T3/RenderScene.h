#include "Render.hpp"
#include "../Types/D3DMesh.h"
#include "../Types/BlendGraph.h"
#include "../Types/Skeleton.h"

enum RenderObjectFeature
{
	eRenderObjectFeature_LinearDepth = 0x0,
	eRenderObjectFeature_ParticleSoftDepthTest = 0x1,
	eRenderObjectFeature_SceneColorTexture = 0x2,
	eRenderObjectFeature_Count = 0x3,
};

enum RenderObjectCategory
{
	eRenderCategory_Mesh = 0x1,
	eRenderCategory_Text = 0x2,
	eRenderCategory_Particle = 0x4,
	eRenderCategory_Decal = 0x8,
	eRenderCategory_Post = 0x10,
	eRenderCategory_DebugVisualize = 0x20,
	eRenderCategory_Visible = 0x10000,
	eRenderCategory_ForceShadowVisible = 0x20000,
	eRenderCategory_ForceEnlightenVisible = 0x40000,
	eRenderCategory_Static = 0x80000,
	eRenderCategory_Deformable = 0x100000,
	eRenderCategory_CubeBake = 0x200000,
	eRenderCategory_BakeAsStatic = 0x400000,
	eRenderCategory_CastShadowsAsStatic = 0x800000,
	eRenderCategory_AnyVisibleMask = 0x70000,
	eRenderCategory_ShadowStaticMask = 0x880000,
	eRenderCategory_All = 0x7FFFFFFF,
};

struct RenderObjectViewContext : ListNode<RenderObjectViewContext>
{
	RenderSceneView* mRenderView;
	unsigned int mFlags;
	//T3OcclusionObject* mpPrimaryOcclusionObject;
	//T3OcclusionObject** mpSecondaryOcclusionObjects;
};

struct RenderObjectInterface;
struct T3MaterialInstance;

struct RenderObjectContext
{
	RenderObjectInterface* mpRenderObject;
	Symbol mName;
	LinkedList<RenderObjectViewContext> mViewList;
};

struct RenderGBufferFlagsCache
{

	struct Entry
	{
		unsigned int mFlags;
		char mIndex;
	};

	T3GFXBuffer* mpFlagsBuffer;
	unsigned int* mpFlagsBufferData;
	Entry mHash[997];
	char mNextIndex;
};

struct RenderParameters
{
	RenderFrame* mpRenderFrame;
	RenderFrameScene* mpRenderScene;
	RenderViewPass* mpComputeRenderPass;
	//RenderJobManager* mpRenderJobManager;
	//RenderBoneMatrixCache* mpBoneMatrixCache;
	RenderGBufferFlagsCache* mpGBufferFlagsCache;
	T3RenderTargetContext* mpTargetContext;
	//LightSceneContext* mpLightContext;
	int mSceneColorTextureRenderLayer;
	unsigned int mObjectIndex;
	unsigned int mFrameIndex;
	bool mbBakedLightingEnabled;
};

//Base class for any renderable object.
struct RenderObjectInterface : ListNode<RenderObjectInterface>
{
	Scene* mpScene;
	//Ptr<EnvironmentTile> mpEnvTile;
	Symbol mName;
	u32 mObjectID;
	int mRenderLayer;
	BitSet<enum RenderObjectFeature, 3, 0> mRenderFeatures;
	u32 mRenderDirty;
	RenderObjectCategory mRenderCategory;
	RenderObjectCategory mEnvTileRenderCategory;
	u32 mStaticStateID;
	u32 mDynamicStateID;

	inline virtual ~RenderObjectInterface() {}

	inline virtual void OnMaterialDirty(T3MaterialInstance*) {}

	inline virtual void OnMaterialLightEnvGroupDirty(T3MaterialInstance*) {}

	inline virtual void SetMaterialDirty(u32) {}

	inline virtual void PrepareToRender(RenderFrameUpdateList* updateList) {}

	inline virtual void PrepareCulling(LinearHeap* heap, RenderObjectContext*) {}

	inline virtual void UpdateSkeletonAnimation(LinearHeap* heap, RenderObjectContext*, bool) {}

	inline virtual void Render(LinkedList<RenderObjectViewContext>*, RenderParameters*) {}

};

struct T3MaterialInstanceTextureOverrides
{
	Map<Symbol, Handle<T3Texture>, std::less<Symbol> > mTextureOverridesByName;
	Map<int, Handle<T3Texture>, std::less<int> > mTextureOverridesByType;
	Set<Symbol, std::less<Symbol> > mTextureOverridesHiddenByName;
};

struct T3MaterialTextureInstance
{
	Handle<T3Texture> mhBaseTexture;
	Handle<T3Texture> mhTexture;
	Handle<T3Texture> mhOverrideTexture;
	T3Texture* mpCachedTexture;
	BitSet<enum T3MaterialTextureType, 17, 0> mTextureTypes;
	char mLayout;
	char mNumToonShades;
	char mFlags;
};

struct T3MaterialTransformInstance
{
	float mValue[13];
};

struct T3MaterialParameterBufferInstance
{
	void* mpBuffer;
	unsigned int mScalarSize;
	unsigned int mScalarOffset;
};

struct T3MaterialInstance : ListNode<T3MaterialInstance>
{
	RenderObjectInterface* mpRenderObject;
	PropertySet* mhAgentProperties;
	PropertySet* mhBaseMaterial;
	PropertySet* mhCurrentMaterial;
	PropertySet* mhMaterial;
	T3MaterialData* mpMaterialData;
	DCArray<T3MaterialCompiledData>* mpCompiledDataArray;
	int mCompiledDataIndex = 0;
	//Ptr<RenderTexture> mpRenderTexture;
	Symbol mBaseMaterialName;
	Symbol mLegacyRenderTextureProperty;
	RenderQualityType mRenderQuality;
	BitSet<enum T3MaterialPassType, 21, 0> mMaterialPasses;
	BitSet<enum T3MaterialChannelType, 46, 0> mMaterialChannels;
	BitSet<enum T3MaterialPropertyType, 31, 0> mMaterialProperties;
	BitSet<enum T3MaterialShaderInput, 73, 0> mShaderInputs;
	BitSet<enum T3MaterialSceneTextureType, 2, 0> mSceneTextures;
	//T3MaterialOptimizationType mMaterialOptimization;
	T3MaterialDomainType mMaterialDomain;
	T3MaterialLightModelType mLightModel;
	T3MaterialLODFullyRough mFullyRough;
	Symbol mLightGroupKey;
	T3BlendMode mBaseBlendMode;
	T3BlendMode mCachedBlendMode;
	Color mOutlineColor;
	Vector3 mNPRLineFalloffParameters;
	Vector3 mNPRLineAlphaFalloffParameters;
	Vector4 mEnlightenEmissiveColorAndIntensity;
	Vector4 mEnlightenAlbedoColorAndIntensity;
	float mEnlightenTransparency;
	BitSet<enum T3EffectParameterType, 150, 0> mEffectParameterTypes;
	T3EffectParameterGroup* mEffectParameterGroup;
	//T3EffectParameterCacheRef mMaterialToolParamRef;
	BitSet<enum T3EffectFeature, 30, 54> mEffectFeatureMask;
	BitSet<enum T3MaterialBlendModeType, 4, 0> mSupportedBlendModes;
	Symbol mLightGroup;
	T3MaterialTextureInstance mTextures[16];
	DCArray<T3MaterialTransformInstance> mTransforms;
	T3MaterialParameterBufferInstance mParameterBuffers[2];
	T3MaterialParameterBufferInstance mPreShaderParameterBuffer;
	T3LightEnvGroup mLightEnvGroup;
	int mLightGroupIndex;
	int mPrimaryTextureIndex;
	BoundingBox mBoundingBox;
	Sphere mBoundingSphere;
	Flags mFlags;
};

struct RenderObject_Mesh : RenderObjectInterface
{

	struct TriangleSetInstance
	{
		BitSet<enum T3EffectFeature, 30, 54> mStateEffectFeatures;
		BitSet<enum T3EffectParameterType, 150, 0> mEffectParameterTypes;
		T3EffectParameterGroup* mEffectParameters;
		T3RenderStateBlock mStateBlock;
		BitSet<enum T3EffectFeature, 54, 0> mStaticEffectFeatures;
		T3EffectCacheRef mEffectRef[12];
		int mLightGroupIndex;
		unsigned int mRenderDirty;
	};

	struct MeshLODInstance {
		Sphere mBoundingSphere;
		BoundingBox mBoundingBox;
		std::vector<TriangleSetInstance> mTriangleSets[2];
		BinaryBuffer mEffectParameterGroupBuffer;
		unsigned int mVertexStateIndex;
		T3MeshBufferFormatEntry mComputeLineVertexFormat[32];
		unsigned int mComputeLineVertexEntryCount;
		unsigned int mComputeLineVertexFormatHash;
		DCArray<Symbol> mBones;
		BitSet<enum T3EffectParameterType, 150, 0> mEffectParameterTypes;
		T3EffectParameterGroup* mEffectParameters;
		//T3EffectParameterCacheRef mInstanceParamRef;
		Vector2 mSceneLightmapTextureScale;
		Vector2 mSceneLightmapTextureOffset;
		unsigned int mSceneLightmapPageIndex;
		float mSceneLightmapHDRScale;
		float mShadowDepthBias;
		unsigned int mRenderDirty;
		bool mbHasSceneLightmap;
	};

	struct MeshVertexStateInstance {
		std::shared_ptr<T3GFXVertexState> mpVertexState[14];
	};

	struct VertexAnimationInstance {
		//anim mixer base ptrs
	};

	struct MeshInstance
	{
		Sphere mBoundingSphere;
		BoundingBox mBoundingBox;
		D3DMesh* mhD3DMesh;
		RenderObject_Mesh* mpRenderMesh;
		PropertySet* mhSceneProperties;
		PropertySet mMeshProperties;//store it here instead
		std::vector<MeshLODInstance> mLODs;
		std::vector<MeshVertexStateInstance> mVertexStates;
		std::vector<int> mTxInstanceIndex;
		std::vector<int> mMaterialInstanceIndex;
		//std::vector<VertexAnimationInstance> mVertexAnimationInstances;
		BitSet<enum T3MaterialPassType, 21, 0> mMaterialPasses;
		BitSet<enum T3MaterialPassType, 21, 0> mActiveMaterialPasses;
		std::vector<int> mBoneIndices;
		std::shared_ptr<T3GFXBuffer> mpBoneBuffer;
		Handle<Skeleton> mhResolvedSkeleton;
		std::shared_ptr<T3GFXBuffer> mpComputeSkinningVertexIn[8];
		std::shared_ptr<T3GFXBuffer> mpComputeSkinningVertexOut[5];
		u32 mComputeSkinningInputCount;
		u32 mComputeSkinningOutputCount;
		u32 mComputeSkinningNormalCount;
		u32 mSkinningOutputIndex;
		//T3MeshCPUSkinningInstance mCPUSkinningInstance;
		BoundingBox mAnimatedBoundingBox;
		Sphere mAnimatedBoundingSphere;
		int mPrevLODIndex;
		int mAnimatedStreamVertexSize;
		unsigned int mSkinnedFrame;
		bool mbPositionAnimationActive;
		bool mbNormalAnimationActive;
		bool mbDeformable;
		bool mbInitialized;
		bool mbVisible;
		bool mbRecreateMeshData;
		int mNormal0SrcOffset;
		int mNormal1SrcOffset;
		int mNormal2SrcOffset;
		int mTexCoord0SrcOffset;
		int mTexCoord1SrcOffset;
		int mColor0SrcOffset;
		int mColor1SrcOffset;
		int mNormal0DstOffset;
		int mNormal1DstOffset;
		int mNormal2DstOffset;
		int mTexCoord0DstOffset;
		int mTexCoord1DstOffset;
		int mColor0DstOffset;
		int mColor1DstOffset;
		bool mbRenderProceduralLines;
		Set<String, std::less<String> > mShaderErrorList;
	};
	
	struct TextureInstance
	{
		RenderObject_Mesh* mpMeshInstance;
		BoundingBox mBoundingBox;
		Sphere mBoundingSphere;
		T3MeshTextureType mTextureType;
		T3Texture* mpCurTexture;
		float mMaxObjAreaPerUVArea;
	};

	struct LegacyTextureAnimatedValues
	{
		Symbol mTextureName;
		Symbol mTextureNameWithoutExtension;
		//std::shared_ptr<AnimationMixerBase> mpMixerTransform2D[13];
		//std::shared_ptr<AnimationMixerBase> mpMixerOverride;
		//std::shared_ptr<AnimationMixerBase> mpMixerVisibile;
		float mTransform2DValue[13];
		bool mbVisible;
	};

	inline ~RenderObject_Mesh() {
		if (mpMaterialParameterBuffer)
			delete mpMaterialParameterBuffer;
		if (mpEffectParameterBuffer)
			delete mpEffectParameterBuffer;
		if (mpMeshBatchParameterBuffer)
			delete mpMeshBatchParameterBuffer;
		mpMeshBatchParameterBuffer = 0;
		mpEffectParameterBuffer = 0;
		mpMaterialParameterBuffer = 0;
	}

	D3DMesh* mhBaseMesh;
	std::vector<D3DMesh*> mpMeshList;
	std::vector<MeshInstance> mMeshInstanceList;
	//Ptr<Agent> mpAgent;
	std::vector<TextureInstance> mTextureInstances;
	std::vector<T3MaterialInstance> mMaterialInstances;
	std::vector<LightGroupInstance> mLightGroupInstances;
	//DCArray<T3LightEnvGroupInstance> mLightEnvGroupInstances;
	std::vector<LegacyTextureAnimatedValues> mLegacyTextureAnimations;
	T3EffectParameterBuffer* mpEffectParameterBuffer=0;
	T3EffectParameterBuffer* mpMeshBatchParameterBuffer=0;
	std::weak_ptr<LightGroup> mpShadowCastGroup;
	std::weak_ptr<LightGroup> mpShadowReceiveGroup;
	int mLightEnvGroupToInstanceIndex[8];
	BitSet<enum T3LightEnvGroup, 20, 0> mValidLightEnvGroups;
	BinaryBuffer mMaterialParameterBuffer;
	T3EffectParameterBuffer* mpMaterialParameterBuffer=0;
	std::vector<PropertySet*> mMaterialRuntimeProperties;
	float mLODScale;
	int mLODBias;
	bool mbHasStaticLighting;
	RenderQualityType mRenderQuality;
	int mFeatureLevel;
	bool mbBakedLightEffects;
	Sphere mBoundingSphere;
	BoundingBox mBoundingBox;
	Sphere mAnimatedBoundingSphere;
	BoundingBox mAnimatedBoundingBox;
	BoundingBox mPrevAnimatedBoundingBox;
	BitSet<enum T3MaterialPassType, 21, 0> mMaterialPasses;
	BitSet<enum T3MaterialPassType, 21, 0> mActiveMaterialPasses;
	bool mbDraw;
	bool mbCameraFacing;
	CameraFacingTypes mCamFacingType;
	bool mbZTest;
	bool mbZWrite;
	bool mbZWriteAlpha;
	bool mbZ3DAlpha;
	ZTestFunction mZFunc;
	bool mbColorWrite;
	bool mbCulling;
	bool mbForceAsAlpha;
	bool mbHasGlow;
	bool mbHasAlpha;
	float mObjectScale;
	Vector3 mAxisScale;
	Vector3 mCombinedScale;
	float mMaxScale;
	bool mbIsScaled;
	Color mConstantEmission;
	float mRimBumpScale;
	RenderMaskWrite mMaskWrite;
	RenderMaskTest mMaskTest;
	bool mbDisableBakeLight;
	bool mbPreviouslyVisible;
	float mMaterialTime;
	float mMaterialPrevTime;
	Color mDiffuseColor;
	bool mbAllowNPRLines;
	float mVisibilityThresholdScale;
	float mShadowVisibilityThresholdScale;
	bool mbShadowUseLowLOD;
	bool mbAlphaAntialiasing;
	//Ptr<LightProbeState> mpLightProbeState;
	Map<Symbol, Symbol, std::less<Symbol> > mLightGroups;
	Symbol mShadowCastGroupName;
	Symbol mShadowReceiveGroupName;
	Symbol mLightEnvNodeName;
	//Ptr<Node> mpLightEnvNode;
	Vector3 mLightEnvNodeOffset;
	Vector3 mLightEnvPosition;
	float mLightEnvIntensity;
	float mLightEnvReflectionIntensity;
	bool mbLightEnvEnable;
	bool mbLightEnvReflectionEnable;
	float mLightEnvReflectionIntensityShadow;
	Symbol mLightRigName;
	//Ptr<CinematicLightRig> mpLightRig;
	unsigned int mCachedLightEnvShadowCastGroupMask;
	T3LightEnvGroup mLightEnvGroup;
	unsigned int mLightEnvShadowCastGroupMask;
	bool mbLightEnvCastShadows;
	float mLightmapScale;
	RenderLightmapUVGenerationType mLightmapUVGenerationType;
	DCArray<unsigned short> mStationaryLightIndices;
	Flags mSceneLightmapFlags;
	bool mbCastShadows;
	bool mbReceiveShadowsNew;
	bool mbRecieveShadowsDecal;
	bool mbRecieveShadowsDoublesided;
	float mRecShadowsIntensity;
	bool mbMotionBlurEnabled;
	bool mFogOverride;
	bool mFogEnabled;
	Color mFogColor;
	float mFogMinDistance;
	float mFogMaxDistance;
	bool mbFXColorActive;
	bool mbRenderToonOutline;
	bool mbOverrideToonOutlineColor;
	Color mToonOutlineColor;
	float mNPRLineFalloff;
	float mNPRLineBias;
	float mNPRLineFalloffBiasOverride;
	float mNPRLineAlphaFalloff;
	float mNPRLineAlphaBias;
	float mNPRLineAlphaFalloffBiasOverride;
	bool mSSLineEnable;
	bool mbRenderAfterPostEffects;
	bool mbRenderAfterAntiAliasing;
	bool mbForceLinearDepthWrite;
	float mBrushScale;
	float mBrushNearDetailBias;
	float mBrushFarDetailBias;
	bool mbBrushForceEnable;
	bool mbBrushScaleByVertex;
	float mAlphaStreamValue;
	float mAlphaMultiply;
	bool mbConstantAlpha;
	float mAmbientOcclusionLightmap;
	bool mbPreparedToDraw;
	int mFrameAnimationUpdated;
	int mFrameRootAnimationUpdated;
	//Job* mpCurrentAnimationJob;
	Transform mCachedWorldTransform;
	Matrix4 mCachedWorldMatrix;
	Matrix4 mCachedInvWorldMatrix;
	Matrix4 mCachedPrevWorldMatrix;
	float mCachedDistanceToCamera;
	int mTransformChangedCounter;
	EnumMeshDebugRenderType mDebugRender;
	bool mbDebugRenderBoneBounds;
	bool mbShowNormals;
	bool mbShowTangents;
	bool mbShowBitangents;
	bool mbShowBounds;
	Vector3 mOffsetPosition;
	Vector3 mOffsetScale;
	MeshSceneEnlightenData mEnlightenData;
	T3MaterialInstanceTextureOverrides mTextureOverrides;
};

struct T3PostMaterialParams
{
	Handle<PropertySet> mhMaterial;
	float mMaterialTime;
};

struct RenderObject_Viewport : ListNode<RenderObject_Viewport>
{
	Scene* mpScene;
	//Ptr<Agent> mpAgent;
	Camera* mpCamera;
	Vector2 mViewportOrigin;
	Vector2 mViewportSize;
	bool mbViewportRelative;
	Symbol mCameraName;
	bool mbYFlipped;
};

struct RenderObject_PostMaterial : RenderObjectInterface
{
	//Ptr<Agent> mpAgent;
	T3PostMaterialParams mParams;
	unsigned __int64 mMaterialCRC;
	std::shared_ptr<T3MaterialInstance> mpMaterialInstance;
	BinaryBuffer mMaterialParameterBuffer;
	std::shared_ptr<T3EffectParameterBuffer> mpMaterialParameterBuffer;
	std::vector<PropertySet*> mMaterialRuntimeProperties;
};

struct T3DecalParams
{
	Handle<PropertySet> mhMaterial;
	Vector3 mSize;
	float mScale;
	float mNormalThreshold;
	float mVisibilityThresholdScale;
	float mMaterialTime;
	float mNormalOpacity;
	Flags mFlags;
};

struct RenderObject_Decal : RenderObjectInterface
{
	//Ptr<Agent> mpAgent;
	T3DecalParams mParams;
	T3EffectCacheRef mEffectRef[3];
	std::shared_ptr<T3MaterialInstance> mpMaterialInstance;
	std::shared_ptr<PropertySet> mpCachedMaterial;
	BinaryBuffer mMaterialParameterBuffer;
	std::shared_ptr<T3EffectParameterBuffer> mpMaterialParameterBuffer;
	std::vector<PropertySet*> mMaterialRuntimeProperties;
};

struct PrepareSceneParams
{
	RenderQualityType mRenderQuality;
	/*RenderObjectCategory*/u32 mRenderCategoryAny;
	/*RenderObjectCategory*/u32 mRenderCategoryAll;
	unsigned int mShadowMapAtlasResolution;
	unsigned int mShadowCascadeCount;
	bool mbRenderHiddenScenes;
	bool mbAllowAfterEffects;
	bool mbAllowReflectionEnvironment;
	bool mbAllowDebugRender;
	bool mbAllowBakedLighting;
};

struct RenderSceneContext
{
	Scene* mpScene;
	Camera* mpViewCamera;
	//LightManager* mpLightManager;
	//LightSceneContext* mpLightContext;
	Symbol mName;
	//RenderBoneMatrixCache* mpBoneMatrixCache;
	RenderGBufferFlagsCache* mpGBufferFlagsCache;
	RenderFrameScene* mpRenderScene;
	RenderSceneView* mpMainRenderView;
	RenderViewPass* mpComputeRenderPass;
	RenderObjectContext* mObjectList;
	RenderObject_PostMaterial* mpActivePostMaterial;
	unsigned int mCameraCutFrameIndex;
	int mRenderLayer;
	int mSceneColorTextureRenderLayer;
	int mObjectCapacity;
	int mObjectCount;
	bool mbPostEffects;
	bool mbAntialiasing;
	bool mbBakedLightingEnabled;
};

// SCENE PASSES

struct ScenePassParams
{
	T3RenderPass mPass;
	T3RenderTargetIDSet mRenderTarget;
	bool mbPreserveRenderTarget;
	bool mbDefaultRenderTarget;
	bool mbClearColor;
	bool mbClearDepth;
	bool mbClearStencil;
	bool mbForceClear;
	bool mbAntialiasing;
	RenderScopeGPU mGPUScope;

	inline ScenePassParams() : mbPreserveRenderTarget(false), mbDefaultRenderTarget(false), mGPUScope(eRenderScopeGPU_None),
		mPass(eRenderPass_Clear), mbClearColor(false), mbClearDepth(false), mbClearStencil(false), mbForceClear(false), mbAntialiasing(false) {}

};

RenderViewPass* _PrepareScenePassBase(ScenePassParams& params, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext, const char* szName);

RenderViewPass* _PrepareScenePass_Clear(RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);

RenderViewPass* _PrepareScenePass_Compute(RenderSceneView* pMainViews, T3RenderTargetContext& targetContext, int targetWidth, int targetHeight);

RenderViewPass* _PrepareScenePass_DeferredDecal_Emmisive(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);

RenderViewPass* _PrepareScenePass_DeferredDecal_GBuffer(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);

RenderViewPass* _PrepareScenePass_Downsampled(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext, RenderQualityType type);

RenderViewPass* _PrepareScenePass_ForceLinearDepth(Scene* pScene, Camera* pCamera, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);

RenderViewPass* _PrepareScenePass_GBuffer(Scene* pScene, Camera* pCamera, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);

void _PrepareScenePass_GBufferLines(Scene* pScene, Camera* pCamera, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);

RenderViewPass* _PrepareScenePass_GBuffer_Simple(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);

RenderViewPass* _PrepareScenePass_Glow(RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);

RenderViewPass* _PrepareScenePass_Lines(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);

RenderViewPass* _PrepareScenePass_Main_AfterSSLines(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);

RenderViewPass* _PrepareScenePass_Main_Alpha(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);

RenderViewPass* _PrepareScenePass_Main_Alpha_Antialiased(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);

RenderViewPass* _PrepareScenePass_Main_Alpha_PreResolve(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);

RenderViewPass* _PrepareScenePass_Main_Extended(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext, T3RenderPass pass, bool bDefaultRenderTarget);

RenderViewPass* _PrepareScenePass_Main_Opaque(RenderFrameScene& scene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext, bool bClearColor, bool bClearDepthTarget);

RenderViewPass* _PrepareScenePass_SSS(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext);


