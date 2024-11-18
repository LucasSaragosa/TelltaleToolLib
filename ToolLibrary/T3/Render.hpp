#pragma once

#include "LinearHeap.h"
#include "T3Effect.h"
#include "../Types/Scene.h"
#include "Camera.h"
#include "T3EffectParameters.h"
#include "LinearHeap.h"
#include "../LinkedList.h"

enum RenderIndexType
{
	eIndexType_None = 0x0,
	eIndexType_Quads = 0x1,
	eIndexType_IndexBuffer = 0x2,
};

enum RenderInstanceType
{
	eInstanceType_None = 0x0,
	eInstanceType_Corners = 0x1,
};

enum RenderExecuteResult
{
	eRenderExecuteResult_Error = 0x0,
	eRenderExecuteResult_SuccessFinished = 0x1,
	eRenderExecuteResult_SuccessRepeat = 0x2,
};

struct DynamicDrawData : ListNode<DynamicDrawData>
{
	std::shared_ptr<T3GFXBuffer> mpVertexBuffer;
	std::shared_ptr<T3GFXBuffer> mpIndexBuffer;
	std::shared_ptr<T3GFXVertexState> mpVertexState;
	RenderIndexType mIndexType;
	RenderInstanceType mInstanceType;
	u32 mFormatHash;
	u64 mFrameUsed;
	u32 mVertexCount;
	u32 mIndexCount;
	bool mbGPUWritable;
};

struct T3GFXDynamicResource : ListNode<T3GFXDynamicResource>
{
	T3GFXDynamicResourceType mType;
	u32 mHash;
};

struct T3GFXDynamicResourceContext
{
	LinkedList<T3GFXDynamicResource> mUsedResources;
};

struct T3GFXUniformBufferUpdateContext
{
	LinkedList<T3EffectParameterBuffer> mBuffers;
};

struct T3GFXSurface
{
	void* mpData;
	T3TextureLayout mLayout;
	T3SurfaceFormat mFormat;
	u32 mWidthInBlocks;
	u32 mHeightInBlocks;
	u32 mDepth;
	u32 mArraySize;
	u32 mPitch;
	u32 mSlicePitch;
};

struct RenderFrameUpdateList {

	struct ResourceEntry_NonBlockingTextureLoad
	{
		std::shared_ptr<T3Texture> mpTexture;
		float mMipLevel;
	};

	static std::vector<ResourceEntry_NonBlockingTextureLoad> smExtraNonBlockingLoads;

	struct BufferData
	{
		void* mpData;
		unsigned int mSize;
		bool mbAsyncHeap;
	};

	struct ResourceEntry_ParameterBuffer : ListNode<ResourceEntry_ParameterBuffer>
	{

		std::shared_ptr<T3EffectParameterBuffer> mpBuffer;
		RenderFrameUpdateList::BufferData mData;
		unsigned int mSize;
		unsigned int mOffset;
		T3EffectParameterType mType;
	};

	struct ResourceEntry_MeshBuffer : ListNode<ResourceEntry_MeshBuffer>
	{
		std::shared_ptr<T3GFXBuffer> mpBuffer;
		//if not null, this is a realloc
		std::shared_ptr<T3GFXVertexState> mpDirtyVertexState;
		RenderFrameUpdateList::BufferData mData;
		unsigned int mCount;
		bool mHasDirectMap;
	};

	struct ResourceEntry_Texture : ListNode<ResourceEntry_Texture>
	{
		std::shared_ptr<T3Texture> mpTexture;
		TextureStreamingPriority mPriority;
		float mMipLevel;
	};

	struct ResourceEntry_UpdateTexture : ListNode<ResourceEntry_UpdateTexture>
	{
		std::shared_ptr<T3Texture> mpTexture;
		RenderFrameUpdateList::BufferData mData;
		T3GFXSurface mSurface;
		T3ResourceUsage mUsage;
		unsigned int mWidth;
		unsigned int mHeight;
		unsigned int mFace;
		bool mCPUAccessible;
	};

	struct ResourceEntry_ToonTexture : ListNode<ResourceEntry_ToonTexture>
	{
		std::shared_ptr<T3Texture> mpTexture;
		Color mStart;
		Color mEnd;
		int mCount;
	};

	struct ResourceEntry_GenericFunc : ListNode<ResourceEntry_GenericFunc>
	{
		RenderExecuteResult (*mpFunction)(u32, void*);
		void* mpBaton;
	};

	struct ResourceEntry_CopyBuffer : ListNode<ResourceEntry_CopyBuffer>
	{
		std::shared_ptr<T3GFXBuffer> mpDstBuffer;
		std::shared_ptr<T3GFXBuffer> mpSrcBuffer;
	};

	std::vector<void*> mTempFrameBuffers;

	inline void _ClearTempBuffers(){
		for (auto& it : mTempFrameBuffers)
			free(it);
		mTempFrameBuffers.clear();
	}

	inline ~RenderFrameUpdateList(){
		_ClearTempBuffers();
	}

	LinearHeap* mHeap;
	u64 mFrameIndex;
	T3GFXDynamicResourceContext/*&*/ mDynamicResourceContext;
	LinkedList<ResourceEntry_ParameterBuffer> mParameterBufferList;
	LinkedList<ResourceEntry_MeshBuffer> mMeshBufferList;
	LinkedList<ResourceEntry_Texture> mTextureList;
	LinkedList<ResourceEntry_UpdateTexture> mTextureUpdateList;
	LinkedList<ResourceEntry_ToonTexture> mToonTextureList;
	//std::vector<ResourceEntry_JobHandleWait> mJobHandleWaitList;
	//std::vector<ResourceEntry_JobWait> mJobWaitList;
	//std::vector<ResourceEntry_JobWait> mJobFailedList;
	LinkedList<ResourceEntry_GenericFunc> mGenericFunctionList;
	LinkedList<ResourceEntry_CopyBuffer> mCopyBufferList;
	T3EffectParameterBuffer* mpCurrentStreamBuffer;
	void* mpCurrentStreamBufferData;
	u32 mCurrentStreamBufferScalarSize;
	u32 mCurrentStreamBufferScalarOffset;
	T3GFXUniformBufferUpdateContext mUniformBufferUpdateContext;
	u32 mMaxLinearHeapSize;
	u32 mTotalAllocated;
	u32 mTotalBufferUpdateAllocated;
	u32 mTotalVertexUpdateAllocated;
	u32 mBufferAllocated[eEffectParameter_UniformBufferCount];

	inline RenderFrameUpdateList(LinearHeap* heap, u64 frame) : mHeap(heap), mFrameIndex(frame) {}

	inline RenderFrameUpdateList() : RenderFrameUpdateList(0,0) {}

	//Execute the given function with the given baton argument when next frame BeginFrame is called
	inline void Execute(RenderExecuteResult(*pFunc)(unsigned int, void*), void* pBaton){
		ResourceEntry_GenericFunc& job = *mHeap->NewNoDestruct<ResourceEntry_GenericFunc>();
		mGenericFunctionList.insert_tail(&job);
		job.mpBaton = pBaton;
		job.mpFunction = pFunc;
	}

	inline void ExecuteFrameCommands(){
		while(!mCopyBufferList.empty()){
			ResourceEntry_CopyBuffer* job = mCopyBufferList.remove_tail();
			T3::CopyBuffer(*job->mpSrcBuffer, *job->mpDstBuffer);
		}
	}

	inline void InitializeToonTexture(std::shared_ptr<T3Texture> pBuffer, const Color& start, const Color& end, int count){
		ResourceEntry_ToonTexture& job = *mHeap->NewNoDestruct<ResourceEntry_ToonTexture>();
		mToonTextureList.insert_tail(&job);
		job.mpTexture = std::move(pBuffer);
		job.mCount = count;
		job.mStart = start;
		job.mEnd = end;
	}

	//Put the data into the returned buffer and copy the new data for it. 
	inline void* UpdateBuffer(std::shared_ptr < T3GFXBuffer> pBuffer, unsigned int count){
		ResourceEntry_MeshBuffer& job = *mHeap->NewNoDestruct<ResourceEntry_MeshBuffer>();
		mMeshBufferList.insert_tail(&job);
		job.mpBuffer = pBuffer;
		job.mCount = count;
		job.mData.mbAsyncHeap = false;
		job.mData.mSize = count * pBuffer->mStride;
		job.mData.mpData = calloc(1, job.mData.mSize);
		mTempFrameBuffers.push_back(job.mData.mpData);
		return job.mData.mpData;
	}

	//return temp buffer to put data into
	inline void* UpdateMeshBuffer(std::shared_ptr<T3GFXVertexState> vertexState, std::shared_ptr<T3GFXBuffer> pBuffer, unsigned int count){
		ResourceEntry_MeshBuffer& job = *mHeap->NewNoDestruct<ResourceEntry_MeshBuffer>();
		mMeshBufferList.insert_tail(&job);
		job.mCount = count;
		job.mpBuffer = pBuffer;
		job.mData.mbAsyncHeap = false;
		if (T3::IsGFXBufferReallocation(pBuffer->mResourceUsage, pBuffer->mCount, count)) {
			job.mpDirtyVertexState = vertexState;
		}else if(pBuffer->mCount > count){
			count = pBuffer->mCount;
		}
		pBuffer->mCount = count;
		job.mData.mbAsyncHeap = false;
		job.mData.mpData = calloc(1, job.mData.mSize);
		mTempFrameBuffers.push_back(job.mData.mpData);
		return job.mData.mpData;
	}

	//return temp buffer to put the data into
	inline u8* UpdateParameterBuffer(std::shared_ptr<T3EffectParameterBuffer> pBuffer, u32 offset, u32 size, T3EffectParameterType bufferType){
		if (bufferType >= eEffectParameter_UniformBufferCount)
			return 0;
		if (!size)
			size = sBufferDesc[bufferType].mScalarSize << 2;
		ResourceEntry_ParameterBuffer& job = *mHeap->NewNoDestruct<ResourceEntry_ParameterBuffer>();
		mParameterBufferList.insert_tail(&job);
		job.mpBuffer = pBuffer;
		job.mOffset = offset;
		job.mSize = size;
		job.mType = bufferType;
		if (bufferType != eEffectParameter_Unknown)
			mBufferAllocated[bufferType] += size;
		job.mData.mbAsyncHeap = false;
		job.mData.mSize = job.mSize;
		job.mData.mpData = calloc(1, job.mData.mSize);
		mTempFrameBuffers.push_back(job.mData.mpData);
		return (u8*)job.mData.mpData;
	}

	inline void UpdateTexture2D(void* pData, int dataSize, std::shared_ptr<T3Texture> pTexture, unsigned int width, unsigned int height, T3SurfaceFormat format, T3ResourceUsage usage, bool cpuAccessible){
		ResourceEntry_UpdateTexture* job = 0;
		for (ResourceEntry_UpdateTexture* it = mTextureUpdateList.head(); it; it = it->next) {
			if (it->mpTexture == pTexture) {
				job = it;
				break;
			}
		}
		if (!job)
			job = mHeap->NewNoDestruct<ResourceEntry_UpdateTexture>();
		job->mSurface.mFormat = format;
		job->mSurface.mLayout = eTextureLayout_2D;
		T3SurfaceFormatDesc result = T3::T3SurfaceFormat_GetDesc(format);
		u32 v16 = (result.mBlockWidth + width - 1) / result.mBlockWidth;
		job->mSurface.mWidthInBlocks = v16;
		u32 v17 = v16;
		u32 v18 = (result.mBlockHeight + height - 1) / result.mBlockHeight;
		job->mSurface.mHeightInBlocks = v18;
		u32 v19 = result.mBitsPerBlock * v17;
		job->mData.mSize = dataSize;
		job->mData.mbAsyncHeap = 0;
		job->mData.mpData = pData;
		u32 v20 = (unsigned int)(v19 + 7) >> 3;
		job->mSurface.mPitch = v20;
		job->mSurface.mSlicePitch = v18 * v20;
		job->mpTexture = pTexture;
		job->mUsage = usage;
		job->mCPUAccessible = cpuAccessible;
		job->mWidth = width;
		job->mHeight = height;
	}

	inline void* UpdateTexture2DArray(T3GFXSurface& surface, std::shared_ptr<T3Texture> pTexture, unsigned int width, unsigned int height, unsigned int arraySize, T3SurfaceFormat format, T3ResourceUsage usage, bool cpuAccessible) {
		T3SurfaceFormatDesc desc = T3::T3SurfaceFormat_GetDesc(format);
		ResourceEntry_UpdateTexture& job = *mHeap->NewNoDestruct<ResourceEntry_UpdateTexture>();
		mTextureUpdateList.insert_tail(&job);
		job.mCPUAccessible = cpuAccessible;
		job.mSurface.mFormat = format;
		job.mSurface.mDepth = 1;
		job.mSurface.mLayout = eTextureLayout_2DArray;
		job.mSurface.mWidthInBlocks = (width + desc.mBlockWidth - 1) / desc.mBlockWidth;
		u32 tmp = (desc.mBitsPerBlock * job.mSurface.mWidthInBlocks + 7) >> 8;
		job.mSurface.mPitch = tmp;//to bytes per block
		job.mSurface.mHeightInBlocks = (height + desc.mBlockHeight - 1) / desc.mBlockHeight;
		job.mSurface.mSlicePitch = tmp * job.mSurface.mPitch;
		job.mSurface.mArraySize = arraySize;
		job.mData.mSize = job.mSurface.mSlicePitch * arraySize;
		job.mData.mpData = calloc(1, job.mData.mSize);
		job.mpTexture = pTexture;
		job.mWidth = width;
		job.mHeight = height;
		job.mUsage = usage;
		surface = job.mSurface;//copy
		mTempFrameBuffers.push_back(job.mData.mpData);
		return job.mData.mpData;
	}

	inline void* UpdateTexture3D(T3GFXSurface& surface, std::shared_ptr<T3Texture> pTexture, unsigned int width, unsigned int height, unsigned int depth, T3SurfaceFormat format, T3ResourceUsage usage, bool cpuAccessible) {
		T3SurfaceFormatDesc desc = T3::T3SurfaceFormat_GetDesc(format);
		ResourceEntry_UpdateTexture& job = *mHeap->NewNoDestruct<ResourceEntry_UpdateTexture>();
		mTextureUpdateList.insert_tail(&job);
		job.mCPUAccessible = cpuAccessible;
		job.mSurface.mFormat = format;
		job.mSurface.mDepth = depth;;
		job.mSurface.mLayout = eTextureLayout_3D;
		job.mSurface.mWidthInBlocks = (width + desc.mBlockWidth - 1) / desc.mBlockWidth;
		u32 tmp = (desc.mBitsPerBlock * job.mSurface.mWidthInBlocks + 7) >> 8;
		job.mSurface.mPitch = tmp;//to bytes per block
		job.mSurface.mHeightInBlocks = (height + desc.mBlockHeight - 1) / desc.mBlockHeight;
		job.mSurface.mSlicePitch = tmp * job.mSurface.mPitch;
		job.mData.mSize = job.mSurface.mSlicePitch * depth;
		job.mData.mpData = calloc(1, job.mData.mSize);
		job.mpTexture = pTexture;
		job.mWidth = width;
		job.mHeight = height;
		job.mUsage = usage;
		surface = job.mSurface;//copy
		mTempFrameBuffers.push_back(job.mData.mpData);
		return job.mData.mpData;
	}

	inline void CancelTextureStreaming(std::shared_ptr<T3Texture> pTexture){
		for (ResourceEntry_Texture* it = mTextureList.head(); it; it = it->next) {
			if(it->mpTexture == pTexture){
				mTextureList.remove(it);
				break;
			}
		}
		_RemoveFromNonBlockingLoadList(pTexture.get());
	}

	inline void* UpdateTexture2D(T3GFXSurface& surface, std::shared_ptr<T3Texture> pTexture, unsigned int width, unsigned int height, T3SurfaceFormat format, T3ResourceUsage usage, bool cpuAccessible) {
		T3SurfaceFormatDesc desc = T3::T3SurfaceFormat_GetDesc(format);
		ResourceEntry_UpdateTexture& job = *mHeap->NewNoDestruct<ResourceEntry_UpdateTexture>();
		mTextureUpdateList.insert_tail(&job);
		job.mCPUAccessible = cpuAccessible;
		job.mSurface.mFormat = format;
		job.mSurface.mDepth = 1;
		job.mSurface.mLayout = eTextureLayout_2D;
		job.mSurface.mWidthInBlocks = (width + desc.mBlockWidth - 1) / desc.mBlockWidth;
		u32 tmp = (desc.mBitsPerBlock * job.mSurface.mWidthInBlocks + 7) >> 8;
		job.mSurface.mPitch = tmp;//to bytes per block
		job.mSurface.mHeightInBlocks = (height + desc.mBlockHeight - 1) / desc.mBlockHeight;
		job.mSurface.mSlicePitch = tmp * job.mSurface.mPitch;
		job.mSurface.mArraySize = 0;
		job.mData.mSize = job.mSurface.mSlicePitch;
		job.mData.mpData = calloc(1, job.mData.mSize);
		job.mpTexture = pTexture;
		job.mWidth = width;
		job.mHeight = height;
		job.mUsage = usage;
		surface = job.mSurface;//copy
		mTempFrameBuffers.push_back(job.mData.mpData);
		return job.mData.mpData;
	}

	inline bool UpdateTextureStreaming(std::shared_ptr<T3Texture> pTexture, TextureStreamingPriority priority, float mipLevel){
		if (pTexture->IsFullyLoaded() || pTexture->IsEmpty())
			return false;
		pTexture->ModifyStreamingRefCount(1);
		if (priority != eTextureStreaming_Requested) {
			ResourceEntry_Texture& job = *mHeap->NewNoDestruct<ResourceEntry_Texture>();
			mTextureList.insert_tail(&job);
			job.mpTexture = pTexture;
			job.mPriority = priority;
			job.mMipLevel = mipLevel;
		}else{
			//priority is requested, so non blocking
			ResourceEntry_NonBlockingTextureLoad& load = smExtraNonBlockingLoads.emplace_back();
			load.mMipLevel = mipLevel;
			load.mpTexture = std::move(pTexture);
		}
	}

	inline void BeginFrame(){

	}

	inline void EndFrame(){

	}

	static inline void OnTextureDestroyed(T3Texture* pTexture)
	{
		_RemoveFromNonBlockingLoadList(pTexture);
	}

	inline static void _RemoveFromNonBlockingLoadList(T3Texture* pTexture){
		while(pTexture->mLoadPendingRefCount > 0){
			int x = 0;
			for(auto& it : smExtraNonBlockingLoads){
				if(it.mpTexture.get() == pTexture){
					pTexture->ModifyLoadPendingRefCount(-1);
					auto i = smExtraNonBlockingLoads.begin();
					std::advance(i, x);
					smExtraNonBlockingLoads.erase(i);
					break;
				}
				x++;
			}
		}
	}

	static inline void Shutdown(){
		smExtraNonBlockingLoads.clear();
	}

};

struct RenderSceneParams
{
	Scene* mpScene;
	RenderQualityType mRenderQuality;
	RenderDebugMode mDebugRenderMode;
	u32 mShadowMapAtlasResolution;
	u32 mShadowCascadeCount;
	bool mbAfterEffects;
	bool mbReflectionEnvironment;
	bool mbCameraCut;
};

struct GFXPlatformClipPlanes
{
	Vector4 mPlanes[4];
	u32 mCount;
};

struct T3EffectDrawParams
{
	RenderFrameStats* mpFrameStats;
	T3RenderTargetList* mpRenderTargetList;
	T3EffectParameterBoundState* mpParameterBoundState;
	GFXPlatformClipPlanes* mpGFXClipPlanes;
	GFXPlatformScissorRect mScissorRect;
	T3SurfaceFormat mRenderTargetFormat[8];
	T3SurfaceFormat mDepthTargetFormat;
	u32 mTargetWidth;
	u32 mTargetHeight;
};

struct RenderViewParams
{

	Camera* mpCamera;
	//T3OcclusionView* mpOcclusionView;
	T3RenderRelativeViewport mViewport;
	int mViewportGutter;
	RenderViewType mViewType;
	T3EffectParameterGroupStack mParameterStack;
	EffectFeatures_WDC mEffectFeatures;
	RenderClipPlanes mClipPlanes;
	Vector2 mJitter;
	Vector2 mPrevJitter;
	u32 mLightEnvGroupMask;
	u32 mCascadeCount;
	u32 mVisibilityBitCount;
	bool mbZClipEnabled;
	bool mbEnabled;
	struct
	{
		float mLeft;
		float mTop;
		float mRight;
		float mBottom;
	} mViewportScissorRect;
};

struct RenderSceneView;

struct RenderViewPassBase : ListNode<RenderViewPassBase>
{
	RenderSceneView* mpRenderView;
	RenderViewPassType mPassType;
};

struct RenderSubViewParams
{
	bool mbPushRenderTarget;
	bool mbPopRenderTarget;
	bool mbResetColorTarget;
};

struct RenderViewPass_SubView : RenderViewPassBase
{
	RenderSubViewParams mParams;
	RenderSceneView* mpSubView;
};

struct RenderPassParams
{
	T3RenderRelativeViewport mViewportOverride;
	BitSet<enum T3RenderPass, 22, 0> mRequiredPassesAll;
	BitSet<enum T3RenderPass, 22, 0> mRequiredPassesAny;
	T3RenderPass mPass;
	T3RenderPassType mPassType;
	T3RenderTargetIDSet mRenderTarget;
	T3RenderClear mForceRenderClear;
	T3RenderHiStencilMode mHiStencilMode;
	T3EffectParameterGroupStack mParameterStack;
	RenderAntialiasType mAntialiasType;
	i32 mOverrideTargetWidth;
	i32 mOverrideTargetHeight;
	float mDepthBoundsMin;
	float mDepthBoundsMax;
	bool mbDefaultRenderTarget;
	bool mbResolveDepthTarget;
	bool mbRequiredClearTarget;
	bool mbOverrideClearValues;
	bool mbOverrideViewport;
	bool mbEnabled;
};

struct T3RenderInst : ListNode<T3RenderInst>
{
	/*Used to sort for render layers. BIT FORMAT:
	Bits 0-25: ?? (0)
	Bits 26-36: sub layer (0-1023)
	Bits 37-45: ?? (1)
	Bits 46-62: layer + 0x8000 (layer is signed short range)
	Bits 63-64: transparent mode (MSB, meaning most significant in determining order)
	*/
	u64 mSortKey;
	GFXPlatformScissorRect mScissorRect;
	T3EffectParameterGroupStack mParameterStack;
	T3EffectParameterGroupStack* mpBaseParameters;
	std::shared_ptr<T3GFXVertexState> mpVertexState;
	std::shared_ptr<T3GFXBuffer> mpIndirectArgsBuffer;
	T3EffectCacheVariantRef mEffectRef;
	T3RenderStateBlock mStateBlock;
	u32 mObjectIndex;
	u32 mIndirectArgsByteOffset;
	u32 mBaseIndex;
	u32 mStartIndex;
	u32 mIndexCount;
	u16 mInstanceCount;
	u16 mMinIndex;
	u16 mMaxIndex;
	u8 mIndexBufferIndex;
	u8 mEffectPass;
	u8 mPrimitive;

	/**
	 * Sets the render layer. This determines the draw order.
	 */
	inline void SetRenderLayer(T3TransparencyMode mode, int layer, int sublayer){
		layer = min(0x7FFF, max(layer, -0x8000));
		sublayer = min(0x3FF, max(sublayer, 0));
		mSortKey = ((u64)mode << 62) | ((u64)sublayer << 26) | ((u64)(layer + 0x8000) << 46) | 0x3FF000000000llu;
	}

	/**
	 * Gets the current render layer of this draw instance.
	 */
	inline int GetRenderLayer() {
		return (mSortKey >> 46) - 0x8000;
	}

	/**
	 * Sets the range of indices to draw.
	 */
	inline void SetIndexRange(u32 minIndex, u32 maxIndex){
		mMinIndex = minIndex;
		mMaxIndex = maxIndex;
	}

	inline void DrawIndirect(GFXPlatformPrimitiveType type, std::shared_ptr<T3GFXBuffer> pIndirectArgsBuffer, u32 indirectArgsByteOffset){
		mBaseIndex = mStartIndex = mIndexCount = mInstanceCount = 0;
		mPrimitive = type;
		mpIndirectArgsBuffer = std::move(pIndirectArgsBuffer);
		mIndirectArgsByteOffset = indirectArgsByteOffset;
	}

	inline void DrawPrimitive(GFXPlatformPrimitiveType type, u32 start_index, u32 prim_count, u32 instance_count){
		mStartIndex = start_index;
		mPrimitive = type;
		mInstanceCount = instance_count;
		mIndexCount = ::T3::GFXPlatformPrimitiveType_GetNumVerts(type, prim_count);
	}

	inline void DrawVertices(GFXPlatformPrimitiveType type, u32 start_index, u32 vertex_count, u32 instance_count){
		mInstanceCount = instance_count;
		mPrimitive = type;
		mStartIndex = start_index;
		mIndexCount = vertex_count;
	}

	inline bool operator<(const T3RenderInst& rhs) const {
		return mSortKey < rhs.mSortKey;
	}

};

struct T3DispatchInst : ListNode<T3DispatchInst>
{

	u64 mSortKey;
	T3EffectParameterGroupStack mParameterStack;
	T3EffectParameterGroupStack* mpBaseParameters;
	std::shared_ptr<T3GFXBuffer> mpIndirectArgsBuffer;
	T3EffectCacheVariantRef mEffectRef;
	u32 mObjectIndex;
	u32 mIndirectArgsByteOffset;
	u32 mEffectPass;
	u32 mDispatchSizeX;
	u32 mDispatchSizeY;
	u32 mDispatchSizeZ;
	bool mbAsync;

	inline void DispatchIndirect(GFXPlatformPrimitiveType type, std::shared_ptr<T3GFXBuffer> pIndirectArgsBuffer, u32 indirectArgsByteOffset) {
		mpIndirectArgsBuffer = std::move(pIndirectArgsBuffer);
		mIndirectArgsByteOffset = indirectArgsByteOffset;
	}

	inline bool operator<(const T3DispatchInst& rhs) const {
		return mSortKey < rhs.mSortKey;
	}

};

struct RenderSceneView;

//Keep this using vectors not linked lists because ease of sorting
struct T3RenderInstManager
{

	LinkedList<T3RenderInst> mRenderInstList;
	LinkedList<T3DispatchInst> mDispatchInstList;

	inline void AddRenderInst(T3RenderInst* node) {
		mRenderInstList.insert_tail(node);
	}

	inline void AddDispatchInst(T3DispatchInst* node){
		mDispatchInstList.insert_tail(node);
	}

	void Execute(RenderSceneView& sceneView, T3EffectDrawParams& drawParams);

};

struct RenderInstSort {

	bool operator()(const T3RenderInst& lhs, const T3RenderInst& rhs){
		return lhs.mSortKey < rhs.mSortKey;
	}

	bool operator()(const T3DispatchInst& lhs, const T3DispatchInst& rhs) {
		return lhs.mSortKey < rhs.mSortKey;
	}

};

struct RenderViewPass : RenderViewPassBase, ListNode<RenderViewPass>
{

	LinearHeap* mHeap;
	RenderPassParams mParams;
	unsigned __int64 mProfileToken;
	const char* mName;
	T3EffectParameterGroupStack mParameterStack;
	T3EffectParameterGroupStack mFullScreenClearParameterStack;
	T3RenderInstManager mRenderManager;
	T3RenderViewport mViewport;
	T3RenderTargetReferences mRenderTargetReferences;
	T3SurfaceMultisample mTargetMultisample;
	i32 mTargetWidth;
	i32 mTargetHeight;

	inline T3RenderInst* PushRenderInst(){
		T3RenderInst* inst = mHeap->NewNoDestruct<T3RenderInst>();
		mRenderManager.AddRenderInst(inst);
		return inst;
	}

	inline T3DispatchInst* PushDispatchInst() {
		T3DispatchInst* inst = mHeap->NewNoDestruct<T3DispatchInst>();
		mRenderManager.AddDispatchInst(inst);
		return inst;
	}

	inline T3RenderInst* PushRenderInst(T3EffectParameterGroupStack& stack){
		T3RenderInst* inst = mHeap->NewNoDestruct<T3RenderInst>();
		inst->mParameterStack = stack;
		inst->mpBaseParameters = &mParameterStack;
		mRenderManager.AddRenderInst(inst);
		return inst;
	}

	inline T3DispatchInst* PushDispatchInst(T3EffectParameterGroupStack& stack) {
		T3DispatchInst* inst = mHeap->NewNoDestruct<T3DispatchInst>();
		inst->mParameterStack = stack;
		inst->mpBaseParameters = &mParameterStack;
		mRenderManager.AddDispatchInst(inst);
		return inst;
	}

};

struct RenderFrame;
struct RenderFrameScene;

inline const char* _SetRenderStructName(LinearHeap* pHeap, const char* szFormat, ...){
	char tmp[0x400];
	tmp[0] = 0;
	va_list v{};
	va_start(v, szFormat);
	int sz = vsnprintf_s(tmp, 0x400, szFormat, v);
	if (sz < 0)
		sz = -1;
	char* dest = pHeap->Alloc(++sz, 1);
	memcpy(dest, tmp, sz);
	va_end(v);
	return dest;
}

struct RenderFrameContext
{

	struct TargetEntry
	{
		T3RenderTargetIDSet mCurrentRenderTarget;
		bool mbCurrentDefaultRenderTarget;
		bool mbDepthResolved;
	};

	TargetEntry mRenderTargetStack[4];
	T3RenderPassType mRenderPassType = eRenderPassType_None;
	T3EffectParameterBoundState mParameterBoundState;
	u64 mFrameIndex;
	int mRenderTargetStackPos = 0;
	float mDepthBoundsMin = -1.f;
	float mDepthBoundsMax = -1.f;
	bool mbRenderedToDefaultTarget = false;
};

struct RenderDefaultPassParams
{
	T3RenderTargetIDSet mRenderTarget;
	T3RenderClear mForceClear;
	bool mbRequiredClearTarget;
	bool mbDefaultRenderTarget;
	bool mbIncludeLightAssignmentView;
};

struct RenderSceneView : ListNode<RenderSceneView>
{

	LinearHeap* mHeap;
	const char* mName;
	RenderFrameScene* mpRenderScene;
	RenderViewParams mParams;
	Matrix4 mViewProjectionMatrix;
	DynamicFeatures_WDC mEffectFeatures;
	LinkedList<RenderViewPassBase> mPassList;
	BitSet<enum T3RenderPass, eRenderPass_Count, 0> mPasses;
	RenderViewPass* mScenePassList[T3RenderPass::eRenderPass_Count];
	RenderSceneView* mpLightAssignmentView;
	u32* mpVisibilityBits;
	T3RenderTargetReferences mRenderTargetReferences;
	T3EffectParameterGroupStack mParameterStack;
	i32 mLightViewIndex;
	bool mbInvertDepth;

	void _Execute(RenderFrameContext& context, RenderFrameStats& frameStats, T3RenderTargetList& targetList, BitSet<T3RenderPass, 22, 0>& baseValidPasses, RenderSubViewParams* pSubViewParams);

	inline Scene* GetScene();
	
	inline RenderViewPass* GetScenePass(T3RenderPass pass){
		return mScenePassList[pass];
	}

	inline T3RenderInst* PushRenderInst(T3RenderPass pass) {
		return mScenePassList[pass]->PushRenderInst();
	}

	inline T3DispatchInst* PushDispatchInst(T3RenderPass pass) {
		return mScenePassList[eRenderPass_Main_Compute]->PushDispatchInst();
	}

	inline T3RenderInst* PushRenderInst(T3RenderPass pass, T3EffectParameterGroupStack& stack) {
		return mScenePassList[pass]->PushRenderInst(stack);
	}

	inline T3DispatchInst* PushDispatchInst(T3RenderPass pass, T3EffectParameterGroupStack& stack) {
		return mScenePassList[eRenderPass_Main_Compute]->PushDispatchInst(stack);
	}

	inline void SetVisibilityBit(u32 objectIndex){
		mpVisibilityBits[objectIndex >> 5] |= (1u << (objectIndex & 0x1F));
	}

	void PushDefaultPasses(T3RenderTargetContext& targetContext, RenderDefaultPassParams& params);

	RenderViewPass* AllocatePass(RenderPassParams& params, T3RenderTargetContext& targetContext);

	void PushPass(RenderViewPass* pRenderPass, T3RenderTargetContext& targetContext);

	RenderSceneView* PushSubView(const RenderSubViewParams& params);

	RenderSceneView* PushSubView(const RenderSubViewParams& params, RenderViewParams& subParams);

	void PushSubView(const RenderSubViewParams& params, RenderSceneView* pSubView);

	inline RenderViewPass* PushPass(RenderSceneView* pParentView, RenderPassParams& params, T3RenderTargetContext& targetContext)
	{
		RenderViewPass* pass = pParentView->AllocatePass(params, targetContext);
		PushPass(pass, targetContext);
		return pass;
	}

	inline RenderViewPass* PushPass(RenderPassParams& params, T3RenderTargetContext& targetContext)
	{
		RenderViewPass* pass = AllocatePass(params, targetContext);
		PushPass(pass, targetContext);
		return pass;
	}

};

struct RenderFrameScene : ListNode<RenderFrameScene>
{
	struct PassParameters : ListNode<PassParameters>
	{
		RenderViewType mViewType;
		Camera* mpCamera;
		T3RenderViewport mViewport;
		RenderClipPlanes mClipPlanes;
		Vector2 mJitter;
		Vector2 mPrevJitter;
		i32 mTargetWidth;
		i32 mTargetHeight;
		u32 mTargetSampleCount;
		T3EffectParameterGroup* mpParameterGroup;
		T3EffectParameterUniformBufferPointers mParameterBuffers;
		T3EffectParameterGroupStack mParameterStack;
	};

	u64 mFrameIndex;
	LinearHeap* mHeap;
	const char* mName;
	RenderFrame* mpRenderFrame;
	T3Texture* mpShadowGoboTexture;
	RenderSceneParams mParams;
	T3EffectSceneParams mSceneParams;
	DynamicFeatures_WDC mEffectFeatures;
	T3EffectParameterGroupStack mParameterStack;
	LinkedList<PassParameters> mPassParameterList;
	LinkedList<RenderSceneView> mViewForLightingList;
	LinkedList<RenderViewPass> mPassList;

	inline RenderSceneView* AllocateView(RenderViewParams& params) {
		RenderSceneView* view = mHeap->NewNoDestruct<RenderSceneView>();
		view->mName = "View";
		view->mViewProjectionMatrix = Matrix4::Identity();
		view->mParams = params;
		u32 numWords = params.mVisibilityBitCount >> 5;
		view->mpVisibilityBits = (u32*)mHeap->Alloc(numWords * 4, 4);
		for (u32 i = 0; i < numWords; i++)
			view->mpVisibilityBits[i] = 0;
		if (!view->mParams.mpCamera) {
			view->mParams.mpCamera = mHeap->New<Camera>();
			view->mParams.mpCamera->SetOrthoParameters(0.f, 1.f, 0.f, 1.f, 0.f, 1.0f);
			view->mParams.mpCamera->SetNearClip(0.f);
			view->mParams.mpCamera->SetFarClip(1.0f);
		}
		view->mViewProjectionMatrix = view->mParams.mpCamera->GetViewMatrix() * view->mParams.mpCamera->GetProjectionMatrix();
		view->mbInvertDepth = view->mParams.mpCamera->IsInvertedDepth();
		return view;
	}

	T3EffectParameterGroupStack _GetParametersForPass(RenderViewType viewType, Camera* pCamera,
		Vector2& jitter, Vector2& prevJitter, RenderClipPlanes& clipPlanes, int targetWidth,
		int targetHeight, unsigned int targetSampleCount, T3RenderViewport& viewport);

	void _FinalizeParameters();

};

inline bool SetEffectFeatureIfValid(DynamicFeatures_WDC& features, T3EffectFeature feature, T3EffectQuality quality, PlatformType platform){
	T3EffectFeatureDesc& desc = sFeatureDesc_WDC[feature];
	if (desc.mExcludeQuality[quality] || desc.mExcludePlatforms[platform])
		return false;
	features.Set(feature, 1);
	return true;
};

inline T3Texture* SetSceneTexture(T3EffectParameterType param, T3Texture* hTx, T3EffectParameterTextures& textureParams, RenderFrameUpdateList& updateList) {
	if (!hTx)
		return 0;
	hTx->SetUsedOnFrame(updateList);
	textureParams[param].mStorage = eEffectParameterStorage_Texture;
	textureParams[param].mpTexture = hTx;
	return hTx;
}

inline void SetRenderTarget(T3EffectParameterType param, T3EffectParameterTextures& textureParams, T3RenderTargetConstantID rtID){
	textureParams[param].mRenderTargetID = rtID;
	textureParams[param].mStorage = eEffectParameterStorage_RenderTarget;
}

struct RenderFrame
{

	u64 mFrameIndex;
	LinearHeap* mHeap;
	DynamicFeatures_WDC mEffectFeatures;
	RenderFrameUpdateList* mResourceUpdateList;
	T3RenderTargetList mRenderTargetList;
	LinkedList<DynamicDrawData> mDynamicGeometry;
	T3GFXDynamicResourceContext mDynamicResourceContext;
	T3EffectParameterGroupStack mParameterStack;
	T3EffectCacheVariantRef mClearEffectRef;
	T3EffectParameterGroup* mpParameterGroup;
	//T3OcclusionFrame* mpOcclusionFrame;
	LinkedList<RenderSceneView> mViewList;
	LinkedList<RenderFrameScene> mSceneList;
	bool mbToolFrame;
	bool mbDebugCaptureFrame;

	inline RenderFrameScene* AllocateEmptyScene() {
		RenderFrameScene* a = mHeap->NewNoDestruct<RenderFrameScene>();
		a->mParameterStack = mParameterStack;
		mSceneList.insert_tail(a);
		return a;
	}

	inline void Finalize(T3RenderTargetContext* context){
		T3RenderTargetUtil::InitializeList(mRenderTargetList, *mHeap, context);
		for (RenderFrameScene* it = mSceneList.head(); it; it = it->next)
			it->_FinalizeParameters();
	}

	RenderFrameScene* AllocateScene(const RenderSceneParams& params);

	inline void PushView(RenderSceneView* pRenderView){
		if(pRenderView->mParams.mViewType == eRenderView_Main ||
			pRenderView->mParams.mViewType == eRenderView_RenderTexture ||
			pRenderView->mParams.mViewType == eRenderView_RenderViewport ||
			pRenderView->mParams.mViewType == eRenderView_LightBake){
			pRenderView->mpRenderScene->mViewForLightingList.insert_tail(pRenderView);
		}
		mViewList.insert_tail(pRenderView);
	}

	inline void PushFrontView(RenderSceneView* pRenderView) {
		if (pRenderView->mParams.mViewType == eRenderView_Main ||
			pRenderView->mParams.mViewType == eRenderView_RenderTexture ||
			pRenderView->mParams.mViewType == eRenderView_RenderViewport ||
			pRenderView->mParams.mViewType == eRenderView_LightBake) {
			pRenderView->mpRenderScene->mViewForLightingList.insert_head(pRenderView);
		}
		mViewList.insert_head(pRenderView);
	}

	inline RenderSceneView* PushView(RenderFrameScene& scene, RenderViewParams& params){
		RenderSceneView* view = scene.AllocateView(params);
		PushView(view);
		return view;
	}

	inline RenderSceneView* PushFrontView(RenderFrameScene& scene, RenderViewParams& params) {
		RenderSceneView* view = scene.AllocateView(params);
		PushFrontView(view);
		return view;
	}

};

//Dumbed down version off Telltales swapping frame system (very common in game engines). Don't touch, use RenderThreadContext.
struct RenderThreadFrame {

	LinearHeap mHeap;
	RenderFrame* mpRenderFrame;
	RenderFrameUpdateList* mpUpdateList;

};

struct RenderObject_Mesh;
struct RenderObject_Viewport;

//You need to one of these locally held to render using the library. Each game loop simply call GameRender followed by FrameExecute.
//mRenderedFrameStats tells statistics about the last rendered frame (after the call to FrameExecute).
//Ensure you update the render user struct (see T3EffectUser.h), with updated delta time and frame index etc.
//Use SetScene to set the current scene. Ensure GetResource accesses a preloaded set of resources which may be required.
struct RenderThreadContext {

	u64 mFrameIndex;
	RenderThreadFrame mCurrentFrame;
	RenderFrameStats mRenderedFrameStats;

	std::vector<Camera*> mpCachedCameras;
	std::vector<RenderObject_Viewport*> mpSceneViewports;
	std::vector<RenderObject_Mesh*> mpSceneMeshes;
	Scene* mpUserScene = 0;
	PropertySet* mpUserSceneTopmostProperties = 0;

	void SetScene(Scene* pScene);

	/**
	 * GameRender::RenderFrame. This in the full game fills up the current main thread submitting frame for rendering. This then gets swapped at the end of the frame
	 * and then the render thread executes that frame next frame in its loop. This would be called on the main thread. This version is singlethreaded so call this FIRST.
	 */
	void GameRender();

	/**
	 * RenderThread::Execute. This executes and renders the frame, in which the frame was filled by the GameRender() function. This would be called on the render thread but
	 * because this is a singlethreaded version, we just call this function on the main thread after game render.
	 */
	void FrameExecute();

	/*
	* You need to implement this. This should find and load a resource. You should ideally have preloaded the scene using the preloadpackage (RTS). This function will request
	* the resource instance (loaded from file). Cache it, load them all at once, doesn't matter and its up to you how its done.
	* This MUST return for ALL resources, especially module_xxx.prop properties!
	*/
	virtual void* GetResource(Symbol name, MetaClassDescription* pResourceType) = 0;

	/**
	 * Return and open a shader package. Return null if it can't be found. Open the data stream and return one allocated with new. Delete will be called by the user of the return of this function.
	 */
	virtual DataStream* OpenShaderPackage(const String& fxpack) = 0;

	/**
	 * Clears this context. Everything is freed. Automagically called by destructor but you should ideally to end the game loop.
	 */
	void Clear();

	inline ~RenderThreadContext() {
		Clear();
	}

};

inline Scene* RenderSceneView::GetScene() {
	return mpRenderScene->mParams.mpScene;
}

namespace T3ViewportUtil {

	inline void Set(T3RenderViewport* viewport, T3RenderRelativeViewport* src, int targetWidth, int targetHeight)
	{
		viewport->mWidth =   (signed int)std::floorf((float)targetWidth * src->mWidth);
		viewport->mHeight =  (signed int)std::floorf((float)targetHeight * src->mHeight);
		viewport->mOriginX = (signed int)std::floorf((float)targetWidth * src->mOriginX);
		viewport->mOriginY = (signed int)std::floorf((float)targetHeight * src->mOriginY);
		viewport->mMinDepth = src->mMinDepth;
		viewport->mMaxDepth = src->mMaxDepth;
	}

	//Add spacing (gutter) around the viewport
	inline void AddGutter(T3RenderViewport* viewport, int gutter)
	{
		viewport->mOriginX += gutter;
		viewport->mWidth -= 2 * gutter;
		viewport->mHeight -= 2 * gutter;
		viewport->mOriginY += gutter;
	}

}

namespace T3EffectCache {

	inline void Dispatch(T3DispatchInst& inst, T3EffectDrawParams& params){}

	inline void Draw(T3RenderInst& inst, T3EffectDrawParams& params){}

}