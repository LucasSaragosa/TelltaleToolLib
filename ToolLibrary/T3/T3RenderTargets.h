#pragma once

#include "T3Effect.h"
#include <vector>
#include "../LinkedList.h"
#include "LinearHeap.h"

struct T3RenderTargetEnableFlags
{
	Flags mFlags;//0-7 flags are must idset::mrendertargetid is set or not
};

struct T3RenderTargetIDSurface
{
	T3RenderTargetID mTargetID;
	u32 mMipLevel;
	u32 mSliceIndex;

	inline bool operator==(const T3RenderTargetIDSurface& rhs){
		return (u32)mTargetID.mValue == (u32)rhs.mTargetID.mValue && mMipLevel == rhs.mMipLevel && mSliceIndex == rhs.mSliceIndex;
	}

};

struct T3RenderTargetReferences
{

	struct Entry : ListNode<Entry> {
		T3RenderTargetID mTargetID;
	};

	LinkedList<Entry> mReferences;
};

struct T3RenderTargetIDSet
{
	T3RenderTargetIDSurface mRenderTargetID[8];
	T3RenderTargetIDSurface mDepthTargetID;

	inline T3RenderTargetIDSet(T3RenderTargetConstantID id, u32 mip, u32 slice){
		mRenderTargetID[0].mTargetID.mValue = id;
		mRenderTargetID[0].mMipLevel = mip;
		mRenderTargetID[0].mSliceIndex = slice;
	}

	inline T3RenderTargetIDSet() : T3RenderTargetIDSet(eRenderTargetID_Unknown, 0,0) {}

	inline T3RenderTargetIDSet(T3RenderTargetID id, u32 mip, u32 slice) :
		T3RenderTargetIDSet((T3RenderTargetConstantID)id.mValue, mip, slice){}

	inline void SetRenderTarget(const T3RenderTargetID& id, i32 targetIndex, u32 mip, u32 slice) {
		mRenderTargetID[targetIndex].mMipLevel = mip;
		mRenderTargetID[targetIndex].mSliceIndex = slice;
		mRenderTargetID[targetIndex].mTargetID = id;
	}

	inline void SetDepthTarget(const T3RenderTargetID& id, u32 mip, u32 slice) {
		mDepthTargetID.mMipLevel = mip;
		mDepthTargetID.mSliceIndex = slice;
		mDepthTargetID.mTargetID = id;
	}

	inline bool operator==(const T3RenderTargetIDSet& rhs){
		for (int i = 0; i < 8; i++)
			if (!(mRenderTargetID[i] == rhs.mRenderTargetID[i]))
				return false;
		return mDepthTargetID == rhs.mDepthTargetID;
	}

}; 

struct T3RenderTarget
{
	std::shared_ptr<T3Texture> mpTexture;
	T3RenderTargetUsage mUsage;
	T3TextureLayout mLayout;
	T3SurfaceMultisample mMultisample;
	bool mbLocked;
};

struct T3RenderTargetParams
{
	GFXPlatformAllocationType mAllocationType;
	int mWidth;
	int mHeight;
	int mDepth;
	int mArraySize;
	int mNumMipLevels;
	T3RenderTargetUsage mUsage;
	T3SurfaceAccess mAccess;
	T3TextureLayout mLayout;
	T3SurfaceMultisample mMultisample;
	T3RenderTargetClear mClear;
	T3ResourceUsage mResourceUsage;
	GFXPlatformFastMemHeap mFastMemHeap;

	inline T3RenderTargetParams() : mFastMemHeap(eGFXPlatformFastMemHeap_None), mResourceUsage(eResourceUsage_RenderTarget),
		mClear{} {}

};

struct T3RenderTargetResult
{
	u32 mPendingCount;
	std::shared_ptr<T3RenderTarget> mpTarget;

	inline void Clear();

	inline ~T3RenderTargetResult() {
		Clear();
	}

};

struct T3RenderTargetList
{

	struct Entry
	{
		const char* mName;
		std::shared_ptr<T3Texture> mpInitialExternalTexture;
		std::shared_ptr<T3Texture> mpTarget;
		T3RenderTargetResult* mpTargetRef;
		T3RenderTargetParams mParams;
		i32 mInitialRefCount;
		i32 mRefCount;
		bool mbOutputTargetValid;
		bool mbExternalTexture;
		bool mbTemporary;
	};

	u32 mEntryCount = 0;
	Entry* mEntries;

};

struct T3RenderTargetContext
{

	struct DefaultEntry
	{
		std::shared_ptr<T3Texture> mpExternalTexture;
		int mRefCount;
	};

	struct TemporaryEntry : ListNode<TemporaryEntry>
	{
		const char* mName;
		T3RenderTargetID mID;
		T3RenderTargetParams mParams;
		T3RenderTargetResult* mpTargetRef;
		std::shared_ptr<T3Texture> mpExternalTexture;
		int mRefCount;
	};

	LinearHeap* mHeap;
	DefaultEntry mDefaultList[eRenderTargetID_Count];
	LinkedList<TemporaryEntry> mTemporaryList;
	int mNextID;//next id for a temp target. must never be count id count for render targets.

	inline T3RenderTargetContext() : mNextID(eRenderTargetID_Count){}

};

struct RenderFrameUpdateList;

struct T3RenderTargetSurface
{
	std::shared_ptr<T3Texture> mpTexture;
	u32 mMipLevel;
	u32 mSliceIndex;
};

struct T3RenderTargetSet
{
	T3RenderTargetSurface mRenderTarget[8];
	T3RenderTargetSurface mDepthTarget;
	i32 mWidth;
	i32 mHeight;

	inline bool HasRenderTarget(){
		for (int i = 0; i < 8; i++)
			if (mRenderTarget[i].mpTexture)
				return true;
		return false;
	}

	T3RenderTargetSet() = default;

};

namespace T3RenderTargetUtil {

	inline T3RenderTargetConstantID GetSceneBackBufferHDR(RenderAntialiasType aliasing){
		T3RenderTargetConstantID ret = eRenderTargetID_BackBufferHDR;
		if (aliasing <= eRenderAntialias_TAA_MSAA_8x && aliasing >= eRenderAntialias_MSAA_2x)
			ret = eRenderTargetID_BackBufferHDR_MSAA;
		return ret;
	}

	inline std::shared_ptr<T3Texture> GetTarget(std::shared_ptr<T3RenderTargetResult> result)
	{
		return result->mpTarget->mpTexture;
	}

	inline std::vector<T3RenderTarget>& GetFreeRenderTargets() {
		static std::vector<T3RenderTarget> sFreeTargets{};
		return sFreeTargets;
	}

	inline std::vector<T3RenderTarget>& GetUsedRenderTargets() {
		static std::vector<T3RenderTarget> sUsedTargets{};
		return sUsedTargets;
	}

	inline std::vector<T3RenderTarget>& GetReattachRenderTargets() {
		static std::vector<T3RenderTarget> sRTargets{};
		return sRTargets;
	}

	inline void FreeAllRenderTargets() {
		GetReattachRenderTargets().clear();
		GetFreeRenderTargets().clear();
		GetUsedRenderTargets().clear();
	}

	inline void _FreeUnusedRenderTargets(u32 ageThrshold, u64 gfxFrameIndex){
		bool update = false;
		while (1) {
			for (auto it = GetFreeRenderTargets().begin(); it != GetFreeRenderTargets().end(); it++)
				if ((gfxFrameIndex - it->mpTexture->mLastUsedFrame) >= 900) {
					GetFreeRenderTargets().erase(it);
					update = 1;
				}
			if (!update)
				break;
		}
	}

	inline void ReattachRenderTarget(T3RenderTarget target){
		GetReattachRenderTargets().push_back(std::move(target));
	}

	inline bool InitializeExternalTarget(std::shared_ptr<T3Texture> pTexture, T3RenderTargetParams params, String name){
		if (pTexture->IsValidImmediate() && params.mWidth == pTexture->mWidth && params.mHeight == pTexture->mHeight)
			return true;//already done
		T3SamplerStateBlock samplerState = kDefault;
		T3Texture::CreateParams texp{};
		texp.mFormat = eSurface_RGBA8;
		if (params.mUsage == eRenderTargetUsage_RGBA8S)
			texp.mFormat = eSurface_RGBA8S;
		if (params.mUsage == eRenderTargetUsage_sRGB565 || params.mUsage == eRenderTargetUsage_RGB565)
			texp.mFormat = eSurface_RGB565;
		if (params.mUsage == eRenderTargetUsage_sRGB565 || params.mUsage == eRenderTargetUsage_sRGBA8)
			texp.mGamma = eSurfaceGamma_sRGB;
		if (params.mUsage == eRenderTargetUsage_RGBA1010102)
			texp.mFormat = eSurface_ARGB2101010;
		else if (params.mUsage == eRenderTargetUsage_RGBA16F)
			texp.mFormat = eSurface_RGBA16F;
		else if (params.mUsage == eRenderTargetUsage_RGBA32F)
			texp.mFormat = eSurface_RGBA32F;
		else if (params.mUsage == eRenderTargetUsage_RGBA1010102F)
			texp.mFormat = eSurface_RGBA1010102F;
		else if (params.mUsage == eRenderTargetUsage_RGB111110F)
			texp.mFormat = eSurface_RGBA1010102F;
		else if (params.mUsage == eRenderTargetUsage_RGB9E5F)
			texp.mFormat = eSurface_RGB9E5F;
		else if (params.mUsage == eRenderTargetUsage_R8)
			texp.mFormat = eSurface_R8;
		else if (params.mUsage == eRenderTargetUsage_RG8)
			texp.mFormat = eSurface_RG8;
		else if (params.mUsage == eRenderTargetUsage_RG16F)
			texp.mFormat = eSurface_RG16F;
		else if (params.mUsage == eRenderTargetUsage_R32F)
			texp.mFormat = eSurface_R32F;
		else if (params.mUsage == eRenderTargetUsage_RG32F)
			texp.mFormat = eSurface_RG32F;
		else if (params.mUsage == eRenderTargetUsage_RGBA16F)
			texp.mFormat = eSurface_RGBA16;
		else if (params.mUsage == eRenderTargetUsage_RG16)
			texp.mFormat = eSurface_RG16;
		else if (params.mUsage == eRenderTargetUsage_RGBA16S)
			texp.mFormat = eSurface_RGBA16S;
		else if (params.mUsage == eRenderTargetUsage_RG16S)
			texp.mFormat = eSurface_RG16S;
		else if (params.mUsage == eRenderTargetUsage_R16)
			texp.mFormat = eSurface_R16;
		else if (params.mUsage == eRenderTargetUsage_R16UI)
			texp.mFormat = eSurface_R16UI;
		else if (params.mUsage == eRenderTargetUsage_RG16UI)
			texp.mFormat = eSurface_RG16UI;
		else if (params.mUsage == eRenderTargetUsage_R32)
			texp.mFormat = eSurface_R32;
		else if (params.mUsage == eRenderTargetUsage_RG32)
			texp.mFormat = eSurface_RG32;
		else if (params.mUsage == eRenderTargetUsage_RGBA32)
			texp.mFormat = eSurface_RGBA32;
		else if (params.mUsage == eRenderTargetUsage_ShadowMap16)
			texp.mFormat = eSurface_Depth16;
		else if (params.mUsage == eRenderTargetUsage_ShadowMap24)
			texp.mFormat = eSurface_Depth24;
		else if (params.mUsage == eRenderTargetUsage_Depth16)
			texp.mFormat = eSurface_Depth16;
		else if (params.mUsage == eRenderTargetUsage_Depth32F)
			texp.mFormat = eSurface_Depth32F;
		else if (params.mUsage == eRenderTargetUsage_DepthStencil)
			texp.mFormat = eSurface_DepthStencil32;
		else if (params.mUsage == eRenderTargetUsage_Depth)
			texp.mFormat = eSurface_Depth32F;
		else if (params.mUsage == eRenderTargetUsage_LinearDepth)
			texp.mFormat = eSurface_R32F;
		if (params.mUsage == eRenderTargetUsage_ShadowMap16 || params.mUsage == eRenderTargetUsage_ShadowMap24) {
			samplerState.InternalSetSamplerState(eSamplerState_MipBias_Value, 1);
			samplerState.InternalSetSamplerState(eSamplerState_WrapU_Value, 2);
			samplerState.InternalSetSamplerState(eSamplerState_WrapV_Value, 2);
		}
		else if (params.mUsage == eRenderTargetUsage_LinearDepth)
			samplerState.InternalSetSamplerState(eSamplerState_MipBias_Value, 0);
		texp.mTag = Symbol::kEmptySymbol;
		texp.mAccess = params.mAccess;
		texp.mAllocationType = params.mAllocationType;
		texp.mArraySize = params.mArraySize;
		texp.mbPrimaryDisplaySurface = texp.mbCPUAccessible = texp.mbVideoTexture = false;
		texp.mDepth = params.mDepth;
		texp.mLayout = params.mLayout;
		texp.mFastMemHeap = eGFXPlatformFastMemHeap_None;
		texp.mMultisample = params.mMultisample;
		texp.mHeight = params.mHeight;
		texp.mWidth = params.mWidth;
		texp.mUsage = params.mResourceUsage;
		texp.mNumMipLevels = params.mNumMipLevels;
		pTexture->mSamplerState = samplerState;
		return pTexture->CreateTexture(texp) > 0;
	}


	inline void ReleaseRenderTarget(std::shared_ptr<T3Texture> rt) {
		for (auto it = GetUsedRenderTargets().begin(); it != GetUsedRenderTargets().end(); it++) {
			if (it->mpTexture == rt) {
				GetFreeRenderTargets().push_back(std::move(*it));
				GetUsedRenderTargets().erase(it);
				return;
			}
		}
	}

	inline void ReleaseReference(T3RenderTargetList& list, T3RenderTargetID& id) {
		if (id.mValue < list.mEntryCount) {
			T3RenderTargetList::Entry& entry = list.mEntries[id.mValue];
			if (!entry.mbExternalTexture) {
				ReleaseRenderTarget(entry.mpTarget);
				entry.mbOutputTargetValid = false;
			}
			entry.mpTarget.reset();
		}
	}

	inline void ReleaseReferences(T3RenderTargetReferences& refs, T3RenderTargetList& list) {
		for (T3RenderTargetReferences::Entry* ref = refs.mReferences.head(); ref; ref = ref->next) {
			ReleaseReference(list, ref->mTargetID);
		}
	}

	inline void ResetList(T3RenderTargetList& list){
		for (int i = 0; i < list.mEntryCount; i++) {
			T3RenderTargetList::Entry& entry = list.mEntries[i];
			entry.mpTarget = entry.mpInitialExternalTexture;
			entry.mRefCount = entry.mInitialRefCount;
			entry.mbExternalTexture = entry.mbOutputTargetValid = entry.mpInitialExternalTexture.operator bool();
		}
	}

	inline void SwapTarget(T3RenderTargetList& list, T3RenderTargetID& dstTargetID, T3RenderTargetID& srcTargetID) {
		if(dstTargetID.mValue < list.mEntryCount && srcTargetID.mValue < list.mEntryCount){
			T3RenderTargetList::Entry& src = list.mEntries[srcTargetID.mValue], & dst = list.mEntries[dstTargetID.mValue];
			if (!dst.mbExternalTexture && dst.mpTarget)
				ReleaseRenderTarget(std::move(dst.mpTarget));
			if (dst.mbExternalTexture)
				--dst.mRefCount;
			dst.mbExternalTexture = false;
			dst.mbOutputTargetValid = true;
			dst.mpTarget = src.mpTarget;
			src.mpTarget.reset();
			src.mbOutputTargetValid = false;
		}
	}

	inline void ReleaseReference(T3RenderTargetList& list, T3RenderTargetIDSet& idSet) {
		for(int i = 0; i < 8; i++){
			ReleaseReference(list, idSet.mRenderTargetID[i].mTargetID);
		}
		ReleaseReference(list, idSet.mDepthTargetID.mTargetID);
	}

	inline void ReleaseList(T3RenderTargetList& list) {
		for (int i = 0; i < list.mEntryCount; i++) {
			T3RenderTargetList::Entry& entry = list.mEntries[i];
			if (entry.mpTargetRef){
				for (auto it = GetUsedRenderTargets().begin(); it != GetUsedRenderTargets().end(); it++) {
					if(it->mpTexture == entry.mpTargetRef->mpTarget->mpTexture){
						GetUsedRenderTargets().erase(it);
						break;
					}
				}
			}
			if (!entry.mbExternalTexture) {
				ReleaseRenderTarget(entry.mpTarget);
				entry.mbOutputTargetValid = false;
			}
			entry.mpTarget.reset();
		}
	}

	inline T3RenderTarget _CreateRenderTarget(T3RenderTargetParams params, String name){
		T3RenderTarget t{};
		t.mpTexture = std::make_shared<T3Texture>();
		if (InitializeExternalTarget(t.mpTexture, params, name)) {
			t.mLayout = params.mLayout;
			t.mMultisample = params.mMultisample;
			t.mUsage = params.mUsage;
			t.mpTexture->mName = std::move(name);
		}
		else t.mpTexture.reset();
		return t;
	}

	inline void _InitializeRenderTarget(T3RenderTargetUsage usage, u32 w, u32 h, u32 mipLevels) {
		T3RenderTargetParams params{};
		params.mAllocationType = eGFXPlatformAllocation_RenderTarget;
		params.mDepth = 1;
		params.mAccess = eSurface_ReadWrite;
		params.mMultisample = eSurfaceMultisample_None;
		params.mUsage = usage;
		params.mNumMipLevels = mipLevels;
		params.mWidth = w;
		params.mFastMemHeap = eGFXPlatformFastMemHeap_None;
		params.mHeight = h;
		T3RenderTarget target = _CreateRenderTarget(params, "");
		if (target.mpTexture)
			GetFreeRenderTargets().push_back(std::move(target));
	}

	inline T3RenderTarget GetRenderTarget(T3RenderTargetParams params, String name){
		for (auto it = GetFreeRenderTargets().begin(); it != GetFreeRenderTargets().end(); it++) {
			if (it->mMultisample == params.mMultisample && it->mUsage == params.mUsage
				&& it->mLayout == params.mLayout) {
				std::shared_ptr<T3Texture> tex = it->mpTexture;
				if (   tex->mWidth == params.mWidth
					&& tex->mHeight == params.mHeight
					&& tex->mDepth == params.mDepth
					&& tex->mArraySize == params.mArraySize
					&& tex->mNumMipLevels == params.mNumMipLevels
					&& tex->mResourceUsage == params.mResourceUsage
					&& tex->mSurfaceAccess == params.mAccess)
				{
					T3RenderTarget target = std::move(*it);
					GetFreeRenderTargets().erase(it);
					tex->mName = name;
					return target;
				}
			}
		}
		T3RenderTarget target = _CreateRenderTarget(params, std::move(name));
		GetUsedRenderTargets().push_back(target);
		return target;
	}

	inline void BeginFrame(u64 frameIndex){
		for (auto it = GetReattachRenderTargets().begin(); it != GetReattachRenderTargets().end(); it++)
			GetUsedRenderTargets().push_back(std::move(*it));
		GetReattachRenderTargets().clear();
		_FreeUnusedRenderTargets(900, frameIndex);
	}

	inline T3RenderTarget DetachRenderTarget(std::shared_ptr<T3Texture> texture){
		for(auto it = GetUsedRenderTargets().begin(); it != GetUsedRenderTargets().end(); it++){
			if (it->mpTexture == texture) {
				GetUsedRenderTargets().erase(it);
				return *it;
			}
		}
		return T3RenderTarget{};
	}

	inline void GetResolution(T3RenderTargetDesc& desc, int baseWidth, int baseHeight, float baseScale, int mipLevel, int& outWidth, int& outHeight){
		if (!desc.mbAllowRenderScale)
			baseScale = 1.0f;
		::T3::GetMipSize((int)floorf((float)baseWidth * desc.mScaleX * baseScale),
			(int)floorf((float)baseHeight * desc.mScaleY * baseScale), 0, &outWidth, &outHeight);
	}

	inline bool GetResolution(T3RenderTargetList& list, T3RenderTargetID& id, u32 mipLevel, int& outWidth, int& outHeight){
		if (id.mValue >= list.mEntryCount)
			return false;
		::T3::GetMipSize(list.mEntries[id.mValue].mParams.mWidth, list.mEntries[id.mValue].mParams.mHeight, mipLevel, &outWidth, &outHeight);
		return true;
	}

	inline bool GetResolution(T3RenderTargetContext* pContext, T3RenderTargetID& id, u32 mipLevel, int& outWidth, int& outHeight){
		if(id.IsTemporary()){
			for (auto it = pContext->mTemporaryList.head(); it; it = it->next) {
				if (it->mID.mValue == id.mValue) {
					::T3::GetMipSize(it->mParams.mWidth, it->mParams.mHeight, mipLevel, &outWidth, &outHeight);
					return true;
				}
			}
		}else{
			T3RenderTargetDesc& Desc = sTargetDesc[id.mValue];
			float Scale = Desc.mbAllowRenderScale ? sGlobalRenderConfig.mRenderScale : 1.0f;
			u32 gameWidth = 0, gameHeight = 0;
			TelltaleToolLib_GetRenderAdaptersStruct()->GetResolution(gameWidth, gameHeight);
			GetResolution(Desc, gameWidth, gameHeight, Scale, 0, outWidth, outHeight);
			return true;
		}
		return false;
	}

	inline void GetResolution(T3RenderTargetContext* pContext, T3RenderTargetIDSet& idSet, int& outWidth, int& outHeight) {
		int w = 0, h = 0;
		outWidth = outHeight = 0;
		for (int i = 0; i < 8; i++) {
			if (GetResolution(pContext, idSet.mRenderTargetID[i].mTargetID, idSet.mRenderTargetID[i].mMipLevel, w, h)) {
				outWidth = max(w, outWidth);
				outHeight = max(h, outHeight);
			}
		}
		if (GetResolution(pContext, idSet.mDepthTargetID.mTargetID, idSet.mDepthTargetID.mMipLevel, w, h)) {
			outWidth = max(w, outWidth);
			outHeight = max(h, outHeight);
		}
	}

	inline void GetResolution(T3RenderTargetList& list, T3RenderTargetIDSet& idSet, int& outWidth, int& outHeight){
		int w = 0, h = 0;
		outWidth = outHeight = 0;
		for(int i = 0; i < 8; i++){
			if(GetResolution(list, idSet.mRenderTargetID[i].mTargetID, idSet.mRenderTargetID[i].mMipLevel, w, h)){
				outWidth = max(w, outWidth);
				outHeight = max(h, outHeight);
			}
		}
		if (GetResolution(list, idSet.mDepthTargetID.mTargetID, idSet.mDepthTargetID.mMipLevel, w, h)) {
			outWidth = max(w, outWidth);
			outHeight = max(h, outHeight);
		}
	}

	inline bool GetParams(T3RenderTargetContext* pContext, T3RenderTargetParams& params, T3RenderTargetID id){
		if(id.IsTemporary()){
			for (auto it = pContext->mTemporaryList.head(); it; it = it->next) {
				if(it->mID.mValue == id.mValue){
					params = it->mParams;
					return true;
				}
			}
			return false;
		}
		T3RenderTargetDesc& Desc = sTargetDesc[(int)id.mValue];
		params.mUsage = Desc.mUsage;
		params.mAccess = Desc.mAccess;
		params.mMultisample = Desc.mMultisample;
		params.mLayout = eTextureLayout_2D;
		params.mArraySize = 1;
		params.mFastMemHeap = Desc.mFastMemHeap;
		params.mClear = Desc.mClear;
		GetResolution(pContext, id, 0, params.mWidth, params.mHeight);
		if(Desc.mNumMipLevels > 1){
			params.mNumMipLevels = (int)::T3::GetNumMipLevelsNeeded(eSurface_RGBA8, params.mWidth, params.mHeight);
			params.mNumMipLevels = Desc.mNumMipLevels;
			if (Desc.mNumMipLevels < params.mNumMipLevels)
				params.mNumMipLevels = Desc.mNumMipLevels;
		}
		return 1;
	}

	inline void InitializeList(T3RenderTargetList& list, LinearHeap& heap, T3RenderTargetContext* context){
		context->mNextID = eRenderTargetID_Count;//just in case
		list.mEntries = (T3RenderTargetList::Entry*)heap.Alloc(context->mNextID * sizeof(T3RenderTargetList::Entry), 8);
		int id = context->mNextID;
		do{
			T3RenderTargetList::Entry& entry = list.mEntries[id-1];
			entry.mParams.mResourceUsage = eResourceUsage_RenderTarget;
			entry.mParams.mFastMemHeap = eGFXPlatformFastMemHeap_None;
		} while (--id);
		for(u32 i = 0; i < eRenderTargetID_Count; i++){
			T3RenderTargetList::Entry& entry = list.mEntries[i];
			entry.mName = "untagged-fixme";
			if (sTargetDesc[i].mName)
				entry.mName = sTargetDesc[i].mName;
			entry.mInitialRefCount = context->mDefaultList[i].mRefCount;
			GetParams(context, entry.mParams, T3RenderTargetID{(T3RenderTargetConstantID)i});
			entry.mpInitialExternalTexture = context->mDefaultList[i].mpExternalTexture;
			if(i <= eRenderTargetID_BackBufferHDR || i == eRenderTargetID_BackBufferHDR_MSAA){
				entry.mParams.mClear.mClearColor = sGlobalRenderConfig.mClearColor;
				if (i == eRenderTargetID_BackBufferHDR || i == eRenderTargetID_BackBufferHDR_MSAA)
					entry.mParams.mClear.mClearColor /= sGlobalRenderConfig.mHDRColorBufferScale;
;			}
		}
		for (T3RenderTargetContext::TemporaryEntry* it = context->mTemporaryList.head(); it; it = it->next) {
			T3RenderTargetContext::TemporaryEntry& tmp = *it;
			T3RenderTargetList::Entry& entry = list.mEntries[(u32)tmp.mID.mValue];
			entry.mParams = tmp.mParams;
			entry.mInitialRefCount = tmp.mRefCount;
			entry.mpTargetRef = tmp.mpTargetRef;
			entry.mbTemporary = true;
			entry.mpInitialExternalTexture = tmp.mpExternalTexture;
		}
		context->mTemporaryList.clear();
	}

	inline bool HasReferences(T3RenderTargetContext* context, T3RenderTargetID& id)
	{
		if (id.mValue < eRenderTargetID_Count && id.mValue >= 0)
			return context->mDefaultList[(int)id.mValue].mRefCount > 0;
		for (auto it = context->mTemporaryList.head(); it; it=it->next)
			if (it->mID.mValue == id.mValue)
				return it->mRefCount > 0;
		return false;
	}

	inline T3RenderTargetList::Entry* GetOutputTargetEntry(T3RenderTargetList& list, T3RenderTargetID& id, u64 gfxFrame) {
		if (id.mValue >= list.mEntryCount)
			return 0;
		T3RenderTargetList::Entry* pEntry = &list.mEntries[(int)id.mValue];
		std::shared_ptr<T3Texture> target{};
		if (pEntry->mbOutputTargetValid)
			target = pEntry->mpTarget;
		else if (pEntry->mbExternalTexture) {
			pEntry->mRefCount--;
			pEntry->mbExternalTexture = false;
			pEntry->mpTarget.reset();
		}
		else{
			if (pEntry->mpTarget)
				ReleaseRenderTarget(std::move(pEntry->mpTarget));
		}
		if (pEntry->mbExternalTexture || target) {
			if (target)
				target->mLastUsedFrame = gfxFrame;
			return pEntry;
		}
		T3RenderTarget t = GetRenderTarget(pEntry->mParams, pEntry->mName);
		pEntry->mpTarget = t.mpTexture;
		pEntry->mbOutputTargetValid = pEntry->mpTarget.operator bool();
		pEntry->mpTarget->mLastUsedFrame = gfxFrame;
		return pEntry;
	}

	inline std::shared_ptr<T3Texture> GetInputTarget(T3RenderTargetList& list, T3RenderTargetID id){
		if (id.mValue >= list.mEntryCount)
			return 0;
		return list.mEntries[id.mValue].mpTarget;
	}

	inline std::shared_ptr<T3Texture> GetOutputTarget(T3RenderTargetList& list, T3RenderTargetID id, u64 frameIndex) {
		return GetOutputTargetEntry(list, id, frameIndex)->mpTarget;
	}

	inline void GetOutputTargetSet(T3RenderTargetList& list, T3RenderTargetSet& outputTarget, T3RenderTargetClear& clear, T3RenderTargetIDSet& inputSet, u64 frame){
		for(int i = 0; i < 8; i++){
			T3RenderTargetList::Entry* pEntry = GetOutputTargetEntry(list, inputSet.mRenderTargetID[i].mTargetID, frame);
			if(pEntry){
				outputTarget.mRenderTarget[i].mpTexture = pEntry->mpTarget;
				outputTarget.mRenderTarget[i].mSliceIndex = inputSet.mRenderTargetID[i].mSliceIndex;
				outputTarget.mRenderTarget[i].mMipLevel = inputSet.mRenderTargetID[i].mMipLevel;
				if(pEntry->mpTarget){
					int w = 0, h = 0;
					::T3::GetMipSize(pEntry->mpTarget->mWidth, pEntry->mpTarget->mHeight, inputSet.mRenderTargetID[i].mMipLevel, &w, &h);
					outputTarget.mWidth = max(outputTarget.mWidth, w);
					outputTarget.mHeight = max(outputTarget.mHeight, h);
				}
				if (i == 0)
					clear.mClearColor = pEntry->mParams.mClear.mClearColor;
			}
		}
		T3RenderTargetList::Entry* pEntry = GetOutputTargetEntry(list, inputSet.mDepthTargetID.mTargetID, frame);
		if (pEntry) {
			outputTarget.mDepthTarget.mpTexture = pEntry->mpTarget;
			outputTarget.mDepthTarget.mSliceIndex = inputSet.mDepthTargetID.mSliceIndex;
			outputTarget.mDepthTarget.mMipLevel =   inputSet.mDepthTargetID.mMipLevel;
			if (pEntry->mpTarget) {
				int w = 0, h = 0;
				::T3::GetMipSize(pEntry->mpTarget->mWidth, pEntry->mpTarget->mHeight, inputSet.mDepthTargetID.mMipLevel, &w, &h);
				outputTarget.mWidth = max(outputTarget.mWidth, w);
				outputTarget.mHeight = max(outputTarget.mHeight, h);
			}
			clear.mClearStencil = pEntry->mParams.mClear.mClearStencil;
			clear.mClearDepth = pEntry->mParams.mClear.mClearDepth;
		}
	}

	inline void AddReference(T3RenderTargetContext& context, T3RenderTargetID& id){
		if (id.mValue != eRenderTargetID_Unknown){
			if(id.mValue >= eRenderTargetID_Count){
				for (auto it = context.mTemporaryList.head(); it; it = it->next) {
					if (it->mID.mValue == id.mValue)
						it->mRefCount++;
					return;
				}
			}else{
				context.mDefaultList[(int)id.mValue].mRefCount++;
			}
		}
	}

	inline void AddReference(T3RenderTargetContext& context, T3RenderTargetIDSet& idSet){
		AddReference(context, idSet.mDepthTargetID.mTargetID);
		for (int i = 0; i < 8; i++)
			AddReference(context, idSet.mRenderTargetID[i].mTargetID);
	}

	inline void AddReference(T3RenderTargetReferences& refs, T3RenderTargetContext& context, T3RenderTargetID& id){
		T3RenderTargetReferences::Entry* entry = (T3RenderTargetReferences::Entry*)context.mHeap->NewNoDestruct<T3RenderTargetReferences::Entry>();
		entry->mTargetID = id;
		AddReference(context, id);
	}

	inline T3RenderTargetID AllocateTemporaryTarget(T3RenderTargetContext* context, T3RenderTargetParams& params){
		T3RenderTargetContext::TemporaryEntry& tmp = *((T3RenderTargetContext::TemporaryEntry*)context->mHeap->NewNoDestruct<T3RenderTargetContext::TemporaryEntry>());
		context->mTemporaryList.insert_tail(&tmp);
		tmp.mParams = params;
		tmp.mID.mValue = (T3RenderTargetConstantID)context->mNextID++;
		if (!tmp.mParams.mNumMipLevels)
			tmp.mParams.mNumMipLevels = (int)::T3::GetNumMipLevelsNeeded(eSurface_RGBA8, params.mWidth, params.mHeight);
		return tmp.mID;
	}

	inline T3RenderTargetID AllocateTemporaryTarget(T3RenderTargetContext* context, int width, int height, T3RenderTargetUsage usage, GFXPlatformFastMemHeap fastMemHeap)
	{
		T3RenderTargetParams params{};

		params.mLayout = eTextureLayout_2D;
		params.mWidth = width;
		params.mMultisample = eSurfaceMultisample_None;
		params.mClear.mClearStencil = 0;
		params.mClear.mClearColor = Color::Black;
		params.mUsage = usage;
		params.mFastMemHeap = fastMemHeap;
		params.mClear.mClearDepth = 1.0f;
		params.mAllocationType = eGFXPlatformAllocation_RenderTarget;
		params.mDepth = 1;
		params.mArraySize = 1;
		params.mNumMipLevels = 1;
		params.mAccess = eSurface_ReadWrite;
		params.mResourceUsage = eResourceUsage_RenderTarget;
		params.mHeight = height;
		return AllocateTemporaryTarget(std::move(context), params);
	}

	inline T3RenderTargetID AllocateTemporaryTarget(T3RenderTargetContext* context, int width, int height, T3RenderTargetUsage usage, T3SurfaceAccess access)
	{
		T3RenderTargetParams params{};

		params.mLayout = eTextureLayout_2D;
		params.mWidth = width;
		params.mMultisample = eSurfaceMultisample_None;
		params.mClear.mClearStencil = 0;
		params.mClear.mClearColor = Color::Black;
		params.mUsage = usage;
		params.mFastMemHeap = eGFXPlatformFastMemHeap_None;
		params.mClear.mClearDepth = 1.0f;
		params.mAllocationType = eGFXPlatformAllocation_RenderTarget;
		params.mDepth = 1;
		params.mArraySize = 1;
		params.mNumMipLevels = 1;
		params.mAccess = access;
		params.mResourceUsage = eResourceUsage_RenderTarget;
		params.mHeight = height;
		return AllocateTemporaryTarget(std::move(context), params);
	}

	inline void GetTargetClear(T3RenderTargetContext* pContext, T3RenderTargetClear& clear, T3RenderTargetIDSet& idSet){
		for(int i = 0; i < 8; i++){
			T3RenderTargetParams params{};
			if (GetParams(pContext, params, idSet.mRenderTargetID[i].mTargetID))
				clear.mClearColor = params.mClear.mClearColor;
		}
		T3RenderTargetParams params{};
		if (GetParams(pContext, params, idSet.mDepthTargetID.mTargetID)) {
			clear.mClearDepth = params.mClear.mClearDepth;
			clear.mClearStencil = params.mClear.mClearStencil;
		}
	}

	inline T3SurfaceMultisample GetTargetMultisample(T3RenderTargetContext* pContext, T3RenderTargetIDSet& idSet) {
		T3SurfaceMultisample ms = eSurfaceMultisample_None;
		for (int i = 0; i < 8; i++) {
			T3RenderTargetParams params{};
			if (GetParams(pContext, params, idSet.mRenderTargetID[i].mTargetID))
				ms = params.mMultisample;
		}
		T3RenderTargetParams params{};
		if (GetParams(pContext, params, idSet.mDepthTargetID.mTargetID))
			ms = params.mMultisample;
		return ms;
	}

	inline T3RenderTargetID RegisterTarget(T3RenderTargetContext* pContext, T3RenderTargetResult& ref, T3RenderTargetParams& params){
		T3RenderTargetID id = AllocateTemporaryTarget(pContext, params);
		pContext->mTemporaryList.tail()->mpTargetRef = &ref;
		pContext->mTemporaryList.tail()->mRefCount = 1;
		return id;
	}

	// Next two impl in T3Texture.cpp

	T3RenderTargetID RegisterTarget(T3RenderTargetContext* pContext, RenderFrameUpdateList& updateList, std::shared_ptr<T3Texture> pTexture, T3RenderTargetParams& params);

	T3RenderTargetID RegisterTarget(T3RenderTargetContext* pContext, RenderFrameUpdateList& updateList, T3RenderTargetConstantID id, std::shared_ptr<T3Texture> pTexture);

}

inline void T3RenderTargetResult::Clear() {
	T3RenderTargetUtil::GetReattachRenderTargets().push_back(std::move(*mpTarget));
	mpTarget.reset();
}