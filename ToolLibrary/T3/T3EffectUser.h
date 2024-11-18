#ifndef _T3EFFECT_USER_H
#define _T3EFFECT_USER_H

// THIS FILE DECLARES THE BASE CLASS WHCIH YOU CAN OVERRIDE FUNCTIONS FROM TO IMPLEMENT YOUR OWN RENDER API
// TO USE WITH THE TELLTALE RENDER TYPES

#include "../TelltaleToolLibrary.h"

struct T3GFXVertexState;
struct T3GFXBuffer;
struct T3EffectParameterBuffer;
struct T3Texture;

struct T3BufferView;
struct T3TextureView;
enum T3EffectParameterType : u32;
struct RenderFrameStats;
struct T3EffectParameterBoundState;
enum T3RenderHiStencilMode;
enum PlatformType;
struct T3RenderTargetSet;
struct T3RenderClear;
struct T3RenderViewport;

class ToolLibRenderAdapters {
public:

	/**
	 * Upload the static vertices in the given vertex state to the GPU. Most of this time this can just call UpdateGFXBufferFromCPUBuffer with the vert buffer.
	 */
	virtual void EndStaticVertices(T3GFXVertexState& state);

	/**
	 * Upload the static indices in the given vertex state to the GPU. Most of this time this can just call UpdateGFXBufferFromCPUBuffer with the index buffer.
	 */
	virtual void EndStaticIndices(T3GFXVertexState& state);
	
	virtual void UpdateGFXBufferFromCPUBuffer(T3GFXBuffer* pBuffer);

	/**
	 * Called when a T3GFXResource is constructed.
	 */
	virtual void OnResourceCreation(T3GFXResource* pResource);

	/**
	 * Called when a T3GFXResource is about to be destroyed;
	 */
	virtual void OnResourceDestroy(T3GFXResource* pResource);

	/**
	 * Test the capability of the render API and return if it supports the parameter cap.
	 * 
	 * At the moment, if the graphics library uses binding for buffers (eg opengl) then this will be OK.
	 * However any non binding defered stuff won't work.
	 * 
	 */
	virtual bool TestCap(GFXPlatformCapability cap);

	/**
	 * Bind the given buffer to the render API context. Update the stats.
	 */
	virtual bool BindBuffer(T3EffectParameterBoundState& state, RenderFrameStats& stats, T3EffectParameterType paramType, T3EffectParameterBuffer* pBuffer, u32 bufferOffset);

	/**
	 * Bind the given buffer *data* to the render API context. Update the stats.
	 */
	virtual bool BindBuffer_0(T3EffectParameterBoundState& state, RenderFrameStats& stats, T3EffectParameterType paramType, const void* pData, u32 scalarSize);

	/**
	 * Bind the given generic buffer (eg vertices) to the render API context. Update the stats.
	 */
	virtual bool BindGenericBuffer(T3EffectParameterBoundState& state, RenderFrameStats& stats, T3EffectParameterType paramType, T3GFXBuffer* pBuffer, T3BufferView& baseView);

	/**
	 * Bind the given texture to the render API. Update the stats.
	 */
	virtual bool BindTexture(T3EffectParameterBoundState& state, RenderFrameStats& stats, T3EffectParameterType paramType, T3Texture* pTexture, T3TextureView& baseView);

	/**
	 * Upload the given texture view of the given texture to the GPU.
	 */
	virtual void UpdateTextureData(T3Texture* pTexture, const T3TextureView& view, void* pTexData, u32 texDataSize, u32 mipWidth, u32 mipHeight);

	/**
	 * Put the current game resolution of the viewable area (backbuffer size) into the output arguments.
	 */
	virtual void GetResolution(u32& outWidth, u32& outHeight);

	/**
	 * Return the delta time from this and last frame.
	 */
	virtual float GetDeltaTime();

	/**
	 * Return the current frame index.
	 */
	virtual u64 GetFrameIndex();

	/**
	 * This can effect rendering quality. Just return PC (windows) or mac or linux.
	 */
	virtual PlatformType GetPlatform();

	/**
	 * Push a render target.
	 */
	virtual void PushRenderTarget(const T3RenderTargetSet& set, const T3RenderClear& renderClear);

	/**
	 * Pops a render target.
	 */
	virtual void PopRenderTarget();

	/**
	 * Sets the current viewport.
	 */
	virtual void SetCurrentViewport(const T3RenderViewport& viewport);

	/**
	 * Switches the current render target set.
	 */
	virtual void SwitchRenderTarget(const T3RenderTargetSet& set, const T3RenderClear& renderClear, bool bSetColorTarget, bool bSetDepthTarget, bool bClear);

	/**
	 * Clear the RT
	 */
	virtual void Clear(const T3RenderClear& clear);

	/**
	 * Switch the current render target to the default depth and color one (index 0). RenderDevice::mpDefaultRenderTarget/DepthBuffer.
	 */
	virtual void SwitchDefaultRenderTarget(const T3RenderClear& clear);

};

#endif

#ifdef __T3EFFECTUSER_IMPL_DEFAULTS

void ToolLibRenderAdapters::SwitchDefaultRenderTarget(const T3RenderClear& clear) {}

void ToolLibRenderAdapters::Clear(const T3RenderClear& clear) {}

void ToolLibRenderAdapters::SwitchRenderTarget(const T3RenderTargetSet& set, const T3RenderClear& renderClear, bool bSetColorTarget, bool bSetDepthTarget, bool bClear) {}

void ToolLibRenderAdapters::SetCurrentViewport(const T3RenderViewport& viewport) {}

void ToolLibRenderAdapters::PopRenderTarget(){}

void ToolLibRenderAdapters::PushRenderTarget(const T3RenderTargetSet& set, const T3RenderClear& renderClear) {}

void ToolLibRenderAdapters::GetResolution(u32& outWidth, u32& outHeight) {}

void ToolLibRenderAdapters::EndStaticVertices(T3GFXVertexState& state){}

void ToolLibRenderAdapters::EndStaticIndices(T3GFXVertexState& state) {}

void ToolLibRenderAdapters::OnResourceCreation(T3GFXResource* pResource) {}

void ToolLibRenderAdapters::OnResourceDestroy(T3GFXResource* pResource) {}

void ToolLibRenderAdapters::UpdateGFXBufferFromCPUBuffer(T3GFXBuffer* pBuffer) {}

bool ToolLibRenderAdapters::TestCap(GFXPlatformCapability) { return false; }

bool ToolLibRenderAdapters::BindBuffer(T3EffectParameterBoundState& state, RenderFrameStats& stats, T3EffectParameterType paramType, T3EffectParameterBuffer* pBuffer, u32 bufferOffset) { return false; }

bool ToolLibRenderAdapters::BindBuffer_0(T3EffectParameterBoundState& state, RenderFrameStats& stats, T3EffectParameterType paramType, const void* pData, u32 scalarSize) { return false; }

bool ToolLibRenderAdapters::BindGenericBuffer(T3EffectParameterBoundState& state, RenderFrameStats& stats, T3EffectParameterType paramType, T3GFXBuffer* pBuffer, T3BufferView& baseView) { return false; }

bool ToolLibRenderAdapters::BindTexture(T3EffectParameterBoundState& state, RenderFrameStats& stats, T3EffectParameterType paramType, T3Texture* pTexture, T3TextureView& baseView) { return false; }

void ToolLibRenderAdapters::UpdateTextureData(T3Texture* pTexture, const T3TextureView& view, void* pTexData, u32 texDataSize, u32 w, u32 h) {}

float ToolLibRenderAdapters::GetDeltaTime() { return 0.f; }

u64 ToolLibRenderAdapters::GetFrameIndex() { return 0llu; }

PlatformType ToolLibRenderAdapters::GetPlatform() { return ePlatform_PC; }

#endif