#ifndef _PARTICLE_PROPS
#define _PARTICLE_PROPS

#include "../Meta.hpp"
#include "DCArray.h"
#include "ObjectSpaceIntrinsics.h"
#include "HandleObjectInfo.h"
#include "Chore.h"
#include "Animation.h"
#include "T3Texture.h"

struct ParticlePropConnect {
	ParticlePropModifier mModType;
	ParticlePropDriver mDriveType;
	float mDriveMin, mDriveMax;
	float mModMin, mModMax;
	bool mInvert;
};

struct ParticlePropertySamples {

	struct Sample {

		//mColor is a RGBA(XYZW), and range 0-1
		Vector4 mPosition, mOrientation, mColor;

		Sample() = default;

	};

	Sample* mpSamples;
	long mSampleCount;

	ParticlePropertySamples() : mSampleCount(0), mpSamples(0) {}

	~ParticlePropertySamples() {
		Clear();
	}

	inline void Clear(){
		if (mpSamples)
			delete[] mpSamples;
		mpSamples = 0;
		mSampleCount = 0;
	}

	inline void Create(int numSamples){
		Clear();
		mpSamples = new Sample[numSamples];
		mSampleCount = numSamples;
	}

	static METAOP_FUNC_IMPL__(SerializeAsync) {
		MetaStream* pStream = (MetaStream*)pUserData;
		ParticlePropertySamples* me = (ParticlePropertySamples*)pObj;
		MetaOpResult result = Meta::MetaOperation_SerializeAsync(pObj, pObjDescription, pContextDescription, pUserData);
		if (result == eMetaOp_Succeed) {
			if (pStream->IsRead())
				me->Create(me->mSampleCount);
			pStream->serialize_bytes(me->mpSamples, me->mSampleCount * sizeof(Sample));//can just do this, engine used this in tool only
		}
		return result;
	}

};

enum ParticlePropertiesTexture {
	eParticlePropTexture_Position = 0,
	eParticlePropTexture_Orientation = 1,
	eParticlePropTexture_Color = 2,
	eParticlePropTexture_Rotation3D = 3,
	eParticlePropTexture_Count = 4,
};

//.PARTICLE FILES
struct ParticleProperties {

	enum AnimationDataFlags {
		eAnimData_Chore = 1,//Animation::mpChore must not be null!
		eAnimData_Samples = 2,//Animation::mpSamples must not be null!
	};

	struct AnimationParams {
		Vector3 mPositionMin;
		Vector3 mPositionMax;

		AnimationParams() = default;

	};

	struct Animation {

		//SERIALIZED 
		Symbol mName;
		AnimationParams mParams;
		Flags mDataFlags;

		//RUNTIME ALLOCATED
		ParticlePropertySamples* mpSamples = 0;
		Chore* mpChore = 0;

		Animation() = default;

		inline ~Animation(){
			if(mpChore){
				delete mpChore;
				mpChore = 0;
			}
			if (mpSamples)
				delete mpSamples;
			mpSamples = 0;
		}

	};

	String mName;
	Flags mTextureFlags;//bit field for number of textures stored?
	long mTextureCount;
	DCArray<Animation> mAnimations;
	T3Texture* mpTexture[eParticlePropTexture_Count]{ 0,0,0,0 };

	ParticleProperties() = default;

	inline ~ParticleProperties(){
		for (int i = 0; i < 4; i++) {
			if (mpTexture[i])
				delete mpTexture[i];
			mpTexture[i] = 0;
		}
	}

	inline T3Texture* GetTexture(ParticlePropertiesTexture texture){
		return mpTexture[(int)texture];
	}

	inline int GetIndexForAnimation(const Symbol& name){
		for(int i = 0; i < mAnimations.mSize; i++){
			if (mAnimations[i].mName == name)
				return i;
		}
		return -1;
	}

	inline void SetTexture(ParticlePropertiesTexture texture, T3Texture* pGrantedOwnershipTex){
		if (mpTexture[texture])
			delete mpTexture[texture];
		mpTexture[texture] = pGrantedOwnershipTex;
	}

	static METAOP_FUNC_IMPL__(SerializeAsync) {
		ParticleProperties* me = (ParticleProperties*)pObj;
		if (((MetaStream*)pUserData)->IsWrite()) {
			me->mTextureCount = 0;
			me->mTextureFlags &= 0xF;
			for(int i = 0; i < eParticlePropTexture_Count; i++){
				if (me->mpTexture[i])
					me->mTextureFlags |= (1 << i);
				else
					me->mTextureFlags &= ~(1 << i);
				if (me->mTextureFlags.mFlags & (1 << i))
					me->mTextureCount++;
			}
		}
		MetaOpResult result = Meta::MetaOperation_SerializeAsync(pObj, pObjDescription, pContextDescription, pUserData);
		if (result != eMetaOp_Succeed)
			return result;
		for (int i = 0; i < me->mAnimations.GetSize(); i++) {
			Animation& anim = me->mAnimations[i];
			if(anim.mDataFlags.mFlags & eAnimData_Chore){
				if(((MetaStream*)pUserData)->IsRead()){
					if(anim.mpChore)
						delete anim.mpChore;
					anim.mpChore = new Chore;
				}
				//Stores a chore
				result = PerformMetaSerializeAsync<Chore>((MetaStream*)pUserData, anim.mpChore);
				if (result != eMetaOp_Succeed)
					return result;
			}else if(anim.mDataFlags.mFlags & eAnimData_Samples){
				if (((MetaStream*)pUserData)->IsRead()) {
					if (anim.mpSamples)
						delete anim.mpSamples;
					anim.mpSamples = new ParticlePropertySamples;
				}
				//Stores a ParticlePropertySamples
				result = PerformMetaSerializeAsync<ParticlePropertySamples>((MetaStream*)pUserData, anim.mpSamples);
				if (result != eMetaOp_Succeed)
					return result;
			}
		}
		if (me->mTextureCount > 0) {
			//just in case
			if (me->mTextureFlags.mFlags > 15)
				me->mTextureFlags = 15;
			for(int i = 0; i < eParticlePropTexture_Count; i++){
				if((me->mTextureFlags.mFlags & (1 << i))){
					if (((MetaStream*)pUserData)->IsRead()) {
						if (me->mpTexture[i])
							delete me->mpTexture[i];
						me->mpTexture[i] = new T3Texture;
					}
					result = PerformMetaSerializeAsync<T3Texture>((MetaStream*)pUserData, me->mpTexture[i]);
					if (result != eMetaOp_Succeed)
						return result;
				}
			}
		}
		return result;
	}

};

#endif