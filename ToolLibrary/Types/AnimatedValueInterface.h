// This file was written by Lucas Saragosa. The code derives from Telltale Games' Engine.
// I do not intend to take credit for it, however; Im the author of this interpretation of 
// the engine and require that if you use this code or library, you give credit to me and
// the amazing Telltale Games.

#ifndef _ANIMATED_VALUE_INTERFACE
#define _ANIMATED_VALUE_INTERFACE

#include "../Meta.hpp"
#include "ObjectSpaceIntrinsics.h"
#include "../TelltaleToolLibrary.h"

struct AnimationValueSerializeContext {
	MetaStream* mStream;
	char* mpBuffer;
	int mBufferSize;
	int mBufferOffset;

	void BeginValue() {
		mBufferOffset = (mBufferOffset + 0x1F) & 0xFFFFFFE0;
	}

	char* Allocate(int size) {
		int o = mBufferOffset;
		mBufferOffset += ((size + 3) & 0xFFFFFFFC);
		return &mpBuffer[o];
	}

};

struct AnimationValueInterfaceBase {

	enum Flags {
		eDisabled = 0x1,
		eTimeBehavior = 0x2,
		eWeightBehavior = 0x4,
		eMoverAnim = 0x10,
		ePropertyAnimation = 0x20,
		eTextureMatrixAnimation = 0x40,
		eAudioDataAnimation = 0x80,
		eDontOptimize = 0x100,
		eHomogeneous = 0x200,
		eMixerScaled = 0x400,
		eMixerHomogeneous = 0x800,
		eStyleAnimation = 0x1000,
		ePropForceUpdate = 0x2000,
		eMixerOwned = 0x4000,
		eMixerDirty = 0x8000, 
		eAdditive = 0x10000,
		eExternallyOwned = 0x20000,
		eDontMixPausedControllers = 0x40000,
		eRuntimeAnimation = 0x80000,
		eTransientAnimation = 0x100000,
		eToolOnly = 0x200000,
		eKeyedAttachmentAnimation = 0x400000,
		eMixerWeightedBlend = 0x800000,
		eValueKind = 0xFF000000,
	};

	inline static const char* FlagToString(Flags flag) {
		switch(flag){
		case eDisabled:
			return "Disabled";
		case eTimeBehavior:
			return "Time Behaviour";
		case eWeightBehavior:
			return "Weight Behaviour";
		case eMoverAnim:
			return "Mover Animation";
		case ePropertyAnimation:
			return "Property Animation";
		case eTextureMatrixAnimation:
			return "Texture Matrix Animation";
		case eAudioDataAnimation:
			return "Audio Data Animation";
		case eDontOptimize:
			return "Don't Optimize";
		case eHomogeneous:
			return "Homogeneous";
		case eMixerScaled:
			return "Mixer Scaled";
		case eMixerHomogeneous:
			return "Mixer Homogeneous";
		case eStyleAnimation:
			return "Style Animation";
		case ePropForceUpdate:
			return "Prop Force Update";
		case eMixerOwned:
			return "Mixer Owned";
		case eMixerDirty:
			return "Mixer Dirty";
		case eAdditive:
			return "Additive";
		case eExternallyOwned:
			return "Externally owned";
		case eDontMixPausedControllers:
			return "Don't mix paused controllers";
		case eRuntimeAnimation:
			return "Runtime Animation";
		case eTransientAnimation:
			return "Transient Animation";
		case eToolOnly:
			return "Tool Only";
		case eValueKind:
			return "Value Kind";
		case eMixerWeightedBlend:
			return "Mixer Weighted Blend";
		case eKeyedAttachmentAnimation:
			return "Keyed Attachment Animation";
		default:
			return "Unknown";
		}
	}

	enum ValueType {
		eValue_Time = 0x1,
		eValue_Weight = 0x2,
		eValue_Skeletal = 0x3,
		eValue_Mover = 0x4,
		eValue_Property = 0x5,
		eValue_AdditiveMask = 0x6,
		eValue_TargetedMover = 0x7,
		eValue_SkeletonPose = 0x8,
		eValue_SkeletonRootAnimation = 0x9,
		eValue_Texture_Start = 0x10,
		eValue_Texture_MoveU = 0x11,
		eValue_Texture_MoveV = 0x12,
		eValue_Texture_ScaleU = 0x13,
		eValue_Texture_ScaleV = 0x14,
		eValue_Texture_Rotate = 0x15,
		eValue_Texture_Override = 0x16,
		eValue_Texture_Visibility = 0x17,
		eValue_Texture_ShearU = 0x18,
		eValue_Texture_ShearV = 0x19,
		eValue_Texture_ShearOriginU = 0x1A,
		eValue_Texture_ShearOriginV = 0x1B,
		eValue_Texture_RotateOriginU = 0x1C,
		eValue_Texture_RotateOriginV = 0x1D,
		eValue_Texture_ScaleOriginU = 0x1E,
		eValue_Texture_ScaleOriginV = 0x1F,
		eValue_Texture_End = 0x1F,
		eValue_Audio_Start = 0x20,
		eValue_Audio_Pan = 0x21,
		eValue_Audio_Pitch = 0x22,
		eValue_Audio_Reverb_Wet_Level = 0x23,
		eValue_Audio_Reverb_Dry_Level = 0x24,
		eValue_Audio_Low_Pass_Filter_Cutoff = 0x25,
		eValue_Audio_High_Pass_Filter_Cutoff = 0x26,
		eValue_Audio_Event_Parameter = 0x27,
		eValue_Audio_Surround_Direction = 0x28,
		eValue_Audio_Surround_Extent = 0x29,
		eValue_Audio_LFE_Send = 0x2A,
		eValue_Audio_LangResVolume = 0x2B,
		eValue_Audio_End = 0x3F,
		eValue_Vertex_Start = 0x40,
		eValue_Vertex_Position = 0x41,
		eValue_Vertex_Normal = 0x42,
		eValue_Vertex_End = 0x60,
		eValue_AutoAct = 0x61,
		eValue_ExplicitComputeValue = 0x62,
	};

	inline static const char* GetValueType(ValueType type) {
		if (type == eValue_Time)
			return "Time";
		else if (type == eValue_Weight)
			return "Weight";
		else if (type == eValue_Skeletal)
			return "Skeletal";
		else if (type == eValue_Mover)
			return "Mover";
		else if (type == eValue_Property)
			return "Property";
		else if (type == eValue_AdditiveMask)
			return "Additive Mask";
		else if (type == eValue_TargetedMover)
			return "Targeted Mover";
		else if (type == eValue_SkeletonPose)
			return "Skeleton Pose";
		else if (type == eValue_SkeletonRootAnimation)
			return "Skeleton Root Animation";
		else if (type == eValue_Texture_MoveU)
			return "Texture Move U";
		else if (type == eValue_Texture_MoveV)
			return "Texture Move V";
		else if (type == eValue_Texture_ScaleU)
			return "Texture Scale U";
		else if (type == eValue_Texture_ScaleV)
			return "Texture Scale V";
		else if (type == eValue_Texture_Rotate)
			return "Texture Rotate";
		else if (type == eValue_Texture_Override)
			return "Texture Override";
		else if (type == eValue_Texture_Visibility)
			return "Texture Visibility";
		else if (type == eValue_Texture_ShearU)
			return "Texture Sheav U";
		else if (type == eValue_Texture_ShearV)
			return "Texture Shear V";
		else if (type == eValue_Texture_ShearOriginU)
			return "Texture Shear Origin U";
		else if (type == eValue_Texture_ShearOriginV)
			return "Texture Shear Origin V";
		else if (type == eValue_Texture_RotateOriginU)
			return "Texture Rotate Origin U";
		else if (type == eValue_Texture_RotateOriginV)
			return "Texture Rotate Origin V";
		else if (type == eValue_Texture_ScaleU)
			return "Texture Scale U";
		else if (type == eValue_Texture_ScaleV)
			return "Texture Scale V";
		else if (type == eValue_Audio_Pan)
			return "Audio Pan";
		else if (type == eValue_Audio_Pitch)
			return "Audio Pitch";
		else if (type == eValue_Audio_Reverb_Wet_Level)
			return "Audio Reverb Wet Level";
		else if (type == eValue_Audio_Reverb_Dry_Level)
			return "Audio Reverb Dry Level";
		else if (type == eValue_Audio_Low_Pass_Filter_Cutoff)
			return "Audio Low Pass Filter Cutoff";
		else if (type == eValue_Audio_High_Pass_Filter_Cutoff)
			return "Audio High Pass Filter Cutoff";
		else if (type == eValue_Audio_Event_Parameter)
			return "FMOD Event Parameter";
		else if (type == eValue_Audio_Surround_Direction)
			return "Audio Surround Direction";
		else if (type == eValue_Audio_Surround_Extent)
			return "Audio Surround Extent";
		else if (type == eValue_Audio_LFE_Send)
			return "Audio Low Freq. Effects Send";
		else if (type == eValue_Audio_LangResVolume)
			return "Audio Language Resource Volume";
		else if (type == eValue_Vertex_Position)
			return "Vertex Position";
		else if (type == eValue_Vertex_Normal)
			return "Vertex Normal";
		else if (type == eValue_AutoAct)
			return "Auto Act";
		else if (type == eValue_ExplicitComputeValue)
			return "Explicit Compute Value";
		else
			return "Unknown";
	}

	Symbol mName;
	u32 mFlags;
	//DO NOT TOUCH
	u16 mVersion;
	 
	AnimationValueInterfaceBase() :mFlags(0), mVersion(0){}

	virtual MetaClassDescription* GetMetaClassDescription() {
		return ::GetMetaClassDescription<AnimationValueInterfaceBase>();
	}

	virtual MetaClassDescription* GetValueClassDescription() {
		return NULL;
	}

	virtual void* GetMetaClassObjPointer() {
		return this;
	}

	INLINE bool GetAdditive() {
		//if dirty, clearmixer etc
		return !(mFlags & Flags::eAdditive);
	}

	INLINE bool GetActive() {
		return !(mFlags & Flags::eDisabled);
	}

	virtual float GetMinTime() {
		return 10'000.0f;
	}

	INLINE void SetType(ValueType type) {
		mFlags |= (type << 24);
	}

	INLINE ValueType GetType() {
		return (ValueType)(mFlags >> 24);
	}

	virtual MetaOpResult SerializeIn(AnimationValueSerializeContext* context, u32 version) {
		return PerformMetaSerializeFull(context->mStream, GetMetaClassObjPointer(), GetMetaClassDescription());
	}

	virtual void SerializeOut(MetaStream* stream) {
		PerformMetaSerializeFull(stream, GetMetaClassObjPointer(), GetMetaClassDescription());
	}

	virtual ~AnimationValueInterfaceBase() {}

};

template<typename T> struct AnimatedValueInterface : AnimationValueInterfaceBase {

	virtual MetaClassDescription* GetValueClassDescription() override {
		return ::GetMetaClassDescription<T>();
	}

	virtual MetaClassDescription* GetMetaClassDescription() override {
		return ::GetMetaClassDescription<AnimatedValueInterface<T>>();
	}

};

struct Float4 {
	float v[4];//most likely platform specific for accel computation
};

struct SkeletonPose {
	Float4* mEntries;

	SkeletonPose() {
		mEntries = NULL;
	}

	void SetTransform(int i, Transform* src) {
		mEntries[0].v[(i & 3) + 4 * (7 * (i / 4))] = src->mTrans.x;
		mEntries[1].v[(i & 3) + 4 * (7 * (i / 4))] = src->mTrans.y;
		mEntries[2].v[(i & 3) + 4 * (7 * (i / 4))] = src->mTrans.z;
		mEntries[3].v[(i & 3) + 4 * (7 * (i / 4))] = src->mRot.x;
		mEntries[4].v[(i & 3) + 4 * (7 * (i / 4))] = src->mRot.y;
		mEntries[5].v[(i & 3) + 4 * (7 * (i / 4))] = src->mRot.z;
		mEntries[6].v[(i & 3) + 4 * (7 * (i / 4))] = src->mRot.w;
	}

	void GetTransform(Transform* dst, int i) {
		int index = (i & 3) + 28 * (i / 4);
		dst->mTrans.x = mEntries[0].v[index];
		dst->mTrans.y = mEntries[1].v[index];
		dst->mTrans.z = mEntries[2].v[index];
		dst->mRot.x = mEntries[3].v[index];
		dst->mRot.y = mEntries[4].v[index];
		dst->mRot.z = mEntries[5].v[index];
		dst->mRot.w = mEntries[6].v[index];
	}

	~SkeletonPose() = default;

};

struct CompressedSkeletonPoseKeys2 : AnimationValueInterfaceBase {

	//format for data header in mpData once serialized in
	struct Header {
		Vector3 mMinDeltaV, mRangeDeltaV, mMinDeltaQ, mRangeDeltaQ, mMinVector, mRangeVector;
		int mRangeTime, mBoneCount, mSampleDataSize;
	};

	char* mpData;
	u32 mDataSize;
	bool mDataExternallyOwned;

	CompressedSkeletonPoseKeys2() {
		mpData = NULL;
		mDataSize = 0;
		mDataExternallyOwned = false;
	}

	virtual void SerializeOut(MetaStream* stream) override {
		stream->serialize_uint32(&mDataSize);
		stream->serialize_bytes(mpData, mDataSize);
	}

	virtual void* GetMetaClassObjPointer() override {
		return this;
	}

	virtual MetaOpResult SerializeIn(AnimationValueSerializeContext* context, u32 version) override {
		context->mStream->serialize_uint32(&mDataSize);
		mDataExternallyOwned = true;
		mpData = context->Allocate(mDataSize);
		context->mStream->serialize_bytes(mpData, mDataSize);
		return eMetaOp_Succeed;
	}

	virtual MetaClassDescription* GetValueClassDescription() override {
		return ::GetMetaClassDescription<SkeletonPose>();
	}

	virtual MetaClassDescription* GetMetaClassDescription() override {
		return ::GetMetaClassDescription<CompressedSkeletonPoseKeys2>();
	}

	INLINE void _ReleaseData() {
		if (!mDataExternallyOwned && mpData && mDataSize > 0) {
			delete[] mpData;
			mpData = NULL;
			mDataSize = 0;
			mDataExternallyOwned = false;
		}
	}

	~CompressedSkeletonPoseKeys2() {
		_ReleaseData();
	}

};

struct SkeletonPoseValue : AnimationValueInterfaceBase{

	struct Sample {
		float mTime;
		float mRecipTimeToNextSample;
		DCArray<Transform> mValues;
		DCArray<int> mTangents;
	};

	struct BoneEntry {
		Symbol mName;
		u32 mFlags;
	};

	virtual MetaClassDescription* GetValueClassDescription() override {
		return ::GetMetaClassDescription<SkeletonPose>();
	}

	virtual void* GetMetaClassObjPointer() override {
		return this;
	}

	virtual MetaClassDescription* GetMetaClassDescription() override {
		return ::GetMetaClassDescription<SkeletonPoseValue>();
	}

	DCArray<BoneEntry> mBones;
	DCArray<Sample> mSamples;

};

struct CompressedSkeletonPoseKeys : AnimationValueInterfaceBase {

	//format for data header in mpData once serialized in
	struct Header {
		Vector4 mMinDeltaV, mRangeDeltaV, mMinDeltaQ, mRangeDeltaQ, mMinVector, mRangeVector;
		int mBoneCount, mSampleCount, mValueCount, mBlockCount;
	};

	char* mpData;
	u32 mDataSize;
	bool mDataExternallyOwned;

	CompressedSkeletonPoseKeys() {
		mpData = NULL;
		mDataSize = 0;
		mDataExternallyOwned = false;
		mFlags = 0;
	}

	virtual void* GetMetaClassObjPointer() override {
		return this;
	}

	virtual MetaOpResult SerializeIn(AnimationValueSerializeContext* context, u32 version) override {
		context->mStream->serialize_uint32(&mDataSize);
		mDataExternallyOwned = true;
		mpData = context->Allocate((mDataSize + 0x1f) & 0xFFFFFFE0);
		context->mStream->serialize_bytes(mpData, mDataSize);
		return eMetaOp_Succeed;
	}

	virtual MetaClassDescription* GetValueClassDescription() override {
		return ::GetMetaClassDescription<SkeletonPose>();
	}

	virtual MetaClassDescription* GetMetaClassDescription() override {
		return ::GetMetaClassDescription<CompressedSkeletonPoseKeys>();
	}

	virtual void SerializeOut(MetaStream* stream) override {
		stream->serialize_uint32(&mDataSize);
		stream->serialize_bytes(mpData, mDataSize);
	}

	~CompressedSkeletonPoseKeys() {
		if (!mDataExternallyOwned && mpData && mDataSize > 0) {
			delete[] mpData;
			mDataExternallyOwned = false;
			mpData = NULL;
			mDataSize = 0;
		}
	}

};

template<typename T>
struct SingleValue : AnimationValueInterfaceBase {

	T mComputedValue;

	static METAOP_FUNC_IMPL__(SerializeAsync) {
		SingleValue<T>* pThis = (SingleValue<T>*)pObj;
		MetaClassDescription* pClass = ::GetMetaClassDescription<T>();
		MetaOperation op = pClass->GetOperationSpecialization(74);
		if (!op)
			op = Meta::MetaOperation_SerializeAsync;
		return op(&pThis->mComputedValue, pClass, 0, pUserData);
	}

};

#endif