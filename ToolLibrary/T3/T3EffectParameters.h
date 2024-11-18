#pragma once

#include "T3Effect.h"
#include "T3RenderTargets.h"
#include "Camera.h"
#include "../Types/Scene.h"
#include "LinearHeap.h"
#include "../LinkedList.h"

// SHADER (EFFECT) PARAMETERS

struct T3EffectParameterBuffer : ListNode<T3EffectParameterBuffer>
{

	std::shared_ptr<T3GFXBuffer> mpGFXBuffer;
	void* mpCPUBuffer;
	Flags mFlags;
	GFXPlatformAllocationType mAllocationType;
	T3GFXUniformBufferUsage mUsage;
	unsigned int mScalarSize;
};

//Cached pointers for T3EffectParameterGroup. For Uniform
struct T3EffectParameterUniformBufferPointers
{
	void* mpBuffer[eEffectParameter_UniformBufferCount];
};

//Cached offsets for T3EffectParameterGroup
struct T3EffectParameterUniformBufferOffsets
{
	int mOffset[eEffectParameter_UniformBufferCount];

	inline T3EffectParameterUniformBufferOffsets() {
		for(int i = 0; i < eEffectParameter_UniformBufferCount; i++){
			mOffset[i] = -1;
		}
	}

	//Sets bits in argument if the offset in this struct is non zero for that parameter (Uniform buffer)
	inline void SetParameters(BitSet<enum T3EffectParameterType, 150, 0>& paramTypes) {
		for (int i = 0; i < 30; i++) {
			if (mOffset[i] >= 0)
				paramTypes.Set((T3EffectParameterType)i, 1);
		};
	};

};

struct T3EffectParameterBufferEntry
{
	T3EffectParameterBuffer* mpBuffer;
	unsigned __int64 mOffset;
};

struct T3EffectParameterBufferDataEntry
{
	void* mpData;
	unsigned int mScalarSize;
};

/*
memory format: buffer size  (u16), param count (u16), for num params u16 off over 4, u8 type, u8 pad byte, THEN parameter data: for each
u64 param T3EffectParameter_XXX pointer, then offset (u64) from base.
*/
struct T3EffectParameterGroupHeader
{
	u16 mParameterScalarOffset[eEffectParameter_Count];
	u8 mParameterType[eEffectParameter_Count];
	u32 mHeaderScalarSize;
	u32 mBufferScalarSize;
	u32 mMaxScalarAlign;
	u32 mParameterCount;

};

struct alignas(4) _T3EffectBufferParam {
	u16 off;//offset
	u8 type;//enum T3EffectParameterType
	u8 storage;//enum T3EffectParameterStorage
};

enum T3TextureViewFlags
{
	eGFXPlatformTextureView_DisableSRGB = 0x1,
	eGFXPlatformTextureView_ReadStencil = 0x2,
	eGFXPlatformTextureView_FormatAliasU32 = 0x4,
};

//Normally in the GFXPlatform layer. Moved to here for conv. MUST BE 8 BYTES
struct T3TextureView {

	u16 mSliceIndex;
	u16 mSliceCount;
	u16 mFlags;
	u8 mMipIndex;
	u8 mMipCount;

};

struct T3BufferView {
	u32 mScalarOffset;
	u32 mScalarSize;
};

struct T3TextureParameter
{
	T3EffectParameterStorage mStorage;
	T3TextureView mView;
	T3Texture* mpTexture;
	int mRenderTargetID;//not a union to enable dtor and ctor
};

struct T3EffectParameterTextures
{

private: T3TextureParameter mTexture[eEffectParameter_SamplerCount];

public:

  inline T3TextureParameter& operator[](T3EffectParameterType param) {
	  return mTexture[param - eEffectParameter_FirstSampler];
  }

  inline void SetParameters(BitSet<enum T3EffectParameterType, 150, 0>& paramTypes){
	  for (int i = 0; i < 0x59; i++) {
		  if (mTexture[i].mStorage != eEffectParameterStorage_Empty) {
			  paramTypes.Set((T3EffectParameterType)(i + eEffectParameter_FirstSampler), 1);
		  }
	  }
  }

};

struct RenderFrameUpdateList;

/**
* 
 A shader parameter group. This is a logical grouping of shader input parameters and data. This struct is layed out in memory as the variables
 in it below followed by a header buffer which describes all the parameter types and offsets. Then is the main buffer data.
 
 A parameter group is created using one of the static Create(XXX) member functions.

 You can then dynamically at runtime use the functions in this struct to set parameters, whether they be uniform/generic buffers, render targets, etc.

 This struct in memory, with two example parameter:

 OFFSET | DATA

 0x0000 | T3EffectParameterGroup::mScalarSize
 0x0002 | T3EffectParameterGroup::mParameterCount
 0x0004 | Parameter Desc 1. Layout is _T3EffectBufferParam struct.
 0x0008 | Parameter Desc 2. Same.
 0x000C | Parameter Value 1. Layuot is _T3EffectBufferParamData struct.
 0x0010 | Parameter Value 2. Same.

 */
struct T3EffectParameterGroup
{
	u16 mScalarSize;
	u16 mParameterCount;

	/**
	* 
	 * Add uniform buffers to this parameter group.
	 * 
	 * Arguments 'buffers' and 'offsets' are a list of up to 30 (see parameter type enum) uniform buffer parameters.
	 * index into those those argument lists with eEffectParameter_UniformBufferXXX.
	 * Consecutive from 0, any non zero offsets are set if the parameter exists in this parameter group.
	 * 
	 * Offsets argument is not touched. But pointers is. You leave it initially empty and this will be set with the pointers
	 * relative to the pCachedBufferData argument - with the offset at its index added.
	 * 
	 * pAssignedBuffer is the actual GPU buffer to assign to each uniform buffer parameter type you specify through indices in the offsets argument.
	 * 
	 * This structs parameter data is modified, and baseOffset is added onto each of the offsets you specify and stored in this.
	 * This would be offset pCachedBufferData is from the start of the buffer data.
	 * 
	 * pCachedBufferData can be NULL.
	 * 
	 */
	inline void AddUniformBuffer(T3EffectParameterBuffer* pAssignedBuffer, void* pCachedBufferData,
		T3EffectParameterUniformBufferPointers& buffers, const T3EffectParameterUniformBufferOffsets& offsets, const u32 baseOffset) {
		static_assert(sizeof(_T3EffectBufferParam) == 4, "Invalid size for parameter. Ensure compiler packing is not messing with _T3EffectBufferParam");
		_T3EffectBufferParam* params = (_T3EffectBufferParam*)this + 1;
		for (u32 i = 0; i < mParameterCount; i++) {
			if (params[i].type < eEffectParameter_UniformBufferCount) {
				u32 off = offsets.mOffset[i];
				if (off >= 0) {
					//Set the parameter storage type.
					params[i].storage = eEffectParameterStorage_UniformBuffer;

					//cache the buffer into buffers argument above
					buffers.mpBuffer[i] = pCachedBufferData ? ((char*)pCachedBufferData + (off << 2)) : 0;

					//Set the parameter data value pointer.
					u8* pParamData = ((u8*)this + ((u32)params[i].off << 2));
					*((T3EffectParameterBuffer**)(pParamData + 0)) = pAssignedBuffer;
					*((u64*)(pParamData + 8)) = (u64)(baseOffset + off);
				}
			}
		}
	}

	//See AddUniformData. Adds data, not any actual GPU uniform buffers. pData cannot be 0.
	inline void AddUniformBufferData(void* pData, T3EffectParameterUniformBufferPointers* buffers,
		const T3EffectParameterUniformBufferOffsets* offsets, const u32 scalarSize) {
		u8* lastData = 0;
		u32 last = 0;
		for (u32 i = 0; i < mParameterCount; i++) {
			_T3EffectBufferParam* params = (_T3EffectBufferParam*)this + 1;
			if (params[i].type < eEffectParameter_UniformBufferCount) {
				u32 off = offsets->mOffset[i];
				if (off >= 0) {
					//Set the parameter storage type.
					params[i].storage = eEffectParameterStorage_UniformBufferData;

					//cache the buffer into buffers argument above
					buffers->mpBuffer[i] = (u8*)pData + ((u32)offsets->mOffset[i] << 2);

					//Set the parameter data value pointer.
					if (lastData)
						*((u32*)(lastData + 8)) = off - last;
					lastData = ((u8*)this + ((u32)params[i].off << 2));
					*((void**)(lastData + 0)) = buffers->mpBuffer[i];
					last = off;
				}
			}
		}
		if (lastData)
			*((u32*)(lastData + 8)) = scalarSize - last;
	}

	inline void SetTextures(T3EffectParameterTextures& textures){
		static_assert(sizeof(T3TextureView) == 8, "T3TextureView struct size is not 8. Struct is packed in code so something went wrong!");
		_T3EffectBufferParam* params = (_T3EffectBufferParam*)this + 1;
		for(int i = 0; i < mParameterCount; i++){
			if(params[i].type >= eEffectParameter_FirstSampler && params[i].type <= eEffectParameter_LastSampler){
				const T3TextureParameter& textureParam = textures[(T3EffectParameterType)params[i].type];
				if(textureParam.mStorage == eEffectParameterStorage_Texture){
					params[i].storage = eEffectParameterStorage_Texture;
					u64* paramData = (u64*)((u8*)this + ((u32)params[i].off << 2));
					paramData[1] = (u64)textureParam.mpTexture;
					paramData[0] = *((u64*)(&textureParam.mView));		
				}else if(textureParam.mStorage < eEffectParameterStorage_Count){
					params[i].storage = textureParam.mStorage;
					u64* paramData = (u64*)((u8*)this + ((u32)params[i].off << 2));
					paramData[1] = (u64)textureParam.mpTexture;
				}
				else
					params[i].storage = eEffectParameterStorage_Empty;
			}
		}
	}

	inline void SetUniformBuffer(T3EffectParameterType param, T3EffectParameterBuffer* pBuffer, u32 scalarOffset) {
		int p = GetIndexForParameter(param);
		if (p != -1) {
			_T3EffectBufferParam* params = (_T3EffectBufferParam*)this + 1;
			params[p].storage = eEffectParameterStorage_UniformBuffer;
			u64* paramData = (u64*)((u8*)this + ((u32)params[p].off << 2));
			paramData[1] = (u64)scalarOffset;
			paramData[0] = (u64)pBuffer;
		}
	}

	//uniformBufferData should be scalarSize bytes
	void* SetUniformBufferData(T3EffectParameterType param, void* uniformBufferData, u32 scalarSize);

	inline void SetGenericBuffer(T3EffectParameterType param, const std::shared_ptr<T3GFXBuffer>& pBuffer, const T3BufferView& view){
		int p = GetIndexForParameter(param);
		if (p != -1) {
			_T3EffectBufferParam* params = (_T3EffectBufferParam*)this + 1;
			params[p].storage = eEffectParameterStorage_GenericBuffer;
			u64* paramData = (u64*)((u8*)this + ((u32)params[p].off << 2));
			paramData[1] = (u64)pBuffer.get();
			paramData[0] = *((u64*)(&view));
		}
	}

	inline void SetEmpty(T3EffectParameterType param){
		int p = GetIndexForParameter(param);
		if (p != -1) {
			_T3EffectBufferParam* params = (_T3EffectBufferParam*)this + 1;
			params[p].storage = eEffectParameterStorage_Empty;
		}
	}

	inline void SetRenderTarget(T3EffectParameterType param, const T3RenderTargetID& id, const T3RenderTargetAccess& access, const T3TextureView& view){
		int p = GetIndexForParameter(param);
		if (p != -1) {
			_T3EffectBufferParam* params = (_T3EffectBufferParam*)this + 1;
			params[p].storage = access == eRenderTargetAccess_Input ? eEffectParameterStorage_RenderTarget : eEffectParameterStorage_RenderTargetOutput;
			u64* paramData = (u64*)((u8*)this + ((u32)params[p].off << 2));
			paramData[1] = (u64)id.mValue;
			paramData[0] = *((u64*)(&view));
		}
	}

	inline void SetTexture(T3EffectParameterType param, const std::shared_ptr<T3Texture>& pTexture, const T3TextureView& view){
		int p = GetIndexForParameter(param);
		if(p != -1){
			_T3EffectBufferParam* params = (_T3EffectBufferParam*)this + 1;
			params[p].storage = eEffectParameterStorage_Texture;
			u64* paramData = (u64*)((u8*)this + ((u32)params[p].off << 2));
			paramData[1] = (u64)pTexture.get();
			paramData[0] = *((u64*)(&view));
		}
	}

	inline int GetIndexForParameter(T3EffectParameterType param){
		_T3EffectBufferParam* params = (_T3EffectBufferParam*)this + 1;
		for(int i = 0; i < mParameterCount; i++){
			if (params[i].type == param)
				return i;
		}
		return -1;
	}

	//Actively returns a buffer you can put the parameter data into. Render frame update list will then update the buffers when needed.
	u8* UpdateParameterBufferData(RenderFrameUpdateList& updateList, T3EffectParameterType type, u32 size);

	//by lib, gets pointer to param data storage (16 bytes)
	inline u8* _GetParameterStorageInternal(int paramIndex){
		return (u8*)this + ((u64)((_T3EffectBufferParam*)this)[1 + paramIndex].off << 2);
	}

	static inline T3EffectParameterGroup* Create(void* pDestBuffer, const T3EffectParameterGroupHeader& header) {
		//normally not in the header
		for (int i = 0; i < header.mParameterCount; i++) {
			*(u16*)((u8*)pDestBuffer + 4 * (i + 1)) = (u16)header.mHeaderScalarSize + header.mParameterScalarOffset[i];
			*(u8*)((u8*)pDestBuffer + 4 * (i + 1) + 2) = header.mParameterType[i];
			*(u8*)((u8*)pDestBuffer + 4 * (i + 1) + 3) = eEffectParameterStorage_Empty;
		}
		*((u16*)pDestBuffer + 0) = (u16)header.mBufferScalarSize;
		*((u16*)pDestBuffer + 1) = (u16)header.mParameterCount;
		return (T3EffectParameterGroup*)pDestBuffer;
	}

	static inline T3EffectParameterGroup* CreateForType(void* pDestBuffer, T3EffectParameterType param) {
		*(u16*)((u8*)pDestBuffer + 4 * (0 + 1)) = 1;
		*(u8*)((u8*)pDestBuffer + 4 * (0 + 1) + 2) = param;
		*(u8*)((u8*)pDestBuffer + 4 * (0 + 1) + 3) = eEffectParameterStorage_Empty;
		*((u16*)pDestBuffer + 0) = sClassDesc[sBufferDesc[param].mClass].mScalarSize + 1;
		*((u16*)pDestBuffer + 1) = 1;
		return (T3EffectParameterGroup*)pDestBuffer;
	}

	static inline u32 InitializeHeader(T3EffectParameterGroupHeader& header, BitSet<T3EffectParameterType, eEffectParameter_Count, 0>& params) {
		int param = 0;
		u16 counter = 0;
		u32 maxAlign = 1;
		for (int i = 0; i < eEffectParameter_Count; i++) {
			if (params[(T3EffectParameterType)i]) {
				header.mParameterType[param] = i;
				header.mParameterScalarOffset[param] = counter;
				counter += sBufferDesc[i].mScalarSize;
				if (sClassDesc[sBufferDesc[i].mClass].mScalarAlign > maxAlign)
					maxAlign = sClassDesc[sBufferDesc[i].mClass].mScalarAlign;
				param++;
			}
		}
		header.mParameterCount = param;
		header.mHeaderScalarSize = (counter + 1);
		header.mBufferScalarSize = header.mHeaderScalarSize + (counter << 1);
		header.mMaxScalarAlign = maxAlign;
		return header.mBufferScalarSize << 2;
	}

	static inline void InitializeHeaderForType(T3EffectParameterGroupHeader& header, T3EffectParameterType param) {
		header.mParameterCount = 1;
		header.mParameterType[0] = (u8)param;
		header.mParameterScalarOffset[0] = 0;
		header.mHeaderScalarSize = 1;
		header.mBufferScalarSize = 1 + sClassDesc[sBufferDesc[param].mClass].mScalarSize;
		header.mMaxScalarAlign = sClassDesc[sBufferDesc[param].mClass].mScalarAlign;
	}

	//Creates a parameter group with T3EffectParameterGroupHeader::mBufferScalarSize once that header type is initialized.
	static inline T3EffectParameterGroup* _AllocateParameterGroup(LinearHeap& heap, u32 headerBufferScalarSize, u32 headerScalarAlign) {
		return (T3EffectParameterGroup*)heap.Alloc((u32)headerBufferScalarSize << 2, (u32)headerScalarAlign << 2);
	}

	static inline T3EffectParameterGroup* Create(LinearHeap& heap, BitSet<enum T3EffectParameterType, 150, 0>& params) {
		T3EffectParameterGroupHeader header{};
		u32 size = InitializeHeader(header, params);
		T3EffectParameterGroup* group = _AllocateParameterGroup(heap, header.mBufferScalarSize, header.mMaxScalarAlign);
		group->mParameterCount = header.mParameterCount;
		group->mScalarSize = header.mBufferScalarSize;
		_T3EffectBufferParam* p = (_T3EffectBufferParam*)group + 1;
		for(u32 i = 0; i < header.mParameterCount; i++){
			p[i].off = header.mHeaderScalarSize + header.mParameterScalarOffset[i];
			p[i].storage = eEffectParameterStorage_Empty;
			p[i].type = header.mParameterType[i];
		}
		return group;
	}

};

struct T3EffectParameterGroupStack;

inline void PushParameterGroup(LinearHeap& heap, T3EffectParameterGroupStack* self, T3EffectParameterGroup* group);
inline void PushParameterStack(LinearHeap& heap, T3EffectParameterGroupStack* self, T3EffectParameterGroupStack& stack);

namespace T3EffectParameterUtil {

	//Returns 
	inline u32 AllocateUniformBuffer(u32 baseOffset, BitSet<enum T3EffectParameterType, 150, 0>& paramTypes, T3EffectParameterUniformBufferOffsets& offsets){
		u32 result = (baseOffset + 63) & 0xFFFFFFC0u;//to ensure alignment
		for(int i = 0; i < eEffectParameter_UniformBufferCount; i++){
			if(paramTypes[(T3EffectParameterType)i]){
				offsets.mOffset[i] = result;
				result += (sBufferDesc[i].mScalarSize + 63) & 0xFFFFFFC0u;
			}
		}
		return result;
	};

	inline u32 GetCount(BitSet<enum T3EffectParameterType, 150, 0>& paramTypes){
		u32 n = 0;
		for (int i = 0; i < 150; i++)
			if (paramTypes[(T3EffectParameterType)i])
				++n;
		return n;
	};

	inline bool Initialize(T3EffectParameterBuffer* buffer, unsigned int scalarSize, T3GFXUniformBufferUsage usage)
	{
		bool result;
		buffer->mScalarSize = scalarSize;
		result = 1;
		buffer->mUsage = usage;
		return result;
	}

	inline u32 InitializeForType(T3EffectParameterBuffer* buffer, T3EffectParameterType paramType, T3GFXUniformBufferUsage usage)
	{
		T3EffectParameterBuffer* v3;
		T3GFXUniformBufferUsage v4;
		u32 result;
		v3 = buffer;
		v4 = usage;
		result = sBufferDesc[paramType].mScalarSize;
		v3->mScalarSize = result;
		v3->mUsage = v4;
		return result;
	}

	inline u32 InitializeForTypes(T3EffectParameterBuffer* buffer, BitSet<enum T3EffectParameterType, 150, 0>& paramTypes,
		T3EffectParameterUniformBufferOffsets& offsets, T3GFXUniformBufferUsage usage)
	{
		T3EffectParameterBuffer* v4;
		T3GFXUniformBufferUsage v5; 
		u32 result;
		v4 = buffer;
		v5 = usage;
		result = AllocateUniformBuffer(0, paramTypes, offsets);
		v4->mScalarSize = result;
		v4->mUsage = v5;
		return result;
	}

}

struct T3EffectParameterGroupStack
{

	T3EffectParameterGroup* mpGroup;
	T3EffectParameterGroupStack* mpParent;
	BitSet<enum T3EffectParameterType, 150, 0> mParamTypes;

	//Allocate a new parameter group, with just one parameter.
	inline T3EffectParameterGroup* AllocateParameter(LinearHeap& heap, T3EffectParameterType param){
		T3EffectParameterGroupHeader tmpHeader{};
		T3EffectParameterGroup::InitializeHeaderForType(tmpHeader, param);
		T3EffectParameterGroup* pGroup = T3EffectParameterGroup::_AllocateParameterGroup(heap, tmpHeader.mBufferScalarSize, tmpHeader.mMaxScalarAlign);
		T3EffectParameterGroup::CreateForType(pGroup, param);
		PushParameterGroup(heap,this, pGroup);
		mParamTypes.Set(param, 1);
		return pGroup;
	}

	inline T3EffectParameterGroup* AllocateParameters(LinearHeap& heap, BitSet<enum T3EffectParameterType, 150, 0>& params){
		T3EffectParameterGroupHeader tmpHeader{};
		T3EffectParameterGroup::InitializeHeader(tmpHeader, params);
		T3EffectParameterGroup* pGroup = T3EffectParameterGroup::_AllocateParameterGroup(heap, tmpHeader.mBufferScalarSize, tmpHeader.mMaxScalarAlign);
		T3EffectParameterGroup::Create(pGroup, tmpHeader);
		PushParameterGroup(heap, this, pGroup);
		mParamTypes.Set(params);
		return pGroup;
	}

	T3EffectParameterGroup* AllocateParametersWithBuffer(void* buffer, u32 bufSize, RenderFrameUpdateList& updateList,
		T3EffectParameterUniformBufferPointers& buffers, BitSet<enum T3EffectParameterType, 150, 0>& params);

	T3EffectParameterGroup* AllocateParametersWithBuffer(RenderFrameUpdateList& updateList,
		T3EffectParameterUniformBufferPointers& buffers, BitSet<enum T3EffectParameterType, 150, 0>& params);


	//increases ref count of pGroup, adds to this.
	inline void PushParameters(LinearHeap& heap, T3EffectParameterGroup* pGroup){
		PushParameterGroup(heap, this, pGroup);
		_T3EffectBufferParam* params = (_T3EffectBufferParam*)pGroup + 1;
		for (int i = 0; i < pGroup->mParameterCount; i++)
			mParamTypes.Set((T3EffectParameterType)params[i].type, 1);
	}

	//increases ref count of pGroup, adds to this.
	inline void PushParametersWithTypes(LinearHeap& heap, T3EffectParameterGroup* pGroup, BitSet<enum T3EffectParameterType, 150, 0>& params) {
		PushParameterGroup(heap, this, pGroup);
		mParamTypes.Set(params);
	}

	//ref count of argument at end of this function will be the same
	inline void PushParametersCopyWithTypes(LinearHeap& heap, T3EffectParameterGroup* pGroup, BitSet<enum T3EffectParameterType, 150, 0>& params) {
		T3EffectParameterGroup* pCopy = T3EffectParameterGroup::_AllocateParameterGroup(heap, pGroup->mScalarSize, 16);
		memcpy(pCopy, pGroup, (u64)pGroup->mScalarSize << 2);
		pGroup = std::move(pCopy);
		PushParameterGroup(heap, this, pGroup);
		mParamTypes.Set(params);
	}

	//ref count of argument at end of this function will be the same
	inline void PushParametersCopy(LinearHeap& heap, T3EffectParameterGroup* pGroup) {
		T3EffectParameterGroup* pCopy = T3EffectParameterGroup::_AllocateParameterGroup(heap, pGroup->mScalarSize, 16);
		memcpy(pCopy, pGroup, (u64)pGroup->mScalarSize << 2);
		pGroup = std::move(pCopy);
		PushParameterGroup(heap, this, pGroup);
		_T3EffectBufferParam* params = (_T3EffectBufferParam*)pGroup + 1;
		for (int i = 0; i < pGroup->mParameterCount; i++)
			mParamTypes.Set((T3EffectParameterType)params[i].type, 1);
	}

	inline void PushParametersStack(LinearHeap& heap, T3EffectParameterGroupStack& stack) {
		PushParameterStack(heap, this, stack);
	}

	inline T3EffectParameterGroupStack() : mpGroup(0) {}

	inline ~T3EffectParameterGroupStack() {}

};

inline void PushParameterGroup(LinearHeap& heap, T3EffectParameterGroupStack* self, T3EffectParameterGroup* group){
	if(self->mpGroup){
		T3EffectParameterGroupStack* pStack = heap.NewNoDestruct<T3EffectParameterGroupStack>();
		pStack->mpParent = self->mpParent;
		pStack->mpGroup = group;
		pStack->mParamTypes = self->mParamTypes;
		self->mpParent = std::move(pStack);
	}else{
		self->mpGroup = group;
	}
}

inline void PushParameterStack(LinearHeap& heap, T3EffectParameterGroupStack* self, T3EffectParameterGroupStack& stack)
{
	if(stack.mpParent){
		PushParameterStack(heap, self, *stack.mpParent);
	}else{
		PushParameterGroup(heap, self, stack.mpGroup);
		self->mParamTypes.Set(stack.mParamTypes);
	}
}

struct T3EffectParameterBoundState;

inline void BindParameterStack(T3EffectParameterBoundState& boundState, RenderFrameStats& stats, BitSet<enum T3EffectParameterType, 150, 0>& paramsToSet, T3EffectParameterGroupStack& pParameters, T3RenderTargetList& targetList);

struct T3EffectParameterBoundState
{

	struct BufferBinding
	{
		union
		{
			T3EffectParameterBuffer* mpBuffer;
			const void* mpBufferData;
		};

		union
		{
			u32 mBufferOffset;
			u32 mBufferDataSize;
		};
	};

	struct GenericBufferBinding
	{
		T3GFXBuffer* mpBuffer;
		T3BufferView mView;
	};

	struct TextureBinding
	{
		T3Texture* mpTexture;
		T3TextureView mView;
		T3SamplerStateBlock mSamplerState;
		T3SamplerStateBlock mSamplerStateMask;
	};

	u64 mFrameIndex;
	std::shared_ptr<GFXPlatformProgram> mpCurrentProgram;
	std::shared_ptr<GFXPlatformShader> mpCurrentComputeShader;
	T3EffectParameterBoundState::BufferBinding mBufferBinding[eEffectParameter_UniformBufferCount];
	T3EffectParameterBoundState::GenericBufferBinding mGenericBufferBinding[eEffectParameter_GenericBufferCount];
	T3EffectParameterBoundState::TextureBinding mTextureBinding[eEffectParameter_SamplerCount];
	T3RenderTargetID mBoundOutputTargetIDs[eEffectParameter_SamplerCount];
	u32 mBoundOutputTargetCount;
	u32 mMaterialSamplerMipBias;
};

namespace T3EffectParameterUtil {

	inline void ResetBinding(T3EffectParameterBoundState& state, RenderFrameStats& stats){
		memset(state.mBufferBinding, 0, sizeof(T3EffectParameterBoundState::BufferBinding) * eEffectParameter_UniformBufferCount);
		T3TextureView empty{};
		T3BufferView emptyBuf{};
		for (int i = 0; i < eEffectParameter_SamplerCount; i++) {
			if (state.mTextureBinding[i].mpTexture) {
				TelltaleToolLib_GetRenderAdaptersStruct()->BindTexture(state, stats, (T3EffectParameterType)(eEffectParameter_FirstSampler + i), 0, empty);
				state.mTextureBinding[i].mView = empty;
			}
		}
		for (int i = 0; i < eEffectParameter_GenericBufferCount; i++) {
			if (state.mGenericBufferBinding[i].mpBuffer) {
				TelltaleToolLib_GetRenderAdaptersStruct()->BindGenericBuffer(state, stats, (T3EffectParameterType)(eEffectParameter_FirstGenericBuffer + i), 0, emptyBuf);
				state.mGenericBufferBinding[i].mView = emptyBuf;
			}
		}
	}

	inline void BindEffectParameters(T3EffectParameterBoundState& boundState, RenderFrameStats& stats, T3EffectParameterGroupStack& paramStack,
		T3EffectParameterGroupStack& pBaseParameters, BitSet<enum T3EffectParameterType, 150, 0>& paramTypes, T3RenderTargetList& targetList){
		BitSet<enum T3EffectParameterType, 150, 0> paramsToSet = paramTypes;
		BindParameterStack(boundState, stats, paramsToSet, paramStack, targetList);
		BindParameterStack(boundState, stats, paramsToSet, pBaseParameters, targetList);
		
		//Now bind defaults
		for(int i = 0; i < 150; i++){
			if(paramsToSet[(T3EffectParameterType)i]){
				T3EffectParameterClass clazz = sBufferDesc[i].mClass;
				if(clazz == eEffectParameterClass_GenericBuffer){
					std::shared_ptr<T3GFXBuffer> pBuffer = T3::spDefaultGenericBuffer[sBufferDesc[i].mDefaultGenericBufferType];
					T3BufferView empty{};
					TelltaleToolLib_GetRenderAdaptersStruct()->BindGenericBuffer(boundState, stats, (T3EffectParameterType)i, pBuffer.get(), empty);
				}else if(clazz == eEffectParameterClass_Sampler){
					std::shared_ptr<T3Texture> pTexture{};
					T3EffectParameterDefaultTextureType type = sBufferDesc[i].mDefaultTextureType;
					if (type == eEffectParameterDefaultTexture_Black)
						pTexture = T3::spDefaultTexture[eDefaultTextureType_Black].mpTexture;
					else if (type == eEffectParameterDefaultTexture_Black3D)
						pTexture = T3::spDefaultTexture[eDefaultTextureType_Black3D].mpTexture;
					else if(type == eEffectParameterDefaultTexture_BlackCube)
						pTexture = T3::spDefaultTexture[eDefaultTextureType_BlackCubeMap].mpTexture;
					else if(type == eEffectParameterDefaultTexture_White)
						pTexture = T3::spDefaultTexture[eDefaultTextureType_White].mpTexture;
					else if(type == eEffectParameterDefaultTexture_Lightmap)
						pTexture = T3::spDefaultTexture[eDefaultTextureType_DefaultLightmap].mpTexture;
					else if (type == eEffectParameterDefaultTexture_BoneMatrix)
						pTexture = T3::spDefaultTexture[eDefaultTextureType_DefaultBoneMatrix].mpTexture;
					T3TextureView empty{};
					TelltaleToolLib_GetRenderAdaptersStruct()->BindTexture(boundState, stats, (T3EffectParameterType)i, pTexture.get(), empty);
				}else if(clazz == eEffectParameterClass_UniformBuffer){
					TelltaleToolLib_GetRenderAdaptersStruct()->BindBuffer_0(boundState, stats, (T3EffectParameterType)i,
						sBufferDesc[i].mpDefaultBuffer, sBufferDesc[i].mScalarSize);
				}
			}
		}
	};

}

inline void BindParameterStack(T3EffectParameterBoundState& boundState, RenderFrameStats& stats, BitSet<enum T3EffectParameterType, 150, 0>& paramsToSet, T3EffectParameterGroupStack& pParameters, T3RenderTargetList& targetList){
	T3EffectParameterGroupStack* currentParam = &pParameters;
	while(currentParam){
		if(currentParam->mpGroup){
			T3EffectParameterGroup* pGroup = currentParam->mpGroup;
			_T3EffectBufferParam* params = (_T3EffectBufferParam*)pGroup + 1;
			for(i32 paramIndex = pGroup->mParameterCount - 1; paramIndex >= 0; paramIndex--){
				u64* paramData = (u64*)((u8*)pGroup + ((u32)params[paramIndex].off << 2));
				if(params[paramIndex].storage == eEffectParameterStorage_UniformBuffer){
					T3EffectParameterBoundState::BufferBinding* data = (T3EffectParameterBoundState::BufferBinding*)paramData;
					TelltaleToolLib_GetRenderAdaptersStruct()->BindBuffer(boundState, stats,
						(T3EffectParameterType)params[paramIndex].type, data->mpBuffer, data->mBufferOffset);
					if (paramData[0])
						paramsToSet.Set((T3EffectParameterType)params[paramIndex].type, 0);
				}
				else if (params[paramIndex].storage == eEffectParameterStorage_UniformBufferData) {
					T3EffectParameterBoundState::BufferBinding* data = (T3EffectParameterBoundState::BufferBinding*)paramData;
					TelltaleToolLib_GetRenderAdaptersStruct()->BindBuffer_0(boundState, stats,
						(T3EffectParameterType)params[paramIndex].type, data->mpBufferData, data->mBufferDataSize);
					paramsToSet.Set((T3EffectParameterType)params[paramIndex].type, 0);
				}else if (params[paramIndex].storage == eEffectParameterStorage_GenericBuffer) {
					T3EffectParameterBoundState::GenericBufferBinding* data = (T3EffectParameterBoundState::GenericBufferBinding*)paramData;
					TelltaleToolLib_GetRenderAdaptersStruct()->BindGenericBuffer(boundState, stats,
						(T3EffectParameterType)params[paramIndex].type, data->mpBuffer, data->mView);
					if (paramData[0])
						paramsToSet.Set((T3EffectParameterType)params[paramIndex].type, 0);
				}
				else if (params[paramIndex].storage == eEffectParameterStorage_Texture) {
					T3EffectParameterBoundState::TextureBinding* data = (T3EffectParameterBoundState::TextureBinding*)paramData;
					TelltaleToolLib_GetRenderAdaptersStruct()->BindTexture(boundState, stats,
						(T3EffectParameterType)params[paramIndex].type, data->mpTexture, data->mView);
					if (paramData[0])
						paramsToSet.Set((T3EffectParameterType)params[paramIndex].type, 0);
				}else{
					//render targets
					std::shared_ptr<T3Texture> pRTTex{};
					u64* pRawView = paramData + 1;
					T3TextureView view = *((T3TextureView*)(paramData + 0));
					T3RenderTargetID id = *((T3RenderTargetID*)(paramData + 1));
					if (params[paramIndex].storage == eEffectParameterStorage_RenderTarget)
						pRTTex = T3RenderTargetUtil::GetInputTarget(targetList, id);
					else
						pRTTex = T3RenderTargetUtil::GetOutputTarget(targetList, id, boundState.mFrameIndex);
					boundState.mBoundOutputTargetIDs[boundState.mBoundOutputTargetCount++] = id;
					TelltaleToolLib_GetRenderAdaptersStruct()->BindTexture(boundState, stats,
						(T3EffectParameterType)params[paramIndex].type, pRTTex.get(), view);
					if(pRTTex)
						paramsToSet.Set((T3EffectParameterType)params[paramIndex].type, 0);
				}
			}
		}
		currentParam = currentParam->mpParent;
	}
};

struct alignas(16) T3EffectParameter_BufferCamera
{
	Matrix4 mViewProjMatrix;
	Vector4 mTransposeViewZ;
	float mHFOVParam;
	float mVFOVParam;
	float mCameraNear;
	float mCameraFar;
	float mDecalFadeScale;
	float mDecalFadeBias;
	float mPadding0;
	float mPadding1;
	float mExposure;
	float mExposedColorToBuffer;
	float mBufferToExposedColor;
	float mAspectRatio;
	Vector2 mUnprojectDepthParams;
	Vector2 mGameSize;
	Vector2 mBackbufferSize;
	Vector2 mBackbufferInvSize;
	Vector3 mCameraPosition;
	float mSharpDetailFilter;
	Vector4 mViewport;
	Matrix4 mProjectionMatrix;
	Matrix4 mViewMatrix;
	Matrix4 mInvViewMatrix;
	Matrix4 mInvViewProjMatrix;
	Matrix4 mPrevViewProjMatrix;
	Matrix4 mViewToPrevViewMatrix;
	Vector2 mCameraFilterSize;
	Vector2 mRelativePresentationScale;
	Vector4 mClipPlanes[4];
	Vector4 mJitterOffset;
	Vector2 mHBAOBufferParams;
	float mInvertedDepth;
	float mPostEnabled;
	int mViewportInt[4];
	u32 mTargetSampleCount;
};

struct alignas(4) T3EffectParameter_BufferSkinning
{
	u32 mBoneMatrixLocation[4];
	Vector4 mBoneMatrixLocationf;
};

struct alignas(4) T3EffectParameter_BufferInstance
{
	Vector4 mLightmapTransform;
	Vector4 mTexCoordTransform0;
	Vector4 mTexCoordTransform1;
	Vector4 mTexCoordTransform2;
	Vector4 mTexCoordTransform3;
	Vector3 mPositionScale;
	float mShadowDepthBias;
	Vector3 mPositionOffset;
	float mLightmapIntensity;
	Vector3 mPositionWScale;
	float mLightmapPageIndex;
};

struct alignas(4) T3EffectParameter_BufferMaterialTool
{
	Color mSelectedColor;
};

struct alignas(16) T3EffectParameter_BufferShadow
{
	Vector3 mShadowDirection;
	float mShadowIntensity;
	Vector3 mShadowPosition;
	float mFilterRadius;
	Color mShadowColor;
	Color mGutterColor;
	Vector4 mShadowMatrix0[4];
	Vector4 mShadowMatrix1[4];
	Vector4 mShadowMatrix2[4];
	Vector4 mShadowMatrix3[4];
	Matrix4 mShadowViewMatrix;
	Vector4 mShadowViewport;
	Vector4 mPCFKernel[4];
	Vector4 mTextureMatrix0;
	Vector2 mTextureMatrix1;
	float mShadowDistanceScale;
	float mShadowDistanceBias;
	Vector3 mSceneMin;
	float mShadowCascadeSplitLambda;
	Vector3 mSceneMax;
	float mShadowCascadeCount;
	float mShadowMinDepth;
	float mShadowMaxDepth;
	float mShadowSoftness;
	float mShadowInvRadius;
	float mShadowDistanceFalloff;
	float mShadowInvDeltaCosConeAngle;
	float mShadowModulatedShadowIntensity;
	float mShadowMomentBias;
	Vector2 mProjectedShadowDirection;
	float mShadowSDSMTexelScale;
	float mShadowSDSMTexelBias;
	Vector3 mShadowGridCellSize;
	float mShadowCosConeAngle;
	float mShadowMapResolution;
	float mShadowMapInvResolution;
	float mShadowMapResolution1;
	float mShadowMapInvResolution1;
	u32 mShadowArrayIndex;
	u32 mShadowGoboIndex;
	u32 mShadowMapTexelMax;
	u32 mShadowMultisampleCount;
	u32 mLightGroupMask;
	u32 mLightTileIndex;
};

struct alignas(4) T3EffectParameter_BufferParticle
{
	Vector2 mTextureSheetParams;
	Vector2 mParticlePivotParams;
	Vector2 mLinearDepthAlphaScaleBias;
	float mAlphaRef;
	float mPadding0;
};

struct alignas(4) T3EffectParameter_BufferBrush
{
	float mBrushNearDetail;
	float mBrushFarDetail;
	float mBrushFar;
	float mBrushInvFar;
	float mBrushOutlineThreshold;
	float mBrushOutlineColorThreshold;
	float mBrushOutlineFalloff;
	float mBrushOutlineScale;
	float mBrushNearScale;
	float mBrushFarScale;
	float mBrushFarRamp;
	float mBrushFarMaxScale;
	Vector2 mBrushFilterSize;
	Vector2 mBrushFilterMax;
	float mBrushOutlineSize;
	float mBrushFarScaleBoost;
};

struct alignas(4) T3EffectParameter_BufferGaussian
{
	Vector4 mRow0;
	Vector4 mRow1;
	Vector4 mRow2;
	Vector4 mRow3;
	Vector4 mRow4;
};

struct alignas(4) T3EffectParameter_BufferSceneDebug
{
	Color mAlphaOverride;
	Vector4 mIntensityRange;
	RenderDebugMode mDebugMode;
};

struct alignas(4) T3EffectParameter_BufferDecal
{
	Vector4 mWorldToObject[3];
	float mNormalThreshold;
	float mDoubleSided;
	float mNormalOpacity;
};

struct alignas(4) T3EffectParameter_BufferMesh
{
  u32 mVertexCount;
  u32 mDstVertexCapacity;
  u32 mNormalCount;
  u32 mSrcVertexStride;
  u32 mDstVertexStride;
  u32 mSrcUVVertexStride;
  i32 mNormal0SrcOffset;
  i32 mNormal1SrcOffset;
  i32 mNormal2SrcOffset;
  i32 mTexCoord0SrcOffset;
  i32 mTexCoord1SrcOffset;
  i32 mColor0SrcOffset;
  i32 mColor1SrcOffset;
  i32 mNormal0DstOffset;
  i32 mNormal1DstOffset;
  i32 mNormal2DstOffset;
  i32 mTexCoord0DstOffset;
  i32 mTexCoord1DstOffset;
  i32 mColor0DstOffset;
  i32 mColor1DstOffset;
};

struct alignas(4) T3EffectParameter_BufferMeshBatch
{
	u32 mBatchIndex;
	u32 mStartIndex;
	u32 mNumPrimitives;
	u32 mAdjacencyStartIndex;
	u32 mBaseIndex;
};

struct alignas(4) T3EffectParameter_BufferMeshDynamicBatch
{
	u32 mLineGenerationBatchIndex;
	u32 mMeshBatchVertexOffset;
	u32 mMeshBatchVertexCapacity;
};

struct alignas(4) T3EffectParameter_BufferMeshDebugBatch
{
	Color mBatchDebugColor;
};

struct alignas(4) T3EffectParameter_BufferLights
{
	Vector4 mLightMinDistance;
	Vector4 mLightInvDeltaDistance;
	Vector4 mLightPositionX;
	Vector4 mLightPositionY;
	Vector4 mLightPositionZ;
	Color mSHParam0;
	Color mSHParam123[3];
	Color mSHParam45678[5];
	Color mSHMin;
	Vector3 mPrimaryShadowDirection;
	float mPadding0;
	Vector4 mLightWrapAroundA;
	Vector4 mLightWrapAroundB;
	Vector4 mLightToonOpacity;
	Vector4 mLightAmbientOcclusion;
	Vector4 mLightColorCorrection;
	Color mLight0Color;
	Color mLight0SpecularColor;
	Vector3 mPrimaryRimDirection;
	float mPrimaryRimWrapAround;
	Vector3 mPrimaryRimColor;
	float mPrimaryRimOcclusion;
	Vector4 mLightColorR;
	Vector4 mLightColorG;
	Vector4 mLightColorB;
	Vector4 mLightSpecularColorR;
	Vector4 mLightSpecularColorG;
	Vector4 mLightSpecularColorB;
	Vector4 mLightRimColorR;
	Vector4 mLightRimColorG;
	Vector4 mLightRimColorB;
	Vector4 mLightRimWrapAround;
	Vector4 mLightRimOcclusion;
	Vector4 mReflectionSHParams;
	Vector3 mLightmapSpecularPosition;
	float mLightmapSpecularIntensity;
	Vector4 mCellBandColors0[4];
	Vector4 mCellBandColors1[4];
	Vector4 mCellBandColors2[4];
	Vector4 mCellBandColors3[4];
	Vector4 mCellBlendMode;
	Vector4 mCellLayerWeight;
	Vector4 mLightEnabled;
	Vector4 mLightIntensity;
	Vector4 mLightBlendMask;
	Vector4 mNPRSpecularIntensity;
};

struct alignas(4) T3EffectParameter_BufferHBAO
{
	Vector4 mUVToView;
	Vector4 mBaseColor;
	Vector4 mTargetScale;
	Vector4 mFullRes_TexelSize;
	Vector4 mLayerRes_TexelSize;
	Vector4 mJitter;
	Vector4 mHBAOScreenParams;
	Vector4 mProjectionParams;
	Vector2 mDeinterleaving_Offset00;
	Vector2 mDeinterleaving_Offset10;
	Vector2 mDeinterleaving_Offset01;
	Vector2 mDeinterleaving_Offset11;
	Vector2 mLayerOffset;
	float mDistanceFalloff;
	float mHBAODepthRange;
	float mRadius;
	float mMaxRadiusPixels;
	float mNegInvRadius2;
	float mAngleBias;
	float mAOmultiplier;
	float mIntensity;
	float mNoiseTexSize;
	float mBlurSharpness;
	float mColorBleedSaturation;
	float mAlbedoMultiplier;
	float mLuminanceInfluence;
	float mMaxDistance;
};

template<u32 Num>
struct alignas(4) T3EffectParameter_BufferLightEnvDataBase
{
	u32 mPositionData[Num][4];
	u32 mDirectionData[Num][4];
	u32 mColorData[Num][4];
	u32 mShadowViewportData[Num][4];
	u32 mShadowMatrixData[Num*2][4];
};

struct T3EffectParameter_BufferLightEnvData_High : T3EffectParameter_BufferLightEnvDataBase<256>
{
};

struct T3EffectParameter_BufferLightEnvData_Medium : T3EffectParameter_BufferLightEnvDataBase<256>
{
};

struct T3EffectParameter_BufferLightEnvData_Low : T3EffectParameter_BufferLightEnvDataBase<1>
{
};

struct alignas(4) T3EffectParameter_BufferLightEnvView
{
	float mLightGridDepthScale;
	float mLightGridDepthBias;
	float mLightInvZBinCellSize;
	float mPadding0;
	u32 mLightGridLog2TileSize;
	u32 mLightGridRowPitch;
	u32 mLightGridSlicePitch;
	u32 mLightGridCellPitch;
	u32 mLightGridDepthSliceMax;
	u32 mLightZBinGroupStride;
	u32 mLightComputeGroupToTileShift;
};

struct alignas(4) T3EffectParameter_BufferShadowVolume
{
	Vector4 mTextureScale[3];
	Vector4 mTextureBias[3];
	Vector4 mMatrixData[9];
	u32 mVolumeCount;
};

struct alignas(4) T3EffectParameter_BufferLightEnv
{
	Vector3 mKeyLightColor;
	float mKeyLightDiffuseIntensity;
	float mKeyLightWrap;
	float mKeyLightSpecularIntensity;
	float mKeyLightOpacity;
	float mPadding0;
	Vector3 mKeyLightDirection;
	float mKeyLightModulatedShadowIntensity;
	float mKeyLightShadowFadeScale;
	float mKeyLightShadowFadeBias;
	float mKeyLightShadowMaxDistance;
	float mKeyLightHBAOParam;
	Vector3 mFogColor;
	float mFogStartDistance;
	float mFogMinOpacity;
	float mFogHeightFalloff;
	float mFogDensity;
	float mEnlightenIntensity;
	Vector4 mKeyLightShadowMatrices[12];
	u32 mLightCount;
};

struct alignas(0x10) T3EffectParameter_BufferScene
{
	Color mColorPlane;
	Color mFogColor;
	Vector3 mShadowColor;
	float mBrushMipBias;
	Vector2 mFogParams;
	float mShadowLightBleedReduction;
	float mShadowTraceMinDistance;
	Vector3 mShadowMomentDepthBias;
	float mShadowTraceInvDeltaDistance;
	Vector4 mTetrahedralMatrices[12];
	float mShadowAtlasResolution;
	float mShadowAtlasInvResolution;
	float mGraphicBlackAlpha;
	float mTAAWeight;
	float mGraphicBlackThreshold;
	float mGraphicBlackSoftness;
	float mGraphicBlackNear;
	float mGraphicBlackFar;
	Vector3 mWindDirection;
	float mWindSpeed;
	float mWindIdleStrength;
	float mWindIdleSpacialFrequency;
	float mWindGustSpeed;
	float mWindGustStrength;
	float mWindGustSpacialFrequency;
	float mWindGustIdleStrengthMultiplier;
	float mTime;
	float mPrevTime;
	float mWindGustSeparationExponent;
	float mHDR_PaperWhiteNits;
	float mNoiseScale;
	float mShadowInvCascadeCount;
	float mHDRRange;
	float mGlobalLevelScalar;
	float mGlobalLevelAdder;
	float mGlobalLevelIntensity;
	Matrix4 mLineSamplingOffsets;
	float mNPRLineAlphaFalloff;
	float mNPRLineAlphaBias;
	float mNPRLineFalloff;
	float mNPRLineBias;
	u32 mMSAASampleCount;
	u32 mNoiseFrameIndex0;
	u32 mNoiseFrameIndex1;
};

struct alignas(4) T3EffectParameter_BufferPost
{
	Vector4 mFxaaConsoleRcpFrameOpt;
	Vector4 mFxaaConsoleRcpFrameOpt2;
	Vector4 mFxaaConsole360RcpFrameOpt2;
	Vector4 mFxaaConsole360ConstDir;
	Vector4 mTonemapParams;
	Vector4 mTonemapToeCoeff;
	Vector4 mTonemapShoulderCoeff;
	Vector4 mTonemapFarToeCoeff;
	Vector4 mTonemapFarShoulderCoeff;
	float mTonemapCutoff;
	float mTonemapFarCutoff;
	float mBloomThreshold;
	float mBloomIntensity;
	float mLevelScalar;
	float mLevelAdder;
	float mLevelIntensity;
	float mPad0;
	float mDofNear;
	float mDofFar;
	float mDofNearRamp;
	float mDofFarRamp;
	float mDofNearMax;
	float mDofFarMax;
	float mDofVignetteMax;
	float mDofResolutionFraction;
	float mDofNearBlurRadiusPixels;
	float mDofInvNearBlurRadiusPixels;
	float mZRange;
	float mDofMaxCocRadiusPixels;
	float mDofFarRadiusRescale;
	float mDofDebug;
	float mDofNearBlurryPlaneZ;
	float mDofNearSharpPlaneZ;
	float mDofFarSharpPlaneZ;
	float mDofFarBlurryPlaneZ;
	float mDofNearScale;
	float mDofFarScale;
	Color mVignetteTint;
	Color mRadialBlurTint;
	float mRadialBlurInRadius;
	float mRadialBlurInvDeltaRadius;
	float mRadialBlurIntensity;
	float mRadialBlurScale;
	Vector4 mTonemapRGBToeCoeffs[4];
	Vector4 mTonemapRGBShoulderCoeffs[4];
	Vector4 mTonemapRGBCutoffs;
	Vector4 mTonemapRGBFarCutoffs;
	Vector4 mTonemapRGBFarToeCoeffs[4];
	Vector4 mTonemapRGBFarShoulderCoeffs[4];
	Vector3 mTonemapRGBWhitePoint;
	float mTonemapWhitePoint;
	Vector3 mTonemapRGBFarWhitePoint;
	float mTonemapFarWhitePoint;
	float mVignetteStart;
	float mVignetteRange;
	float mVignetteFalloff;
	float mPad1;
	float mMotionBlurIntensity;
	float mDofFarResolutionFraction;
	float mDofCoverageBoost;
	float mPad2;
	float mDofFarBlurRadius;
	float mZNearClip;
	float mDofStrength;
	float mBokehBrightnessDeltaThreshold;
	float mBokehBrightnessThreshold;
	float mBokehBlurThreshold;
	float mBokehMinSize;
	float mBokehMaxSize;
	float mBokehFalloff;
	float mBokehDomainSizeX;
	float mBokehDomainSizeY;
	int mMaxBokehVertices;
	Vector3 mBokehAberrationOffsetsX;
	float mForceLinearDepthOffset;
	Vector3 mBokehAberrationOffsetsY;
	float mPad3;
	Color mSSLineColor;
	float mSSLineDepthMagnitude;
	float mSSLineDepthExponent;
	float mSSLineDepthFadeNear;
	float mSSLineDepthFadeFar;
	Vector3 mSSLineLightDirection;
	float mSSLineLightMagnitude;
	float mSSLineThickness;
	float mSSLineLightExponent;
	int mSSLineDebug;
};

struct alignas(4) T3EffectParameter_BufferSimple
{
	Color mColor;
	Vector4 mClipPlane;
	Vector4 mTextureMatrix0;
	Vector3 mBoundsMin;
	float mScreenSpaceDepthScale;
	Vector3 mBoundsMax;
	float mScreenSpaceDepthOffset;
	Vector2 mTextureMatrix1;
	float mVisibilityThreshold;
	float mPadding2;
	Vector4 mPrimitiveParams;
	u32 mCopyOffset[2];
	u32 mCopySize[2];
	u32 mIndex;
};

struct alignas(4) T3EffectParameter_BufferObject
{
	Vector4 mWorldMatrix[3];
	Vector4 mPrevWorldMatrix[3];
	Vector3 mObjectScale;
	float mAlpha;
	Vector3 mDiffuseColor;
	float mRimBumpDepth;
	Vector3 mReflectionColor;
	float mReflectionMipCount;
	Vector4 mEnlightenUVTransform;
	Vector3 mReflectionMatrix0;
	float mReflectionIntensityShadow;
	Vector3 mReflectionMatrix1;
	float mBrushNearDetailBias;
	Vector3 mReflectionMatrix2;
	float mReceiveShadowsIntensity;
	Vector3 mAnimatedPositionScale;
	float mMaterialTime;
	Vector3 mAnimatedPositionBias;
	float mMaterialPrevTime;
	Vector3 mAnimatedPositionInvScale;
	float mBrushFarDetailBias;
	Vector3 mAnimatedPositionInvBias;
	float mBrushScale;
	Vector3 mPrevAnimatedPositionScale;
	float mBrushScaleByVertex;
	Vector3 mPrevAnimatedPositionBias;
	float mPadding0;
	unsigned int mStencilRef;
	unsigned int mStencilMask;
};

struct alignas(4) T3EffectParameter_BufferLightCin
{

	struct alignas(4) LightParams
	{
		Vector3 mColor;
		float mWrap;
		float mDiffuseIntensity;
		float mSpecularIntensity;
		float mShadowIntensity;
		float mEnable;
		Vector3 mDirection;
		float mHBAOWeight;
	};

	LightParams mLightParams[3];
	float mLightEnvIntensity;
	float mEnlightenIntensity;
	float mEnlightenSaturation;
	float mPadding0;
};

struct alignas(4) T3EffectParameter_BufferLightAmbient
{
	Vector4 mLightDiffuseSHR;
	Vector4 mLightDiffuseSHG;
	Vector4 mLightDiffuseSHB;
	Vector4 mLightSpecularSHR;
	Vector4 mLightSpecularSHG;
	Vector4 mLightSpecularSHB;
};


struct LightInstance
{
	Transform mWorldTransform;
	LightType mLightType;
	float mIntensity;
	float mDimmer;
	float mToonPriority;
	float mToonOpacity;
	float mColorCorrection;
	float mDiffuseIntensity;
	float mSpecularIntensity;
	float mRimIntensity;
	float mAmbientOcclusion;
	int mRenderLayer;
	float mSpotInnerRadius;
	float mSpotOuterRadius;
	float mSpotAlpha;
	Handle<T3Texture> mSpotTexture;
	T3BlendMode mSpotBlendMode;
	Vector2 mSpotTextureTranslate;
	Vector2 mSpotTextureScale;
	Vector2 mSpotTextureShear;
	Vector2 mSpotTextureShearOrigin;
	float mSpotTextureRotate;
	Vector2 mSpotTextureRotateOrigin;
	bool mbOnOff;
	bool mbDynamicOnLightMap;
	bool mbIsKeyLight;
	bool mbWorldTransformDirty;
	bool mbStatic;
	Ptr<Agent> mpAgent;
	Color mColor;
	Color mDarkColor;
	Color mCachedFinalColor;
	Color mCellBand0Color;
	Color mCellBand1Color;
	Color mCellBand2Color;
	Color mCellBand3Color;
	float mNPRSpecularIntensity;
	EnumLightCellBlendMode mCellBlendMode;
	float mCellBlendWeight;
	float mMaxDistance;
	float mMinDistance;
	float mWrapAround;
	float mRimWrapAround;
	float mRimOcclusion;
	float mShadowMaxDistance;
	float mShadowDistanceFalloff;
	int mShadowCascadeCount;
	float mShadowBias;
	Set<Symbol, std::less<Symbol>> mGroups;
	float mLightBlendMask;
};

struct LightGroup;

struct LightGroupInstance
{

	struct PointLightEntry
	{
		std::shared_ptr<LightInstance> mpLight;
		float mFade;
		float mBakeTime;
		bool mbIsBaked;
	};

	struct SHLightEntry
	{
		std::shared_ptr<LightInstance> mpLight;
		float mBakeTime;
		bool mbIsBaked;
	};

	std::shared_ptr<T3EffectParameterBuffer> mpNonLightmapParameterBuffer;
	std::shared_ptr<T3EffectParameterBuffer> mpLightmapParameterBuffer;
	BitSet<enum T3EffectFeature, 30, 54> mLightmapFeatures;
	BitSet<enum T3EffectFeature, 30, 54> mNonLightmapFeatures;
	PointLightEntry mPointLights[4];
	std::vector<LightGroupInstance::SHLightEntry> mSHPointLights;
	std::vector<LightGroupInstance::SHLightEntry> mDirectionalLights;
	std::shared_ptr<LightInstance> mpAmbientLight;
	std::shared_ptr<LightInstance> mpShadowLight;
	std::shared_ptr<LightInstance> mpLightmapLight;
	LightGroup* mpLightGroup;
	Symbol mLightingGroup;
	Sphere mBoundingSphere;
	BoundingBox mBoundingBox;
	unsigned int mRenderDirty;
	bool mbBakeDirty;
	bool mbDirty;
	bool mbHasNormalmap;
	bool mbHasToonLighting;
};

struct SortedLightEntry {
	float mIntensity;
	int mIndex;
};

struct LightGroup {

	struct SortedLightEntry
	{
		std::shared_ptr<LightInstance> mpLight;
		float mPriority;
		bool mbKeyLight;
	};

	Scene* mpScene;
	Symbol mName;
	Flags mTypeFlags;
	std::vector<std::shared_ptr<LightInstance>> mLights;
	//Set<Ptr<RenderObject_Mesh>, std::less<Ptr<RenderObject_Mesh> > > mRenderObjects[3];
	std::vector<LightGroupInstance> mInstances;
	i32 mShadowLayerIndex;
	i32 mShadowLayerStencilValue;

};


struct T3EffectLightCinParams
{
	RenderQualityType mRenderQuality;
	T3LightCinematicParams mLightParams[3];
	T3LightCinematicRigParams mLightRigParams;
};

struct T3EffectHBAOParams
{
	Vector4 mUVToView;
	Vector4 mBaseColor;
	Vector4 mTargetScale;
	Vector4 mFullRes_TexelSize;
	Vector4 mLayerRes_TexelSize;
	Vector4 mJitter;
	Vector4 mHBAOScreenParams;
	Vector4 mProjectionParams;
	Vector2 mDeinterleaving_Offset00;
	Vector2 mDeinterleaving_Offset10;
	Vector2 mDeinterleaving_Offset01;
	Vector2 mDeinterleaving_Offset11;
	Vector2 mLayerOffset;
	float mDistanceFalloff;
	float mHBAODepthRange;
	float mRadius;
	float mMaxRadiusPixels;
	float mNegInvRadius2;
	float mAngleBias;
	float mAOmultiplier;
	float mIntensity;
	float mNoiseTexSize;
	float mBlurSharpness;
	float mColorBleedSaturation;
	float mAlbedoMultiplier;
	float mLuminanceInfluence;
	float mMaxDistance;
};

struct T3EffectLightParams
{
	Matrix4 mObjToWorld;
	Color mAmbientColor;
	Color mLightProbeTint;
	float mLightProbeIntensity;
	float mLightProbeSaturation;
	LightProbeState* mpLightProbeState;
	bool mbLightmapped;
};

struct T3EffectParameter_LightEnvDataPointers
{
	//all are xxx[.][4]
	u32** mPositionData;
	u32** mDirectionData;
	u32** mColorData;
	u32** mShadowViewportData;
	u32** mShadowMatrixData;
	u32 mMaxLightCount;
};

struct T3EffectMaterialToolParams
{
	Color mSelectedColor;
};

struct T3EffectGaussianParams
{
	float mKernelWidth;
	float mSigmaSquared;
};

struct __declspec(align(16)) T3EffectObjectParams
{
	Matrix4 mInvWorldMatrix;
	Matrix4 mPrevWorldMatrix;
	Matrix4 mReflectionMatrix;
	Vector3 mObjectScale;
	BoundingBox mAnimatedBoundingBox;
	BoundingBox mPrevAnimatedBoundingBox;
	Color mDiffuseColor;
	float mAlpha;
	float mRimBumpDepth;
	float mBrushScale;
	float mBrushNearDetailBias;
	float mBrushFarDetailBias;
	float mMaterialTime;
	float mMaterialPrevTime;
	float mReceiveShadowsIntensity;
	Color mReflectionTint;
	float mReflectionIntensity;
	int mReflectionTextureSize;
	float mReflectionHDR;
	float mReflectionIntensityShadow;
	Vector4 mEnlightenUVTransform;
	unsigned int mStencilRef;
	unsigned int mStencilMask;
	bool mbBrushForceEnable;
	bool mbBrushScaleByVertex;
};

struct T3EffectSimpleParams
{
	float mTextureMatrix[6];
	Color mColor;
	Plane mClipPlane;
	BoundingBox mBoundingBox;
	float mVisibilityThreshold;
	unsigned int mIndex;
	u32 mCopyOffset[2];
	u32 mCopySize[2];
	float mScreenSpaceDepthScale;
	float mScreenSpaceDepthOffset;
	unsigned int mNumSides;
	unsigned int mNumSlices;
	float mConeAngle;
	float mConeRadius;
};

struct T3EffectSceneDebugParams
{
	RenderDebugMode mDebugRenderMode;
	float mIntensityRangeMin;
	float mIntensityRangeMax;
};

enum T3EnvironmentFlags
{
	eEnvironmentFlag_Enabled = 0x1,
	eEnvironmentFlag_FogEnabled = 0x2,
	eEnvironmentFlag_EnabledOnHigh = 0x10000,
	eEnvironmentFlag_EnabledOnMedium = 0x20000,
	eEnvironmentFlag_EnabledOnLow = 0x40000,
};

struct T3LightEnvGroupParams
{
	int mPriority;
	float mEnlightenIntensity;
	unsigned int mLightGroupMask;
	Flags mFlags;
};

struct T3EnvironmentParams
{
	int mPriority;
	Color mFogColor;
	float mFogStartDistance;
	float mFogMaxOpacity;
	float mFogDensity;
	float mFogHeightFalloff;
	float mFogHeight;
	unsigned int mLightGroupMask;
	Flags mFlags;
};

struct T3LightEnvParams
{
	T3LightEnvType mType;
	T3LightEnvMobility mMobility;
	T3LightEnvShadowType mShadowType;
	T3LightEnvShadowQuality mShadowQuality;
	T3LightEnvLODBehavior mLODBehavior;
	T3LightEnvBakeOnStatic mBakeOnStatic;
	Transform mTransform;
	Symbol mShadowGobo;
	Color mColor;
	float mIntensity;
	float mDimmer;
	float mDiffuseIntensity;
	float mSpecularIntensity;
	float mNPRBanding;
	Vector3 mNPRBandThresholds;
	float mWrap;
	float mRadius;
	float mOpacity;
	float mDistanceFalloff;
	float mInnerConeAngle;
	float mOuterConeAngle;
	float mShadowSoftness;
	float mShadowModulatedIntensity;
	float mShadowNearClip;
	float mShadowDepthBias;
	Vector2 mShadowGoboScale;
	Vector2 mShadowGoboTranslate;
	float mVisibilityThresholdScale;
	int mPriority;
	unsigned int mLightGroupMask;
	Flags mFlags;
	HBAOParticipationType mHBAOParticipationType;
	float mShadowMapQualityDistanceScale;
	T3LightEnvEnlightenBakeBehavior mEnlightenBakeBehavior;
	float mEnlightenIntensity;
};

struct T3LightShadowMapCacheRef
{
	u32 mFrameUsed;
	u16 mAllocatedMask;
	u8 mArrayIndex;
	u8 mGridLevel;
	u8 mBlockOffsetX;
	u8 mBlockOffsetY;
	u8 mBlockSize;
};

struct T3LightEnvKeyShadowMap
{
	std::vector<u64> mStaticShadowCasters;
	std::shared_ptr<T3Texture> mpStaticShadowMap;
	T3LightShadowMapCacheRef mCacheRef[4];
	Matrix4 mWorldToShadow[4];
	BoundingBox mStaticShadowCasterBounds;
	float mValidCascadeMaxDistance;
	float mShadowMaxDistance;
	unsigned int mStaticShadowMapResolution;
	unsigned int mCascadeCount;
	unsigned int mUpdateCounter;
};

struct T3EffectSceneParams
{
	Color mFogColor;
	Color mColorPlane;
	Color mShadowColor;
	float mShadowMomentBias;
	float mShadowDepthBias;
	float mShadowPositionOffsetBias;
	float mShadowLightBleedReduction;
	float mShadowTraceMaxDistance;
	unsigned int mShadowAtlasResolution;
	unsigned int mShadowCascadeCount;
	float mFogNear;
	float mFogFar;
	bool mbAfterEffects;
	float mNPRLineFalloff;
	float mNPRLineBias;
	float mNPRLineAlphaFalloff;
	float mNPRLineAlphaBias;
	float mGlobalLevelWhite;
	float mGlobalLevelBlack;
	float mGlobalLevelIntensity;
	float mGraphicBlackAlpha;
	float mGraphicBlackThreshold;
	float mGraphicBlackSoftness;
	float mGraphicBlackNear;
	float mGraphicBlackFar;
	RenderWindParams mWindParams;
	float mTAAWeight;
	float mNoiseScale;
	float mTime;
	float mPrevTime;
	float mHDR_PaperWhiteNits;
};

struct T3EffectLightEnvParams
{
	std::shared_ptr<Camera> mpCamera;
	RenderQualityType mRenderQuality;
	T3LightEnvGroupParams mGroupParams;
	T3EnvironmentParams mEnvParams;
	T3LightEnvParams mKeyLightParams;
	Vector3 mKeyLightDirection;
	std::shared_ptr<T3LightEnvKeyShadowMap> mpKeyLightShadowMap;
	float mKeyLightShadowMaxDistance;
	Matrix4 mKeyLightShadowMatrix;
	u32 mLightGroupIndex;
	u32 mLightTileIndex;
	u32 mLightCount;
	bool mbUseFixedKeyLightShadowMatrix;
};

struct T3EffectInstanceParams
{
	T3MeshTexCoordTransform mLightmapTransform;
	T3MeshTexCoordTransform mTexCoordTransform0;
	T3MeshTexCoordTransform mTexCoordTransform1;
	T3MeshTexCoordTransform mTexCoordTransform2;
	T3MeshTexCoordTransform mTexCoordTransform3;
	Vector3 mPositionScale;
	Vector3 mPositionOffset;
	Vector3 mPositionWScale;
	float mLightmapIntensity;
	float mShadowDepthBias;
	u32 mLightmapPageIndex;
};

struct T3EffectPostParams
{
	float mLevelWhite;
	float mLevelBlack;
	float mLevelIntensity;
	float mTonemapIntensity;
	RenderTonemapFilmicParams mTonemapFilmicParams;
	RenderTonemapFilmicParams mTonemapFarFilmicParams;
	RenderTonemapFilmicRGBParams mFXTonemapRGBParams;
	RenderTonemapFilmicRGBParams mFXTonemapRGBFarParams;
	float mBloomThreshold;
	float mBloomIntensity;
	float mDofNear;
	float mDofFar;
	float mDofAutoFocusNear;
	float mDofAutoFocusFar;
	float mDofNearRamp;
	float mDofFarRamp;
	float mDofNearMax;
	float mDofFarMax;
	float mDofNearBlurRadiusPixels;
	float mDofInvNearBlurRadiusPixels;
	float mDofMaxCocRadiusPixels;
	float mDofFarRadiusRescale;
	float mDofDebug;
	float mZRange;
	float mZNearClip;
	float mDofNearBlurryPlaneZ;
	float mDofNearSharpPlaneZ;
	float mDofFarSharpPlaneZ;
	float mDofFarBlurryPlaneZ;
	float mDofNearScale;
	float mDofFarScale;
	float mBokehBrightnessThreshold;
	float mBokehBrightnessDeltaThreshold;
	float mBokehBlurThreshold;
	float mBokehMinSize;
	float mBokehMaxSize;
	float mBokehDomainSizeX;
	float mBokehDomainSizeY;
	float mBokehFalloff;
	float mDofVignetteMax;
	float mDofResolutionFraction;
	Color mVignetteTint;
	float mVignetteFalloff;
	float mVignetteStart;
	float mVignetteEnd;
	Color mRadialBlurTint;
	float mRadialBlurInRadius;
	float mRadialBlurOutRadius;
	float mRadialBlurIntensity;
	float mRadialBlurScale;
	float mCameraFar;
	float mCameraFocalLength;
	int mTargetWidth;
	int mTargetHeight;
	bool mbDOFFieldOfViewAdjustEnabled;
	float mMotionBlurIntensity;
	float mDofFarResolutionFraction;
	float mDofCoverageBoost;
	float mDofFarBlurRadius;
	Vector3 mBokehAberrationOffsetsX;
	Vector3 mBokehAberrationOffsetsY;
	int mMaxBokehVertices;
	float mForceLinearDepthOffset;
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
};

struct T3EffectShadowParams
{
	std::shared_ptr<Camera> mpSceneCamera;
	Matrix4 mShadowMatrix[4];
	Matrix4 mShadowViewMatrix;
	float mTextureMatrix[6];
	Vector3 mShadowPosition;
	Vector3 mShadowDirection;
	Vector4 mShadowViewport;
	Vector3 mSceneMin;
	Vector3 mSceneMax;
	Vector3 mShadowGridCellSize;
	Color mLightColor;
	Color mGutterColor;
	float mLightIntensity;
	float mLightRadius;
	Vector2 mLightSpotParams;
	float mLightDistanceFalloff;
	float mLightModulatedShadowIntensity;
	int mShadowMapResolution;
	int mShadowMapResolution1;
	float mShadowFadeMinDistance;
	float mShadowFadeMaxDistance;
	float mFilterRadius;
	float mShadowSoftness;
	float mShadowCascadeSplitLambda;
	float mShadowMinDepth;
	float mShadowMaxDepth;
	float mShadowMomentBias;
	u32 mShadowCascadeCount;
	u32 mShadowArrayIndex;
	u32 mShadowGoboIndex;
	u32 mLightGroupMask;
	u32 mLightTileIndex;
	bool mbStaticShadowMap;
	bool mbLightWorldSpace;
};

struct T3EffectParticleParams
{
	int mTextureSheetX;
	int mTextureSheetY;
	float mParticlePivotX;
	float mParticlePivotY;
	float mAlphaRef;
	Vector2 mLinearDepthAlphaScaleBias;
};

struct T3EffectBrushParams
{
	float mBrushOutlineSize;
	float mBrushOutlineThreshold;
	float mBrushOutlineColorThreshold;
	float mBrushOutlineFalloff;
	float mBrushOutlineScale;
	float mBrushFar;
	float mBrushFarRamp;
	float mBrushNearScale;
	float mBrushNearDetail;
	float mBrushFarScale;
	float mBrushFarDetail;
	float mBrushFarMaxScale;
	float mBrushFarScaleBoost;
	float mCameraFar;
	float mCameraFocalLength;
	float mHFOV;
	float mVFOV;
	int mTargetWidth;
	int mTargetHeight;
	bool mbDOFFieldOfViewAdjustEnabled;
};


struct T3EffectCameraParams
{
	T3RenderViewport mViewport;
	Matrix4 mViewMatrix;
	Matrix4 mPrevViewMatrix;
	Matrix4 mProjectionMatrix;
	Matrix4 mPrevProjectionMatrix;
	Vector3 mCameraPosition;
	RenderClipPlanes mClipPlanes;
	Vector2 mJitter;
	Vector2 mPrevJitter;
	float mNearClip;
	float mFarClip;
	float mHFOV;
	float mVFOV;
	float mAspectRatio;
	float mExposure;
	i32 mTargetWidth;
	i32 mTargetHeight;
	u32 mTargetSampleCount;
	bool mbAfterEffects;
	bool mbInvertedDepth;
};

namespace T3EffectCameraUtil {

	inline void GetParametersForCamera(T3EffectCameraParams& cameraParams, Scene* pScene, Camera* pCamera, bool flipY) {
		cameraParams.mViewMatrix = pCamera->GetViewMatrix();
		if (flipY)
			cameraParams.mViewMatrix = cameraParams.mViewMatrix * MatrixScaling(1.0f, -1.0f, 1.0f);
		cameraParams.mProjectionMatrix = pCamera->GetProjectionMatrix();
		if (pScene) {
			cameraParams.mPrevProjectionMatrix = pScene->mPrevCameraProjMatrix;
			cameraParams.mPrevViewMatrix = pScene->mPrevCameraViewMatrix;
		}
		else {
			cameraParams.mPrevViewMatrix = cameraParams.mViewMatrix;
			cameraParams.mPrevProjectionMatrix = cameraParams.mProjectionMatrix;
		}
		cameraParams.mCameraPosition = Vector3(pCamera->GetWorldMatrix().GetColumn(3));
		cameraParams.mNearClip = pCamera->mNearClip;
		cameraParams.mFarClip = pCamera->mFarClip;
		cameraParams.mAspectRatio = pCamera->mAspectRatio;
		cameraParams.mExposure = pCamera->mExposure;
		cameraParams.mbInvertedDepth = pCamera->IsInvertedDepth();
		pCamera->GetAdjustedFOV(cameraParams.mHFOV, cameraParams.mVFOV);
	}

	inline void SetParameters(T3EffectParameter_BufferCamera* pBuffer, const T3EffectCameraParams& cameraParams) {
		float invWidth = cameraParams.mTargetWidth <= 0 ? 0.f : 1.0f / (float)cameraParams.mTargetWidth;
		float invHeight = cameraParams.mTargetHeight <= 0 ? 0.f : 1.0f / (float)cameraParams.mTargetHeight;
		Matrix4 tmp = MatrixTranslation({ invWidth * cameraParams.mJitter.x,-invHeight * cameraParams.mJitter.y,0.f });
		pBuffer->mViewMatrix = cameraParams.mViewMatrix;
		pBuffer->mProjectionMatrix = cameraParams.mProjectionMatrix * tmp;
		float hbaoScale = sGlobalRenderConfig.mHBAOResolution == eHBAOResolutionFull ? 1.0f : sGlobalRenderConfig.mHBAOResolution == eHBAOResolutionHalf ? 0.5f : 0.25f;
		u32 w, h{};
		TelltaleToolLib_GetRenderAdaptersStruct()->GetResolution(w, h);
		pBuffer->mHBAOBufferParams = { (float)w * hbaoScale, (float)h * hbaoScale };
		pBuffer->mViewProjMatrix = pBuffer->mViewMatrix * pBuffer->mProjectionMatrix;
		pBuffer->mInvViewMatrix = pBuffer->mViewMatrix.Inverse();
		pBuffer->mInvViewProjMatrix = pBuffer->mViewProjMatrix.Inverse();
		pBuffer->mJitterOffset = Vector4(cameraParams.mJitter, cameraParams.mPrevJitter);
		pBuffer->mPrevViewProjMatrix = cameraParams.mPrevViewMatrix * cameraParams.mPrevProjectionMatrix;
		pBuffer->mViewToPrevViewMatrix = pBuffer->mInvViewMatrix * cameraParams.mPrevViewMatrix;
		pBuffer->mHFOVParam = 2 * tanf(0.5f * cameraParams.mHFOV);
		pBuffer->mVFOVParam = 2 * tanf(0.5f * cameraParams.mVFOV);
		pBuffer->mAspectRatio = cameraParams.mAspectRatio;
		pBuffer->mCameraNear = cameraParams.mNearClip;
		pBuffer->mCameraFar = cameraParams.mFarClip;
		pBuffer->mCameraPosition = cameraParams.mCameraPosition;
		float inv = pBuffer->mHFOVParam * pBuffer->mCameraFar * 800.f;
		if (inv > 0.f)
			inv = 1.0f / inv;
		pBuffer->mCameraFilterSize.x = inv;
		inv = pBuffer->mVFOVParam * pBuffer->mCameraFar * 800.f;
		if (inv > 0.f)
			inv = 1.0f / inv;
		pBuffer->mCameraFilterSize.y = inv;
		pBuffer->mViewport = Vector4((float)cameraParams.mViewport.mWidth * invWidth,
			(float)cameraParams.mViewport.mHeight * invHeight, (float)cameraParams.mViewport.mOriginX * invWidth,
			(float)cameraParams.mViewport.mOriginY * invHeight);
		pBuffer->mViewportInt[0] = cameraParams.mViewport.mWidth;
		pBuffer->mViewportInt[1] = cameraParams.mViewport.mHeight;
		pBuffer->mViewportInt[2] = cameraParams.mViewport.mOriginX;
		pBuffer->mViewportInt[3] = cameraParams.mViewport.mOriginY;
		pBuffer->mBackbufferSize = Vector2(cameraParams.mTargetWidth, cameraParams.mTargetHeight);
		pBuffer->mBackbufferInvSize = Vector2(invWidth, invHeight);
		pBuffer->mRelativePresentationScale = Vector2((float)cameraParams.mTargetWidth / 1280.f, (float)cameraParams.mTargetHeight / 710.f);
		pBuffer->mGameSize = Vector2(1.0f / ((float)w * sGlobalRenderConfig.mRenderScale), 1.0f / ((float)h * sGlobalRenderConfig.mRenderScale));
		float totDepth = pBuffer->mCameraFar - pBuffer->mCameraNear;
		pBuffer->mUnprojectDepthParams = Vector2((pBuffer->mCameraNear / totDepth), 1.0f - (pBuffer->mCameraFar / totDepth));
		pBuffer->mSharpDetailFilter = 200.f * invWidth * pBuffer->mCameraFar;
		(float)((sQualityEntry[sGlobalRenderConfig.mQuality].mDecalVisibleDistance / pBuffer->mCameraFar) - (float)((float)(sQualityEntry[sGlobalRenderConfig.mQuality].mDecalVisibleDistance - 2.0f) * (float)(1.0f / pBuffer->mCameraFar)));
		pBuffer->mDecalFadeScale = -1.0f / (float)((sQualityEntry[sGlobalRenderConfig.mQuality].mDecalVisibleDistance / pBuffer->mCameraFar) - (float)((float)(sQualityEntry[sGlobalRenderConfig.mQuality].mDecalVisibleDistance - 2.0f) * (float)(1.0f / pBuffer->mCameraFar)));
		pBuffer->mDecalFadeBias = (sQualityEntry[sGlobalRenderConfig.mQuality].mDecalVisibleDistance / pBuffer->mCameraFar) * -pBuffer->mDecalFadeScale;
		if (cameraParams.mbAfterEffects) {
			pBuffer->mExposure = powf(2.0f, cameraParams.mExposure) / sGlobalRenderConfig.mHDRColorBufferScale;
			pBuffer->mBufferToExposedColor = sGlobalRenderConfig.mHDRColorBufferScale;
			pBuffer->mExposedColorToBuffer = 1.0f / pBuffer->mBufferToExposedColor;
		}
		else {
			pBuffer->mExposure = powf(2.0f, cameraParams.mExposure);
			pBuffer->mBufferToExposedColor = pBuffer->mExposedColorToBuffer = 1.0f;
		}
		for (int i = 0; i < cameraParams.mClipPlanes.mCount; i++)
			pBuffer->mClipPlanes[i] = cameraParams.mClipPlanes.mPlanes[i].mPlane;
		for (int i = cameraParams.mClipPlanes.mCount; i < 4; i++)
			pBuffer->mClipPlanes[i] = Vector4(0.f, 0.f, 0.f, 1.0f);
		pBuffer->mInvertedDepth = cameraParams.mbInvertedDepth ? -1.0f : 1.0f;
		if (cameraParams.mbAfterEffects)
			pBuffer->mPostEnabled = 1.0f;
		else
			pBuffer->mPostEnabled = -1.f;
	}

}

namespace T3EffectBrushUtil {

	inline void SetParameters(T3EffectParameter_BufferBrush* pBuffer, const T3EffectBrushParams& params) {
		float fr = params.mCameraFar;
		if (fr < 0.00000f)
			fr = 0.00000f;
		pBuffer->mBrushOutlineSize = 2.f * params.mBrushOutlineSize;
		pBuffer->mBrushOutlineThreshold = 0.1f * params.mBrushOutlineThreshold;
		pBuffer->mBrushOutlineColorThreshold = params.mBrushOutlineColorThreshold;
		pBuffer->mBrushOutlineFalloff = params.mBrushOutlineFalloff;
		pBuffer->mBrushOutlineScale = params.mBrushOutlineScale;
		pBuffer->mBrushNearScale = params.mBrushNearScale;
		pBuffer->mBrushNearDetail = 10.f * params.mBrushNearDetail;
		pBuffer->mBrushFarDetail = 10.f * params.mBrushFarDetail;
		pBuffer->mBrushFarScaleBoost = 5.0f * params.mBrushFarScaleBoost;
		pBuffer->mBrushFarScale = params.mBrushFarScale;
		pBuffer->mBrushFarMaxScale = params.mBrushFarMaxScale;
		pBuffer->mBrushFar = params.mBrushFar / fr;
		float ramp = pBuffer->mBrushFar;
		if (params.mbDOFFieldOfViewAdjustEnabled) {
			float div = params.mCameraFocalLength;
			if (div < 0.001f)
				div = 0.001f;
			ramp /= div;
		}
		pBuffer->mBrushFarRamp = ramp;
		pBuffer->mBrushInvFar = 1.0f / max(params.mBrushFar / fr, 0.000001f);
		float h = 2.0f * tanf(params.mHFOV * 0.5f);
		float v = 2.0f * tanf(params.mVFOV * 0.5f);
		pBuffer->mBrushFilterMax = Vector2(320.f / params.mTargetWidth, 320.f / params.mTargetHeight);
		pBuffer->mBrushFilterSize = Vector2(1.0f / (h * fr * 800.f), 1.0f / (v * fr * 800.f));
	}

}

namespace T3EffectHBAO {

	inline void SetParameters(T3EffectParameter_BufferHBAO* buffer, T3EffectHBAOParams& materialParams)
	{
		buffer->mUVToView = materialParams.mUVToView;
		buffer->mBaseColor = materialParams.mBaseColor;
		buffer->mTargetScale = materialParams.mTargetScale;
		buffer->mFullRes_TexelSize = materialParams.mFullRes_TexelSize;
		buffer->mLayerRes_TexelSize = materialParams.mLayerRes_TexelSize;
		buffer->mJitter = materialParams.mJitter;
		buffer->mHBAOScreenParams = materialParams.mHBAOScreenParams;
		buffer->mProjectionParams = materialParams.mProjectionParams;
		buffer->mDeinterleaving_Offset00 = materialParams.mDeinterleaving_Offset00;
		buffer->mDeinterleaving_Offset10 = materialParams.mDeinterleaving_Offset10;
		buffer->mDeinterleaving_Offset01 = materialParams.mDeinterleaving_Offset01;
		buffer->mDeinterleaving_Offset11 = materialParams.mDeinterleaving_Offset01;
		buffer->mLayerOffset = materialParams.mLayerOffset;
		buffer->mDistanceFalloff = materialParams.mDistanceFalloff;
		buffer->mHBAODepthRange = materialParams.mHBAODepthRange;
		buffer->mRadius = materialParams.mRadius;
		buffer->mMaxRadiusPixels = materialParams.mMaxRadiusPixels;
		buffer->mNegInvRadius2 = materialParams.mNegInvRadius2;
		buffer->mAngleBias = materialParams.mAngleBias;
		buffer->mAOmultiplier = materialParams.mAOmultiplier;
		buffer->mIntensity = materialParams.mIntensity;
		buffer->mNoiseTexSize = materialParams.mNoiseTexSize;
		buffer->mBlurSharpness = materialParams.mBlurSharpness;
		buffer->mColorBleedSaturation = materialParams.mColorBleedSaturation;
		buffer->mAlbedoMultiplier = materialParams.mAlbedoMultiplier;
		buffer->mLuminanceInfluence = materialParams.mLuminanceInfluence;
		buffer->mMaxDistance = materialParams.mMaxDistance;
	}

}

namespace T3EffectInstanceUtil {

	inline void SetParameters(T3EffectParameter_BufferInstance* buffer, T3EffectInstanceParams& instParams) {
		buffer->mPositionScale.x = instParams.mPositionScale.x;
		buffer->mPositionScale.y = instParams.mPositionScale.y;
		buffer->mPositionScale.z = instParams.mPositionScale.z;
		buffer->mPositionWScale.x = instParams.mPositionWScale.x;
		buffer->mPositionWScale.y = instParams.mPositionWScale.y;
		buffer->mPositionWScale.z = instParams.mPositionWScale.z;
		buffer->mPositionOffset.x = instParams.mPositionOffset.x;

		buffer->mPositionOffset.y = instParams.mPositionOffset.y;
		buffer->mPositionOffset.z = instParams.mPositionOffset.z;
		buffer->mLightmapIntensity = instParams.mLightmapIntensity;
		buffer->mLightmapPageIndex = (float)(3 * instParams.mLightmapPageIndex);
		buffer->mShadowDepthBias = instParams.mShadowDepthBias;
		//mm_shuffle was a painful experience
		buffer->mLightmapTransform = Vector4(instParams.mLightmapTransform.mOffset, instParams.mLightmapTransform.mScale);
		buffer->mTexCoordTransform0 = Vector4(instParams.mTexCoordTransform0.mOffset, instParams.mTexCoordTransform0.mScale);
		buffer->mTexCoordTransform1 = Vector4(instParams.mTexCoordTransform1.mOffset, instParams.mTexCoordTransform1.mScale);
		buffer->mTexCoordTransform2 = Vector4(instParams.mTexCoordTransform2.mOffset, instParams.mTexCoordTransform2.mScale);
		buffer->mTexCoordTransform3 = Vector4(instParams.mTexCoordTransform3.mOffset, instParams.mTexCoordTransform3.mScale);
	}

}

inline void SetLightParams(T3EffectParameter_BufferLightCin::LightParams* bufferParams, T3LightCinematicType lightType,
	T3LightCinematicParams& lightParams, float cinLightIntensity) {
	if (lightParams.mFlags.IsSet(eLightCinematicFlag_Enabled)) {
		float scale = lightParams.mDimmer * lightParams.mIntensity * cinLightIntensity;
		Color linear = lightParams.mColor.GammaToLinear();
		bufferParams->mColor = Vector3(linear.r * scale, linear.g * scale, linear.b * scale);
		bool flag = lightParams.mShadowTraceLength > 0.000001f && lightParams.mShadowIntensity > 0.000001f;
		bool flagOther = lightParams.mHBAOParticipationType.mVal == eHBAOParticipationTypeForceOn ||
			lightParams.mHBAOParticipationType.mVal == eHBAOParticipationTypeAuto && lightType != eLightCinematicType_Key;
		bufferParams->mDirection = Vector3(-1.0f) * lightParams.mDirection;
		bufferParams->mDiffuseIntensity = lightParams.mDiffuseIntensity;
		bufferParams->mSpecularIntensity = lightParams.mSpecularIntensity;
		bufferParams->mShadowIntensity = fminf(1.0f, fmaxf(0.f, lightParams.mShadowIntensity));
		if (!flag || sGlobalRenderConfig.mbAllowWrapShadowedLights)
			bufferParams->mWrap = lightParams.mWrap;
		else
			bufferParams->mWrap = 0.f;
		bufferParams->mHBAOWeight = flagOther ? 1.0f : 0.0f;
		bufferParams->mEnable = fminf(1.0f, fmaxf(0.f, cinLightIntensity));
	}
	else {
		memset(bufferParams, 0, sizeof(T3EffectParameter_BufferLightCin::LightParams));
	}
}

namespace T3EffectLightEnvUtil {

	inline void SetParameters(T3EffectParameter_BufferLightEnv* buffer, T3EffectLightEnvParams& lightEnvParams) {
		buffer->mLightCount = lightEnvParams.mLightCount;
		buffer->mEnlightenIntensity = lightEnvParams.mGroupParams.mEnlightenIntensity;
		if (lightEnvParams.mKeyLightParams.mFlags.IsSet(eLightEnvFlag_Enabled)
			&& lightEnvParams.mKeyLightParams.mFlags.IsSet(eLightEnvFlag_GroupEnabled)
			&& lightEnvParams.mKeyLightParams.mFlags.IsSet(eLightEnvFlag_TileEnabled)) {
			float mult = lightEnvParams.mKeyLightParams.mDimmer * lightEnvParams.mKeyLightParams.mIntensity;
			if (lightEnvParams.mKeyLightParams.mShadowType == eLightEnvShadowType_Modulated &&
				lightEnvParams.mKeyLightParams.mMobility != eLightEnvMobility_Static) {
				if (lightEnvParams.mRenderQuality > eRenderQuality_Low)
					buffer->mKeyLightModulatedShadowIntensity = 10.f * lightEnvParams.mKeyLightParams.mShadowModulatedIntensity;
				else
					buffer->mKeyLightModulatedShadowIntensity = 2.f * lightEnvParams.mKeyLightParams.mShadowModulatedIntensity;
			}
			else buffer->mKeyLightModulatedShadowIntensity = 0.f;
			buffer->mKeyLightColor = Vector3(lightEnvParams.mKeyLightParams.mColor.GammaToLinear());
			buffer->mKeyLightDiffuseIntensity = mult * lightEnvParams.mKeyLightParams.mDiffuseIntensity;
			buffer->mKeyLightSpecularIntensity = mult * lightEnvParams.mKeyLightParams.mSpecularIntensity;
			buffer->mKeyLightOpacity = fminf(1.0f, fmaxf(0.0f, lightEnvParams.mKeyLightParams.mOpacity * lightEnvParams.mKeyLightParams.mDimmer));
			if (lightEnvParams.mKeyLightParams.mShadowType != eLightEnvShadowType_PerLight || sGlobalRenderConfig.mbAllowWrapShadowedLights)
				buffer->mKeyLightWrap = lightEnvParams.mKeyLightParams.mWrap;
			else
				buffer->mKeyLightWrap = 0.f;
			if (lightEnvParams.mbUseFixedKeyLightShadowMatrix) {
				buffer->mKeyLightShadowMatrices[0] = lightEnvParams.mKeyLightShadowMatrix.GetColumn(0);
				buffer->mKeyLightShadowMatrices[1] = lightEnvParams.mKeyLightShadowMatrix.GetColumn(1);
				buffer->mKeyLightShadowMatrices[2] = lightEnvParams.mKeyLightShadowMatrix.GetColumn(2);
			}
			else {
				if (lightEnvParams.mpKeyLightShadowMap) {
					for (int i = 0; i < lightEnvParams.mpKeyLightShadowMap->mCascadeCount; i++) {
						buffer->mKeyLightShadowMatrices[3 * i] = lightEnvParams.mpKeyLightShadowMap->mWorldToShadow[0].GetColumn(0);
						buffer->mKeyLightShadowMatrices[3 * i] = lightEnvParams.mpKeyLightShadowMap->mWorldToShadow[0].GetColumn(1);
						buffer->mKeyLightShadowMatrices[3 * i] = lightEnvParams.mpKeyLightShadowMap->mWorldToShadow[0].GetColumn(2);
					}
					float tmp = lightEnvParams.mKeyLightShadowMaxDistance / lightEnvParams.mpCamera->mFarClip;
					buffer->mKeyLightShadowFadeScale = 1.0f / (0.1f * tmp);
					buffer->mKeyLightShadowFadeBias = -1.0f * buffer->mKeyLightShadowFadeScale * 0.9f * tmp;
					buffer->mKeyLightShadowMaxDistance = tmp * lightEnvParams.mpKeyLightShadowMap->mValidCascadeMaxDistance;
				}
				else {
					buffer->mKeyLightShadowMaxDistance = -1.f;
					buffer->mKeyLightShadowFadeScale = buffer->mKeyLightShadowFadeBias = 0.f;
					memset(buffer->mKeyLightShadowMatrices, 0, 12 * sizeof(Vector4));
				}
			}
		}
		else {
			buffer->mKeyLightShadowMaxDistance = -1.f;
			memset(buffer->mKeyLightShadowMatrices, 0, 12 * sizeof(Vector4));
		}
		if (!lightEnvParams.mEnvParams.mFlags.IsSet(eLightEnvFlag_TileEnabled)) {
			buffer->mFogColor = Color::Black;
			buffer->mFogMinOpacity = 1.f;
			buffer->mFogStartDistance = 0.f;
			buffer->mFogHeightFalloff = 0.0001f;
			buffer->mFogDensity = 0.f;
		}
		else {
			Matrix4 worldMat = lightEnvParams.mpCamera->GetWorldMatrix();
			buffer->mFogColor = lightEnvParams.mEnvParams.mFogColor.GammaToLinear();
			buffer->mFogHeightFalloff = 0.01f * lightEnvParams.mEnvParams.mFogHeightFalloff;
			float tmp = (lightEnvParams.mEnvParams.mFogHeight - worldMat._Entries[3][1]) * buffer->mFogHeightFalloff;
			tmp = fminf(110.f, fmaxf(-110.f, tmp));
			tmp = powf(2.0f, tmp);
			buffer->mFogDensity = 0.01f * lightEnvParams.mEnvParams.mFogDensity * tmp;
			buffer->mFogMinOpacity = fminf(1.0f, fmaxf(0.f, 1.0f - lightEnvParams.mEnvParams.mFogMaxOpacity));
			buffer->mFogStartDistance = lightEnvParams.mEnvParams.mFogStartDistance;
		}
	}

	inline void SetAmbientParams(T3EffectParameter_BufferLightAmbient* buffer, Color diffuseSH[9], Color specularSH[9])
	{
		float v3;
		float v4;
		float v5;
		float v6;
		float v7;
		float v8;
		float v9;
		float v10;
		float v11;
		float v12;
		float v13;
		float v14;
		float v15;
		float v16;
		float v17;
		float v18;
		float v19;
		float v20;

		v3 = diffuseSH[2].r;
		v4 = diffuseSH[1].r;
		v5 = diffuseSH->r;
		buffer->mLightDiffuseSHR.y = diffuseSH[3].r;
		buffer->mLightDiffuseSHR.z = v4;
		buffer->mLightDiffuseSHR.w = v3;
		buffer->mLightDiffuseSHR.x = v5;
		v6 = diffuseSH[2].g;
		v7 = diffuseSH[1].g;
		v8 = diffuseSH->g;
		buffer->mLightDiffuseSHG.y = diffuseSH[3].g;
		buffer->mLightDiffuseSHG.z = v7;
		buffer->mLightDiffuseSHG.w = v6;
		buffer->mLightDiffuseSHG.x = v8;
		v9 = diffuseSH[2].b;
		v10 = diffuseSH[1].b;
		v11 = diffuseSH->b;
		buffer->mLightDiffuseSHB.y = diffuseSH[3].b;
		buffer->mLightDiffuseSHB.z = v10;
		buffer->mLightDiffuseSHB.w = v9;
		buffer->mLightDiffuseSHB.x = v11;
		v12 = specularSH[2].r;
		v13 = specularSH[1].r;
		v14 = specularSH->r;
		buffer->mLightSpecularSHR.y = specularSH[3].r;
		buffer->mLightSpecularSHR.z = v13;
		buffer->mLightSpecularSHR.w = v12;
		buffer->mLightSpecularSHR.x = v14;
		v15 = specularSH[1].g;
		v16 = specularSH->g;
		v17 = specularSH[2].g;
		buffer->mLightSpecularSHG.y = specularSH[3].g;
		buffer->mLightSpecularSHG.z = v15;
		buffer->mLightSpecularSHG.w = v17;
		buffer->mLightSpecularSHG.x = v16;
		v18 = specularSH[2].b;
		v19 = specularSH[1].b;
		v20 = specularSH->b;
		buffer->mLightSpecularSHB.y = specularSH[3].b;
		buffer->mLightSpecularSHB.z = v19;
		buffer->mLightSpecularSHB.w = v18;
		buffer->mLightSpecularSHB.x = v20;
	}

}

namespace T3EffectLightCinUtil {

	struct EntrySort {

		bool operator()(const SortedLightEntry& lhs, const SortedLightEntry& rhs) {
			return lhs.mIntensity <= rhs.mIntensity;
		}

	};

	inline void SetParameters(T3EffectParameter_BufferLightCin* buffer, T3EffectParameter_BufferLightAmbient* pBufferAmbientList[20], T3EffectLightCinParams& lightCinParams) {
		float envIntensity = lightCinParams.mLightRigParams.mLightEnvIntensity;
		float cinIntesity = lightCinParams.mLightRigParams.mLightCinIntensity;
		if (lightCinParams.mRenderQuality == eRenderQuality_Mid && lightCinParams.mLightRigParams.mFlags.IsSet(eLightCinematicRigFlag_LODIntensityOnMedium)) {
			envIntensity = 1.0f;
			cinIntesity = 1.0f;
		}
		if (lightCinParams.mRenderQuality == eRenderQuality_LowPlus || lightCinParams.mRenderQuality == eRenderQuality_Low || lightCinParams.mRenderQuality == eRenderQuality_Lowest) {
			if (lightCinParams.mLightRigParams.mFlags.IsSet(eLightCinematicRigFlag_LODIntensityOnLow)) {
				envIntensity = 1.0f;
				cinIntesity = 1.0f;
			}
		}
		if (pBufferAmbientList) {
			Color shParam0[8]{};
			Color shParam1[8]{};
			SortedLightEntry entry[3]{};
			for (int i = 0; i < 3; i++) {
				entry[i].mIndex = i;
				if (lightCinParams.mLightParams[i].mFlags.IsSet(eLightCinematicFlag_Enabled))
					entry[i].mIntensity = 0.0f;
				else {
					Color linear = lightCinParams.mLightParams[i].mColor.GammaToLinear();
					//take contribution from rgb. most from g, then r, then b.
					entry[i].mIntensity = (float)((float)((float)((float)((float)(linear.r * 0.3f) + (float)(linear.g * 0.59f))
						+ (float)(linear.b * 0.11f))
						* lightCinParams.mLightParams[i].mIntensity
						* lightCinParams.mLightParams[i].mSpecularIntensity)
						* lightCinParams.mLightParams[i].mIntensity);
				}
			}
			std::sort(entry, entry + 3, EntrySort{});
			::SetLightParams(&buffer->mLightParams[0], eLightCinematicType_Key, lightCinParams.mLightParams[entry[0].mIndex], cinIntesity);
			::SetLightParams(&buffer->mLightParams[1], eLightCinematicType_Rim, lightCinParams.mLightParams[entry[1].mIndex], cinIntesity);
			memset(&buffer->mLightParams[2], 0, sizeof(T3EffectParameter_BufferLightCin::LightParams));
			if (lightCinParams.mLightParams[entry[1].mIndex].mFlags.IsSet(eLightCinematicFlag_Enabled)) {
				Color color = lightCinParams.mLightParams[entry[1].mIndex].mColor.GammaToLinear();
				color.r *= lightCinParams.mLightParams[entry[1].mIndex].mDimmer * lightCinParams.mLightParams[entry[1].mIndex].mIntensity * lightCinParams.mLightParams[entry[1].mIndex].mDiffuseIntensity * cinIntesity;
				color.g *= lightCinParams.mLightParams[entry[1].mIndex].mDimmer * lightCinParams.mLightParams[entry[1].mIndex].mIntensity * lightCinParams.mLightParams[entry[1].mIndex].mDiffuseIntensity * cinIntesity;
				color.b *= lightCinParams.mLightParams[entry[1].mIndex].mDimmer * lightCinParams.mLightParams[entry[1].mIndex].mIntensity * lightCinParams.mLightParams[entry[1].mIndex].mDiffuseIntensity * cinIntesity;
				color.a *= lightCinParams.mLightParams[entry[1].mIndex].mDimmer * lightCinParams.mLightParams[entry[1].mIndex].mIntensity * lightCinParams.mLightParams[entry[1].mIndex].mDiffuseIntensity * cinIntesity;
				Vector3 dir = Vector3(-1.0f) * lightCinParams.mLightParams[entry[1].mIndex].mDirection;
				memset(shParam0, 0, 0x80);
				memset(shParam1, 0, 0x80);
				//diffuse
				SHProjectDirectionalLight(shParam0, dir, color, lightCinParams.mLightParams[entry[1].mIndex].mWrap);
				//specular
				color = lightCinParams.mLightParams[entry[1].mIndex].mColor.GammaToLinear();
				color.r *= lightCinParams.mLightParams[entry[1].mIndex].mDimmer * lightCinParams.mLightParams[entry[1].mIndex].mIntensity * lightCinParams.mLightParams[entry[1].mIndex].mSpecularIntensity * cinIntesity;
				color.g *= lightCinParams.mLightParams[entry[1].mIndex].mDimmer * lightCinParams.mLightParams[entry[1].mIndex].mIntensity * lightCinParams.mLightParams[entry[1].mIndex].mSpecularIntensity * cinIntesity;
				color.b *= lightCinParams.mLightParams[entry[1].mIndex].mDimmer * lightCinParams.mLightParams[entry[1].mIndex].mIntensity * lightCinParams.mLightParams[entry[1].mIndex].mSpecularIntensity * cinIntesity;
				color.a *= lightCinParams.mLightParams[entry[1].mIndex].mDimmer * lightCinParams.mLightParams[entry[1].mIndex].mIntensity * lightCinParams.mLightParams[entry[1].mIndex].mSpecularIntensity * cinIntesity;
				SHProjectDirectionalLight(shParam1, dir, color, lightCinParams.mLightParams[entry[1].mIndex].mWrap);
			}
			for (int i = 0; i < 20; i++) {
				if (pBufferAmbientList[i]) {
					/*pBufferAmbientList[i]->mLightDiffuseSHR += Vector4(shParam0[0].r, shParam0[1].r, shParam0[1].r, shParam0[0].r);
					pBufferAmbientList[i]->mLightDiffuseSHG += Vector4(shParam0[0].g, shParam0[1].g, shParam0[1].g, shParam0[0].g);
					pBufferAmbientList[i]->mLightDiffuseSHB += Vector4(shParam0[0].b, shParam0[1].b, shParam0[1].b, shParam0[0].b);
					pBufferAmbientList[i]->mLightSpecularSHR += Vector4(shParam1[0].r, shParam1[3].r, shParam1[1].r, shParam1[2].r);
					pBufferAmbientList[i]->mLightSpecularSHG += Vector4(shParam1[0].g, shParam1[3].g, shParam1[1].g, shParam1[2].g);
					pBufferAmbientList[i]->mLightSpecularSHB += Vector4(shParam1[0].b, shParam1[3].b, shParam1[1].b, shParam1[2].b);*/
					T3EffectLightEnvUtil::SetAmbientParams(pBufferAmbientList[i], shParam0, shParam1);
				}
			}
		}
		else {
			::SetLightParams(&buffer->mLightParams[0], eLightCinematicType_Key, lightCinParams.mLightParams[0], cinIntesity);
			::SetLightParams(&buffer->mLightParams[1], eLightCinematicType_Rim, lightCinParams.mLightParams[2], cinIntesity);
			//2 to 1 instead?
			::SetLightParams(&buffer->mLightParams[2], eLightCinematicType_Fill, lightCinParams.mLightParams[1], cinIntesity);
		}
		buffer->mLightEnvIntensity = envIntensity;
		buffer->mEnlightenIntensity = lightCinParams.mLightRigParams.mEnlightenIntensity;
		buffer->mEnlightenSaturation = lightCinParams.mLightRigParams.mEnlightenSaturation;
	}

}

namespace T3EffectLightsUtil {

	inline void SetConstantColor(T3EffectParameter_BufferLights* buffer, Color color) {
		memset(buffer, 0, sizeof(T3EffectParameter_BufferLights));
		buffer->mSHParam0 = color;
	}

	/*inline void SetLights(T3EffectParameter_BufferLights* buffer, BitSet<enum T3EffectFeature, 30, 54>& effectFeatures,
		LightGroupInstance& lightGroupInst, T3EffectLightParams& lightParams){
		memset(buffer, 0, sizeof(T3EffectParameter_BufferLights));
		for(int i = 0; i < 4; i++){
			if(lightGroupInst.mPointLights[i].mpLight && !lightGroupInst.mPointLights[i].mbIsBaked){

			}
		}
	};*/


}

namespace T3EffectMaterialToolUtil {

	inline void SetParameters(T3EffectParameter_BufferMaterialTool* buffer, T3EffectMaterialToolParams& params) {
		buffer->mSelectedColor = params.mSelectedColor;
	}

}

namespace T3EffectObjectUtil {

	inline void SetParameters(T3EffectParameter_BufferObject* buffer, Matrix4 worldMatrix, T3EffectObjectParams params) {
		buffer->mWorldMatrix[0] = worldMatrix.GetColumn(0);
		buffer->mWorldMatrix[1] = worldMatrix.GetColumn(1);
		buffer->mWorldMatrix[2] = worldMatrix.GetColumn(2);
		//buffer->mWorldMatrix[3] = worldMatrix.GetColumn(3);
		buffer->mPrevWorldMatrix[0] = params.mPrevWorldMatrix.GetColumn(0);
		buffer->mPrevWorldMatrix[1] = params.mPrevWorldMatrix.GetColumn(1);
		buffer->mPrevWorldMatrix[2] = params.mPrevWorldMatrix.GetColumn(2);
		//buffer->mPrevWorldMatrix[3] = params.mPrevWorldMatrix.GetColumn(3);
		buffer->mObjectScale = params.mObjectScale;
		buffer->mReflectionMatrix0 = Vector3(params.mReflectionMatrix.GetColumn(0));
		buffer->mReflectionMatrix1 = Vector3(params.mReflectionMatrix.GetColumn(1));
		buffer->mReflectionMatrix2 = Vector3(params.mReflectionMatrix.GetColumn(2));
		buffer->mAlpha = params.mAlpha;
		buffer->mRimBumpDepth = params.mRimBumpDepth;
		buffer->mReceiveShadowsIntensity = fmaxf(0.0f, params.mReceiveShadowsIntensity);
		buffer->mStencilMask = params.mStencilMask;
		buffer->mStencilRef = params.mStencilRef;
		buffer->mBrushScale = (params.mbBrushForceEnable ? 0.5f : 0.f) + fminf(0.5f, fmaxf(0.f, params.mBrushScale * 0.5f));
		buffer->mBrushFarDetailBias = params.mBrushFarDetailBias * 10.f;
		buffer->mBrushNearDetailBias = params.mBrushNearDetailBias * 10.f;
		buffer->mBrushScaleByVertex = params.mbBrushScaleByVertex ? 1.0f : 0.f;
		buffer->mDiffuseColor = params.mDiffuseColor;
		buffer->mMaterialPrevTime = params.mMaterialPrevTime;
		buffer->mMaterialTime = params.mMaterialTime;
		buffer->mReflectionIntensityShadow = params.mReflectionIntensityShadow;
		buffer->mReflectionColor.x = sqrtf(fmaxf(params.mReflectionTint.r * params.mReflectionIntensity, 0.0f)) * params.mReflectionHDR;
		buffer->mReflectionColor.y = sqrtf(fmaxf(params.mReflectionTint.g * params.mReflectionIntensity, 0.0f)) * params.mReflectionHDR;
		buffer->mReflectionColor.z = sqrtf(fmaxf(params.mReflectionTint.b * params.mReflectionIntensity, 0.0f)) * params.mReflectionHDR;
		buffer->mReflectionMipCount = log2f(params.mReflectionTextureSize) + 1.0f;
		buffer->mEnlightenUVTransform = params.mEnlightenUVTransform;
		buffer->mAnimatedPositionScale = params.mAnimatedBoundingBox.mMax - params.mAnimatedBoundingBox.mMin;
		buffer->mAnimatedPositionBias = params.mAnimatedBoundingBox.mMin;
		buffer->mAnimatedPositionInvScale = Vector3(1.0f) / buffer->mAnimatedPositionScale;
		buffer->mAnimatedPositionInvBias = Vector3(-1.0f) * (params.mAnimatedBoundingBox.mMin / buffer->mAnimatedPositionScale);
		buffer->mPrevAnimatedPositionScale = params.mPrevAnimatedBoundingBox.mMax - params.mPrevAnimatedBoundingBox.mMin;
		buffer->mPrevAnimatedPositionBias = params.mPrevAnimatedBoundingBox.mMin;
	}

}

namespace T3EffectParticleUtil {

	inline void SetParameters(T3EffectParameter_BufferParticle* buffer, T3EffectParticleParams& params) {
		buffer->mTextureSheetParams = Vector2((float)params.mTextureSheetX, (float)params.mTextureSheetY);
		buffer->mParticlePivotParams = Vector2(params.mParticlePivotX, params.mParticlePivotY);
		buffer->mAlphaRef = params.mAlphaRef;
		buffer->mLinearDepthAlphaScaleBias = params.mLinearDepthAlphaScaleBias;
	}

}

struct RenderTonemapFilmicResult
{
	Vector4 mTonemapToeCoeff;
	Vector4 mTonemapShoulderCoeff;
	float mTonemapCutoff;
};

struct RenderTonemapFilmicRGBResults
{
	RenderTonemapFilmicResult mResults[3];
};

namespace T3EffectPostUtil {

	inline RenderTonemapFilmicResult GetFilmicResult(RenderTonemapFilmicParams& params) {
		RenderTonemapFilmicResult res{};
		res.mTonemapCutoff = params.mPivot;
		float tmp099 = fminf(0.99f, params.mShoulderIntensity);
		float tmp1 = fminf(0.99f, fmaxf(1.0f - params.mToeIntensity, 0.f));
		if (params.mbInvertCurve) {
			res.mTonemapToeCoeff.x = ((1.0f - tmp099) * (params.mPivot - fmaxf(params.mBlackPoint, 0.f))) /
				(((params.mWhitePoint - params.mPivot) * (1.0f - tmp1)) + ((1.0f - tmp099) * (params.mPivot - params.mBlackPoint)));
			res.mTonemapToeCoeff.y = -1.0f * res.mTonemapToeCoeff.x * fmaxf(params.mBlackPoint, 0.0f);
			res.mTonemapToeCoeff.z = tmp1;
			res.mTonemapToeCoeff.w = ((1.0f - tmp1) * params.mPivot) - fmaxf(params.mBlackPoint, 0.0f);

			res.mTonemapShoulderCoeff.x = ((1.0f - tmp099) * (1.0f - res.mTonemapToeCoeff.x)) - (tmp099 * res.mTonemapToeCoeff.x);
			res.mTonemapShoulderCoeff.y = (res.mTonemapToeCoeff.x * params.mWhitePoint) -
				((params.mPivot * res.mTonemapToeCoeff.x) * (1.0f - tmp099)) -
				((params.mPivot * (1.0f - res.mTonemapToeCoeff.x)) * (1.0f - tmp099));
			res.mTonemapShoulderCoeff.z = -tmp099;
			res.mTonemapShoulderCoeff.w = params.mWhitePoint - (params.mPivot * (1.0f - tmp099));
		}
		else {
			float tmp = ((1.0f - tmp1) * (params.mPivot - fmaxf(params.mBlackPoint, 0.f))) /
				(((params.mWhitePoint - params.mPivot) * (1.0f - tmp099)) + ((1.0f - tmp1) * (params.mPivot - params.mBlackPoint)));
			res.mTonemapToeCoeff.x = (1.0f - tmp1) * tmp;
			res.mTonemapToeCoeff.y = (tmp * fmaxf(params.mBlackPoint, 0.f)) * (tmp1 - 1.0f);
			res.mTonemapToeCoeff.z = -tmp1;
			res.mTonemapToeCoeff.w = params.mPivot - (fmaxf(params.mBlackPoint, 0.f) * (1.0f - tmp1));

			res.mTonemapShoulderCoeff.x = (tmp * tmp099) + (1.0f - tmp);
			res.mTonemapShoulderCoeff.y = ((tmp * params.mWhitePoint) * (1.0f - tmp099)) + ((tmp - 1.0f) * params.mPivot) - (tmp * params.mPivot);
			res.mTonemapShoulderCoeff.z = tmp099;
			res.mTonemapShoulderCoeff.w = ((tmp099 * params.mWhitePoint) - params.mPivot);
		}
		return res;
	}

	inline void GetFilmicRGBResults(RenderTonemapFilmicRGBResults& out, RenderTonemapFilmicRGBParams& params) {
		for (int i = 0; i < 3; i++) {
			RenderTonemapFilmicParams r{};
			r.mbInvertCurve = params.mbInvertCurves[i];
			r.mBlackPoint = params.mBlackPoints[i];
			r.mPivot = params.mPivots[i];
			r.mShoulderIntensity = params.mShoulderIntensities[i];
			r.mToeIntensity = params.mToeIntensities[i];
			r.mWhitePoint = params.mWhitePoints[i];
			out.mResults[i] = GetFilmicResult(r);
		}
	}

	inline void SetParameters(T3EffectParameter_BufferPost* buffer, T3EffectPostParams& params, float frameTime) {
		float toneint = params.mTonemapIntensity;
		if (toneint <= 0.001f) {
			buffer->mTonemapParams = Vector4(-1.0f * params.mTonemapFilmicParams.mBlackPoint *
				(1.0f / (params.mTonemapFilmicParams.mWhitePoint - params.mTonemapFilmicParams.mBlackPoint)));
			buffer->mTonemapParams.w = (1.0f / (params.mTonemapFilmicParams.mWhitePoint - params.mTonemapFilmicParams.mBlackPoint));
		}
		else {
			if (toneint < 0.001f)
				toneint = 0.001f;
			buffer->mTonemapParams.x = ((1.0f / toneint) + params.mTonemapFilmicParams.mWhitePoint) /
				(params.mTonemapFilmicParams.mWhitePoint - params.mTonemapFilmicParams.mBlackPoint);
			buffer->mTonemapParams.y = ((1.0f / toneint) + params.mTonemapFilmicParams.mWhitePoint) /
				(buffer->mTonemapParams.x * params.mTonemapFilmicParams.mWhitePoint);
			buffer->mTonemapParams.z = 0.f;
			buffer->mTonemapParams.w = 1.0f / toneint;
		}
		buffer->mTonemapCutoff = INFINITY;
		RenderTonemapFilmicResult filterParamsNormal = GetFilmicResult(params.mTonemapFilmicParams);
		RenderTonemapFilmicResult filterParamsFar = GetFilmicResult(params.mTonemapFarFilmicParams);
		buffer->mTonemapCutoff = filterParamsNormal.mTonemapCutoff;
		buffer->mTonemapFarCutoff = filterParamsFar.mTonemapCutoff;
		buffer->mTonemapToeCoeff = filterParamsNormal.mTonemapToeCoeff;
		buffer->mTonemapFarToeCoeff = filterParamsFar.mTonemapToeCoeff;
		//shoulder
		buffer->mTonemapShoulderCoeff = filterParamsNormal.mTonemapShoulderCoeff - filterParamsNormal.mTonemapToeCoeff;
		buffer->mTonemapFarShoulderCoeff = filterParamsFar.mTonemapShoulderCoeff - filterParamsFar.mTonemapToeCoeff;
		buffer->mTonemapWhitePoint = params.mTonemapFilmicParams.mWhitePoint;
		buffer->mTonemapFarWhitePoint = params.mTonemapFarFilmicParams.mWhitePoint;

		// ------------------------------- NOT SURE. the decompiled looks weird. followed exactly and got this. looks wrong?
		RenderTonemapFilmicRGBResults result{};
		GetFilmicRGBResults(result, params.mFXTonemapRGBParams);
		buffer->mTonemapRGBCutoffs = Vector4(INFINITY, INFINITY, INFINITY, 0.F);
		buffer->mTonemapRGBToeCoeffs[0] = Vector4(result.mResults[0].mTonemapToeCoeff.x, INFINITY, 0.f, 0.f);
		buffer->mTonemapRGBToeCoeffs[1] = Vector4(result.mResults[0].mTonemapToeCoeff.y, 0.f, 0.f, 0.f);
		buffer->mTonemapRGBToeCoeffs[2] = Vector4(0.f, 0.f, 0.f, 0.f);
		buffer->mTonemapRGBToeCoeffs[3] = Vector4(0.f, 0.f, 0.f, 0.f);

		buffer->mTonemapRGBShoulderCoeffs[0] = Vector4(-result.mResults[0].mTonemapToeCoeff.x, 0.f, 0.f, 0.f);
		buffer->mTonemapRGBShoulderCoeffs[1] = Vector4(-result.mResults[0].mTonemapToeCoeff.y, 0.f, 0.f, 0.f);
		buffer->mTonemapRGBShoulderCoeffs[2] = Vector4(0.f, 0.f, 0.f, 0.f);
		buffer->mTonemapRGBShoulderCoeffs[3] = Vector4(0.f, 0.f, 0.f, 0.f);
		// --------------------------------

		buffer->mLevelScalar = 1.0f / (params.mLevelWhite - params.mLevelBlack);
		buffer->mLevelAdder = buffer->mLevelScalar * params.mLevelBlack;
		buffer->mLevelIntensity = 1.0f / fmaxf(0.01f, params.mLevelIntensity);
		buffer->mDofNear = (params.mDofNear + params.mDofAutoFocusNear) / params.mCameraFar;
		buffer->mDofNearRamp = (1.0f / fmaxf(params.mCameraFocalLength, 0.001f)) * params.mCameraFar / fmaxf(0.000001f, params.mDofNearRamp);
		buffer->mDofNearRamp = (1.0f / fmaxf(params.mCameraFocalLength, 0.001f)) * params.mCameraFar / fmaxf(0.000001f, params.mDofFarRamp);

		buffer->mDofNearMax = params.mDofNearMax;
		buffer->mDofFarMax = params.mDofFarMax;
		buffer->mDofVignetteMax = params.mDofVignetteMax;
		buffer->mDofResolutionFraction = params.mDofResolutionFraction;
		buffer->mDofFarResolutionFraction = params.mDofFarResolutionFraction;
		buffer->mDofNearBlurRadiusPixels = params.mDofNearBlurRadiusPixels;
		buffer->mDofInvNearBlurRadiusPixels = params.mDofInvNearBlurRadiusPixels;
		buffer->mDofMaxCocRadiusPixels = params.mDofMaxCocRadiusPixels;
		buffer->mBokehBrightnessDeltaThreshold = params.mBokehBrightnessDeltaThreshold;
		buffer->mBokehBrightnessThreshold = params.mBokehBrightnessThreshold;
		buffer->mBokehBlurThreshold = params.mBokehBlurThreshold;
		buffer->mBokehMinSize = params.mBokehMinSize;
		buffer->mBokehMaxSize = params.mBokehMaxSize;
		buffer->mBokehFalloff = params.mBokehFalloff;
		buffer->mBokehDomainSizeX = params.mBokehDomainSizeX;
		buffer->mBokehDomainSizeY = params.mBokehDomainSizeY;
		buffer->mBokehAberrationOffsetsX.x = params.mBokehAberrationOffsetsX.x;
		buffer->mBokehAberrationOffsetsX.y = params.mBokehAberrationOffsetsX.y;
		buffer->mBokehAberrationOffsetsX.z = params.mBokehAberrationOffsetsX.z;
		buffer->mBokehAberrationOffsetsY.x = params.mBokehAberrationOffsetsY.x;
		buffer->mBokehAberrationOffsetsY.y = params.mBokehAberrationOffsetsY.y;
		buffer->mBokehAberrationOffsetsY.z = params.mBokehAberrationOffsetsY.z;
		buffer->mMaxBokehVertices = params.mMaxBokehVertices;
		buffer->mForceLinearDepthOffset = params.mForceLinearDepthOffset;
		buffer->mZNearClip = params.mZNearClip;
		buffer->mDofFarRadiusRescale = params.mDofFarRadiusRescale;
		buffer->mDofDebug = params.mDofDebug;
		buffer->mZRange = params.mZRange;
		buffer->mDofNearBlurryPlaneZ = params.mDofNearBlurryPlaneZ;
		buffer->mDofNearSharpPlaneZ = params.mDofNearSharpPlaneZ;
		buffer->mDofFarSharpPlaneZ = params.mDofFarSharpPlaneZ;
		buffer->mDofFarBlurryPlaneZ = params.mDofFarBlurryPlaneZ;
		buffer->mDofNearScale = params.mDofNearScale;
		buffer->mDofFarScale = params.mDofFarScale;
		buffer->mDofCoverageBoost = params.mDofCoverageBoost;
		buffer->mDofFarBlurRadius = params.mDofFarBlurRadius;
		buffer->mVignetteFalloff = params.mVignetteFalloff;
		buffer->mVignetteStart = params.mVignetteStart;
		buffer->mVignetteRange = fmaxf(params.mVignetteEnd - params.mVignetteStart, 0.0f);
		buffer->mVignetteTint = params.mVignetteTint;
		buffer->mRadialBlurTint = params.mRadialBlurTint;
		buffer->mRadialBlurInRadius = params.mRadialBlurInRadius;
		buffer->mRadialBlurInvDeltaRadius = 1.0f / fmaxf(0.000001f, params.mRadialBlurOutRadius - params.mRadialBlurInRadius);
		buffer->mRadialBlurScale = params.mRadialBlurScale;
		buffer->mRadialBlurIntensity = params.mRadialBlurIntensity;
		buffer->mBloomThreshold = params.mBloomThreshold;
		buffer->mBloomIntensity = params.mBloomIntensity;
		buffer->mMotionBlurIntensity = fminf(1.0f, fmaxf(0.0f, params.mMotionBlurIntensity / fmaxf(0.000001f, frameTime * 60.f)));
		buffer->mSSLineDepthFadeNear = params.mSSLineDepthFadeNear;
		buffer->mSSLineDepthFadeFar = params.mSSLineDepthFadeFar;
		buffer->mSSLineDepthMagnitude = params.mSSLineDepthMagnitude;
		buffer->mSSLineDepthExponent = params.mSSLineDepthExponent;
		buffer->mSSLineLightDirection = params.mSSLineLightDirection;
		buffer->mSSLineLightMagnitude = params.mSSLineLightMagnitude;
		buffer->mSSLineLightExponent = params.mSSLineLightExponent;
		buffer->mSSLineColor = params.mSSLineColor;

		u32 w = 0, h = 0;
		TelltaleToolLib_GetRenderAdaptersStruct()->GetResolution(w, h);
		buffer->mSSLineThickness = params.mSSLineThickness * (float)h / 1080.f;
		float invTargetWidth = 1.0f / params.mTargetWidth;
		float invTargetHeight = 1.0f / params.mTargetHeight;

		buffer->mSSLineDebug = params.mSSLineDebug;
		buffer->mFxaaConsoleRcpFrameOpt = Vector4(invTargetWidth * -0.5f, invTargetHeight * -0.5f, invTargetWidth * 0.5f, invTargetHeight * 0.5f);
		buffer->mFxaaConsoleRcpFrameOpt2 = Vector4(invTargetWidth * -2.f, invTargetHeight * -2.0f, Vector2(invTargetHeight + invTargetWidth));
		buffer->mFxaaConsole360RcpFrameOpt2 = Vector4(invTargetWidth * 8.f, invTargetHeight * 8.f, invTargetWidth * -4.f, invTargetHeight * -4.f);

	}

}

namespace T3EffectSceneToolUtil {

	inline void SetParameters(T3EffectParameter_BufferSceneDebug* buffer, T3EffectSceneDebugParams& params) {
		buffer->mDebugMode = params.mDebugRenderMode;
		buffer->mIntensityRange = Vector2(params.mIntensityRangeMin, params.mIntensityRangeMax);
		buffer->mAlphaOverride = Color(0.1f, 0.05f, 0.f, 0.f);
	}

}

namespace ShadowUtil {

	extern Matrix4 mTetrahedralRotationNonPacked[4];
	extern Matrix4 mTetrahedralRotationPacked[4];
	extern Matrix4 mTetrahedralViewProjection[4];

	inline Matrix4& GetTetrahedralViewProjectionMatrix(unsigned int faceIndex)
	{
		return mTetrahedralViewProjection[faceIndex];
	}

	inline void BuildWorldToShadowMatrix(Matrix4& outMatrix, const Matrix4& shadowViewProjectionMatrix, float depthBias,
		int shadowMapSize, int gutterSize) {
		Matrix4 m = Matrix4::Scaling(Vector3(0.5f, -0.5f, 1.f)) * MatrixTranslation(Vector3(0.5f, 0.5f, -depthBias));
		m = shadowViewProjectionMatrix * m;
		if (gutterSize) {
			outMatrix = m * Matrix4::Scaling(Vector3(((float)shadowMapSize - 2.0f * (float)gutterSize) / (float)shadowMapSize,
				((float)shadowMapSize - 2.0f * (float)gutterSize) / (float)shadowMapSize, 1.f)) *
				MatrixTranslation(Vector3((float)gutterSize / (float)shadowMapSize, (float)gutterSize / (float)shadowMapSize, 0.f));
		}
	}

	inline void BuildWorldToShadowMatrix(Matrix4& matrix, Camera* pLightCamera, float depthBias) {
		Matrix4 f = pLightCamera->GetViewMatrix() * pLightCamera->GetProjectionMatrix();
		BuildWorldToShadowMatrix(matrix, f, depthBias, 0, 0);
	}

	inline void BuildLocalWorldToShadowMatrix(Matrix4& matrix, Camera* pLightCamera, float depthBias) {
		Matrix4 f = pLightCamera->GetViewMatrix() * pLightCamera->ComputeLocalViewMatrix();
		BuildWorldToShadowMatrix(matrix, f, depthBias, 0, 0);
	}

	inline float GetEVSMBlurRadius(float softness, float shadowWorldArea, float shadowMapSize)
	{
		return (float)((float)(softness * 0.5f) * shadowMapSize) / sqrtf(shadowWorldArea);
	}

	inline void BuildWorldToShadowMatrix(Matrix4& outMatrix, Camera* pLightCamera, float depthBias, int shadowMapSize, int gutterSize)
	{
		BuildWorldToShadowMatrix(outMatrix, pLightCamera->GetViewMatrix() * pLightCamera->GetProjectionMatrix(), depthBias, shadowMapSize, gutterSize);
	}

	inline Color GetEVSMValue(float depth, float posExponent, float negExponent) {
		return Color(
			fminf(65504.f, fmaxf(-65504.f, expf(depth * posExponent))),
			fminf(65504.f, fmaxf(-65504.f, -expf(-depth * negExponent))),
			fminf(65504.f, fmaxf(-65504.f, expf(2.0f * depth * posExponent))),
			fminf(65504.f, fmaxf(-65504.f, expf(-2.0f * depth * posExponent)))
		);
	}

	inline void MakeTetrahedralShadowCamera(Camera* pLightCamera, unsigned int faceIndex, Vector3& lightPosition, float radius, float nearClip) {
		pLightCamera->SetAllowInvertDepth(false);
		pLightCamera->mNearClip = nearClip;
		pLightCamera->mFarClip = radius;
		float w = (3.f * 1.9927368f) + 143.9857f;
		float h = (3.f * 2.785965f) + 125.26439f;
		if (faceIndex >= 2) {
			pLightCamera->SetHFOV(w * 4.0f / 3.0f);
			pLightCamera->SetAspectRatio(tanf(h * 3.14159265f / 360.f) / tanf(w * 3.14159265f / 360.f));
		}
		else {
			pLightCamera->SetHFOV(h * 4.0f / 3.0f);
			pLightCamera->SetAspectRatio(tanf(w * 3.14159265f / 360.f) / tanf(h * 3.14159265f / 360.f));
		}
		Matrix4 m = mTetrahedralRotationPacked[faceIndex];
		m.SetRow(3, Vector4{ lightPosition,1.f });
		pLightCamera->SetWorldMatrix(m);
	}

	inline void MakeTetrahedralShadowViewport(Vector4& viewport, unsigned int faceIndex) {
		if (faceIndex == 0)
			viewport = { 1.0f, 0.5f, 0.0f, 0.5f };
		else if (faceIndex == 1)
			viewport = { 1.0f, 0.5f, 0.0f, 0.0f };
		else if (faceIndex == 2)
			viewport = { 0.5f, 1.0f, 0.5f, 0.0f };
		else
			viewport = { 0.5f, 1.0f, 0.0f, 0.0f };
	}

	inline void Initialize() {
		mTetrahedralRotationNonPacked[0] = MatrixRotationYawPitchRollDegrees(0.0f, 27.367805f, 0.0f);
		mTetrahedralRotationNonPacked[1] = MatrixRotationYawPitchRollDegrees(180.0f, 27.367805f, 0.0f);
		mTetrahedralRotationNonPacked[2] = MatrixRotationYawPitchRollDegrees(-90.0f, -27.367805f, 0.0f);
		mTetrahedralRotationNonPacked[3] = MatrixRotationYawPitchRollDegrees(90.0f, -27.367805f, 0.0f);

		mTetrahedralRotationPacked[0] = MatrixRotationYawPitchRollDegrees(0.0f, 27.367805f, 0.0f);
		mTetrahedralRotationPacked[1] = MatrixRotationYawPitchRollDegrees(180.0f, 27.367805f, 190.0f);
		mTetrahedralRotationPacked[2] = MatrixRotationYawPitchRollDegrees(-90.0f, -27.367805f, -90.0f);
		mTetrahedralRotationPacked[3] = MatrixRotationYawPitchRollDegrees(90.0f, -27.367805f, 90.0f);

		Camera cam{};

		for (int i = 0; i < 4; i++) {
			MakeTetrahedralShadowCamera(&cam, i, Vector3::Zero, 100.f, 1.f);
			Matrix4 f = cam.GetViewMatrix() * cam.GetProjectionMatrix();
			Matrix4 m = Matrix4::Scaling(Vector3(0.5f, -0.5f, 1.f)) * MatrixTranslation(Vector3(0.5f, 0.5f, .0f));
			mTetrahedralViewProjection[i] = f * m;
		}

	}

}

namespace T3EffectSceneUtil {

	inline void SetParameters(T3EffectParameter_BufferScene* buffer, T3EffectSceneParams& params) {
		buffer->mGlobalLevelScalar = 1.0f / (params.mGlobalLevelWhite - params.mGlobalLevelBlack);
		buffer->mGlobalLevelAdder = -params.mGlobalLevelBlack * buffer->mGlobalLevelScalar;
		buffer->mGlobalLevelIntensity = 1.0f / fmaxf(0.001f, params.mGlobalLevelIntensity);
		buffer->mFogParams = Vector2(params.mFogNear, 1.0f / (params.mFogFar - params.mFogNear));
		buffer->mColorPlane = params.mColorPlane;
		buffer->mFogColor = params.mFogColor;
		buffer->mHDRRange = params.mbAfterEffects ? 6.0f : 1.0f;
		buffer->mShadowColor.x = params.mShadowColor.r;
		buffer->mShadowColor.y = params.mShadowColor.g;
		buffer->mShadowColor.z = params.mShadowColor.b;
		buffer->mNPRLineFalloff = params.mNPRLineFalloff;
		buffer->mNPRLineBias = params.mNPRLineBias;
		buffer->mNPRLineAlphaFalloff = params.mNPRLineAlphaFalloff;
		buffer->mNPRLineAlphaBias = params.mNPRLineAlphaBias;
		buffer->mShadowMomentDepthBias = Vector3(params.mShadowMomentBias * 0.001f, params.mShadowDepthBias * 0.001f,
			params.mShadowAtlasResolution * 0.001f * params.mShadowPositionOffsetBias);
		buffer->mShadowLightBleedReduction = params.mShadowLightBleedReduction;
		buffer->mShadowAtlasResolution = params.mShadowAtlasResolution;
		buffer->mShadowAtlasInvResolution = 1.0f / params.mShadowAtlasResolution;
		buffer->mShadowInvCascadeCount = 1.0f / params.mShadowCascadeCount;
		if (params.mShadowTraceMaxDistance <= 0.00001f) {
			buffer->mShadowTraceMinDistance = buffer->mShadowTraceInvDeltaDistance = 0.f;
		}
		else {
			buffer->mShadowTraceMinDistance = -0.7f / (1 - 0.7f);//0.7 is kShadowFalloffDistance
			buffer->mShadowTraceInvDeltaDistance = 1.0f / ((1 - 0.7f) * params.mShadowTraceMaxDistance);
		}
		buffer->mGraphicBlackThreshold = params.mGraphicBlackThreshold;
		buffer->mGraphicBlackSoftness = params.mGraphicBlackSoftness;
		buffer->mGraphicBlackAlpha = params.mGraphicBlackAlpha;
		buffer->mGraphicBlackNear = params.mGraphicBlackNear;
		buffer->mGraphicBlackFar = params.mGraphicBlackFar;
		buffer->mWindDirection.x = params.mWindParams.mDirection.x;
		buffer->mWindDirection.y = params.mWindParams.mDirection.y;
		buffer->mWindDirection.z = params.mWindParams.mDirection.z;
		buffer->mWindSpeed = params.mWindParams.mSpeed;
		buffer->mWindIdleStrength = params.mWindParams.mIdleStrength;
		buffer->mWindIdleSpacialFrequency = params.mWindParams.mIdleSpacialFrequency;
		buffer->mWindGustSpeed = params.mWindParams.mGustSpeed;
		buffer->mWindGustStrength = params.mWindParams.mGustStrength;
		buffer->mWindGustSpacialFrequency = params.mWindParams.mGustSpacialFrequency;
		buffer->mWindGustIdleStrengthMultiplier = params.mWindParams.mGustIdleStrengthMultiplier;
		buffer->mWindGustSeparationExponent = params.mWindParams.mGustSeparationExponent;
		buffer->mTAAWeight = params.mTAAWeight;
		buffer->mTime = params.mTime;
		buffer->mPrevTime = params.mPrevTime;
		buffer->mHDR_PaperWhiteNits = params.mHDR_PaperWhiteNits;
		buffer->mLineSamplingOffsets.SetRow(0, { 0.14307f, -0.136388f,-0.8432f,-0.229383f });
		buffer->mLineSamplingOffsets.SetRow(1, { 0.449228f, 0.662613f, 0.851355f, -0.0548139f });
		buffer->mLineSamplingOffsets.SetRow(2, { -0.00163015f,-0.864458f,-0.185045f,0.665272f });
		buffer->mLineSamplingOffsets.SetRow(3, { -0.362175f, 0.0918911f,0.52452f,-0.486055f });
		buffer->mBrushMipBias = -1.f;
		for (int face = 0; face < 4; face++) {
			Matrix4 mat = ShadowUtil::GetTetrahedralViewProjectionMatrix(face);
			buffer->mTetrahedralMatrices[face * 3 + 0] = mat.GetColumn(0);
			buffer->mTetrahedralMatrices[face * 3 + 1] = mat.GetColumn(1);
			buffer->mTetrahedralMatrices[face * 3 + 2] = mat.GetColumn(2);
		}
		u32 msaa = 1;
		RenderAntialiasType a = sGlobalRenderConfig.mAliasingType;
		if (a == eRenderAntialias_SMAA || a == eRenderAntialias_TAA_MSAA_2x)
			msaa = 2;
		else if (a == eRenderAntialias_MSAA_4x || a == eRenderAntialias_TAA_MSAA_4x)
			msaa = 4;
		else if (a == eRenderAntialias_MSAA_8x || a == eRenderAntialias_TAA_MSAA_8x)
			msaa = 8;
		buffer->mMSAASampleCount = msaa;
		buffer->mNoiseScale = params.mNoiseScale;
		buffer->mNoiseFrameIndex0 = std::rand();
		buffer->mNoiseFrameIndex1 = std::rand();
	}

}

namespace T3EffectShadowUtil {

	inline void SetParameters(T3EffectParameter_BufferShadow* buffer, T3EffectShadowParams& params) {
		buffer->mShadowViewMatrix = params.mShadowViewMatrix;
		buffer->mShadowMatrix0[0] = params.mShadowMatrix[0].GetRow(0);
		buffer->mShadowMatrix0[1] = params.mShadowMatrix[0].GetRow(1);
		buffer->mShadowMatrix0[2] = params.mShadowMatrix[0].GetRow(2);
		buffer->mShadowMatrix0[3] = params.mShadowMatrix[0].GetRow(3);
		buffer->mShadowMatrix1[0] = params.mShadowMatrix[1].GetRow(0);
		buffer->mShadowMatrix1[1] = params.mShadowMatrix[1].GetRow(1);
		buffer->mShadowMatrix1[2] = params.mShadowMatrix[1].GetRow(2);
		buffer->mShadowMatrix1[3] = params.mShadowMatrix[1].GetRow(3);
		buffer->mShadowMatrix2[0] = params.mShadowMatrix[2].GetRow(0);
		buffer->mShadowMatrix2[1] = params.mShadowMatrix[2].GetRow(1);
		buffer->mShadowMatrix2[2] = params.mShadowMatrix[2].GetRow(2);
		buffer->mShadowMatrix2[3] = params.mShadowMatrix[2].GetRow(3);
		buffer->mShadowMatrix3[0] = params.mShadowMatrix[3].GetRow(0);
		buffer->mShadowMatrix3[1] = params.mShadowMatrix[3].GetRow(1);
		buffer->mShadowMatrix3[2] = params.mShadowMatrix[3].GetRow(2);
		buffer->mShadowMatrix3[3] = params.mShadowMatrix[3].GetRow(3);
		buffer->mTextureMatrix0 = Vector4(params.mTextureMatrix[0], params.mTextureMatrix[1], params.mTextureMatrix[2], params.mTextureMatrix[3]);
		buffer->mTextureMatrix1 = Vector2(params.mTextureMatrix[4], params.mTextureMatrix[5]);
		buffer->mFilterRadius = fminf(4.5f, fmaxf(0.f, params.mFilterRadius));
		buffer->mShadowSoftness = params.mShadowSoftness;
		buffer->mShadowCascadeSplitLambda = params.mShadowCascadeSplitLambda;
		buffer->mShadowCascadeCount = (float)params.mShadowCascadeCount;
		buffer->mShadowMinDepth = params.mShadowMinDepth;
		buffer->mShadowMaxDepth = params.mShadowMaxDepth;
		buffer->mShadowInvRadius = params.mLightRadius <= 0.f ? 0.f : 1.0f / params.mLightRadius;
		buffer->mShadowDistanceFalloff = params.mLightDistanceFalloff;
		buffer->mShadowCosConeAngle = params.mLightSpotParams.x;
		buffer->mShadowInvDeltaCosConeAngle = params.mLightSpotParams.y;
		buffer->mShadowDistanceScale = 1.0f / (((1.0f / params.mpSceneCamera->mFarClip) * params.mShadowFadeMaxDistance)
			- (params.mShadowFadeMinDistance / params.mpSceneCamera->mFarClip));
		buffer->mShadowDistanceBias = -buffer->mShadowDistanceScale * (params.mShadowFadeMinDistance / params.mpSceneCamera->mFarClip);
		buffer->mShadowColor = params.mLightColor;
		buffer->mShadowIntensity = params.mLightIntensity;
		buffer->mGutterColor = params.mGutterColor;
		buffer->mShadowArrayIndex = params.mShadowArrayIndex;
		buffer->mShadowGoboIndex = params.mShadowGoboIndex;
		buffer->mShadowMultisampleCount = 2;
		buffer->mLightGroupMask = params.mLightGroupMask;
		buffer->mLightTileIndex = params.mLightTileIndex + 1;
		buffer->mShadowMomentBias = params.mShadowMomentBias * 0.001f;
		buffer->mShadowMapResolution = (float)params.mShadowMapResolution;
		buffer->mShadowMapResolution1 = (float)params.mShadowMapResolution1;
		buffer->mShadowMapInvResolution = buffer->mShadowMapResolution <= 0.f ? 0.f : 1.0 / buffer->mShadowMapResolution;
		buffer->mShadowMapInvResolution1 = buffer->mShadowMapResolution1 <= 0.f ? 0.f : 1.0 / buffer->mShadowMapResolution1;
		buffer->mShadowSDSMTexelScale = buffer->mShadowMapInvResolution / params.mShadowCascadeCount;
		buffer->mShadowSDSMTexelBias = (float)params.mShadowArrayIndex / params.mShadowCascadeCount;
		buffer->mShadowMapTexelMax = params.mShadowMapResolution - 1;
		if (params.mbLightWorldSpace) {
			buffer->mShadowPosition = params.mShadowPosition;
			buffer->mShadowDirection = -1.0f * params.mShadowDirection;
		}
		else {
			buffer->mShadowPosition = params.mpSceneCamera->GetViewMatrix().TransformPoint(buffer->mShadowPosition);
			buffer->mShadowDirection = -1.0f * params.mpSceneCamera->GetViewMatrix().TransformNormal(buffer->mShadowDirection);
			buffer->mProjectedShadowDirection = (params.mpSceneCamera->GetViewMatrix()
				* params.mpSceneCamera->GetProjectionMatrix()).TransformNormal(buffer->mShadowDirection);//should be correct?
			//shadow dir is in range -1 to 1 since projected space. transform to 0-1, add 1 then half it
			buffer->mProjectedShadowDirection.x = (buffer->mProjectedShadowDirection.x
				+ 1.0f) * 0.5f;
			//y is inverted.
			buffer->mProjectedShadowDirection.y = 0.5f - (buffer->mProjectedShadowDirection.y * 0.5f);
		}
		buffer->mSceneMax = params.mSceneMax;
		buffer->mSceneMin = params.mSceneMin;
		buffer->mShadowGridCellSize = params.mShadowGridCellSize;
		buffer->mPCFKernel[0] = Vector4(-0.f, 0.0003662109375f, -0.000732421875f, 0.f);
		buffer->mPCFKernel[1] = Vector4(0.f, -0.0010986328125f, 0.00146484375f, 0.f);
		buffer->mPCFKernel[2] = buffer->mPCFKernel[3] = Vector4::Zero;

	}

}

namespace T3EffectSimpleUtil {

	inline void SetParameters(T3EffectParameter_BufferSimple* buffer, const Color& color, const Plane& plane) {
		buffer->mClipPlane = plane.mPlane;
		buffer->mColor = color;
		buffer->mBoundsMin = buffer->mBoundsMax = 0.f;
		buffer->mIndex = 0;
		buffer->mCopyOffset[0] = buffer->mCopyOffset[1] = 0;
		buffer->mCopySize[0] = buffer->mCopySize[1] = 0;
		buffer->mVisibilityThreshold = 0.f;
		buffer->mScreenSpaceDepthScale = 1.0f;
		buffer->mScreenSpaceDepthOffset = 0.f;
		buffer->mTextureMatrix0 = Vector4(1.0f, 0.f, 0.f, 1.0f);
		buffer->mTextureMatrix1 = Vector2(0.f, 0.f);
		buffer->mPrimitiveParams = Vector4(0.f, 0.f, 1.0f, 0.f);
	}

	inline void SetParameters(T3EffectParameter_BufferSimple* buffer, T3EffectSimpleParams& params) {
		buffer->mColor = params.mColor;
		buffer->mClipPlane = params.mClipPlane.mPlane;
		buffer->mBoundsMin = params.mBoundingBox.mMin;
		buffer->mBoundsMax = params.mBoundingBox.mMax;
		buffer->mVisibilityThreshold = params.mVisibilityThreshold;
		buffer->mIndex = params.mIndex;
		buffer->mTextureMatrix0 = Vector4(
			params.mTextureMatrix[0],
			params.mTextureMatrix[1],
			params.mTextureMatrix[2],
			params.mTextureMatrix[3]
		);
		buffer->mTextureMatrix1 = Vector2(
			params.mTextureMatrix[4],
			params.mTextureMatrix[5]
		);
		buffer->mCopyOffset[0] = params.mCopyOffset[0];
		buffer->mCopyOffset[1] = params.mCopyOffset[1];
		buffer->mCopySize[0] = params.mCopySize[0];
		buffer->mCopySize[1] = params.mCopySize[1];
		buffer->mScreenSpaceDepthOffset = params.mScreenSpaceDepthOffset;
		buffer->mScreenSpaceDepthScale = params.mScreenSpaceDepthScale;
		buffer->mPrimitiveParams.x = cosf(3.14159265f / (float)params.mNumSides);
		float tmp = fminf(1.5707954f/*90deg in rad*/, fmaxf(0.f, 0.5f * params.mConeAngle));
		buffer->mPrimitiveParams.y = sinf(tmp);
		buffer->mPrimitiveParams.z = cosf(tmp);
		buffer->mPrimitiveParams.w = params.mConeRadius;
	}

}

namespace T3EffectUtil {

	inline void SetParameters(T3EffectParameter_BufferGaussian* buffer, T3EffectGaussianParams& params) {
		Vector4* pRow = &buffer->mRow0;
		for (int i = 2; i < 22; i += 4) {
			pRow->x = expf(-0.5f * params.mSigmaSquared * ((float)i - 2.0f) * ((float)i - 2.0f)) /
				sqrtf(params.mSigmaSquared * 3.14159265f * 2.0f);
			pRow->y = expf(-0.5f * params.mSigmaSquared * ((float)i - 1.0f) * ((float)i - 1.0f)) /
				sqrtf(params.mSigmaSquared * 3.14159265f * 2.0f);
			pRow->z = expf(-0.5f * params.mSigmaSquared * ((float)i - 0.0f) * ((float)i - 0.0f)) /
				sqrtf(params.mSigmaSquared * 3.14159265f * 2.0f);
			pRow->w = expf(-0.5f * params.mSigmaSquared * ((float)i + 1.0f) * ((float)i + 1.0f)) /
				sqrtf(params.mSigmaSquared * 3.14159265f * 2.0f);
			pRow++;
		}
	}

}