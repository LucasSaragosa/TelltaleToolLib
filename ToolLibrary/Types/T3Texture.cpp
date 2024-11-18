// This file was written by Lucas Saragosa. The code derives from Telltale Games' Engine.
// I do not intend to take credit for it, however; Im the author of this interpretation of 
// the engine and require that if you use this code or library, you give credit to me and
// the amazing Telltale Games.
#include "../LibraryConfig.h"
#include "T3Texture.h"
#include "ObjectSpaceIntrinsics.h"
#include "../T3/Render.hpp"
#include "../T3/T3EffectParameters.h"

std::vector<RenderFrameUpdateList::ResourceEntry_NonBlockingTextureLoad> RenderFrameUpdateList::smExtraNonBlockingLoads{};

Color Color::Black{0.f,0.f,0.f,1.0f};
Color Color::White{ 1.f,1.f,1.f,1.0f };
Color Color::Red{ 1.f,0.f,0.f,1.0f };
Color Color::Green{ 0.f,1.f,0.f,1.0f };
Color Color::Blue{ 0.f,0.f,1.f,1.0f };
Color Color::Cyan{ 0.f,1.f,1.f,1.0f };
Color Color::Magenta{ 1.f,0.f,1.f,1.0f };
Color Color::Yellow{ 1.f,1.f,0.f,1.0f };
Color Color::DarkRed{ 0.5f,0.f,0.f,1.0f };
Color Color::DarkGreen{ 0.f,0.5f,0.f,1.0f };
Color Color::DarkBlue{ 0.f,0.f,0.5f,1.0f };
Color Color::DarkCyan{ 0.f,0.5f,0.5f,1.0f };
Color Color::DarkMagenta{ 0.5f,0.f,0.5f,1.0f };
Color Color::DarkYellow{ 0.5f,0.5f,0.f,1.0f };
Color Color::Gray{ 0.5f,0.5f,0.5f,1.0f };
Color Color::LightGray{ 0.75f,0.75f,0.75f,1.0f };
Color Color::NonMetal{ 0.4f,0.4f,0.4f,1.0f };

Vector2 Vector2::Zero{0.f, 0.f};
Vector4 Vector4::Zero{0.f, 0.f, 0.f, 0.f};
Vector3 Vector3::Up{ 0.0f, 1.0f, 0.0f };
Vector3 Vector3::Down{ 0.0f, -1.0f, 0.0f };
Vector3 Vector3::Left{ 1.0f, 0.0f, 0.0f };
Vector3 Vector3::Right{ -1.0f, 0.0f, 0.0f };
Vector3 Vector3::Forward{ 0.0f, 0.0f, 1.0f };
Vector3 Vector3::Backward{ 0.0f, 0.0f, -1.0f };
Vector3 Vector3::VeryFar{ 1e6f, 1e6f, 1e6f };
Vector3 Vector3::Identity{ 1.0f, 1.0f, 1.0f };
Vector3 Vector3::Zero{ 0.f, 0.f, 0.f };

Quaternion Quaternion::kIdentity{0.f,0.f,0.f,1.0f};
Quaternion Quaternion::kBackward{ 0.f, 0.f, 0.f, -4.3711388e-8f };
Quaternion Quaternion::kUp{ 0.f, 0.f, 0.f, 0.70710677f };//2^-0.5
Quaternion Quaternion::kDown{ 0.f, 0.f, 0.f, 0.70710677f };//2^-0.5

T3SamplerStateBlock kDefault;

bool T3SamplerStateBlock::sInitialized = false;
T3SamplerStateBlock::SamplerStateEntry T3SamplerStateBlock::smEntries[6] = {};

template<typename T>
void _no_delete(T*) {}

void T3Texture::SetUsedOnFrame(RenderFrameUpdateList& updateList){
    if(!updateList.UpdateTextureStreaming(std::shared_ptr<T3Texture>(this, &_no_delete<T3Texture>), eTextureStreaming_RequireFullTexture, (float)mNumMipLevels))
        T3GFXResource::SetUsedOnFrame(updateList.mFrameIndex);
}

T3RenderTargetID T3RenderTargetUtil::RegisterTarget(T3RenderTargetContext* pContext, RenderFrameUpdateList& updateList, std::shared_ptr<T3Texture> pTexture, T3RenderTargetParams& params) {
	T3RenderTargetID id = AllocateTemporaryTarget(pContext, params);
    pTexture->SetUsedOnFrame(updateList);
    pContext->mTemporaryList.tail()->mpExternalTexture = std::move(pTexture);
	pContext->mTemporaryList.tail()->mRefCount = 1;
    return id;
}

T3RenderTargetID T3RenderTargetUtil::RegisterTarget(T3RenderTargetContext* pContext, RenderFrameUpdateList& updateList, T3RenderTargetConstantID id, std::shared_ptr<T3Texture> pTexture){
    pContext->mDefaultList[id].mRefCount++;
    pContext->mDefaultList[id].mpExternalTexture = std::move(pTexture);
    return T3RenderTargetID{ id };
}

T3EffectParameterGroup* T3EffectParameterGroupStack::AllocateParametersWithBuffer(RenderFrameUpdateList& updateList,
	T3EffectParameterUniformBufferPointers& buffers, BitSet<enum T3EffectParameterType, 150, 0>& params) {
	T3EffectParameterGroup* ret = T3EffectParameterGroup::Create(*updateList.mHeap, params);
	T3EffectParameterUniformBufferOffsets offsets{};
    u32 size = T3EffectParameterUtil::AllocateUniformBuffer(0, params, offsets);
    void* buffer = updateList.mHeap->Alloc(4 * size, 16);
	if (size)
		ret->AddUniformBufferData(buffer, &buffers, &offsets, size);
	PushParameterGroup(*updateList.mHeap, this, ret);
	mParamTypes.Set(params);
	return ret;
};

T3EffectParameterGroup* T3EffectParameterGroupStack::AllocateParametersWithBuffer(void* buffer, u32 bufferSize, RenderFrameUpdateList& updateList,
	T3EffectParameterUniformBufferPointers& buffers, BitSet<enum T3EffectParameterType, 150, 0>& params){
    T3EffectParameterGroup* ret = T3EffectParameterGroup::Create(*updateList.mHeap, params);
    T3EffectParameterUniformBufferOffsets offsets{};
    u32 size = bufferSize;
    if (size)
        ret->AddUniformBufferData(buffer, &buffers, &offsets, size);
    PushParameterGroup(*updateList.mHeap, this, ret);
    mParamTypes.Set(params);
    return ret;
};

u8* T3EffectParameterGroup::UpdateParameterBufferData(RenderFrameUpdateList& updateList, T3EffectParameterType type, u32 size){
    int paramIndex = GetIndexForParameter(type);
    if (paramIndex != -1) {
        _T3EffectBufferParam* params = (_T3EffectBufferParam*)this + 1;
        if (params[paramIndex].storage == eEffectParameterStorage_UniformBufferData)
            return *((u8**)_GetParameterStorageInternal(paramIndex));
        else if (params[paramIndex].storage == eEffectParameterStorage_UniformBuffer) {
            u8** paramData = (u8**)_GetParameterStorageInternal(paramIndex);
            return updateList.UpdateParameterBuffer(std::shared_ptr<T3EffectParameterBuffer>((T3EffectParameterBuffer*)paramData[0], &_no_delete<T3EffectParameterBuffer>)
            , (u32)paramData[1],size, type);
        }
        else return 0;
    }
    else return 0;
}

void* T3EffectParameterGroup::SetUniformBufferData(T3EffectParameterType param, void* uniformBufferData, u32 scalarSize){
    if (!scalarSize)
        scalarSize = sBufferDesc[param].mScalarSize;
	int p = GetIndexForParameter(param);
	if (p != -1) {
		_T3EffectBufferParam* params = (_T3EffectBufferParam*)this + 1;
		u64* paramData = (u64*)((u8*)this + ((u32)params[p].off << 2));
		if(T3::TestCap(eGFXPlatformCap_BindProgramBufferData)){
            params[p].storage = eEffectParameterStorage_UniformBufferData;
			u64* paramData = (u64*)((u8*)this + ((u32)params[p].off << 2));
            paramData[1] = (u64)scalarSize;
            paramData[0] = (u64)uniformBufferData;
            return uniformBufferData;
        }
        else {
           // params[p].storage = eEffectParameterStorage_UniformBuffer;
            //StreamParameterData in updatelist
            MessageBoxA(0,"RenderFrameUpdateList::StreamParameterData is not implemented. Please use bindable render API.", "Unsupported operation", MB_ICONERROR);
        }
	}
}

void T3Texture::CreateSolidTexture(Color color, T3TextureLayout layout){
	CreateParams params{};
	params.mTag = Symbol::kEmptySymbol;
	params.mLayout = eTextureLayout_2D;
	params.mDepth = params.mArraySize = 1;
	params.mNumMipLevels = 1;
	params.mbCPUAccessible = 1;
	params.mbVideoTexture = 0;
	params.mWidth = 1;
	params.mHeight = 0x1;
	params.mFormat = eSurface_RGBA8;
	params.mGamma = eSurfaceGamma_Linear;
	params.mMultisample = eSurfaceMultisample_None;
	params.mAccess = eSurface_ReadOnly;
	u32 textures = CreateTexture(params);
	for(int i = 0; i < textures; i++){
		T3Texture::RegionStreamHeader& header = mRegionHeaders[i];
        DataStreamMemory* pMemory = (DataStreamMemory*)header.mpAsyncStream;
		pMemory->SetPosition(0, DataStreamSeekType::eSeekType_Begin);
		pMemory->SetMode(DataStreamMode::eMode_Write);
		u8 bytes[4]{ 0,0,0,0 };
		bytes[0] = (u8)((i32)(color.r * 255.f) & 0xFF);
		bytes[1] = (u8)((i32)(color.g * 255.f) & 0xFF);
		bytes[2] = (u8)((i32)(color.b * 255.f) & 0xFF);
		bytes[3] = (u8)((i32)(color.a * 255.f) & 0xFF);
		pMemory->SerializeWrite((const char*)bytes, 4);
	}
}

bool T3Texture::CreateRenderTarget(u32 width, u32 height, T3SurfaceFormat format, bool bPrimaryDisplaySurface, bool bVideoTexture){
    CreateParams params{};
	params.mWidth = width;
    params.mAllocationType = eGFXPlatformAllocation_Unknown;
    params.mGamma = eSurfaceGamma_Linear;
	params.mbCPUAccessible = 0;
    params.mDepth = params.mArraySize = 1;
    params.mTag = Symbol("RenderTarget");
	params.mbPrimaryDisplaySurface = bPrimaryDisplaySurface;
	params.mNumMipLevels = 1;
	params.mAccess = eSurface_ReadWrite;
	params.mbVideoTexture = bVideoTexture;
	params.mFastMemHeap = eGFXPlatformFastMemHeap_None;
	params.mHeight = height;
	params.mFormat = format;
	params.mUsage = eResourceUsage_RenderTarget;
    return CreateTexture(params) != 0;
}

void T3Texture::CreateHorizontalGradientTexture(Color startColor, Color endColor, int numSteps){
    CreateParams params{};
    params.mTag = Symbol::kEmptySymbol;
    params.mLayout = eTextureLayout_2D;
    params.mDepth = params.mArraySize = 1;
    params.mNumMipLevels = 1;
    params.mbCPUAccessible = 1;
    params.mbVideoTexture = 0;
    params.mWidth = 0x40;
    params.mHeight = 0x1;
    params.mFormat = eSurface_RGBA8;
    params.mGamma = eSurfaceGamma_Linear;
    params.mMultisample = eSurfaceMultisample_None;
    params.mAccess = eSurface_ReadOnly;
    CreateTexture(params);
    LockContext ctx = Lock(0, 0);
    if (numSteps > 64)
        numSteps = 64;
    float incR = std::fabsf(endColor.r - startColor.r) / (float)numSteps;
    float incG = std::fabsf(endColor.g - startColor.g) / (float)numSteps;
    float incB = std::fabsf(endColor.b - startColor.b) / (float)numSteps;
    float incA = std::fabsf(endColor.a - startColor.a) / (float)numSteps;
    float srtR = std::fminf(startColor.r, endColor.r);
    float srtG = std::fminf(startColor.g, endColor.g);
    float srtB = std::fminf(startColor.b, endColor.b);
    float srtA = std::fminf(startColor.a, endColor.a);
    float step = 0.f;
    for(int i = 0; i < 0x40; i++){//steps = 8, width = 64
		if (i && ((i % numSteps) == 0))
			step += 1.0f;
        ctx.mpTextureData[(i << 2) + 0] = (u8)((u32)((srtR + step * incR) * 255.0f));
        ctx.mpTextureData[(i << 2) + 1] = (u8)((u32)((srtG + step * incG) * 255.0f));
        ctx.mpTextureData[(i << 2) + 2] = (u8)((u32)((srtB + step * incB) * 255.0f));
        ctx.mpTextureData[(i << 2) + 3] = (u8)((u32)((srtA + step * incA) * 255.0f));
    }
    Unlock(ctx);
    mType = eTxToonLookup;
    mToonGradientCutoff = (1.0f / 256.f) + (64.0f / (float)numSteps);
    mAlphaMode = eTxNoAlpha;
    mColorMode = eTxColorFull;
}

const T3Texture::LockContext T3Texture::Lock(u32 mip, u32 face){
    LockContext ctx{};
    ctx.mFace = face;
    ctx.mMipIndex = mip;
    T3SurfaceFormatDesc desc = T3::T3SurfaceFormat_GetDesc(mSurfaceFormat);
    MipSizeData szData{};
    T3::ComputeMipsData(&szData, &desc, mWidth, mHeight, mip + 1, 1, 1);
    ctx.mMipWidth = szData.mSizes[mip].mWidthPix;
    ctx.mMipHeight = szData.mSizes[mip].mHeightPix;
    ctx.mPitch = szData.mSizes[mip].mPitchBytes;
    ctx.mDataSize = ctx.mPitch * szData.mSizes[mip].mHeightBlocks;
    ctx.mpTextureData = 0;
    for(int i = 0; i < mRegionHeaders.mSize; i++){
        if(mRegionHeaders[i].mFaceIndex == face && mRegionHeaders[i].mMipIndex == mip){
            ctx.mpTextureData = (u8*)((DataStreamMemory*)mRegionHeaders[i].mpAsyncStream)->mMemoryBuffer;
            break;
        }
    }
    if(!ctx.mpTextureData){
        TelltaleToolLib_RaiseError("Invalid mip or face passed into T3Texture::Lock which could not be found", ERR);
        __debugbreak();
        return ctx;
    }
    return ctx;
}

void T3Texture::Unlock(const LockContext& context){
    T3TextureView view{};
    view.mMipCount = view.mSliceCount = 1;
    view.mSliceIndex = context.mFace;
    view.mMipIndex = context.mMipIndex;
    TelltaleToolLib_GetRenderAdaptersStruct()->UpdateTextureData(this, view, context.mpTextureData, context.mDataSize, context.mMipWidth, context.mMipHeight);
}

u32 T3Texture::CreateTexture(CreateParams& params) {
    u32 mips = T3::GetNumMipLevelsNeeded(params.mFormat, params.mWidth, params.mHeight);
    if (!params.mNumMipLevels)
        params.mNumMipLevels = mips;
    else {
        if (mips < params.mNumMipLevels)
            params.mNumMipLevels = mips;
    }
    MipSizeData szData{};
    T3SurfaceFormatDesc desc = T3::T3SurfaceFormat_GetDesc(params.mFormat);
    T3::ComputeMipsData(&szData, &desc, params.mWidth, params.mHeight, params.mNumMipLevels, 1, 1);
    mSwizzle.mSwizzle[0] = eRenderSwizzle_R;
    mSwizzle.mSwizzle[1] = eRenderSwizzle_G;
    mSwizzle.mSwizzle[2] = eRenderSwizzle_B;
    mSwizzle.mSwizzle[3] = eRenderSwizzle_A;
    mSurfaceFormat = params.mFormat;
    mResourceUsage = params.mUsage;
    mWidth = params.mWidth;
    mHeight = params.mHeight;
    mDepth = params.mDepth;
    mArraySize = params.mArraySize;
    mNumMipLevels = params.mNumMipLevels;
    mSurfaceAccess = params.mAccess;
    mSurfaceGamma = params.mGamma;
    mSurfaceMultisample = params.mMultisample;
    mTextureLayout = params.mLayout;
    mNumMipLevelsAllocated = mNumMipLevels;
    u32 surfaces = mNumMipLevels;
    if (mDepth == 0)
        mDepth = 1;
    u32 faces = 1;
    if (mTextureLayout == eTextureLayout_CubeArray) {
        surfaces *= 6 * mArraySize;
        faces = 6 * mArraySize;
    }
    else if (mTextureLayout == eTextureLayout_2DArray) {
        surfaces *= mArraySize;
        faces = mArraySize;
    }
    else if (mTextureLayout == eTextureLayout_Cube){
        surfaces *= 6;
        faces = 6;
    }
    mNumSurfacesLoaded = mNumSurfacesRequested = mNumSurfacesRequired = surfaces;
    mRegionHeaders.Clear(0);
    mRegionHeaders.Resize(surfaces);
    for (int mip = 0; mip < mNumMipLevels; mip++) {
        for(int face = 0; face < faces; face++){
			mRegionHeaders.AddElement(0, 0, 0);
			T3Texture::RegionStreamHeader& header = mRegionHeaders[mRegionHeaders.mSize - 1];
            header.mMipIndex = mip;
            header.mFaceIndex = face;
            header.mPitch = szData.mSizes[mip].mPitchBytes;
            header.mSlicePitch = header.mPitch * szData.mSizes[mip].mHeightPix;
            header.mMipCount = 1;//XBOX can be > 1! fuck those.
            header.mDataSize = header.mSlicePitch * mDepth;
			DataStreamMemory* pMemory = new DataStreamMemory(header.mDataSize, 1);
			pMemory->SetPosition(0, DataStreamSeekType::eSeekType_Begin);
			pMemory->SetMode(DataStreamMode::eMode_Write);
            header.mpAsyncStream = pMemory;
        }
    }
    return surfaces;
}

T3Texture::~T3Texture() {
    RenderFrameUpdateList::OnTextureDestroyed(this);
	for (auto it = mAuxilaryData.begin(); it != mAuxilaryData.end(); it++) {
		delete* it;
	}
	for (int i = 0; i < mRegionHeaders.mSize; i++) {
		if (mRegionHeaders[i].mpAsyncStream)
			delete mRegionHeaders[i].mpAsyncStream;
		mRegionHeaders[i].mpAsyncStream = 0;
	}
}

/*
Matrix4.cpp
Written by Matthew Fisher

a 4x4 Matrix4 structure.  Used very often for affine vector transformations.
*/

Matrix4::Matrix4()
{

}

Matrix4::Matrix4(const Matrix4& M)
{
    for (UINT Row = 0; Row < 4; Row++)
    {
        for (UINT Col = 0; Col < 4; Col++)
        {
            _Entries[Row][Col] = M[Row][Col];
        }
    }
}

Matrix4::Matrix4(const Vector3& V0, const Vector3& V1, const Vector3& V2)
{
    _Entries[0][0] = V0.x;
    _Entries[0][1] = V0.y;
    _Entries[0][2] = V0.z;
    _Entries[0][3] = 0.0f;

    _Entries[1][0] = V1.x;
    _Entries[1][1] = V1.y;
    _Entries[1][2] = V1.z;
    _Entries[1][3] = 0.0f;

    _Entries[2][0] = V2.x;
    _Entries[2][1] = V2.y;
    _Entries[2][2] = V2.z;
    _Entries[2][3] = 0.0f;

    _Entries[3][0] = 0.0f;
    _Entries[3][1] = 0.0f;
    _Entries[3][2] = 0.0f;
    _Entries[3][3] = 1.0f;
}

#ifdef USE_D3D
Matrix4::Matrix4(const D3DXMATRIX& M)
{
    for (UINT Row = 0; Row < 4; Row++)
    {
        for (UINT Col = 0; Col < 4; Col++)
        {
            _Entries[Row][Col] = M(Row, Col);
        }
    }
}
#endif

Matrix4& Matrix4::operator = (const Matrix4& M)
{
    for (UINT Row = 0; Row < 4; Row++)
    {
        for (UINT Col = 0; Col < 4; Col++)
        {
            _Entries[Row][Col] = M[Row][Col];
        }
    }
    return (*this);
}

Matrix4 Matrix4::Inverse() const
{
    //
    // Inversion by Cramer's rule.  Code taken from an Intel publication
    //
    double Result[4][4];
    double tmp[12]; /* temp array for pairs */
    double src[16]; /* array of transpose source matrix */
    double det; /* determinant */
    /* transpose matrix */
    for (UINT i = 0; i < 4; i++)
    {
        src[i + 0] = (*this)[i][0];
        src[i + 4] = (*this)[i][1];
        src[i + 8] = (*this)[i][2];
        src[i + 12] = (*this)[i][3];
    }
    /* calculate pairs for first 8 elements (cofactors) */
    tmp[0] = src[10] * src[15];
    tmp[1] = src[11] * src[14];
    tmp[2] = src[9] * src[15];
    tmp[3] = src[11] * src[13];
    tmp[4] = src[9] * src[14];
    tmp[5] = src[10] * src[13];
    tmp[6] = src[8] * src[15];
    tmp[7] = src[11] * src[12];
    tmp[8] = src[8] * src[14];
    tmp[9] = src[10] * src[12];
    tmp[10] = src[8] * src[13];
    tmp[11] = src[9] * src[12];
    /* calculate first 8 elements (cofactors) */
    Result[0][0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
    Result[0][0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
    Result[0][1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
    Result[0][1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
    Result[0][2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
    Result[0][2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
    Result[0][3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
    Result[0][3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
    Result[1][0] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
    Result[1][0] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
    Result[1][1] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
    Result[1][1] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
    Result[1][2] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
    Result[1][2] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
    Result[1][3] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
    Result[1][3] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];
    /* calculate pairs for second 8 elements (cofactors) */
    tmp[0] = src[2] * src[7];
    tmp[1] = src[3] * src[6];
    tmp[2] = src[1] * src[7];
    tmp[3] = src[3] * src[5];
    tmp[4] = src[1] * src[6];
    tmp[5] = src[2] * src[5];

    tmp[6] = src[0] * src[7];
    tmp[7] = src[3] * src[4];
    tmp[8] = src[0] * src[6];
    tmp[9] = src[2] * src[4];
    tmp[10] = src[0] * src[5];
    tmp[11] = src[1] * src[4];
    /* calculate second 8 elements (cofactors) */
    Result[2][0] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
    Result[2][0] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
    Result[2][1] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
    Result[2][1] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
    Result[2][2] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
    Result[2][2] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
    Result[2][3] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
    Result[2][3] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
    Result[3][0] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
    Result[3][0] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
    Result[3][1] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
    Result[3][1] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
    Result[3][2] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
    Result[3][2] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
    Result[3][3] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
    Result[3][3] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];
    /* calculate determinant */
    det = src[0] * Result[0][0] + src[1] * Result[0][1] + src[2] * Result[0][2] + src[3] * Result[0][3];
    /* calculate matrix inverse */
    det = 1.0f / det;

    Matrix4 FloatResult;
    for (UINT i = 0; i < 4; i++)
    {
        for (UINT j = 0; j < 4; j++)
        {
            FloatResult[i][j] = float(Result[i][j] * det);
        }
    }
    return FloatResult;

    //
    // Inversion by LU decomposition, alternate implementation
    //
    /*int i, j, k;

    for (i = 1; i < 4; i++)
    {
        _Entries[0][i] /= _Entries[0][0];
    }

    for (i = 1; i < 4; i++)
    {
        for (j = i; j < 4; j++)
        {
            float sum = 0.0;
            for (k = 0; k < i; k++)
            {
                sum += _Entries[j][k] * _Entries[k][i];
            }
            _Entries[j][i] -= sum;
        }
        if (i == 4-1) continue;
        for (j=i+1; j < 4; j++)
        {
            float sum = 0.0;
            for (int k = 0; k < i; k++)
                sum += _Entries[i][k]*_Entries[k][j];
            _Entries[i][j] =
               (_Entries[i][j]-sum) / _Entries[i][i];
        }
    }

    //
    // Invert L
    //
    for ( i = 0; i < 4; i++ )
    {
        for ( int j = i; j < 4; j++ )
        {
            float x = 1.0;
            if ( i != j )
            {
                x = 0.0;
                for ( int k = i; k < j; k++ )
                    x -= _Entries[j][k]*_Entries[k][i];
            }
            _Entries[j][i] = x / _Entries[j][j];
        }
    }

    //
    // Invert U
    //
    for ( i = 0; i < 4; i++ )
    {
        for ( j = i; j < 4; j++ )
        {
            if ( i == j ) continue;
            float sum = 0.0;
            for ( int k = i; k < j; k++ )
                sum += _Entries[k][j]*( (i==k) ? 1.0f : _Entries[i][k] );
            _Entries[i][j] = -sum;
        }
    }

    //
    // Final Inversion
    //
    for ( i = 0; i < 4; i++ )
    {
        for ( int j = 0; j < 4; j++ )
        {
            float sum = 0.0;
            for ( int k = ((i>j)?i:j); k < 4; k++ )
                sum += ((j==k)?1.0f:_Entries[j][k])*_Entries[k][i];
            _Entries[j][i] = sum;
        }
    }*/
}

Matrix4 Matrix4::Transpose() const
{
    Matrix4 Result;
    for (UINT i = 0; i < 4; i++)
    {
        for (UINT i2 = 0; i2 < 4; i2++)
        {
            Result[i2][i] = _Entries[i][i2];
        }
    }
    return Result;
}

Matrix4 Matrix4::Identity()
{
    Matrix4 Result;
    for (UINT i = 0; i < 4; i++)
    {
        for (UINT i2 = 0; i2 < 4; i2++)
        {
            if (i == i2)
            {
                Result[i][i2] = 1.0f;
            }
            else
            {
                Result[i][i2] = 0.0f;
            }
        }
    }
    return Result;
}

Matrix4 Matrix4::Rotation(const Vector3& _Basis1, const Vector3& _Basis2, const Vector3& _Basis3)
{
    //
    // Verify everything is normalized
    //
    Vector3 Basis1 = _Basis1;
    Vector3 Basis2 = _Basis2;
    Vector3 Basis3 = _Basis3;
    Basis1.Normalize();
    Basis2.Normalize();
    Basis3.Normalize();

    Matrix4 Result;
    Result[0][0] = Basis1.x;
    Result[1][0] = Basis1.y;
    Result[2][0] = Basis1.z;
    Result[3][0] = 0.0f;

    Result[0][1] = Basis2.x;
    Result[1][1] = Basis2.y;
    Result[2][1] = Basis2.z;
    Result[3][1] = 0.0f;

    Result[0][2] = Basis3.x;
    Result[1][2] = Basis3.y;
    Result[2][2] = Basis3.z;
    Result[3][2] = 0.0f;

    Result[0][3] = 0.0f;
    Result[1][3] = 0.0f;
    Result[2][3] = 0.0f;
    Result[3][3] = 1.0f;
    return Result;
}

Matrix4 Matrix4::Camera(const Vector3& Eye, const Vector3& _Look, const Vector3& _Up, const Vector3& _Right)
{
    //
    // Verify everything is normalized
    //
    Vector3 Look = Vector3::Normalize(_Look);
    Vector3 Up = Vector3::Normalize(_Up);
    Vector3 Right = Vector3::Normalize(_Right);

    Matrix4 Result;
    Result[0][0] = Right.x;
    Result[1][0] = Right.y;
    Result[2][0] = Right.z;
    Result[3][0] = -Vector3::Dot(Right, Eye);

    Result[0][1] = Up.x;
    Result[1][1] = Up.y;
    Result[2][1] = Up.z;
    Result[3][1] = -Vector3::Dot(Up, Eye);

    Result[0][2] = Look.x;
    Result[1][2] = Look.y;
    Result[2][2] = Look.z;
    Result[3][2] = -Vector3::Dot(Look, Eye);

    Result[0][3] = 0.0f;
    Result[1][3] = 0.0f;
    Result[2][3] = 0.0f;
    Result[3][3] = 1.0f;
    return Result;
}

Matrix4 Matrix4::LookAt(const Vector3& Eye, const Vector3& At, const Vector3& Up)
{
    Vector3 XAxis, YAxis, ZAxis;
    ZAxis = Vector3::Normalize(Eye - At);
    XAxis = Vector3::Normalize(Vector3::Cross(Up, ZAxis));
    YAxis = Vector3::Normalize(Vector3::Cross(ZAxis, XAxis));

    Matrix4 Result;
    Result[0][0] = XAxis.x;
    Result[1][0] = XAxis.y;
    Result[2][0] = XAxis.z;
    Result[3][0] = -Vector3::Dot(XAxis, Eye);

    Result[0][1] = YAxis.x;
    Result[1][1] = YAxis.y;
    Result[2][1] = YAxis.z;
    Result[3][1] = -Vector3::Dot(YAxis, Eye);

    Result[0][2] = ZAxis.x;
    Result[1][2] = ZAxis.y;
    Result[2][2] = ZAxis.z;
    Result[3][2] = -Vector3::Dot(ZAxis, Eye);

    Result[0][3] = 0.0f;
    Result[1][3] = 0.0f;
    Result[2][3] = 0.0f;
    Result[3][3] = 1.0f;
    return Result;
}

Matrix4 Matrix4::OrthogonalOffCenter(float left, float right, float bottom, float top, float ZNear, float ZFar)
{
	float invRL = 1.0f / (right - left);
	float invTB = 1.0f / (top - bottom);
	float invFN = 1.0f / (ZFar - ZNear);
	Matrix4 Result;
	Result[0][0] = 2.0f * invRL;
	Result[1][0] = 0.0f;
	Result[2][0] = 0.0f;
	Result[3][0] = -(right + left) * invRL;;

	Result[0][1] = 0.0f;
    Result[1][1] = 2.0f * invTB;
	Result[2][1] = 0.0f;
	Result[3][1] = -(top + bottom) * invTB;;

	Result[0][2] = 0.0f;
	Result[1][2] = 0.0f;
    Result[2][2] = -2.0f * invFN;
	Result[3][2] = -(ZFar + ZNear) * invFN;

	Result[0][3] = 0.0f;
	Result[1][3] = 0.0f;
	Result[2][3] = 0.0f;
	Result[3][3] = 1.0f;
	return Result;
}

Matrix4 Matrix4::Orthogonal(float Width, float Height, float ZNear, float ZFar)
{
    Matrix4 Result;
    Result[0][0] = 2.0f / Width;
    Result[1][0] = 0.0f;
    Result[2][0] = 0.0f;
    Result[3][0] = 0.0f;

    Result[0][1] = 0.0f;
    Result[1][1] = 2.0f / Height;
    Result[2][1] = 0.0f;
    Result[3][1] = 0.0f;

    Result[0][2] = 0.0f;
    Result[1][2] = 0.0f;
    Result[2][2] = 1.0f / (ZNear - ZFar);
    Result[3][2] = ZNear / (ZNear - ZFar);

    Result[0][3] = 0.0f;
    Result[1][3] = 0.0f;
    Result[2][3] = 0.0f;
    Result[3][3] = 1.0f;
    return Result;
}

Matrix4 Matrix4::Perspective(float Width, float Height, float ZNear, float ZFar)
{
    Matrix4 Result;
    Result[0][0] = 2.0f * ZNear / Width;
    Result[1][0] = 0.0f;
    Result[2][0] = 0.0f;
    Result[3][0] = 0.0f;

    Result[0][1] = 0.0f;
    Result[1][1] = 2.0f * ZNear / Height;
    Result[2][1] = 0.0f;
    Result[3][1] = 0.0f;

    Result[0][2] = 0.0f;
    Result[1][2] = 0.0f;
    Result[2][2] = ZFar / (ZNear - ZFar);
    Result[3][2] = ZFar * ZNear / (ZNear - ZFar);

    Result[0][3] = 0.0f;
    Result[1][3] = 0.0f;
    Result[2][3] = -1.0f;
    Result[3][3] = 0.0f;
    return Result;
}

Matrix4 Matrix4::PerspectiveFov(float FOV, float Aspect, float ZNear, float ZFar)
{
    float Width = 1.0f / tanf(FOV / 2.0f), Height = Aspect / tanf(FOV / 2.0f);

    Matrix4 Result;
    Result[0][0] = Width;
    Result[1][0] = 0.0f;
    Result[2][0] = 0.0f;
    Result[3][0] = 0.0f;

    Result[0][1] = 0.0f;
    Result[1][1] = Height;
    Result[2][1] = 0.0f;
    Result[3][1] = 0.0f;

    Result[0][2] = 0.0f;
    Result[1][2] = 0.0f;
    Result[2][2] = ZFar / (ZNear - ZFar);
    Result[3][2] = ZFar * ZNear / (ZNear - ZFar);

    Result[0][3] = 0.0f;
    Result[1][3] = 0.0f;
    Result[2][3] = -1.0f;
    Result[3][3] = 0.0f;
    return Result;
}

Matrix4 Matrix4::PerspectiveMultiFov(float FovX, float FovY, float ZNear, float ZFar)
{
    float Width = 1.0f / tanf(FovX / 2.0f), Height = 1.0f / tanf(FovY / 2.0f);

    Matrix4 Result;
    Result[0][0] = Width;
    Result[1][0] = 0.0f;
    Result[2][0] = 0.0f;
    Result[3][0] = 0.0f;

    Result[0][1] = 0.0f;
    Result[1][1] = Height;
    Result[2][1] = 0.0f;
    Result[3][1] = 0.0f;

    Result[0][2] = 0.0f;
    Result[1][2] = 0.0f;
    Result[2][2] = ZFar / (ZNear - ZFar);
    Result[3][2] = ZFar * ZNear / (ZNear - ZFar);

    Result[0][3] = 0.0f;
    Result[1][3] = 0.0f;
    Result[2][3] = -1.0f;
    Result[3][3] = 0.0f;
    return Result;
}

Matrix4 Matrix4::Rotation(const Vector3& Axis, float Angle)
{
    float c = cosf(Angle);
    float s = sinf(Angle);
    float t = 1.0f - c;

    Vector3 NormalizedAxis = Vector3::Normalize(Axis);
    float x = NormalizedAxis.x;
    float y = NormalizedAxis.y;
    float z = NormalizedAxis.z;

    Matrix4 Result;
    Result[0][0] = 1 + t * (x * x - 1);
    Result[0][1] = z * s + t * x * y;
    Result[0][2] = -y * s + t * x * z;
    Result[0][3] = 0.0f;

    Result[1][0] = -z * s + t * x * y;
    Result[1][1] = 1 + t * (y * y - 1);
    Result[1][2] = x * s + t * y * z;
    Result[1][3] = 0.0f;

    Result[2][0] = y * s + t * x * z;
    Result[2][1] = -x * s + t * y * z;
    Result[2][2] = 1 + t * (z * z - 1);
    Result[2][3] = 0.0f;

    Result[3][0] = 0.0f;
    Result[3][1] = 0.0f;
    Result[3][2] = 0.0f;
    Result[3][3] = 1.0f;
    return Result;
}

Matrix4 Matrix4::Rotation(float Yaw, float Pitch, float Roll)
{
    return RotationY(Yaw) * RotationX(Pitch) * RotationZ(Roll);
}

Matrix4 Matrix4::Rotation(const Vector3& Axis, float Angle, const Vector3& Center)
{
    return Translation(-Center) * Rotation(Axis, Angle) * Translation(Center);
}

Matrix4 Matrix4::RotationX(float Theta)
{
    float CosT = cosf(Theta);
    float SinT = sinf(Theta);

    Matrix4 Result = Identity();
    Result[1][1] = CosT;
    Result[1][2] = SinT;
    Result[2][1] = -SinT;
    Result[2][2] = CosT;
    return Result;
}

Matrix4 Matrix4::RotationY(float Theta)
{
    float CosT = cosf(Theta);
    float SinT = sinf(Theta);

    Matrix4 Result = Identity();
    Result[0][0] = CosT;
    Result[0][2] = SinT;
    Result[2][0] = -SinT;
    Result[2][2] = CosT;
    return Result;
}

Matrix4 Matrix4::RotationZ(float Theta)
{
    float CosT = cosf(Theta);
    float SinT = sinf(Theta);

    Matrix4 Result = Identity();
    Result[0][0] = CosT;
    Result[0][1] = SinT;
    Result[1][0] = -SinT;
    Result[1][1] = CosT;
    return Result;
}

Matrix4 Matrix4::Scaling(const Vector3& ScaleFactors)
{
    Matrix4 Result;
    Result[0][0] = ScaleFactors.x;
    Result[1][0] = 0.0f;
    Result[2][0] = 0.0f;
    Result[3][0] = 0.0f;

    Result[0][1] = 0.0f;
    Result[1][1] = ScaleFactors.y;
    Result[2][1] = 0.0f;
    Result[3][1] = 0.0f;

    Result[0][2] = 0.0f;
    Result[1][2] = 0.0f;
    Result[2][2] = ScaleFactors.z;
    Result[3][2] = 0.0f;

    Result[0][3] = 0.0f;
    Result[1][3] = 0.0f;
    Result[2][3] = 0.0f;
    Result[3][3] = 1.0f;
    return Result;
}

Matrix4 Matrix4::Translation(const Vector3& Pos)
{
    Matrix4 Result;
    Result[0][0] = 1.0f;
    Result[1][0] = 0.0f;
    Result[2][0] = 0.0f;
    Result[3][0] = Pos.x;

    Result[0][1] = 0.0f;
    Result[1][1] = 1.0f;
    Result[2][1] = 0.0f;
    Result[3][1] = Pos.y;

    Result[0][2] = 0.0f;
    Result[1][2] = 0.0f;
    Result[2][2] = 1.0f;
    Result[3][2] = Pos.z;

    Result[0][3] = 0.0f;
    Result[1][3] = 0.0f;
    Result[2][3] = 0.0f;
    Result[3][3] = 1.0f;
    return Result;
}

Matrix4 Matrix4::ChangeOfBasis(const Vector3& Source0, const Vector3& Source1, const Vector3& Source2, const Vector3& SourceOrigin,
    const Vector3& Target0, const Vector3& Target1, const Vector3& Target2, const Vector3& TargetOrigin)
{
    Matrix4 RotationComponent = Matrix4(Source0, Source1, Source2).Inverse() * Matrix4(Target0, Target1, Target2);
    //Matrix4 TranslationComponent = Translation(TargetOrigin - SourceOrigin);
    Matrix4 Result = Translation(-SourceOrigin) * RotationComponent * Translation(TargetOrigin);
    return Result;
    //return Translation(TargetOrigin - SourceOrigin);
}

Matrix4 Matrix4::Face(const Vector3& V0, const Vector3& V1)
{
    //
    // Rotate about the cross product of the two vectors by the angle between the two vectors
    //
    Vector3 Axis = Vector3::Cross(V0, V1);
    float Angle = Vector3::AngleBetween(V0, V1);

    if (Angle == 0.0f)
    {
        return Identity();
    }
    else if (Axis.Length() == 0.0f)
    {
        Vector3 basis0, basis1;
        Vector3::CompleteOrthonormalBasis(V0, basis0, basis1);
        return Rotation(basis0, Angle);
    }
    else
    {
        return Rotation(Axis, Angle);
    }
}

Matrix4 Matrix4::Viewport(float Width, float Height)
{
    return Matrix4::Scaling(Vector3(Width * 0.5f, -Height * 0.5f, 1.0f)) * Matrix4::Translation(Vector3(Width * 0.5f, Height * 0.5f, 0.0f));
}

float Matrix4::CompareMatrices(const Matrix4& Left, const Matrix4& Right)
{
    float Sum = 0.0f;
    for (UINT i = 0; i < 4; i++)
    {
        for (UINT i2 = 0; i2 < 4; i2++)
        {
            Sum += std::fabsf(Left[i][i2] - Right[i][i2]);
        }
    }
    return Sum / 16.0f;
}

Matrix4 operator * (const Matrix4& Left, const Matrix4& Right)
{
    Matrix4 Result;
    for (UINT i = 0; i < 4; i++)
    {
        for (UINT i2 = 0; i2 < 4; i2++)
        {
            float Total = 0.0f;
            for (UINT i3 = 0; i3 < 4; i3++)
            {
                Total += Left[i][i3] * Right[i3][i2];
            }
            Result[i][i2] = Total;
        }
    }
    return Result;
}

Matrix4 operator * (const Matrix4& Left, float& Right)
{
    Matrix4 Result;
    for (UINT i = 0; i < 4; i++)
    {
        for (UINT i2 = 0; i2 < 4; i2++)
        {
            Result[i][i2] = Left[i][i2] * Right;
        }
    }
    return Result;
}

Matrix4 operator * (float& Left, const Matrix4& Right)
{
    Matrix4 Result;
    for (UINT i = 0; i < 4; i++)
    {
        for (UINT i2 = 0; i2 < 4; i2++)
        {
            Result[i][i2] = Right[i][i2] * Left;
        }
    }
    return Result;
}

Matrix4 operator + (const Matrix4& Left, const Matrix4& Right)
{
    Matrix4 Result;
    for (UINT i = 0; i < 4; i++)
    {
        for (UINT i2 = 0; i2 < 4; i2++)
        {
            Result[i][i2] = Left[i][i2] + Right[i][i2];
        }
    }
    return Result;
}

Matrix4 operator - (const Matrix4& Left, const Matrix4& Right)
{
    Matrix4 Result;
    for (UINT i = 0; i < 4; i++)
    {
        for (UINT i2 = 0; i2 < 4; i2++)
        {
            Result[i][i2] = Left[i][i2] - Right[i][i2];
        }
    }
    return Result;
}