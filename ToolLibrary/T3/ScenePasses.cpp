#include "Render.hpp"
#include "RenderScene.h"

bool _PreserveRT_MSAA(){
	return sGlobalRenderConfig.mAliasingType >= eRenderAntialias_MSAA_2x && sGlobalRenderConfig.mAliasingType <= eRenderAntialias_TAA_MSAA_8x;
}

void _AddMainPassTargetReferences(T3RenderTargetContext& targetContext, RenderViewPass* pPass){
	T3RenderTargetID id{};
	id.mValue = eRenderTargetID_DeferredDecalBuffer0;
	T3RenderTargetUtil::AddReference(pPass->mRenderTargetReferences, targetContext, id);
	id.mValue = eRenderTargetID_DeferredDecalBuffer1;
	T3RenderTargetUtil::AddReference(pPass->mRenderTargetReferences, targetContext, id);
	id.mValue = eRenderTargetID_DeferredLight0;
	T3RenderTargetUtil::AddReference(pPass->mRenderTargetReferences, targetContext, id);
	id.mValue = eRenderTargetID_DeferredLight1;
	T3RenderTargetUtil::AddReference(pPass->mRenderTargetReferences, targetContext, id);
	id.mValue = eRenderTargetID_DeferredShadows;
	T3RenderTargetUtil::AddReference(pPass->mRenderTargetReferences, targetContext, id);
}

void _PrepareMainPassRenderTarget(T3RenderTargetIDSet& set){
	T3RenderTargetConstantID id = eRenderTargetID_DepthBuffer;
	if (_PreserveRT_MSAA())
		id = eRenderTargetID_DepthBuffer_MSAA;
	set.SetDepthTarget({ id }, 0, 0);
	set.SetRenderTarget(T3RenderTargetID{ T3RenderTargetUtil::GetSceneBackBufferHDR(sGlobalRenderConfig.mAliasingType) }, 0, 0, 0);
}

RenderViewPass* _PrepareScenePass_Clear(RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	ScenePassParams params{};
	params.mbForceClear = 1;
	params.mbClearDepth = 1;
	params.mRenderTarget.SetDepthTarget({ eRenderTargetID_DepthBuffer }, 0, 0);
	params.mRenderTarget.SetRenderTarget({ eRenderTargetID_BackBufferHDR }, 0, 0,0);
	return _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "Clear");
}

RenderViewPass* _PrepareScenePass_Compute(RenderSceneView* pMainView, T3RenderTargetContext& targetContext, int targetWidth, int targetHeight) {
	RenderPassParams params{};
	params.mPass = eRenderPass_Main_Compute;
	params.mPassType = eRenderPassType_Scene;
	params.mOverrideTargetWidth = targetWidth;
	params.mOverrideTargetHeight = targetHeight;
	RenderViewPass* pass = pMainView->PushPass(params, targetContext);
	pass->mName = "Scene Compute";
	return pass;
}

RenderViewPass* _PrepareScenePass_DeferredDecal_Emmisive(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	ScenePassParams params{};
	params.mbAntialiasing = true;
	params.mPass = eRenderPass_DeferredDecal_Emissive;
	_PrepareMainPassRenderTarget(params.mRenderTarget);
	RenderViewPass* pPass = _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "DeferredDecal_Emmisive");
	T3RenderTargetID id{};
	id.mValue = eRenderTargetID_LinearDepth;
	T3RenderTargetUtil::AddReference(pPass->mRenderTargetReferences, targetContext, id);
	id.mValue = eRenderTargetID_GBuffer0;
	T3RenderTargetUtil::AddReference(pPass->mRenderTargetReferences, targetContext, id);
	return pPass;
}

RenderViewPass* _PrepareScenePass_DeferredDecal_GBuffer(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	ScenePassParams params{};
	params.mPass = eRenderPass_DeferredDecal_GBuffer;
	params.mbClearColor = true;
	params.mbForceClear = true;
	params.mbPreserveRenderTarget = _PreserveRT_MSAA();
	params.mRenderTarget.SetDepthTarget({ eRenderTargetID_DepthBuffer }, 0, 0);
	params.mRenderTarget.SetRenderTarget({ eRenderTargetID_DepthBuffer }, 0, 0,0);
	params.mRenderTarget.SetRenderTarget({ eRenderTargetID_DepthBuffer }, 1, 0, 0);

	RenderViewPass* pPass = _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "DeferredDecal_GBuffer");
	T3RenderTargetID id{};
	id.mValue = eRenderTargetID_LinearDepth;
	T3RenderTargetUtil::AddReference(pPass->mRenderTargetReferences, targetContext, id);
	id.mValue = eRenderTargetID_GBuffer0;
	T3RenderTargetUtil::AddReference(pPass->mRenderTargetReferences, targetContext, id);
	return pPass;
}

RenderViewPass* _PrepareScenePass_Downsampled(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext, RenderQualityType type) {
	TelltaleToolLib_RaiseError("Unsupported scene pass", ERR);
	//calls to post util draw.
	return 0;
}

RenderViewPass* _PrepareScenePass_ForceLinearDepth(Scene* pScene, Camera* pCamera, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	ScenePassParams params{};
	params.mPass = eRenderPass_ForceLinearDepth;
	params.mbClearColor = true;
	params.mbForceClear = true;
	params.mbPreserveRenderTarget = _PreserveRT_MSAA();
	params.mRenderTarget.SetDepthTarget({ eRenderTargetID_DepthBuffer }, 0, 0);
	params.mRenderTarget.SetRenderTarget({ eRenderTargetID_AlphaMeshLinearDepth }, 0, 0, 0);

	RenderViewPass* pPass = _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "ForceLinearDepth");
	return pPass;
}

RenderViewPass* _PrepareScenePass_GBuffer(Scene* pScene, Camera* pCamera, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	ScenePassParams params{};
	params.mbClearColor = params.mbClearDepth = params.mbClearStencil = params.mbForceClear = 1;
	params.mPass = eRenderPass_GBuffer;
	params.mbAntialiasing = 1;
	params.mRenderTarget.SetDepthTarget({ !_PreserveRT_MSAA() ? eRenderTargetID_DepthBuffer : eRenderTargetID_DepthBuffer_MSAA }, 0, 0);
	params.mRenderTarget.SetRenderTarget({ !_PreserveRT_MSAA() ? eRenderTargetID_GBuffer0 : eRenderTargetID_GBuffer0_MSAA }, 0, 0, 0);
	params.mRenderTarget.SetRenderTarget({ !_PreserveRT_MSAA() ? eRenderTargetID_Velocity : eRenderTargetID_Velocity_MSAA }, 1, 0, 0);
	return _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "GBuffer");
}

void _PrepareScenePass_GBufferLines(Scene* pScene, Camera* pCamera, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	if(pScene->mFXDOFEnabled || (pCamera&&pCamera->mbDOFEnabled) || pScene->mbGenerateNPRLines){
		ScenePassParams params{};
		params.mbClearDepth = params.mbClearStencil = params.mbForceClear = 1;
		params.mbAntialiasing = 1;
		params.mPass = eRenderPass_LinesCS_GBuffer;
		params.mRenderTarget.SetDepthTarget({ !_PreserveRT_MSAA() ? eRenderTargetID_DepthBuffer : eRenderTargetID_DepthBuffer_MSAA }, 0, 0);
		_PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "LinesCS_GBuffer");
	}
}

RenderViewPass* _PrepareScenePass_GBuffer_Simple(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	ScenePassParams params{};
	params.mPass = eRenderPass_GBuffer_Simple;
	params.mbClearColor = params.mbClearDepth = params.mbClearStencil = params.mbForceClear = 1;
	params.mbAntialiasing = 1;
	_PrepareMainPassRenderTarget(params.mRenderTarget);
	return _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "GBuffer_Simple");
}

RenderViewPass* _PrepareScenePass_Glow(RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	ScenePassParams params{};
	params.mPass = eRenderPass_Glow;
	params.mbClearColor = true;
	params.mbForceClear = true;
	params.mbPreserveRenderTarget = _PreserveRT_MSAA();
	params.mRenderTarget.SetDepthTarget({ eRenderTargetID_DepthBuffer }, 0, 0);
	params.mRenderTarget.SetRenderTarget({ eRenderTargetID_DepthBuffer }, 0, 0, 0);

	RenderViewPass* pPass = _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "Glow");
	T3RenderTargetID id{};
	id.mValue = eRenderTargetID_BackBufferHDRResolved;
	T3RenderTargetUtil::AddReference(pPass->mRenderTargetReferences, targetContext, id);
	id.mValue = eRenderTargetID_LinearDepth;
	T3RenderTargetUtil::AddReference(pPass->mRenderTargetReferences, targetContext, id);
	id.mValue = eRenderTargetID_GBuffer0;
	T3RenderTargetUtil::AddReference(pPass->mRenderTargetReferences, targetContext, id);
	return pPass;
}

RenderViewPass* _PrepareScenePass_Lines(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	ScenePassParams params{};
	params.mPass = eRenderPass_Lines;
	params.mbAntialiasing = 1;
	_PrepareMainPassRenderTarget(params.mRenderTarget);
	return _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "Main_Lines");
}

RenderViewPass* _PrepareScenePass_Main_AfterSSLines(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	ScenePassParams params{};
	params.mPass = eRenderPass_Main_AfterSSLines;
	_PrepareMainPassRenderTarget(params.mRenderTarget);
	RenderViewPass* pass = _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "Main_AfterSSLines");
	_AddMainPassTargetReferences(targetContext, pass);
	T3RenderTargetID id{};
	id.mValue = eRenderTargetID_BackBufferHDRResolved;
	T3RenderTargetUtil::AddReference(pass->mRenderTargetReferences, targetContext, id);
	return pass;
}

RenderViewPass* _PrepareScenePass_Main_Alpha(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	ScenePassParams params{};
	params.mPass = eRenderPass_Main_Alpha;
	params.mGPUScope = eRenderScopeGPU_Alpha;
	params.mRenderTarget.SetDepthTarget({ eRenderTargetID_DepthBuffer }, 0, 0);
	params.mRenderTarget.SetRenderTarget({ eRenderTargetID_BackBufferHDR }, 0, 0, 0);
	RenderViewPass* pass = _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "Main_Alpha");
	_AddMainPassTargetReferences(targetContext, pass);
	T3RenderTargetID id{};
	id.mValue = eRenderTargetID_BackBufferHDRResolved;
	T3RenderTargetUtil::AddReference(pass->mRenderTargetReferences, targetContext, id);
	return pass;
}

RenderViewPass* _PrepareScenePass_Main_Alpha_Antialiased(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	ScenePassParams params{};
	params.mPass = eRenderPass_Main_Alpha_Antialiased;
	params.mbAntialiasing = 1;
	_PrepareMainPassRenderTarget(params.mRenderTarget);
	RenderViewPass* pass = _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "Main_Alpha_Antialiased");
	_AddMainPassTargetReferences(targetContext, pass);
	return pass;
}

RenderViewPass* _PrepareScenePass_Main_Alpha_PreResolve(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	ScenePassParams params{};
	params.mPass = eRenderPass_Main_Alpha_PreResolve;
	params.mRenderTarget.SetDepthTarget({ eRenderTargetID_DepthBuffer }, 0, 0);
	params.mRenderTarget.SetRenderTarget({ eRenderTargetID_BackBufferHDR }, 0, 0, 0);
	RenderViewPass* pass = _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "Main_Alpha");
	_AddMainPassTargetReferences(targetContext, pass);
	return pass;
}

RenderViewPass* _PrepareScenePass_Main_Extended(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext,T3RenderPass pass, bool bDefaultRenderTarget) {
	ScenePassParams params{};
	params.mPass = pass;
	if(bDefaultRenderTarget){
		params.mbDefaultRenderTarget = 1;
	}
	else {
		params.mRenderTarget.SetDepthTarget({ eRenderTargetID_DepthBuffer }, 0, 0);
		params.mRenderTarget.SetRenderTarget({ eRenderTargetID_BackBuffer }, 0, 0, 0);
	}
	return _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, pass == eRenderPass_Main_AfterPostEffects ?
		"Main_AfterPostEffects" : pass == eRenderPass_Main_AfterAntialiasing ? "Main_AfterAntialiasing" : "Main_Extended");
}

RenderViewPass* _PrepareScenePass_Main_Opaque(RenderFrameScene& scene, RenderSceneView** pSceneViews, int viewCount,
	T3RenderTargetContext& targetContext, bool bClearColor, bool bClearDepthTarget) {
	ScenePassParams params{};
	params.mPass = eRenderPass_Main_Opaque;
	params.mGPUScope = eRenderScopeGPU_Main;
	params.mbClearColor = bClearColor;
	params.mbClearDepth = params.mbClearStencil = bClearDepthTarget;
	params.mbAntialiasing = params.mbForceClear = 1;
	_PrepareMainPassRenderTarget(params.mRenderTarget);
	RenderViewPass* pass = _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "Main_Opaque");
	_AddMainPassTargetReferences(targetContext, pass);
	return pass;
}

RenderViewPass* _PrepareScenePass_SSS(Scene* pScene, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext) {
	ScenePassParams params{};
	params.mPass = eRenderPass_DynamicLightMap;
	params.mbClearColor = 1;
	params.mbAntialiasing = 1;
	params.mRenderTarget.SetDepthTarget({ eRenderTargetID_DepthBuffer }, 0, 0);
	return _PrepareScenePassBase(params, pSceneViews, viewCount, targetContext, "SSS");
}

RenderViewPass* _PrepareScenePassBase(ScenePassParams& params, RenderSceneView** pSceneViews, int viewCount, T3RenderTargetContext& targetContext, const char* szName) {
	bool bNeedsClear = params.mbClearColor || params.mbClearDepth || params.mbClearDepth;
	for(int view = 0; view < viewCount; view++){
		RenderSceneView* pView = pSceneViews[0];
		if(params.mbPreserveRenderTarget){
			RenderSubViewParams p{};
			p.mbPopRenderTarget = p.mbPushRenderTarget = 1;
			p.mbResetColorTarget = false;
			RenderSceneView* pView0 = pSceneViews[0]->PushSubView(p);
			pView0->mName = _SetRenderStructName(pSceneViews[0]->mHeap, "SubView \"%s\"", szName);
			pView = pView0;
		}
		if(bNeedsClear && view > 0) {
			RenderPassParams clearParams{};
			clearParams.mPass = eRenderPass_Post;
			clearParams.mAntialiasType = sGlobalRenderConfig.mAliasingType;
			clearParams.mRequiredPassesAny.Set(params.mPass, 1);
			RenderViewPass* pView0 = pView->PushPass(pSceneViews[view], clearParams, targetContext);
			pView0->mName = _SetRenderStructName(pSceneViews[0]->mHeap, "Scene Clear \"%s\"", szName);
		}
		RenderPassParams passParams{};
		passParams.mAntialiasType = sGlobalRenderConfig.mAliasingType;
		passParams.mRenderTarget = params.mRenderTarget;
		passParams.mPass = params.mPass;
		T3RenderPassType type = eRenderPassType_None;
		if (params.mPass <= eRenderPass_GBuffer_Simple || params.mPass == eRenderPass_Lines)
			type = eRenderPassType_GBuffer;
		else if (params.mPass == eRenderPass_Post)
			type = eRenderPassType_Post;
		else
			type = eRenderPassType_Scene;
		passParams.mPassType = type;
		passParams.mbRequiredClearTarget = false;
		if(params.mbForceClear && view == 0){
			passParams.mForceRenderClear.mbColor = params.mbClearColor;
			passParams.mForceRenderClear.mbDepth = params.mbClearDepth;
			passParams.mForceRenderClear.mbStencil = params.mbClearStencil;
		}
		RenderViewPass* pView0 = pView->PushPass(pSceneViews[view], passParams, targetContext);
		pView0->mName = _SetRenderStructName(pSceneViews[0]->mHeap, "Scene \"%s\"", szName);
	}
	return 0;
}