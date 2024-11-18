#include "Render.hpp"
#include "RenderScene.h"

Matrix4 ShadowUtil::mTetrahedralRotationNonPacked[4];
Matrix4 ShadowUtil::mTetrahedralRotationPacked[4];
Matrix4 ShadowUtil::mTetrahedralViewProjection[4];

// RENDER SCENE VIEW


void RenderSceneView::_Execute(RenderFrameContext& context, RenderFrameStats& frameStats, T3RenderTargetList& targetList,
	BitSet<T3RenderPass, 22, 0>& baseValidPasses, RenderSubViewParams* pSubViewParams){
	bool bNeedsPush = pSubViewParams && pSubViewParams->mbPushRenderTarget;
	for(int i = 0; i < eRenderPass_Count; i++){
		if (mScenePassList[i])
			baseValidPasses.Set((T3RenderPass)i, 1);
	}
	for(auto it = mPassList.head(); it; it = it->next){
		if (it->mPassType == eRenderViewPass_SubView){
			RenderViewPass_SubView* pSubViewPass = static_cast<RenderViewPass_SubView*>(it);
			pSubViewPass->mpSubView->_Execute(context, frameStats, targetList, baseValidPasses, &pSubViewPass->mParams);
		}
		else {
			T3RenderTargetEnableFlags enableFlags{};
			RenderViewPass* pCurrentPass = (RenderViewPass*)it;
			context.mParameterBoundState.mMaterialSamplerMipBias = 0;

			bool bDifferent = context.mRenderTargetStack[context.mRenderTargetStackPos].mbCurrentDefaultRenderTarget != pCurrentPass->mParams.mbDefaultRenderTarget ||
				!(context.mRenderTargetStack[context.mRenderTargetStackPos].mCurrentRenderTarget == pCurrentPass->mParams.mRenderTarget);

			bool bResetColorTarget = false;

			if (pCurrentPass->mParams.mbResolveDepthTarget && !context.mRenderTargetStack[context.mRenderTargetStackPos].mbDepthResolved) {
				//ResolveDepthTarget doesn't need anything
				context.mRenderTargetStack[context.mRenderTargetStackPos].mbDepthResolved = true;
			}

			if (bDifferent && !bNeedsPush) {
				for (int i = 0; i < 8; i++) {
					if (!(context.mRenderTargetStack[context.mRenderTargetStackPos].mCurrentRenderTarget.mRenderTargetID[i] ==
						pCurrentPass->mParams.mRenderTarget.mRenderTargetID[i])) {
						bNeedsPush = 1;
						if (pCurrentPass->mParams.mRenderTarget.mRenderTargetID[i].mTargetID.mValue != eRenderTargetID_Unknown)
							enableFlags.mFlags |= (1u << i);
					}
				}
			}

			if (!context.mRenderTargetStack[context.mRenderTargetStackPos].mbCurrentDefaultRenderTarget
				&& pSubViewParams && pSubViewParams->mbPushRenderTarget && pSubViewParams->mbResetColorTarget)
				bResetColorTarget = true;

			T3RenderClear clear{};
			clear.mbColor = pCurrentPass->mParams.mForceRenderClear.mbColor || (pCurrentPass->mParams.mbRequiredClearTarget && bNeedsPush);
			clear.mbStencil = pCurrentPass->mParams.mForceRenderClear.mbStencil || (pCurrentPass->mParams.mbRequiredClearTarget && bDifferent);
			clear.mbDepth = pCurrentPass->mParams.mForceRenderClear.mbDepth || (pCurrentPass->mParams.mbRequiredClearTarget && bDifferent);
			if (pCurrentPass->mParams.mForceRenderClear.mbColor && pCurrentPass->mParams.mbOverrideClearValues)
				clear.mClearColor = pCurrentPass->mParams.mForceRenderClear.mClearColor;
			if (pCurrentPass->mParams.mForceRenderClear.mbStencil && pCurrentPass->mParams.mbOverrideClearValues)
				clear.mClearStencil = pCurrentPass->mParams.mForceRenderClear.mClearStencil;
			if (pCurrentPass->mParams.mForceRenderClear.mbDepth && pCurrentPass->mParams.mbOverrideClearValues)
				clear.mClearDepth = pCurrentPass->mParams.mForceRenderClear.mClearDepth;
			if (mbInvertDepth)
				clear.mClearDepth = 1.0f - clear.mClearDepth;

			T3EffectDrawParams drawParams{};

			T3RenderTargetClear nClear{};
			T3RenderTargetSet tmpSet{};
			if (!pCurrentPass->mParams.mbDefaultRenderTarget) {
				T3RenderTargetUtil::GetOutputTargetSet(targetList, tmpSet, nClear, pCurrentPass->mParams.mRenderTarget, context.mFrameIndex);
				for (int i = 0; i < 8; i++) {
					if (tmpSet.mRenderTarget[i].mpTexture)
						drawParams.mRenderTargetFormat[i] = tmpSet.mRenderTarget[i].mpTexture->mSurfaceFormat;
				}
				if (tmpSet.mDepthTarget.mpTexture)
					drawParams.mDepthTargetFormat = tmpSet.mDepthTarget.mpTexture->mSurfaceFormat;
			}

			GFXPlatformClipPlanes planes{};
			planes.mCount = mParams.mClipPlanes.mCount;
			Matrix4 mult = mParams.mpCamera->GetViewMatrix() * mParams.mpCamera->GetProjectionMatrix();
			for (int i = 0; i < planes.mCount; i++) {
				planes.mPlanes[i] = mParams.mClipPlanes.mPlanes[i].mPlane * mult;
				planes.mPlanes[i] /= planes.mPlanes[i].Magnitude();
			}

			drawParams.mpRenderTargetList = &targetList;
			drawParams.mpParameterBoundState = &context.mParameterBoundState;
			drawParams.mpGFXClipPlanes = &planes;
			drawParams.mpFrameStats = &frameStats;
			context.mParameterBoundState.mpCurrentComputeShader.reset();
			context.mParameterBoundState.mpCurrentProgram.reset();
			if (mParams.mViewportScissorRect.mBottom != 0.0f
				|| mParams.mViewportScissorRect.mTop != 0.0f
				|| mParams.mViewportScissorRect.mRight != 0.0f
				|| mParams.mViewportScissorRect.mLeft != 0.0f) {
				u32 w = 0, h = 0;
				TelltaleToolLib_GetRenderAdaptersStruct()->GetResolution(w, h);
				drawParams.mScissorRect.X = (u16)std::floorf(mParams.mViewportScissorRect.mLeft * (float)w);
				drawParams.mScissorRect.Y = (u16)std::floorf(mParams.mViewportScissorRect.mTop * (float)h);
				drawParams.mScissorRect.W = (u16)std::floorf(((1.0f - mParams.mViewportScissorRect.mRight) - mParams.mViewportScissorRect.mLeft) * (float)w);
				drawParams.mScissorRect.H = (u16)std::floorf(((1.0f - mParams.mViewportScissorRect.mBottom) - mParams.mViewportScissorRect.mTop) * (float)h);
			}

			//if render targets and depth targets are all the same
			if ((context.mRenderTargetStack[context.mRenderTargetStackPos].mCurrentRenderTarget == pCurrentPass->mParams.mRenderTarget)) {
				if (tmpSet.HasRenderTarget())
					TelltaleToolLib_GetRenderAdaptersStruct()->SetCurrentViewport(pCurrentPass->mViewport);
				if (clear.mbColor || clear.mbDepth || clear.mbStencil)
					TelltaleToolLib_GetRenderAdaptersStruct()->Clear(clear);
			}else if (bNeedsPush) {
				bNeedsPush = false;
				if (context.mRenderTargetStack[context.mRenderTargetStackPos].mbCurrentDefaultRenderTarget) {
					TelltaleToolLib_GetRenderAdaptersStruct()->SwitchRenderTarget(tmpSet, clear, true, true, true);
				}
				else {
					if (bResetColorTarget) {
						T3RenderTargetID none{ eRenderTargetID_Unknown };
						for (int i = 0; i < 8; i++)
							context.mRenderTargetStack[context.mRenderTargetStackPos].mCurrentRenderTarget.SetRenderTarget(none, i, 0, 0);
						T3RenderTargetSet tmpSet0{};
						T3RenderClear clear0{};
						memset(&clear0, 0, sizeof(T3RenderClear));
						clear0.mClearDepth = 1.0f;
						TelltaleToolLib_GetRenderAdaptersStruct()->SwitchRenderTarget(tmpSet0, clear0, true, false, true);
					}
					context.mRenderTargetStackPos++;
					TelltaleToolLib_GetRenderAdaptersStruct()->PushRenderTarget(tmpSet, clear);
				}
				if (tmpSet.HasRenderTarget())
					TelltaleToolLib_GetRenderAdaptersStruct()->SetCurrentViewport(pCurrentPass->mViewport);
			}
			else if (pCurrentPass->mParams.mbDefaultRenderTarget == false) {
				if (context.mRenderTargetStack[context.mRenderTargetStackPos].mCurrentRenderTarget.mDepthTargetID == pCurrentPass->mParams.mRenderTarget.mDepthTargetID && enableFlags.mFlags.mFlags == 0) {
					//same depth targets and no diff render targets: set vp and clear
					if (tmpSet.HasRenderTarget())
						TelltaleToolLib_GetRenderAdaptersStruct()->SetCurrentViewport(pCurrentPass->mViewport);
					if (clear.mbColor || clear.mbDepth || clear.mbStencil)
						TelltaleToolLib_GetRenderAdaptersStruct()->Clear(clear);
				}
				else if ((context.mRenderTargetStack[context.mRenderTargetStackPos].mCurrentRenderTarget.mDepthTargetID == pCurrentPass->mParams.mRenderTarget.mDepthTargetID)
					&& !pCurrentPass->mParams.mForceRenderClear.mbDepth && !pCurrentPass->mParams.mForceRenderClear.mbStencil) {
					//sane depth target and not force clearing depth/stencil: switch color, then set vp
					T3RenderTargetSet cpy = tmpSet;
						cpy.mDepthTarget.mpTexture.reset();
					cpy.mDepthTarget.mSliceIndex = cpy.mDepthTarget.mMipLevel = 0;
					TelltaleToolLib_GetRenderAdaptersStruct()->SwitchRenderTarget(cpy, clear, true, false, true);
					if (tmpSet.HasRenderTarget())
						TelltaleToolLib_GetRenderAdaptersStruct()->SetCurrentViewport(pCurrentPass->mViewport);
				}
				else {
					//switch all render targets to tmpSet and then clear. then set vp.
					TelltaleToolLib_GetRenderAdaptersStruct()->SwitchRenderTarget(tmpSet, clear, true, true, true);
					if (tmpSet.HasRenderTarget())
						TelltaleToolLib_GetRenderAdaptersStruct()->SetCurrentViewport(pCurrentPass->mViewport);
				}
			}else{
				//default render target
				clear.mClearColor = sGlobalRenderConfig.mClearColor.GammaToLinear();
				TelltaleToolLib_GetRenderAdaptersStruct()->SwitchDefaultRenderTarget(clear);
				if (tmpSet.HasRenderTarget())
					TelltaleToolLib_GetRenderAdaptersStruct()->SetCurrentViewport(pCurrentPass->mViewport);
				context.mbRenderedToDefaultTarget = true;
			}

			context.mRenderTargetStack[context.mRenderTargetStackPos].mbCurrentDefaultRenderTarget = pCurrentPass->mParams.mbDefaultRenderTarget;
			context.mRenderTargetStack[context.mRenderTargetStackPos].mCurrentRenderTarget = pCurrentPass->mParams.mRenderTarget;
			if(!(context.mRenderTargetStack[context.mRenderTargetStackPos].mCurrentRenderTarget.mDepthTargetID == pCurrentPass->mParams.mRenderTarget.mDepthTargetID))
				context.mRenderTargetStack[context.mRenderTargetStackPos].mbDepthResolved = false;

			if(pCurrentPass->mRenderManager.mDispatchInstList.head() || pCurrentPass->mRenderManager.mRenderInstList.head())
				pCurrentPass->mRenderManager.Execute(*this, drawParams);//draw!

			//if non default, need to remove the reference (added in push pass)
			if (!pCurrentPass->mParams.mbDefaultRenderTarget)
				T3RenderTargetUtil::ReleaseReference(targetList, pCurrentPass->mParams.mRenderTarget);
			T3RenderTargetUtil::ReleaseReferences(pCurrentPass->mRenderTargetReferences, targetList);

		}
	}
	if(pSubViewParams&&pSubViewParams->mbPopRenderTarget){
		--context.mRenderTargetStackPos;
		TelltaleToolLib_GetRenderAdaptersStruct()->PopRenderTarget();
	}
	T3RenderTargetUtil::ReleaseReferences(mRenderTargetReferences, targetList);
}


// TOOL LIBRARY RENDER API

void RenderThreadContext::FrameExecute(){
	if (!mpUserScene)
		return;
	T3RenderTargetContext rtContext{};
	RenderFrameContext frameContext{};
	RenderFrame& renderFrame = *mCurrentFrame.mpRenderFrame;
	frameContext.mFrameIndex = mFrameIndex;
	frameContext.mParameterBoundState.mFrameIndex = mFrameIndex;
	T3RenderTargetUtil::BeginFrame(mFrameIndex);
	T3RenderTargetUtil::ResetList(renderFrame.mRenderTargetList);
	if (renderFrame.mViewList.head()) {
		mCurrentFrame.mpUpdateList->ExecuteFrameCommands();
		for (auto it = renderFrame.mViewList.head(); it; it = it->next) {
			BitSet<T3RenderPass, 22, 0> validPasses{};//none
			it->_Execute(frameContext, mRenderedFrameStats, renderFrame.mRenderTargetList, validPasses, 0);
		}
	}
	T3RenderTargetUtil::ReleaseList(renderFrame.mRenderTargetList);
}

struct Property {

	void* value;
	MetaClassDescription* clazz;

	Property() = default;

	inline operator bool() {
		return value != 0 && clazz != 0;
	}

	template<typename T>
	inline T& Get(){
		return *((T*)value);
	}

};

Property PropertyGet(RenderThreadContext* ctx, PropertySet* pProps, Symbol name, bool bSearchParents, int overflowCheck = 0) {
	static MetaClassDescription* pPropMCD = GetMetaClassDescription<PropertySet>();
	Property prop{ 0,0 };
	if (overflowCheck > 50)
		return prop;
	PropertySet::KeyInfo* pInfo = pProps->GetKeyInfo(name);
	if (pInfo && pInfo->mpValue) {
		prop.value = pInfo->mpValue->mpValue;
		prop.clazz = pInfo->mpValue->mpDataDescription;
	}
	else if (bSearchParents) {
		for (int i = 0; i < pProps->mParentList.mSize; i++) {
			void* pParentInst = ctx->GetResource(pProps->mParentList[i].mhParent.mHandleObjectInfo.mObjectName, pPropMCD);
			if (pParentInst) {
				prop = PropertyGet(ctx,(PropertySet*)pParentInst, name, 1, overflowCheck + 1);
				if (prop)
					break;//found in parent set
			}
		}
	}
	return prop;
}

bool _IsMyParent(RenderThreadContext* ctx, PropertySet* pProps, Symbol parentProp, bool bSearchParents, int _internalCounter = 0) {
	static MetaClassDescription* pPropMCD = GetMetaClassDescription<PropertySet>();
	if (_internalCounter > 50)
		return false;
	for (int i = 0; i < pProps->mParentList.mSize; i++) {
		if (pProps->mParentList[i].mhParent.mHandleObjectInfo.mObjectName == parentProp)
			return true;
	}
	if (bSearchParents) {
		for (int i = 0; i < pProps->mParentList.mSize; i++) {
			void* pParentInst = ctx->GetResource(pProps->mParentList[i].mhParent.mHandleObjectInfo.mObjectName, pPropMCD);
			if (pParentInst) {
				bool prop = _IsMyParent(ctx,(PropertySet*)pParentInst, parentProp, 1, _internalCounter + 1);
				if (prop)
					return true;
			}
		}
	}
	return false;
}

void _AllocateUniqueMeshInstance(RenderObject_Mesh* mesh, RenderThreadContext* ctx, D3DMesh* pStaticMesh){
	//_INITIALIZEMESHINSTANCE
	RenderObject_Mesh::MeshInstance& meshInstance = mesh->mMeshInstanceList.emplace_back();
	meshInstance.mpRenderMesh = mesh;
	meshInstance.mhD3DMesh = pStaticMesh;
	meshInstance.mBoundingBox = pStaticMesh->mMeshData.mBoundingBox;
	meshInstance.mBoundingSphere = pStaticMesh->mMeshData.mBoundingSphere;
	meshInstance.mAnimatedBoundingBox = pStaticMesh->mMeshData.mBoundingBox;
	meshInstance.mAnimatedBoundingSphere = pStaticMesh->mMeshData.mBoundingSphere;
	meshInstance.mhSceneProperties = ctx->mpUserSceneTopmostProperties;
	meshInstance.mLODs.reserve(pStaticMesh->mMeshData.mLODs.mSize);
	for (int lodi = 0; lodi < pStaticMesh->mMeshData.mLODs.mSize; lodi++) {
		T3MeshLOD& srcLOD = pStaticMesh->mMeshData.mLODs[lodi];
		RenderObject_Mesh::MeshLODInstance& lod = meshInstance.mLODs.emplace_back();
		lod.mBoundingBox = srcLOD.mBoundingBox;
		lod.mBoundingSphere = srcLOD.mBoundingSphere;
		lod.mBones = srcLOD.mBones;
		T3EffectParameterGroupHeader* pHeaders = new T3EffectParameterGroupHeader[srcLOD.mNumBatches];
		lod.mEffectParameterTypes.Set(eEffectParameter_UniformBufferInstance, 1);
		u32 totalParamSize = T3EffectParameterGroup::InitializeHeader(pHeaders[0], lod.mEffectParameterTypes);
		u32 maxAlign = 0;
		lod.mTriangleSets[eMeshBatchType_Default].resize(srcLOD.mBatches[eMeshBatchType_Default].mSize);
		int b = 0;
		for (T3MeshBatchType type = eMeshBatchType_Default; type != eMeshBatchType_Count; type=(T3MeshBatchType)(type+1)) {
			for (auto& batch : lod.mTriangleSets[type]) {
				T3MeshBatch& srcBatch = srcLOD.mBatches[type][b++];
				//batch.mEffectParameterTypes.Set(eEffectParameter_UniformBufferLights, 1);
				batch.mEffectParameterTypes.Set(eEffectParameter_UniformBufferMeshBatch, 1);
				if (srcBatch.mTextureIndices.mIndex[eMeshBatchType_Default] >= 0)
					batch.mEffectParameterTypes.Set(eEffectParameter_SamplerLightmapFlat, 1);
				if (srcBatch.mTextureIndices.mIndex[eMeshBatchType_Shadow] >= 0)
					batch.mEffectParameterTypes.Set(eEffectParameter_SamplerStaticShadowmap, 1);
				totalParamSize += T3EffectParameterGroup::InitializeHeader(pHeaders[b + 1], batch.mEffectParameterTypes);
				if (pHeaders[b + 1].mMaxScalarAlign > maxAlign)
					maxAlign = pHeaders[b + 1].mMaxScalarAlign;
			}
			b = 0;
		}
		lod.mEffectParameterGroupBuffer.SetData(totalParamSize, 0, maxAlign << 2);
		lod.mEffectParameters = T3EffectParameterGroup::Create(lod.mEffectParameterGroupBuffer.mpData, pHeaders[0]);
		b = 0;
		u32 off = 0;
		for (T3MeshBatchType type = eMeshBatchType_Default; type != eMeshBatchType_Count; type = (T3MeshBatchType)(type + 1)) {
			for (auto& batch : lod.mTriangleSets[type]) {
				batch.mEffectParameters = T3EffectParameterGroup::Create(lod.mEffectParameterGroupBuffer.mpData + off, pHeaders[b]);
				off += pHeaders[b++].mBufferScalarSize << 2;
			}
			b = 0;
		}
		delete[] pHeaders;
	}
}

void _SetupMesh(PropertySet* pMeshAgentProps, RenderObject_Mesh* mesh, RenderThreadContext* ctx){
	MetaClassDescription* hTex = GetMetaClassDescription<T3Texture>();
	MetaClassDescription* hProp = GetMetaClassDescription<PropertySet>();
	mesh->mMeshInstanceList.reserve(mesh->mpMeshList.size() + 1);
	_AllocateUniqueMeshInstance(mesh, ctx, mesh->mhBaseMesh);
	for (auto& it : mesh->mpMeshList)
		_AllocateUniqueMeshInstance(mesh, ctx, it);
	//init text instances then material instances:
	for(auto& inst: mesh->mMeshInstanceList){
		inst.mTxInstanceIndex.resize(inst.mhD3DMesh->mMeshData.mTextures.mSize);
		for (int tex = 0; tex < inst.mhD3DMesh->mMeshData.mTextures.mSize; tex++){
			T3MeshTexture& texture = inst.mhD3DMesh->mMeshData.mTextures[tex];
			inst.mTxInstanceIndex.push_back((int)mesh->mTextureInstances.size());
			RenderObject_Mesh::TextureInstance& texInstance = mesh->mTextureInstances.emplace_back();
			texInstance.mBoundingBox = texture.mBoundingBox;
			texInstance.mBoundingSphere = texture.mBoundingSphere;
			texInstance.mMaxObjAreaPerUVArea = texture.mMaxObjAreaPerUVArea;
			texInstance.mTextureType = texture.mTextureType;
			texInstance.mpMeshInstance = mesh;
			void* pLoadedTex = ctx->GetResource(texture.mhTexture.mHandleObjectInfo.mObjectName, hTex);//load d3dtx from archives or whatever
			if (pLoadedTex == 0)
				pLoadedTex = T3::spDefaultTexture[eDefaultTextureType_White].mpTexture.get();
			texInstance.mpCurTexture = (T3Texture*)pLoadedTex;
		}
		for(int mat = 0; mat < inst.mhD3DMesh->mMeshData.mMaterials.mSize; mat++){
			T3MeshMaterial& material = inst.mhD3DMesh->mMeshData.mMaterials[mat];
			inst.mTxInstanceIndex.push_back((int)mesh->mMaterialInstances.size());
			T3MaterialInstance& matInstance = mesh->mMaterialInstances.emplace_back();
			matInstance.mpRenderObject = mesh;
			matInstance.mhAgentProperties = pMeshAgentProps;
			matInstance.mBaseMaterialName = material.mBaseMaterialName;
			matInstance.mBoundingBox = material.mBoundingBox;
			matInstance.mBoundingSphere = material.mBoundingSphere;
			matInstance.mhBaseMaterial = matInstance.mhCurrentMaterial = inst.mhD3DMesh->GetResourceMaterial(material.mhMaterial.mHandleObjectInfo.mObjectName);
			if(matInstance.mhBaseMaterial == 0){
				void* pProp = ctx->GetResource(material.mhMaterial.mHandleObjectInfo.mObjectName, hProp);
				if (pProp)
					matInstance.mhBaseMaterial = matInstance.mhCurrentMaterial = (PropertySet*)pProp;
				else{
					TelltaleToolLib_RaiseError("Could not locate material resource in mesh resoures or in RenderThreadContext::GetResource", ERR);
					TTL_Log("Could not find material properties Symbol<%llX>", material.mhMaterial.mHandleObjectInfo.mObjectName.GetCRC());
					continue;
				}
			}
			matInstance.mRenderQuality = sGlobalRenderConfig.mQuality;
			matInstance.mhMaterial = matInstance.mhCurrentMaterial;//normally set in other functino bc material changes on quality, optimization,domain and lua ' - Material Override'
			T3MaterialData* pMaterialData = matInstance.mhBaseMaterial->GetProperty<T3MaterialData>(T3MaterialInternal::kPropKeyMaterialData);
			matInstance.mpMaterialData = pMaterialData;
			matInstance.mpCompiledDataArray = &pMaterialData->mCompiledData2;
			matInstance.mCompiledDataIndex = 0;
			T3MaterialQualityType t = sGlobalRenderConfig.mQuality == eRenderQuality_High ? eMaterialQuality_High : sGlobalRenderConfig.mQuality == eMaterialQuality_Low ? eMaterialQuality_Low : eMaterialQuality_Lowest;
			for(int i = 0; i < matInstance.mpCompiledDataArray->mSize; i++){
				if (matInstance.mpCompiledDataArray->operator[](i).mMaterialQuality == t){
					matInstance.mCompiledDataIndex = i;
					break;
				}
			}
			matInstance.mRenderQuality = sGlobalRenderConfig.mQuality;
			T3MaterialCompiledData* pCompiled = &matInstance.mpCompiledDataArray->operator[](matInstance.mCompiledDataIndex);
			matInstance.mSupportedBlendModes.mWords[0] = pCompiled->mMaterialBlendModes.mWords[0];
			matInstance.mMaterialChannels.mWords[0] = pCompiled->mMaterialChannels.mWords[0];
			matInstance.mMaterialChannels.mWords[1] = pCompiled->mMaterialChannels.mWords[1];
			matInstance.mShaderInputs.mWords[0] = pCompiled->mShaderInputs.mWords[0];
			matInstance.mShaderInputs.mWords[1] = pCompiled->mShaderInputs.mWords[1];
			matInstance.mShaderInputs.mWords[2] = pCompiled->mShaderInputs.mWords[2];
			matInstance.mSceneTextures.mWords[0] = pCompiled->mSceneTextures.mWords[0];
			matInstance.mMaterialPasses.mWords[0] = pCompiled->mMaterialPasses.mWords[0];
			if (pCompiled->mFlags.mFlags & eMaterialCompiledDataFlag_HasBufferMaterialBase)
				matInstance.mEffectParameterTypes.Set(eEffectParameter_UniformBufferMaterialBase, 1);
			if (pCompiled->mFlags.mFlags & eMaterialCompiledDataFlag_HasBufferMaterialMain)
				matInstance.mEffectParameterTypes.Set(eEffectParameter_UniformBufferMaterialMain, 1);
		}
	}
	mesh->mpEffectParameterBuffer = new T3EffectParameterBuffer();
	T3EffectParameterUtil::InitializeForType(mesh->mpEffectParameterBuffer, eEffectParameter_UniformBufferObject, eGFXUniformBufferUsage_Streaming);
	//update mesh inst data
	//init light and light env groups
	//material util update instance for each material using mesh material buffer from *mesh
}

void RenderThreadContext::Clear(){
	for (auto& it : mpSceneMeshes)
		delete it;
	for (auto& it : mpSceneViewports)
		delete it;
	for (auto& it : mpCachedCameras)
		delete it;
	mpSceneMeshes.clear();
	mpSceneViewports.clear();
	mpCachedCameras.clear();
	mpUserSceneTopmostProperties = 0;
	mpUserScene = 0;
}

void RenderThreadContext::SetScene(Scene* pScene){
	if (mpUserScene == pScene || pScene == 0)
		return;
	if(mpUserScene)
		Clear();
	mpUserScene = pScene;
	MetaClassDescription* hMesh = TelltaleToolLib_FindMetaClassDescription("Handle<D3DMesh>", 1);
	MetaClassDescription* hArrMesh = TelltaleToolLib_FindMetaClassDescription("DCArray<Handle<D3DMesh>>", 1);
	MetaClassDescription* hActualMesh = TelltaleToolLib_FindMetaClassDescription("D3DMesh", 1);
	for (auto& pAgent : pScene->mAgentList) {
		//Here we call any OnSetupAgent functions (i inline them here)
		if (_IsMyParent(this, &pAgent->mAgentSceneProps, "module_scene.prop", 1)) {
			mpUserSceneTopmostProperties = &pAgent->mAgentSceneProps;
		}
		if (_IsMyParent(this,&pAgent->mAgentSceneProps, "module_renderable.prop", 1)) {
			//RENDERABLE AGENT
			Property mesh = PropertyGet(this, &pAgent->mAgentSceneProps, "D3D Mesh", 1);
			if (mesh && mesh.clazz == hMesh) {
				D3DMesh* pBaseMesh = (D3DMesh*)GetResource(((Handle<D3DMesh>*)mesh.value)->mHandleObjectInfo.mObjectName, hActualMesh);
				if (pBaseMesh) {
					mesh = PropertyGet(this, &pAgent->mAgentSceneProps, "D3D Mesh List", 1);
					DCArray<Handle<D3DMesh>>* pMeshes = 0;
					RenderObject_Mesh* pRenderMesh = new RenderObject_Mesh();
					pRenderMesh->mhBaseMesh = pBaseMesh;
					if (mesh && mesh.clazz == hArrMesh) {
						pMeshes = (DCArray<Handle<D3DMesh>>*)mesh.value;
						pRenderMesh->mpMeshList.reserve(pMeshes->mSize);
						for (int i = 0; i < pMeshes->mSize; i++) {
							D3DMesh* pNextMesh = (D3DMesh*)GetResource(pMeshes->operator[](i).mHandleObjectInfo.mObjectName, hActualMesh);
							if (!pNextMesh) {
								std::string resolved{};
								TelltaleToolLib_GetGlobalHashDatabase()->FindEntry(0, pMeshes->operator[](i).mHandleObjectInfo.mObjectName.GetCRC(), &resolved);
								if (resolved.length()) {
									TTL_Log("Could not create RenderObject_Mesh for %s - %s was not loaded successfully!\n", pAgent->mAgentName.c_str(), resolved.c_str());
								}
								else {
									TTL_Log("Could not create RenderObject_Mesh for %s - Symbol<%llx> was not loaded successfully!\n", pAgent->mAgentName.c_str(), pMeshes->operator[](i).mHandleObjectInfo.mObjectName.GetCRC());
								}
							}
							else pRenderMesh->mpMeshList.push_back(pNextMesh);
						}
					}
					_SetupMesh(&pAgent->mAgentSceneProps, pRenderMesh, this);
					mpSceneMeshes.push_back(pRenderMesh);
					TTL_Log("Created RenderObject_Mesh for %s! Base Mesh: %s, Num Meshes: %d\n", pAgent->mAgentName.c_str(), pBaseMesh->mName.c_str(), (u32)pRenderMesh->mpMeshList.size());
				}
				else {
					TTL_Log("Could not create RenderObject_Mesh for %s - D3D Mesh could not be loaded or found!\n", pAgent->mAgentName.c_str());
				}
			}
			else {
				TTL_Log("Could not create RenderObject_Mesh for %s - no D3D Mesh property was set in agent props\n", pAgent->mAgentName.c_str());
			}
		}
		if (_IsMyParent(this, &pAgent->mAgentSceneProps, "module_viewport.prop", 1)) {
			Property origin = PropertyGet(this, &pAgent->mAgentSceneProps, "Viewport Origin", 1);//vec2 0,0
			Property size = PropertyGet(this, &pAgent->mAgentSceneProps, "Viewport Size", 1);//0.2,0
			Property camera = PropertyGet(this, &pAgent->mAgentSceneProps, "Viewport Camera", 1);//sym 'cam_default'
			Property relative = PropertyGet(this, &pAgent->mAgentSceneProps, "Viewport Relative", 1);//true
			RenderObject_Viewport* pViewport = new RenderObject_Viewport;
			pViewport->mpScene = mpUserScene;
			pViewport->mCameraName = camera.Get<Symbol>();
			pViewport->mbViewportRelative = relative.Get<bool>();
			pViewport->mViewportSize = relative.Get<Vector2>();
			pViewport->mViewportOrigin = relative.Get<Vector2>();
			mpSceneViewports.push_back(pViewport);
		}
		//module_camera.prop ? take all the stuff from camera member variables and set them from the prop.
		//MORE AGENTS
	}
	if(mpUserSceneTopmostProperties == 0){
		TelltaleToolLib_RaiseError("Could not find topmost scene properties in scene: no agent inherited from module_scene.prop. Cannot set scene.", ERR);
		Clear();
	}
}

void _PrepareSceneContext(RenderThreadContext* ctx, RenderSceneContext& context, RenderFrame& frame,Scene* pScene, PrepareSceneParams& params){
	context.mpScene = pScene;
	context.mpViewCamera = pScene->mpViewCamera.get();
	context.mRenderLayer = pScene->mRenderLayer;
	context.mCameraCutFrameIndex = 0;//not needed
	context.mbAntialiasing = pScene->mbAfterEffectEnabled && pScene->mpViewCamera;
	context.mbBakedLightingEnabled = pScene->mbLightEnvBakeEnabled;
	context.mObjectCapacity = (int)ctx->mpSceneMeshes.size();
	context.mObjectCount = 0;
	RenderObjectContext* pContexts = ctx->mCurrentFrame.mHeap.NewArrayNoDestruct<RenderObjectContext>(context.mObjectCapacity);
	context.mObjectList = pContexts;
	
	int layer = -1000000;
	int maxLayer = 0x7FFF;
	RenderObject_PostMaterial* postMat = 0;
	for(auto& it : ctx->mpSceneMeshes){
		if((params.mRenderCategoryAll & (it->mRenderCategory & it->mEnvTileRenderCategory)) == params.mRenderCategoryAll){
			if((params.mRenderCategoryAny == 0) || (params.mRenderCategoryAny & (it->mRenderCategory & it->mEnvTileRenderCategory))){
				if(((it->mRenderCategory & it->mEnvTileRenderCategory) & eRenderCategory_Post) && postMat == 0 && (it->mRenderLayer > layer)){
					layer = it->mRenderLayer;
					postMat = (RenderObject_PostMaterial*)it;
				}
			}else{
				RenderObjectContext& objectContext = *(pContexts + context.mObjectCount++);
				objectContext.mpRenderObject = it;
				objectContext.mName = Symbol::kEmptySymbol;
				if (it->mRenderFeatures[eRenderObjectFeature_SceneColorTexture] && it->mRenderLayer < maxLayer)
					maxLayer = it->mRenderLayer;
			}
		}
	}
	context.mSceneColorTextureRenderLayer = maxLayer;
	context.mpActivePostMaterial = postMat;
	if(!context.mpRenderScene && (params.mbRenderHiddenScenes || !pScene->mbHidden)){
		RenderSceneParams paramsa{};
		paramsa.mpScene = context.mpScene;
		paramsa.mRenderQuality = params.mRenderQuality;
		bool tmp;
		if (!params.mbAllowAfterEffects || (tmp = pScene->mbAfterEffectEnabled == 0, paramsa.mbAfterEffects = 1, tmp))
			paramsa.mbAfterEffects = 0;
		if (!params.mbAllowReflectionEnvironment || (tmp = pScene->mbLightEnvReflectionEnabled == 0, paramsa.mbReflectionEnvironment = 1, tmp))
			paramsa.mbAfterEffects = 0;
		paramsa.mShadowCascadeCount = params.mShadowCascadeCount;
		paramsa.mShadowMapAtlasResolution = params.mShadowMapAtlasResolution;
		paramsa.mbCameraCut = context.mCameraCutFrameIndex >= ctx->mFrameIndex;
		context.mpRenderScene = ctx->mCurrentFrame.mpRenderFrame->AllocateScene(paramsa);
	}
}

RenderSceneView* _PrepareViewportView(RenderThreadContext* ctx, RenderFrameScene* pRenderScene, RenderObject_Viewport* pViewport){
	Camera* pFoundCamera = 0;
	if (pViewport->mpScene) {
		pFoundCamera = pViewport->mpCamera;
		if (!pFoundCamera) {
			for (auto& cam : ctx->mpCachedCameras) {
				if (cam->mName == pViewport->mCameraName) {
					pFoundCamera = cam;
					break;
				}
			}
		}
		if (!pFoundCamera) {
			pFoundCamera = new Camera;
			pFoundCamera->mName = pViewport->mCameraName;
			//default camera (empty)
			pFoundCamera->SetFarClip(1.0f);
			pFoundCamera->SetNearClip(0.0f);
			pFoundCamera->SetHFOV(60.f);
			pFoundCamera->SetHFOVScale(1.0f);
			ctx->mpCachedCameras.push_back(pFoundCamera);
		}
		pViewport->mpCamera = pFoundCamera;
		RenderViewParams params{};
		params.mpCamera = pFoundCamera;
		params.mViewport.mWidth = pViewport->mViewportSize.x;
		params.mViewport.mHeight = pViewport->mViewportSize.y;
		params.mViewport.mOriginX = pViewport->mViewportOrigin.x;
		params.mViewport.mOriginY = pViewport->mViewportOrigin.y;
		RenderSceneView* pView = pRenderScene->AllocateView(params);
		return pView;
	}
	else return 0;
}

void RenderThreadContext::GameRender(){
	if (!mpUserScene)
		return;
	RenderFrame& renderFrame = *mCurrentFrame.mpRenderFrame;
	//Just start a new frame. First update frame index and clear old stuff.
	mFrameIndex = TelltaleToolLib_GetRenderAdaptersStruct()->GetFrameIndex();
	mRenderedFrameStats = RenderFrameStats{};
	mCurrentFrame.mHeap.FreeAll();
	mCurrentFrame.mpUpdateList = mCurrentFrame.mHeap.NewNoDestruct<RenderFrameUpdateList>();
	mCurrentFrame.mpUpdateList->mFrameIndex = mFrameIndex;
	mCurrentFrame.mpUpdateList->mHeap = &mCurrentFrame.mHeap;
	mCurrentFrame.mpRenderFrame = mCurrentFrame.mHeap.NewNoDestruct<RenderFrame>();
	RenderSceneContext sceneContext{};
	T3RenderTargetContext targetContext{};
	targetContext.mHeap = &mCurrentFrame.mHeap;
	if(!mpUserScene->mbHidden){
		PrepareSceneParams params{};
		params.mShadowCascadeCount = 3;
		params.mRenderCategoryAny = eRenderCategory_ForceEnlightenVisible | eRenderCategory_Visible | eRenderCategory_ForceShadowVisible;
		params.mbRenderHiddenScenes = false;
		params.mbAllowAfterEffects = params.mbAllowDebugRender = params.mbAllowReflectionEnvironment = true;
		_PrepareSceneContext(this, sceneContext, renderFrame, mpUserScene, params);
	}
	for(int i = 0; i < sceneContext.mObjectCount; i++){
		sceneContext.mObjectList[i].mpRenderObject->PrepareToRender(mCurrentFrame.mpUpdateList);
	}

	//allocate a DebugView? gonna skip that
	//RenderViewParams debugParams{};
	//debugParams.mCascadeCount = 1;
	//debugParams.mbEnabled = debugParams.mbZClipEnabled = true;

	for (int i = 0; i < sceneContext.mObjectCount; i++) {
		//This normally just dispatches (in thuis function normally) update skeletal animation jobs (paged list?)
		sceneContext.mObjectList[i].mpRenderObject->UpdateSkeletonAnimation(&mCurrentFrame.mHeap, sceneContext.mObjectList + i, 0);
	}

	//Pretty much everthing depends if we have a view camera. Telltale will test and find the first layered scene with a camera. Just use the current scene one.
	if (sceneContext.mpViewCamera) {
		Camera* agentCamera = sceneContext.mpViewCamera;
		int viewCount = 1 + (int)mpSceneViewports.size();
		RenderSceneView** ppViews = mCurrentFrame.mHeap.NewArrayNoDestruct<RenderSceneView*>(viewCount);
		int _x = 0;
		for (auto& it : mpSceneViewports)
			ppViews[_x++ + 1] = _PrepareViewportView(this, sceneContext.mpRenderScene, it);
		RenderViewParams defView{};
		ppViews[0] = mCurrentFrame.mpRenderFrame->PushView(*sceneContext.mpRenderScene, defView);
		ppViews[0]->mName = "Default View";

		u32 outWidth = 0, outHeight = 0;
		TelltaleToolLib_GetRenderAdaptersStruct()->GetResolution(outWidth, outHeight);

		//prepare passes
		//_PrepareScenePass_Compute(ppViews[0], targetContext, outWidth, outHeight);
		//render gbuffer begin
		//_PrepareScenePass_GBufferLines(mpUserScene, agentCamera, ppViews, viewCount, targetContext);
		//_PrepareScenePass_GBuffer(mpUserScene, agentCamera, ppViews, viewCount, targetContext);
		//G BUFFER RESOLVE POST EFFECT DRAW
		//DEPTH REDUCTION
		//LIGHT ASSIGNMENT VIEWS, OCCLUSION FILTER DRAW POST FX
		//HI STENCIL SHADOWS IF ENABLED (POST FX)
		//_PrepareScenePass_Glow(ppViews, viewCount, targetContext);
		//_PrepareScenePass_ForceLinearDepth(mpUserScene, agentCamera, ppViews, viewCount, targetContext);
		//_PrepareScenePass_DeferredDecal_GBuffer(mpUserScene, ppViews, viewCount, targetContext);
		_PrepareScenePass_Main_Opaque(*sceneContext.mpRenderScene, ppViews, viewCount, targetContext, true, false);
		//if (!mpUserScene->mbSSLinesEnabled)
		//	_PrepareScenePass_Main_AfterSSLines(mpUserScene, ppViews, viewCount, targetContext);
		//shadow passes, light assignment views
		//_PrepareScenePass_DeferredDecal_Emmisive(mpUserScene, ppViews, viewCount, targetContext);
		//post effect apply modulated shadows
		//if (mpUserScene->mbSSLinesEnabled)//also post fx draw ss lines
		//	_PrepareScenePass_Main_AfterSSLines(mpUserScene, ppViews, viewCount, targetContext);
		//if (mpUserScene->mbGenerateNPRLines)
		//	_PrepareScenePass_Lines(mpUserScene, ppViews, viewCount, targetContext);
		//render shadow add shadow passes
		//_PrepareScenePass_Main_Alpha_Antialiased(mpUserScene, ppViews, viewCount, targetContext);
		//MSAA scene resolve
		//_PrepareScenePass_Main_Alpha_PreResolve(mpUserScene, ppViews, viewCount, targetContext);
		//post fx drawing more
		//_PrepareScenePass_Main_Alpha(mpUserScene, ppViews, viewCount, targetContext);
		//post fx draw glow
		// lots of brush post fx if scene brush outline enabled
		//motion blur, other post fx
		//post fx color main
		//_PrepareScenePass_Main_Extended(mpUserScene, ppViews, viewCount, targetContext, eRenderPass_Main_AfterPostEffects, 1);
		//_PrepareScenePass_Main_Extended(mpUserScene, ppViews, viewCount, targetContext, eRenderPass_Main_AfterAntialiasing, 1);


		//allocate light scene context (lines 980 ish)
		//create occlusion views 

	}//LABEL_343 END

}

// RENDER FRAME

inline RenderFrameScene* RenderFrame::AllocateScene(const RenderSceneParams& params) {
	RenderFrameScene* scene = mHeap->NewNoDestruct<RenderFrameScene>();
	scene->mParameterStack = mParameterStack;
	scene->mFrameIndex = mFrameIndex;
	RenderDepthFetchMode mode = eRenderDepthFetch_DepthBuffer;
	PlatformType platform = TelltaleToolLib_GetRenderAdaptersStruct()->GetPlatform();
	SetEffectFeatureIfValid(scene->mEffectFeatures, WDC_eEffectFeature_LinearDepthFetch, T3::GetEffectQuality(sGlobalRenderConfig.mQuality), platform);
	SetEffectFeatureIfValid(scene->mEffectFeatures, WDC_eEffectFeature_DepthBufferFetch, T3::GetEffectQuality(sGlobalRenderConfig.mQuality), platform);
	if (sGlobalRenderConfig.mbGlowEnabled)
		SetEffectFeatureIfValid(scene->mEffectFeatures, WDC_eEffectFeature_GlowOutput, T3::GetEffectQuality(sGlobalRenderConfig.mQuality), platform);
	T3EffectParameterUniformBufferPointers buffers{};
	T3EffectParameterTextures texs{};
	SetSceneTexture(eEffectParameter_SamplerBrushNear, params.mpScene->mhBrushNearTexture.get(), texs, *mResourceUpdateList);
	SetSceneTexture(eEffectParameter_SamplerBrushFar, params.mpScene->mhBrushFarTexture.get(), texs, *mResourceUpdateList);
	SetSceneTexture(eEffectParameter_SamplerLightEnvShadowGobo, scene->mpShadowGoboTexture = params.mpScene->mhEnvLightShadowGoboTexture.get(), texs, *mResourceUpdateList);
	if (params.mbReflectionEnvironment && SetSceneTexture(eEffectParameter_SamplerEnvironment, params.mpScene->mhLightEnvReflectionTexture.get(), texs, *mResourceUpdateList))
		SetEffectFeatureIfValid(scene->mEffectFeatures, WDC_eEffectFeature_SpecularEnvironment, T3::GetEffectQuality(sGlobalRenderConfig.mQuality), platform);
	if (params.mbAfterEffects) {
		SetRenderTarget(eEffectParameter_SamplerDeferredShadows, texs, eRenderTargetID_DeferredShadows);
		SetRenderTarget(eEffectParameter_SamplerDeferredLight0, texs, eRenderTargetID_DeferredLight0);
		SetRenderTarget(eEffectParameter_SamplerDeferredLight1, texs, eRenderTargetID_DeferredLight1);
		SetRenderTarget(eEffectParameter_SamplerLinearDepth, texs, eRenderTargetID_LinearDepth);
		SetRenderTarget(eEffectParameter_SamplerGBuffer0, texs, eRenderTargetID_GBuffer0);
		SetRenderTarget(eEffectParameter_SamplerGBuffer1, texs, eRenderTargetID_GBuffer1);
		SetRenderTarget(eEffectParameter_SamplerDBuffer0, texs, eRenderTargetID_DeferredDecalBuffer0);
		SetRenderTarget(eEffectParameter_SamplerDBuffer1, texs, eRenderTargetID_DeferredDecalBuffer1);
	}
	BitSet<T3EffectParameterType, 150, 0> paramTypes{};
	paramTypes.Set(eEffectParameter_UniformBufferScene, 1);
	T3EffectParameterGroup* pGroup = scene->mParameterStack.AllocateParametersWithBuffer(*mResourceUpdateList, buffers, paramTypes);
	pGroup->SetTextures(texs);
	Scene* actualScene = params.mpScene;
	scene->mSceneParams.mGlobalLevelWhite = sGlobalRenderConfig.mHDRLevelWhite;
	scene->mSceneParams.mGlobalLevelBlack = sGlobalRenderConfig.mHDRLevelBlack;
	bool applyOffset = params.mbAfterEffects && actualScene->mName.find("adv_") != std::string::npos;
	scene->mSceneParams.mGlobalLevelIntensity = sGlobalRenderConfig.mIntensity;
	if (applyOffset && sGlobalRenderConfig.mHDRType != eRenderHDR_None)
		scene->mSceneParams.mGlobalLevelIntensity += 0.25f;
	scene->mSceneParams.mShadowColor = actualScene->mShadowColor;
	scene->mSceneParams.mShadowMomentBias = actualScene->mLightEnvShadowMomentBias;
	scene->mSceneParams.mShadowDepthBias = actualScene->mLightEnvShadowDepthBias;
	scene->mSceneParams.mShadowPositionOffsetBias = actualScene->mLightEnvShadowPositionOffsetBias;
	scene->mSceneParams.mShadowLightBleedReduction = actualScene->mLightEnvShadowLightBleedReduction;
	scene->mSceneParams.mShadowTraceMaxDistance = actualScene->mLightShadowTraceMaxDistance;
	scene->mSceneParams.mbAfterEffects = params.mbAfterEffects;
	scene->mSceneParams.mShadowAtlasResolution = params.mShadowMapAtlasResolution;
	scene->mSceneParams.mShadowCascadeCount = params.mShadowCascadeCount;
	scene->mSceneParams.mWindParams.mDirection = actualScene->mWindParams.mDirection;
	scene->mSceneParams.mWindParams.mSpeed = actualScene->mWindParams.mSpeed;
	scene->mSceneParams.mWindParams.mIdleStrength = actualScene->mWindParams.mIdleStrength;
	scene->mSceneParams.mWindParams.mIdleSpacialFrequency = actualScene->mWindParams.mIdleSpacialFrequency;
	scene->mSceneParams.mWindParams.mGustSpeed = actualScene->mWindParams.mGustSpeed;
	scene->mSceneParams.mWindParams.mGustStrength = actualScene->mWindParams.mGustStrength;
	scene->mSceneParams.mWindParams.mGustSpacialFrequency = actualScene->mWindParams.mGustSpacialFrequency;
	scene->mSceneParams.mWindParams.mGustIdleStrengthMultiplier = actualScene->mWindParams.mGustIdleStrengthMultiplier;
	scene->mSceneParams.mWindParams.mGustSeparationExponent = actualScene->mWindParams.mGustSeparationExponent;
	scene->mSceneParams.mGraphicBlackThreshold = actualScene->mGraphicBlackThreshold;
	scene->mSceneParams.mGraphicBlackSoftness = actualScene->mGraphicBlackSoftness;
	scene->mSceneParams.mGraphicBlackAlpha = actualScene->mGraphicBlackAlpha;
	scene->mSceneParams.mGraphicBlackNear = actualScene->mGraphicBlackNear;
	scene->mSceneParams.mGraphicBlackFar = actualScene->mGraphicBlackFar;
	if (actualScene->mFXColorActive || actualScene->mCameraFXColorActive) {
		scene->mSceneParams.mColorPlane = Vector4(actualScene->mFXColor, actualScene->mFXColorOpacity);
	}
	else
		scene->mSceneParams.mColorPlane = Vector4::Zero;
	scene->mSceneParams.mHDR_PaperWhiteNits = actualScene->mHDR_PaperWhiteNits;
	scene->mSceneParams.mNPRLineFalloff = actualScene->mNPRLineFalloff;
	scene->mSceneParams.mNPRLineBias = actualScene->mNPRLineBias;
	scene->mSceneParams.mNPRLineAlphaFalloff = actualScene->mNPRLineAlphaFalloff;
	scene->mSceneParams.mNPRLineAlphaBias = actualScene->mNPRLineAlphaBias;
	scene->mSceneParams.mNoiseScale = actualScene->mFXNoiseScale;
	scene->mSceneParams.mTime = actualScene->mMaterialTime;
	scene->mSceneParams.mPrevTime = actualScene->mMaterialPrevTime;
	scene->mSceneParams.mTAAWeight = params.mbCameraCut ||
		(TelltaleToolLib_GetRenderAdaptersStruct()->GetDeltaTime() >= sGlobalRenderConfig.mTAAFrameTimeThreshold) ? 1.0f : actualScene->mFXTAAWeight;
	if (sGlobalRenderConfig.mbGammaCorrect) {
		scene->mSceneParams.mColorPlane = scene->mSceneParams.mColorPlane.GammaToLinear();
		scene->mSceneParams.mFogColor = scene->mSceneParams.mFogColor.GammaToLinear();
		scene->mSceneParams.mShadowColor = scene->mSceneParams.mShadowColor.GammaToLinear();
	}
	T3EffectSceneUtil::SetParameters((T3EffectParameter_BufferScene*)buffers.mpBuffer[eEffectParameter_UniformBufferScene - eEffectParameter_FirstUniformBuffer], scene->mSceneParams);
	mSceneList.insert_tail(scene);
	return scene;
}

// RENDER FRAME SCENE

T3EffectParameterGroupStack RenderFrameScene::_GetParametersForPass(RenderViewType viewType, Camera* pCamera,
	Vector2& jitter, Vector2& prevJitter, RenderClipPlanes& clipPlanes, int targetWidth,
	int targetHeight, unsigned int targetSampleCount, T3RenderViewport& viewport) {
	for (PassParameters* _it = mPassParameterList.head(); _it; _it = _it->next) {
		PassParameters& it = *_it;
		if (it.mViewType == viewType
			&& it.mTargetWidth == targetWidth
			&& it.mTargetHeight == targetHeight
			&& it.mTargetSampleCount == targetSampleCount
			&& it.mViewport == viewport
			&& it.mpCamera == pCamera
			&& (float)((float)((float)(it.mJitter.y - jitter.y) * (float)(it.mJitter.y - jitter.y))
				+ (float)((float)(it.mJitter.x - jitter.x) * (float)(it.mJitter.x - jitter.x))) < 0.000001f
			&& (float)((float)((float)(it.mPrevJitter.y - prevJitter.y) * (float)(it.mPrevJitter.y - prevJitter.y))
				+ (float)((float)(it.mPrevJitter.x - prevJitter.x) * (float)(it.mPrevJitter.x - prevJitter.x))) < 0.000001f
			&& clipPlanes.mCount == it.mClipPlanes.mCount)
		{
			bool ok = true;
			for (int i = 0; i < clipPlanes.mCount; i++) {
				if (!(clipPlanes.mPlanes[i].mPlane == it.mClipPlanes.mPlanes[i].mPlane)) {
					ok = false;
					break;
				}
			}
			if (ok)
				return it.mParameterStack;
		}
	}
	BitSet<enum T3EffectParameterType, 150, 0> feat{};
	if (mParams.mpScene) {
		feat.Set(eEffectParameter_UniformBufferCamera, 1);
		if (viewType == eRenderView_Main) {
			feat.Set(eEffectParameter_UniformBufferBrush, 1);
			feat.Set(eEffectParameter_UniformBufferPost, 1);
		}
	}
	T3EffectParameterUniformBufferOffsets offs{};
	PassParameters& pass = *mHeap->NewNoDestruct<PassParameters>();
	mPassParameterList.insert_tail(&pass);
	pass.mViewType = viewType;
	pass.mpCamera = std::move(pCamera);
	pass.mJitter = jitter;
	pass.mPrevJitter = prevJitter;
	pass.mClipPlanes = clipPlanes;
	pass.mTargetWidth = targetWidth;
	pass.mTargetHeight = targetHeight;
	pass.mViewport = viewport;
	pass.mTargetSampleCount = targetSampleCount;
	pass.mpParameterGroup = pass.mParameterStack.AllocateParametersWithBuffer(*mpRenderFrame->mResourceUpdateList, pass.mParameterBuffers, feat);
	return pass.mParameterStack;
}

void RenderFrameScene::_FinalizeParameters(){
	for (PassParameters* _it = mPassParameterList.head(); _it; _it = _it->next) {
		PassParameters& it = *_it;
		Scene* scene = mParams.mpScene;
		T3EffectCameraParams cameraParams{};
		T3EffectBrushParams brushParams{};
		T3EffectPostParams postParams{};
		cameraParams.mAspectRatio = 16.f / 9.f;
		T3EffectCameraUtil::GetParametersForCamera(cameraParams, mParams.mpScene, it.mpCamera, false);
		cameraParams.mJitter = it.mJitter;
		cameraParams.mPrevJitter = it.mPrevJitter;
		cameraParams.mClipPlanes = it.mClipPlanes;
		cameraParams.mTargetWidth = it.mTargetWidth;
		cameraParams.mTargetHeight = it.mTargetHeight;
		cameraParams.mTargetSampleCount = it.mTargetSampleCount;
		cameraParams.mViewport = it.mViewport;
		cameraParams.mbAfterEffects = mParams.mpScene && mParams.mpScene->mbAfterEffectEnabled;
		T3EffectCameraUtil::SetParameters((T3EffectParameter_BufferCamera*)it.mParameterBuffers.mpBuffer[eEffectParameter_UniformBufferCamera], cameraParams);
		brushParams.mBrushNearScale = scene->mFXBrushNearScale;
		brushParams.mBrushNearDetail = scene->mFXBrushNearDetail;
		brushParams.mBrushOutlineSize = scene->mFXBrushOutlineSize;
		brushParams.mBrushOutlineThreshold = scene->mFXBrushOutlineThreshold;
		brushParams.mBrushOutlineScale = scene->mFXBrushOutlineScale;
		brushParams.mBrushOutlineFalloff = scene->mFXBrushOutlineFalloff;
		brushParams.mBrushFarRamp = scene->mFXBrushFarFalloff;
		brushParams.mBrushFar = scene->mFXBrushFar;
		brushParams.mCameraFocalLength = it.mpCamera->mFocalLength;
		brushParams.mbDOFFieldOfViewAdjustEnabled = scene->mbFXDOFFieldOfViewAdjustEnabled;
		brushParams.mBrushFarDetail = scene->mFXBrushFarDetail;
		brushParams.mBrushFarMaxScale = scene->mFXBrushFarMaxScale;
		brushParams.mBrushFarScale = scene->mFXBrushFarScale;
		brushParams.mBrushFarScaleBoost = scene->mFXBrushFarScaleBoost;
		brushParams.mBrushOutlineColorThreshold = scene->mFXBrushOutlineColorThreshold;
		brushParams.mCameraFar = it.mpCamera->mFarClip;
		it.mpCamera->GetAdjustedFOV(brushParams.mHFOV, brushParams.mVFOV);
		brushParams.mTargetWidth = it.mTargetWidth;
		brushParams.mTargetHeight = it.mTargetHeight;

		if(mParams.mDebugRenderMode != eRenderDebugMode_Normal){
			postParams.mBloomIntensity = 0.f;
			postParams.mBloomThreshold = 1.0f;
		}else{
			postParams.mBloomIntensity = scene->mFXBloomIntensity;
			postParams.mBloomThreshold = scene->mFXBloomThreshold;
		}
		if(sGlobalRenderConfig.mHDRType != eRenderHDR_None){
			postParams.mLevelIntensity = scene->mFXLevelsIntensityHDR;
			postParams.mLevelBlack = scene->mFXLevelsBlackHDR;
			postParams.mLevelWhite = scene->mFXLevelsWhiteHDR;
		}else{
			postParams.mLevelIntensity = scene->mFXLevelsIntensity;
			postParams.mLevelBlack = scene->mFXLevelsBlack;
			postParams.mLevelWhite = scene->mFXLevelsWhite;
		}
		postParams.mFXTonemapRGBFarParams = scene->mFXTonemapRGBFarParams;
		postParams.mFXTonemapRGBParams = scene->mFXTonemapRGBParams;
		postParams.mTonemapIntensity = scene->mFXTonemapIntensity;
		postParams.mTonemapFilmicParams = scene->mFXTonemapFilmicParams;
		postParams.mTonemapFarFilmicParams = scene->mFXTonemapFilmicFarParams;
		postParams.mRadialBlurTint = it.mpCamera->GetFXRadialBlurTint();
		postParams.mRadialBlurTint.a = it.mpCamera->GetFXRadialBlurTintIntensity();
		postParams.mRadialBlurInRadius = it.mpCamera->GetFXRadialBlurInRadius();
		postParams.mRadialBlurOutRadius = it.mpCamera->GetFXRadialBlurOutRadius();
		postParams.mRadialBlurScale = it.mpCamera->GetFXRadialBlurScale();
		postParams.mRadialBlurIntensity = it.mpCamera->GetFXRadialBlurIntensity();
		postParams.mMotionBlurIntensity = it.mpCamera->GetFXMotionBlurIntensity();
		postParams.mSSLineColor = scene->mSSLineColor;
		postParams.mSSLineThickness = scene->mSSLineThickness;
		postParams.mSSLineDepthFadeNear = scene->mSSLineDepthFadeFar;
		postParams.mSSLineDepthFadeFar = scene->mSSLineDepthFadeNear;
		postParams.mSSLineDepthMagnitude = scene->mSSLineDepthMagnitude;
		postParams.mSSLineDepthExponent = scene->mSSLineDepthExponent;
		postParams.mSSLineLightDirection = scene->mSSLineLightDirection;
		postParams.mSSLineLightMagnitude = scene->mSSLineLightMagnitude;
		postParams.mSSLineLightExponent = scene->mSSLineLightExponent;
		postParams.mSSLineDebug = scene->mSSLineDebug;
		postParams.mTargetWidth = it.mTargetWidth;
		postParams.mTargetHeight = it.mTargetHeight;
		postParams.mZRange = it.mpCamera->mFarClip - it.mpCamera->mNearClip;
		postParams.mCameraFar = it.mpCamera->mFarClip;
		postParams.mCameraFocalLength = it.mpCamera->mFocalLength;
		postParams.mForceLinearDepthOffset = scene->mFXForceLinearDepthOffset;
		if(it.mpCamera->mbDOFEnabled){
			it.mpCamera->GetDOFParameters(postParams.mDofFar, postParams.mDofNear, postParams.mDofFarRamp, postParams.mDofNearRamp,
				postParams.mDofFarMax, postParams.mDofNearMax, postParams.mDofDebug, postParams.mDofCoverageBoost);
		}else if(scene->mFXDOFEnabled){
			postParams.mDofFar = scene->mFXDOFFar;
			postParams.mDofNear = scene->mFXDOFNear;
			postParams.mDofFarRamp = scene->mFXDOFFarFalloff;
			postParams.mDofNearRamp = scene->mFXDOFNearFalloff;
			postParams.mDofFarMax = scene->mFXDOFFarMax;
			postParams.mDofNearMax = scene->mFXDOFNearMax;
			postParams.mDofDebug = scene->mFXDOFDebug;
			postParams.mDofCoverageBoost = scene->mFXDOFCoverageBoost;
		}
		if(scene->mFXDOFEnabled && sGlobalRenderConfig.mDOFQuality != eDofQualityLevelDisabled){
			postParams.mBokehBlurThreshold = it.mpCamera->GetBokehBlurThreshold();
			postParams.mBokehBrightnessThreshold = it.mpCamera->GetBokehBrightnessThreshold();
			postParams.mBokehMinSize = it.mpCamera->GetBokehMinSize();
			postParams.mBokehMaxSize = it.mpCamera->GetBokehMaxSize();
			postParams.mDofCoverageBoost = 1.0f;
			postParams.mDofNearRamp = postParams.mDofFarRamp = 0.001f;
			postParams.mDofFarMax = fminf(1.0f, fmaxf(0.01f, postParams.mDofFarMax));
			if (postParams.mDofFar <= 0.0f)
				postParams.mDofFar = it.mpCamera->mFarClip;
			float gausN = 0.f, gausF = 0.f;
			float newDOFRadius = 0.f;
			if(sGlobalRenderConfig.mDOFQuality == eDOFQualityLevelLow){
				postParams.mDofFarResolutionFraction = postParams.mDofResolutionFraction = 0.25f;
				gausN = 0.1f;
				gausF = 0.1f;
				newDOFRadius = 24.f;
			}else if(sGlobalRenderConfig.mDOFQuality == eDOFQualityLevelMedium){
				postParams.mDofFarResolutionFraction = postParams.mDofResolutionFraction = 0.5f;
				gausN = 0.05f;
				gausF = 0.05f;
				newDOFRadius = 32.f;
			}else if(sGlobalRenderConfig.mDOFQuality == eDOFQualityLevelHigh){
				postParams.mDofFarResolutionFraction = postParams.mDofResolutionFraction = 0.75f;
				gausN = 0.025f;
				gausF = 0.025f;
				newDOFRadius = 48.f;
			}

			postParams.mDofFarBlurRadius = newDOFRadius;
			postParams.mDofNearBlurRadiusPixels = newDOFRadius;
			postParams.mDofMaxCocRadiusPixels = newDOFRadius;
			postParams.mDofInvNearBlurRadiusPixels = 1.0f / fminf(0.000001f, newDOFRadius);
			postParams.mDofFarRadiusRescale = 10.f * fminf(0.1f, postParams.mDofFarMax);
			postParams.mDofNearScale = ((newDOFRadius / postParams.mDofNearRamp) / fmax(0.000001f, postParams.mDofFarBlurRadius)) * it.mpCamera->mFarClip;
			float tmp = postParams.mDofFarBlurRadius < 0.000001f ? 0.000001f : postParams.mDofFarBlurRadius;
			postParams.mDofFarScale = ((postParams.mDofFarBlurRadius / postParams.mDofFarRamp) / fmax(0.000001f, postParams.mDofFarBlurRadius)) * it.mpCamera->mFarClip;
			postParams.mDofNearBlurryPlaneZ = (postParams.mDofNear - postParams.mDofNearRamp) / it.mpCamera->mFarClip;
			postParams.mDofNearSharpPlaneZ = (postParams.mDofNear - 0.0f) / it.mpCamera->mFarClip;
			postParams.mDofFarBlurryPlaneZ = (postParams.mDofNear + postParams.mDofFarRamp) / it.mpCamera->mFarClip;
			postParams.mDofFarSharpPlaneZ = (postParams.mDofNear - 0.0f) / it.mpCamera->mFarClip;
			if(scene->mFXDOFAutoFocusEnabled){
				if(scene->mFXAutoFocus_Near >= scene->mFXAutoFocus_Far){
					postParams.mDofAutoFocusFar = it.mpCamera->mFarClip;
					postParams.mDofAutoFocusNear = 0.f;
				}else{
					postParams.mDofAutoFocusFar = scene->mFXAutoFocus_Far;
					postParams.mDofAutoFocusNear = scene->mFXAutoFocus_Near;
				}
			}
			postParams.mDofVignetteMax = scene->mFXDOFVignetteMax;
			postParams.mbDOFFieldOfViewAdjustEnabled = scene->mbFXDOFFieldOfViewAdjustEnabled;
		}
		postParams.mVignetteStart = scene->mFXVignetteStart;
		postParams.mVignetteEnd = scene->mFXVignetteEnd;
		postParams.mVignetteTint = scene->mFXVignetteTint;
		postParams.mVignetteFalloff = scene->mFXVignetteFallOff;
		postParams.mRadialBlurTint = postParams.mRadialBlurTint.GammaToLinear();
		postParams.mVignetteTint = postParams.mVignetteTint.GammaToLinear();
		T3EffectPostUtil::SetParameters((T3EffectParameter_BufferPost*)it.mParameterBuffers.mpBuffer[eEffectParameter_UniformBufferPost],
			postParams, TelltaleToolLib_GetRenderAdaptersStruct()->GetDeltaTime());
		T3EffectBrushUtil::SetParameters((T3EffectParameter_BufferBrush*)it.mParameterBuffers.mpBuffer[eEffectParameter_UniformBufferBrush],
			brushParams);
	}
}

// RENDER SCENE VIEW

void RenderSceneView::PushDefaultPasses(T3RenderTargetContext& targetContext, RenderDefaultPassParams& params){
	for(int i = 0; i < eRenderPass_DefaultCount; i++){
		RenderPassParams paramsa{};
		paramsa.mPass = (T3RenderPass)(i + eRenderPass_FirstDefault);
		paramsa.mPassType = eRenderPassType_Scene;
		if ((paramsa.mPass <= eRenderPass_GBuffer_Simple) || paramsa.mPass == eRenderPass_Lines)
			paramsa.mPassType = eRenderPassType_GBuffer;
		else if (paramsa.mPass == eRenderPass_Post)
			paramsa.mPassType = eRenderPassType_Post;
		paramsa.mRenderTarget = params.mRenderTarget;
		//if(paramsa.mPass == eRenderPass_Main_Compute){
		//
		//}
		RenderViewPass* pass = PushPass(paramsa, targetContext);
		switch ((u32)paramsa.mPass)
		{
		case 5u:
			pass->mName = "Main_Compute";
			break;
		case 6u:
			pass->mName = "Main_Opaque";
			break;
		case 9u:
			pass->mName = "Main_Alpha_PreResolve";
			break;
		case 10u:
			pass->mName = "Main_Alpha";
			break;
		case 18u:
			pass->mName = "Main_Lines";
			break;
		case 19u:
			pass->mName = "Main_Lines_GBuffer";
			break;
		case 20u:
			pass->mName = "Main_Lines_LinearDepth";
			break;
		case 11u:
			pass->mName = "Main_AfterPostEffects";
			break;
		case 12u:
			pass->mName = "Main_AfterAntialiasing";
			break;
		}
	}
}

RenderViewPass* RenderSceneView::AllocatePass(RenderPassParams& params, T3RenderTargetContext& targetContext){
	RenderViewPass* pass = mHeap->NewNoDestruct<RenderViewPass>();
	pass->mProfileToken = -1;
	pass->mName = "Pass";
	pass->mpRenderView = this;
	pass->mParams = params;
	if(params.mOverrideTargetWidth > 0 || params.mOverrideTargetHeight > 0){
		pass->mTargetWidth = params.mOverrideTargetWidth;
		pass->mTargetHeight = params.mOverrideTargetHeight;
	}else{
		if (params.mbDefaultRenderTarget) {
			u32 acWidth = 0, acHeight = 0;
			TelltaleToolLib_GetRenderAdaptersStruct()->GetResolution(acWidth, acHeight);
			pass->mTargetWidth = (i32)acWidth;
			pass->mTargetHeight = (i32)acHeight;
		}else{
			T3RenderTargetUtil::GetResolution(&targetContext, params.mRenderTarget, pass->mTargetWidth, pass->mTargetHeight);
		}
	}
	if (params.mbDefaultRenderTarget)
		pass->mTargetMultisample = eSurfaceMultisample_None;
	else
		pass->mTargetMultisample = T3RenderTargetUtil::GetTargetMultisample(&targetContext, params.mRenderTarget);
	if (pass->mTargetWidth <= 0 || pass->mTargetHeight <= 0)
		pass->mViewport = T3RenderViewport();
	else{
		T3ViewportUtil::Set(&pass->mViewport, params.mbOverrideViewport ? &params.mViewportOverride : &mParams.mViewport, pass->mTargetWidth, pass->mTargetHeight);
		T3ViewportUtil::AddGutter(&pass->mViewport, mParams.mViewportGutter);
	}
	u32 samples = 1;
	if (pass->mTargetMultisample == eSurfaceMultisample_2x)
		samples = 2;
	else if (pass->mTargetMultisample == eSurfaceMultisample_4x)
		samples = 4;
	else if (pass->mTargetMultisample == eSurfaceMultisample_8x)
		samples = 8;
	else
		samples = 16;
	T3EffectParameterGroupStack s = mpRenderScene->_GetParametersForPass(mParams.mViewType, mParams.mpCamera, mParams.mJitter, mParams.mPrevJitter,
		mParams.mClipPlanes, pass->mTargetWidth, pass->mTargetHeight, samples, pass->mViewport);
	pass->mParameterStack = mParameterStack;
	pass->mParameterStack.PushParametersStack(*mHeap, params.mParameterStack);
	pass->mParameterStack.PushParametersStack(*mHeap, s);
	mPassList.insert_tail(pass);
	return pass;
}

void RenderSceneView::PushPass(RenderViewPass* pRenderPass, T3RenderTargetContext& targetContext){
	if (pRenderPass->mParams.mPass < eRenderPass_SceneCount)
		mScenePassList[pRenderPass->mParams.mPass] = pRenderPass;
	if (!pRenderPass->mParams.mbDefaultRenderTarget)
		T3RenderTargetUtil::AddReference(targetContext, pRenderPass->mParams.mRenderTarget);
	mPasses.Set(pRenderPass->mParams.mPass, 1);
	mPassList.insert_tail(pRenderPass);
}

RenderSceneView* RenderSceneView::PushSubView(const RenderSubViewParams& params){
	RenderSceneView* view = mpRenderScene->AllocateView(mParams);
	PushSubView(params, view);
	return view;
}

RenderSceneView* RenderSceneView::PushSubView(const RenderSubViewParams& params, RenderViewParams& subParams){
	RenderSceneView* view = mpRenderScene->AllocateView(subParams);
	PushSubView(params, view);
	return view;
}

void RenderSceneView::PushSubView(const RenderSubViewParams& params, RenderSceneView* pSubView) {
	RenderViewPass_SubView* pass = mHeap->NewNoDestruct<RenderViewPass_SubView>();
	pass->mPassType = eRenderViewPass_SubView;
	pass->mpSubView = pSubView;
	pass->mParams = params;
	mPassList.insert_tail(pass);
}

// RENDER INSTANCE MANAGER

void T3RenderInstManager::Execute(RenderSceneView& sceneView, T3EffectDrawParams& drawParams)
{
	if(mDispatchInstList.head()){
		bool updated = false;
		while (1) {
			updated = false;
			for (auto it = mDispatchInstList.head(); it; it = it->next) {
				if ((sceneView.mpVisibilityBits[it->mObjectIndex >> 5] & (1u << (it->mObjectIndex & 0x1F))) != 0){
					mDispatchInstList.remove(it);
					updated = 1;
					break;
				}
			}
			if (!updated)
				break;
		}
		//std::sort(mDispatchInstList.begin(), mDispatchInstList.end(), RenderInstSort{});
		mDispatchInstList.insertionSort();
		for (auto it = mDispatchInstList.head(); it; it = it->next)
			T3EffectCache::Dispatch(*it, drawParams);
	}
	if(mRenderInstList.head()){
		bool updated = false;
		while (1) {
			updated = false;
			for (auto it = mRenderInstList.head(); it; it = it->next) {
				if ((sceneView.mpVisibilityBits[it->mObjectIndex >> 5] & (1u << (it->mObjectIndex & 0x1F))) != 0) {
					mRenderInstList.remove(it);
					updated = 1;
					break;
				}
			}
			if (!updated)
				break;
		}
		//std::sort(mRenderInstList.begin(), mRenderInstList.end(), RenderInstSort{});
		mRenderInstList.insertionSort();
		for (auto it = mRenderInstList.head(); it; it = it->next)
			T3EffectCache::Draw(*it, drawParams);
	}
}
