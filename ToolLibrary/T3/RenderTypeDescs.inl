using DynamicFeatures_WDC = BitSet<enum T3EffectFeature, 30, 54>;
using StaticFeatures_WDC = BitSet<enum T3EffectFeature, 54, 0>;
using EffectFeatures_WDC = BitSet<enum T3EffectFeature, 84, 0>;

struct T3EffectBufferFieldDesc
{
	const char* mName;
	T3EffectParameterType mBuffer;
	T3EffectParameterClass mClass;
	T3EffectParameterPrecision mPrecision;
	unsigned int mArraySize;
	unsigned int mScalarOffset;
};

struct T3EffectFeatureDesc
{
	const char* mName;//0
	const char* mTag;//8
	const char* mSuffix;//16
	StaticFeatures mRequiredStaticFeatures;//24
	StaticFeatures mIncompatibleStaticFeatures;//32
	DynamicFeatures mIncludeDynamicFeatures;//40
	DynamicFeatures mRequiredDynamicFeaturesAny;//44
	EffectQualities mExcludeQuality;//48
	GamePlatforms mExcludePlatforms;//should be 'Exluded', 52
	int mMinRequiredFeatureLevel;//56
	int mMaxSupportedFeatureLevel;//60
	bool mbDynamic;//64
	bool mbToolOnly;//65
};//sizeof 72

struct T3EffectFeatureQualityDesc
{
	EffectFeatures mValidFeaturesForQuality;//8
	StaticFeatures mValidStaticFeatures;//4
	DynamicFeatures mValidDynamicFeatures;//4
};

struct T3EffectDesc
{
	const char* mName;//0
	const char* mFXFileName;//8 8
	const char* mMacroList;//16 10
	int mLength;//Should be 'mNameLength'//24 18
	bool mbToolOnlyEffect;//28 1C
	BitSet<enum T3EffectCompilerFlags, 1, 0> mCompilerFlags;//32 20
	T3EffectCategoryType mCategory;//36 24
	EffectFeatures mValidFeatures;//40 28
	T3EffectFeatureQualityDesc mFeatures[5];//52 34
	EffectQualities mExcludedQuality;//172 94bat2
	GamePlatforms mExcludedPlatforms;//176 98bat2
	unsigned int mStateHash;//180 9Cbat2
	bool mbRequiresMaterial;//184 A0bat2
	std::string mFullName;
};//extra 7 bytes so 8 align

struct T3PostEffectDesc
{

	struct InputBuffer
	{
		T3PostEffectType mProducerType;
		T3RenderTargetConstantID mTargetID;
		unsigned int mViewFlags;
	};

	T3EffectType mEffectType;
	T3PostEffectDesc::InputBuffer mInputBuffers[8];
	T3RenderTargetConstantID mOutputBuffers[8];
	T3RenderTargetConstantID mOutputDepthBuffer;
	int mInputBufferCount;
	int mOutputBufferCount;
	T3RenderClear mRenderClear;
	T3RenderStateBlock mRenderState;
	T3RenderHiStencilMode mHiStencilMode;
	bool mbOverrideClearValues;
	bool mbGenerateMipMaps;
	bool mbFilterMipMaps;
};

struct T3MaterialShaderDesc
{
	const char* mName;
	unsigned int mPassCount;
};

struct T3EffectQualityDesc
{
	const char* mName;
	const char* mSuffix;
};

struct T3MaterialBlendModeDesc
{
	const char* mName;
	DynamicFeatures_WDC mExcludeEffectFeatures;
	bool mbAlphaBlend;
	bool mbAlphaTest;
};

struct T3MaterialDomainDesc
{
	const char* mName;
	BitSet<enum T3MaterialDomainType, 5, 0> mDependentDomains;
	bool mbUsePreShader;
	bool mbExternalShader;
};

struct T3MaterialChannelFormatDesc
{
	const char* mName;
};

struct T3MaterialChannelDesc
{
	const char* mName;
	const char* mMacro;
	T3MaterialChannelFormatType mChannelFormat;
	T3MaterialShaderInput mShaderInput;
	BitSet<enum T3MaterialShaderType, 4, 0> mShaderTypes;
	BitSet<enum T3MaterialPassType, 21, 0> mValidForPasses;
	T3MaterialValueType mValueType;
	T3MaterialPrecisionType mPrecision;
	BitSet<enum T3MaterialQualityType, 3, 0> mExcludeQuality;
	unsigned int mShaderPassIndex[4];
	unsigned int mPriority;
	Vector4 mDefaultValue;
};

struct T3MaterialLightModelDesc
{
	const char* mName;
	const char* mMacro;
	BitSet<enum T3MaterialDomainType, 5, 0> mValidDomains;
	BitSet<enum T3MaterialChannelType, 46, 0> mValidChannels;
	DynamicFeatures_WDC mExcludeEffectFeatures;
	BitSet<enum T3MaterialChannelType, 46, 0> mDefaultChannels;
	BitSet<enum T3MaterialPropertyType, 31, 0> mDefaultProperties;
	BitSet<enum T3MaterialTexturePropertyType, 1, 0> mDefaultTextureProperties;
};

struct T3MaterialOptionalPropertyDesc
{
	char mDefaultValueBuffer[32];
	const char* mName;
	Symbol mParameterName;
	BitSet<enum T3MaterialPropertyType, 31, 0> mDerivedProperties;
	BitSet<enum T3MaterialTexturePropertyType, 1, 0> mDerivedTextureProperties;
	MetaClassDescription* mpDesc;
};

struct T3MaterialPassDesc
{
	const char* mName;
	T3EffectType mEffectType;
	T3MaterialDomainType mDomain;
	BitSet<enum T3MaterialQualityType, 3, 0> mExcludeQuality;
	BitSet<enum T3MaterialBlendModeType, 4, 0> mExcludeBlendModes;
	BitSet<enum T3MaterialChannelType, 46, 0> mRequiredChannelsAll;
	BitSet<enum T3MaterialChannelType, 46, 0> mRequiredChannelsAny;
	BitSet<enum T3MaterialPropertyType, 31, 0> mRequiredPropertiesAll;
	BitSet<enum T3MaterialPropertyType, 31, 0> mRequiredPropertiesAny;
	BitSet<enum T3MaterialChannelType, 46, 0> mSupportingChannels;//should be 'Supported'
	BitSet<enum T3MaterialPropertyType, 31, 0> mValidProperties;
	BitSet<enum T3MaterialTexturePropertyType, 1, 0> mValidTextureProperties;
	bool mbDisableAlphaTest;
	bool mbDisableAlphaBlend;
	bool mbSupportsLightModel;
	bool mbAlwaysEnabled;
};

struct T3MaterialPropertyDesc
{
	const char* mName;
	const char* mMacro;
	Symbol mParameterName;
	BitSet<enum T3MaterialShaderType, 4, 0> mShaderTypes;
	BitSet<enum T3MaterialPassType, 21, 0> mValidForPasses;
	bool mbCreateAnonymousParameter;
	T3MaterialValueType mValueType;
	T3MaterialPrecisionType mPrecision;
	MetaClassDescription* mpDesc;
	Vector4 mDefaultValue;
	bool mbDeprecated;
};

struct T3MaterialValueDesc
{
	unsigned int mScalarSize;
	T3EffectParameterClass mEffectParameterClass;
};

struct T3MaterialSceneTextureDesc
{
	const char* mName;
	T3MaterialValueType mValueType;
	T3MaterialPrecisionType mPrecision;
};

struct T3MaterialShaderInputDesc
{
	const char* mName;
	const char* mMacro;
	T3MaterialValueType mValueType;
	T3MaterialPrecisionType mPrecision;
	BitSet<enum T3MaterialShaderType, 4, 0> mValidShaders;
	BitSet<enum T3MaterialDomainType, 5, 0> mExcludeDomains;
	BitSet<enum T3MaterialPassType, 21, 0> mExcludePasses;
	unsigned int mShaderPassIndex[4];
	T3MaterialChannelType mChannel;
	T3EffectParameterField mEffectParameterField;
	int mEffectParameterFieldIndex;
	Vector4 mDefaultValue;
	bool mbNormalized;
	bool mbEditorHide;
};

struct T3MeshVertexStreamDesc
{
	GFXPlatformVertexAttribute mAttribute;
	unsigned int mAttributeIndex;
};


struct T3RenderTargetDesc
{
	const char* mName;
	T3RenderTargetUsage mUsage;
	T3RenderTargetProducerType mProducer;
	T3RenderTargetClear mClear;
	float mScaleX;
	float mScaleY;
	T3SurfaceMultisample mMultisample;
	T3SurfaceAccess mAccess;
	GFXPlatformFastMemHeap mFastMemHeap;
	int mNumMipLevels;
	bool mbAllowRenderScale;
};

struct T3MaterialTextureDesc
{
	const char* mName;
	T3MaterialTextureLayout mLayout;
	T3MaterialValueType mValueType;
	T3MaterialPrecisionType mPrecision;
	T3MaterialChannelFormatType mFormatType;
};

struct T3MaterialTexturePropertyDesc
{
	const char* mName;
	const char* mMacro;
	Symbol mParameterName;
	T3MaterialTextureType mTextureType;
	BitSet<enum T3MaterialPropertyType, 31, 0> mDerivedProperties;
	Vector4 mDefaultValue;
};

struct T3MaterialTransform2DComponentDesc
{
	const char* mName;
	const char* mParameterName;
	float mDefaultValue;
};

struct QualityEntry
{
	BitSet<enum RenderFeatureType, 22, 0> mFeaturesSupported;
	float mVisibilityThreshold;
	float mShadowVisibilityThreshold;
	float mLighVisibilityThreshold;
	float mDecalVisibleDistance;
};