#pragma once

#include "../Types/ObjectSpaceIntrinsics.h"
#include "../Types/Set.h"
#include "../Types/T3Texture.h"

//You can define this to 1 if you want inverted depth in the camera (if mbAllowInvertedDepth)
#ifndef INVERTED_DEPTH
#define INVERTED_DEPTH 0
#endif

// Telltale CAMERA CLASS implementation

inline void MatrixFinalizePlatformProject(Matrix4& mat){
	//Telltale platform specific finalize projection matrix. Change RH LH etc.
}

//mAgentTransform shuold be updated by YOU. thats the rotation and translation of the viewing camera agent. Must also update mAspectRatio,mScreenWidth/Height yourself! THEN CALL OnScreenModified!
//when agent transform is changed, call on agen transform modified
struct Camera
{
	//BY THE LIBRARY.
	Transform mAgentTransform;
	//YOU MUST UPDATE THIS YOURSELF! THEN CALL OnScreenModified!
	float mAspectRatio;
	int mScreenWidth;
	int mScreenHeight;

private:

	bool mbCullObjects;
	bool mbPushed;
	bool mbIsViewCamera;
	bool mbAllowInvertedDepth;
	bool mbAllowBlending;
	bool mbShouldUpdateBlendDestination;
	Frustum mCachedFrustum;
	Matrix4 mCachedWorldMatrix;
	Matrix4 mCachedViewMatrix;
	Matrix4 mCachedProjectionMatrix;
	bool mbWorldTransformDirty;
	bool mbFrustumDirty;
	bool mbViewMatrixDirty;
	bool mbProjectionMatrixDirty;
public:
	Symbol mName = Symbol::kEmptySymbol;
	Set<Symbol, std::less<Symbol> > mExcludeAgents;
	float mHFOV;
	float mHFOVScale;
	float mFocalLength;
	float mNearClip;
	float mFarClip;
	bool mbIsOrthoCamera;
	float mOrthoLeft;
	float mOrthoRight;
	float mOrthoBottom;
	float mOrthoTop;
	float mOrthoNear;
	float mOrthoFar;
	Color mFXColor;
	bool mFXColorActive;
	float mFXColorOpacity;
	float mExposure;
	bool mFXLevelsActive;
	float mFXLevelsBlack;
	float mFXLevelsWhite;
	float mFXLevelsIntensity;
	bool mFXRadialBlurActive;
	float mFXRadialBlurInnerRadius;
	float mFXRadialBlurOuterRadius;
	float mFXRadialBlurIntensity;
	Color mFXRadialBlurTint;
	float mFXRadialBlurTintIntensity;
	float mFXRadialBlurScale;
	bool mFXMotionBlurActive;
	float mFXMotionBlurStrength;
	bool mFXMotionBlurThresholdActive;
	float mFXMotionBlurThreshold;
	bool mFXMotionBlurThresholdRotActive;
	float mFXMotionBlurRotationThreshold;
	bool mMotionBlurDelay;
	float mDOFNear;
	float mDOFFar;
	float mDOFNearRamp;
	float mDOFFarRamp;
	float mDOFNearMax;
	float mDOFFarMax;
	float mDOFDebug;
	float mDOFCoverageBoost;
	bool mbDOFEnabled;
	bool mUseHQDOF;
	bool mbBrushDOFEnabled;
	Handle<T3Texture> mhBokehTexture;
	bool mUseBokeh;
	float mBokehBrightnessDeltaThreshold;
	float mBokehBrightnessThreshold;
	float mBokehBlurThreshold;
	float mBokehMinSize;
	float mBokehMaxSize;
	float mBokehFalloff;
	float mMaxBokehBufferVertexAmount;
	Vector3 mBokehAberrationOffsetsX;
	Vector3 mBokehAberrationOffsetsY;
	/*
	Symbol mAudioListenerOverrideAgentName;
	Symbol mAudioPlayerOriginOverrideAgentName;
	WeakPtr<Agent> mpAudioListenerOverrideAgent;
	WeakPtr<Agent> mpAudioPlayerOriginOverrideAgent;
	Handle<SoundReverbDefinition> mhAudioReverbOverride;
	SoundEventName<1> mtReverbSnapshotOverride;
	bool mbInLensCallback;*/

public:

	inline Camera() {
		mCachedProjectionMatrix = mCachedViewMatrix = mCachedWorldMatrix = Matrix4::Identity();
		mbWorldTransformDirty = false;
		mbViewMatrixDirty = mbFrustumDirty = mbProjectionMatrixDirty = true;
		mHFOVScale = 1.0f;
		mHFOV = 1.0f;
		mNearClip = mFarClip = -1.f;
		mbIsOrthoCamera = false;
		mOrthoLeft = mOrthoBottom = mOrthoNear = 0.f;
		mOrthoTop = mOrthoRight = mOrthoFar = 1.0f;
		mFXColor = Color::Black;
		mFXLevelsActive = mFXColorActive = mFXRadialBlurActive = false;
		mExposure = mFXColorOpacity = 0.f;
		mFXLevelsBlack = mFXLevelsWhite = 0.f;
		mFXLevelsIntensity = 0.f;
		mFXRadialBlurInnerRadius = 0.f;
		mFXRadialBlurOuterRadius = 1.f;
		mFXRadialBlurIntensity = 0.f;
		mFXRadialBlurTint = Color::Black;
		mFXRadialBlurTintIntensity = 0.f;
		mFXMotionBlurActive = mFXMotionBlurThresholdActive = mFXMotionBlurThresholdRotActive = mMotionBlurDelay = false;
		mFXMotionBlurStrength = mFXMotionBlurRotationThreshold = mFXMotionBlurRotationThreshold = 0.f;
		mDOFNear = 2.0f;
		mDOFFar = 4.0f;
		mDOFNearRamp = 1.0f;
		mDOFFarRamp = 4.0f;
		mDOFNearMax = 1.0f;
		mDOFDebug = 0.f;
		mDOFCoverageBoost = 0.f;
		mbDOFEnabled = false;
		mUseHQDOF = true;
		mbBrushDOFEnabled = false;
		mUseBokeh = false;
		mBokehBrightnessDeltaThreshold = 0.075f;
		mBokehBrightnessThreshold = mBokehBlurThreshold = 0.25f;
		mBokehMinSize = 0.005f;
		mBokehMaxSize = 0.025f;
		mBokehFalloff = 0.7f;
		mMaxBokehBufferVertexAmount = 0.f;
		mBokehAberrationOffsetsX = mBokehAberrationOffsetsY = Vector3();
	}

	inline bool IsAgentExcluded(Symbol agent){
		return mExcludeAgents.find(agent) != mExcludeAgents.cend();
	}

	inline Vector3 Up() {
		if (mbWorldTransformDirty)
			_UpdateCachedTransform();
		return mCachedWorldMatrix.TransformNormal(Vector3::Up);
	}

	inline Vector3 Forward(){
		if (mbWorldTransformDirty)
			_UpdateCachedTransform();
		return mCachedWorldMatrix.TransformNormal(Vector3::Forward);
	}
	
	inline Matrix4 ComputeLocalViewMatrix() {
		return Matrix4::LookAt({ 0.f,0.f,0.f }, Forward(), Up());
	}

	inline Matrix4 ComputeNonInvertedProjectionMatrix(){
		return _BuildProjectionMatrix(mNearClip, mFarClip, false);
	}

	inline Matrix4 _BuildProjectionMatrix(float znear, float zfar, bool bInvertDepth){
		if(znear==zfar){
			znear = mNearClip;
			zfar = mFarClip;
		}
		if(bInvertDepth){
			float tmp = znear;
			znear = zfar;
			zfar = tmp;
		}
		if(mbIsOrthoCamera){
			return Matrix4::OrthogonalOffCenter(mOrthoLeft, mOrthoRight, mOrthoBottom, mOrthoTop, znear, zfar);
		}else{
			return Matrix4::PerspectiveFov((mHFOV * mHFOVScale) * 0.01308997f/*pi/240*/, mAspectRatio, znear, zfar);
		}
	}

	inline Vector3 WorldPosToLogicalScreenPos(const Vector3& world_pos){
		Vector3 v = world_pos - mAgentTransform.mTrans;
		v = v * mAgentTransform.mRot.Conjugate();
		if (v.z == 0.0f)
			v.z = -0.000001f;
		float tmp = ((float)mScreenHeight * 0.5f) / (tanf(mHFOVScale * mHFOV) * /*to rad, pi/240*/0.01308997f * 0.5f/*tan theta over 2, so * 0.5 */);//simple trig
		tmp /= v.z;//perspective divide
		v.x = (((float)mScreenWidth * 0.5f) - (v.x * tmp)) / (float)mScreenWidth;
		v.y = (((float)mScreenHeight * 0.5f) - (v.y * tmp)) / (float)mScreenHeight;
		return v;
	}

	//seems to be the exact same as logical pos
	inline Vector3 WorldPosToViewportPos(const Vector3& worldPos){
		return WorldPosToLogicalScreenPos(worldPos);
	}

	inline Vector3 ScreenPosToViewportPos(int sx, int sy) {
		return ScreenPosToViewportPos(sx, sy, mNearClip);
	}

	inline Vector3 ScreenPosToViewportPos(int sx, int sy, float worldSpaceDepth){
		if (mScreenHeight && mScreenWidth) {
			float v14 = (float)(tanf((float)(mHFOVScale * mHFOV) * 0.0065449849f) * worldSpaceDepth) / ((float)mScreenHeight * 0.5f);
			float v15 = (float)(((float)mScreenHeight / 2.f) - (float)((float)((float)sy / (float)mScreenHeight) * (float)mScreenHeight)) * v14;
			Matrix4 worldmat = GetWorldMatrix();
			float v17 = (float)(((float)mScreenWidth * 0.5f) - (float)((float)((float)sx / (float)mScreenWidth) * (float)mScreenWidth)) * v14;
			float v18 = (float)((float)(v15 * worldmat._Entries[1][0]) + (float)(v17 * worldmat._Entries[0][0]))
				+ (float)(worldSpaceDepth * worldmat._Entries[2][0]);
			float v19 = (float)((float)(v17 * worldmat._Entries[0][2]) + (float)(v15 * worldmat._Entries[1][2]))
				+ (float)(worldSpaceDepth * worldmat._Entries[2][2]);
			float v20 = (float)((float)(v17 * worldmat._Entries[0][1]) + (float)(v15 * worldmat._Entries[1][1]))
				+ (float)(worldSpaceDepth * worldmat._Entries[2][1]);
			return Vector3(v18 + worldmat._Entries[3][0], v20 + worldmat._Entries[3][1], v19 + worldmat._Entries[3][2]);
		}
		else return Vector3::Forward;
	}
	
	inline bool Visible(const Sphere& sphere, const Transform& sphere_xform, const Vector3& renderScale){
		if (!mbCullObjects)
			return true;
		Frustum f = GetFrustum();
		Vector3 v = (sphere.mCenter * sphere_xform.mRot) + sphere_xform.mTrans;
		float threshold = sphere.mRadius * -1.0f * renderScale.x;//only x needed as its a sphere
		int i = 0;
		while ((Vector3::Dot(Vector3(f.mPlane[i].mPlane), v) + f.mPlane[i].mPlane.w) >= threshold) {
			if (++i >= f.mPlaneCount) {
				return true;
			}
		}
		return false;
	}

	inline bool Visible(const BoundingBox& bbox, const Transform& bbox_xform, const Vector3& renderScale) {
		if(mbCullObjects){
			Frustum f = GetFrustum();
			Matrix4 mat = MatrixTransformation(renderScale, bbox_xform.mRot, bbox_xform.mTrans);
			BoundingBox transfomed{};
			transfomed.mMin = mat.TransformPoint(bbox.mMin);
			transfomed.mMax = mat.TransformPoint(bbox.mMax);
			return f.Visible(transfomed);
		}
		return true;
	}

	inline Vector2 ViewportRelativeToAbsolute(const Vector2& pos){
		float v5;
		float v6;
		float v7;
		float v8;
		int outWidth;
		int outHeight;

		Vector2 result{};
		outWidth = mScreenWidth;
		outHeight = mScreenHeight;
		v5 = 1.0f;
		v6 = fmaxf(pos.x, 0.0f);
		if (v6 >= 1.0f)
			v6 = 1.0f;
		v7 = (float)outWidth * v6;
		v8 = fmaxf(pos.y, 0.0f);
		result.x = v7;
		if (v8 < 1.0f)
			v5 = v8;
		result.y = (float)outHeight * v5;
		return result;
	}

	//0.5 to -0.5
	inline Vector3 ViewportPosToDirVector(int sx, int sy){
		if (mScreenHeight && mScreenWidth) {
			if (sx > mScreenWidth)
				sx = mScreenWidth;
			if (sy > mScreenHeight)
				sy = mScreenHeight;
			float v17 = ((float)mScreenWidth * 0.5f) - (float)sx;
			float v18 = ((float)mScreenHeight * 0.5f) - (float)sy;
			float v19 = ((float)mScreenHeight * 0.5f) / tanf((float)(mHFOVScale * mHFOV) * 0.0065449849f);
			Matrix4 v20 = GetWorldMatrix();
			float v21 = v18 * v20._Entries[1][1];
			float v22 = v17 * v20._Entries[0][1];
			float v23 = (float)((float)(v18 * v20._Entries[1][0]) + (float)(v17 * v20._Entries[0][0]))
				+ (float)(v19 * v20._Entries[2][0]);
			float v24 = v19;
			float v25 = v19 * v20._Entries[2][2];
			float v26 = v24 * v20._Entries[2][1];
			float v27 = (float)(v18 * v20._Entries[1][2]) + (float)(v17 * v20._Entries[0][2]);
			Vector3 v6{};
			v6.x = v23;
			v6.y = (v21 + v22) + v26;
			v6.z = v27 + v25;
			v6.Normalize();
			return v6;
		}
		else return Vector3::Forward;
	}

	inline void _UpdateCachedTransform() {
		mbWorldTransformDirty = false;
		mCachedWorldMatrix = MatrixTransformation(mAgentTransform.mRot, mAgentTransform.mTrans);
	}

	inline void GetAdjustedFOV(float& hfov_radians, float& vfov_radians){
		vfov_radians = (mHFOV * mHFOVScale) * 0.01308997f/*pi/240*/;
		hfov_radians = vfov_radians * mAspectRatio;
	}

	inline Matrix4 GetViewMatrix() {
		if(mbViewMatrixDirty){
			if (mbWorldTransformDirty)
				_UpdateCachedTransform();
			Vector3 Eye = Vector3(mCachedWorldMatrix.GetColumn(3));
			Vector3 At = Eye + Forward();
			mCachedViewMatrix = Matrix4::LookAt(Eye, At, Up());
			mbViewMatrixDirty = false;
		}
		return mCachedViewMatrix;
	}

	inline bool IsInvertedDepth() const {
		return mbAllowInvertedDepth && INVERTED_DEPTH;
	}

	inline Frustum GetFrustum() {
		if(mbFrustumDirty){
			Matrix4 viewProjection = GetViewMatrix() * _BuildProjectionMatrix(mNearClip, mFarClip, IsInvertedDepth());
			mCachedFrustum.mPlane[FRUSTUMPLANE_ZNEAR].mPlane = Vector4(Vector3::Forward, 1.0f);
			mCachedFrustum.mPlane[FRUSTUMPLANE_LEFT].mPlane = Vector4(Vector3::Left, 1.0f);
			mCachedFrustum.mPlane[FRUSTUMPLANE_RIGHT].mPlane = Vector4(Vector3::Right, 1.0f);
			mCachedFrustum.mPlane[FRUSTUMPLANE_DOWN].mPlane = Vector4(Vector3::Down, 1.0f);
			mCachedFrustum.mPlane[FRUSTUMPLANE_UP].mPlane = Vector4(Vector3::Up, 1.0f);
			mCachedFrustum.mPlane[FRUSTUMPLANE_ZFAR].mPlane = Vector4(Vector3::Backward, 1.0f);
			mCachedFrustum.TransformBy(viewProjection);
			for (int i = 0; i < mCachedFrustum.mPlaneCount; i++) {
				Vector3 normal = Vector3(mCachedFrustum.mPlane[i].mPlane);
				float mag = normal.Magnitude();
				normal /= mag;
				mCachedFrustum.mPlane[i].mPlane = Vector4(normal, mag * mCachedFrustum.mPlane[i].mPlane.w);
			}
			mbFrustumDirty = false;
		}
		return mCachedFrustum;
	}

	inline void OnScreenModified(){
		mbProjectionMatrixDirty = true;
	}

	inline void OnAgentTransformModified(){
		mbWorldTransformDirty = true;
	}

	inline Matrix4 GetProjectionMatrix() {
		if(mbProjectionMatrixDirty){
			float zNear = mbIsOrthoCamera ? mOrthoNear : mNearClip;
			float zFar = mbIsOrthoCamera ? mOrthoFar : mFarClip;
			mCachedProjectionMatrix = _BuildProjectionMatrix(zNear, zFar, IsInvertedDepth());
			mbProjectionMatrixDirty = false;
		}
		return mCachedProjectionMatrix;
	}

	inline void LookAt(const Vector3& worldAt){
		if (mbWorldTransformDirty)
			_UpdateCachedTransform();
		Vector3 Translation = Vector3(mCachedWorldMatrix.GetColumn(3));
		Vector3 normalDir = worldAt - Translation;
		normalDir.Normalize();
		Quaternion Rotation = Quaternion(normalDir);
		mCachedWorldMatrix = MatrixTransformation(Rotation, Translation);
		mbFrustumDirty = true;
		mbViewMatrixDirty = true;
		mbWorldTransformDirty = false;
	}

	inline void LookAt(const Vector3& worldEye, const Vector3& worldAt){
		Vector3 normalDir = worldAt - worldEye;
		normalDir.Normalize();
		Quaternion Rotation = Quaternion(normalDir);
		mCachedWorldMatrix = MatrixTransformation(Rotation, worldEye);
		mbFrustumDirty = true;
		mbViewMatrixDirty = true;
	}

	/*telltale impl seems to be broken. always returns 1.
	inline bool PointInFrustum(const Vector3& worldPosition){
		Frustum frustum = GetFrustum();
		return frustum.
	}*/

	inline void SetViewMatrix(const Matrix4& lhs){
		mCachedWorldMatrix = Matrix4::Identity();
		mCachedViewMatrix = lhs;
		mbFrustumDirty = mbViewMatrixDirty = false;
	}

	inline void SetWorldMatrix(const Matrix4& lhs) {
		mCachedWorldMatrix = lhs;
		mbViewMatrixDirty = true;
		mbFrustumDirty = true;
		mbWorldTransformDirty = false;
	}

	inline void SetWorldPosition(const Vector3& position){
		mCachedWorldMatrix.SetColumn(3, Vector4(position, 1.0f));
		mbFrustumDirty = mbViewMatrixDirty = true;
		mbWorldTransformDirty = false;
	}

	inline void SetWorldQuaternion(const Quaternion& quat){
		if (mbWorldTransformDirty)
			_UpdateCachedTransform();
		mCachedWorldMatrix = MatrixTransformation(quat, Vector3(mCachedWorldMatrix.GetColumn(3)));
		mbFrustumDirty = mbViewMatrixDirty = true;
		mbWorldTransformDirty = false;
	}

	inline void SetWorldTransform(const Transform& transform){
		mCachedWorldMatrix = MatrixTransformation(transform.mRot, transform.mTrans);
		mbFrustumDirty = mbViewMatrixDirty = true;
	}

	inline Matrix4& GetWorldMatrix() {
		if (mbWorldTransformDirty)
			_UpdateCachedTransform();
		return mCachedWorldMatrix;
	}

	inline void SetOrthoParameters(float orthoLeft, float orthoRight, float orthoTop, float orthoBottom, float orthoNear, float orthoFar)
	{
		mOrthoBottom = orthoBottom;
		mOrthoLeft = orthoLeft;
		mOrthoFar = orthoFar;
		mOrthoRight = orthoRight;
		mOrthoTop = orthoTop;
		mOrthoNear = orthoNear;
		mbIsOrthoCamera = 1;
		mbProjectionMatrixDirty = 1;
		mbFrustumDirty = 1;
	}

	inline void SetNearClip(float near_clip, int renderDeviceDepthSize = 24)
	{
		if (near_clip != mNearClip)
		{
			if (renderDeviceDepthSize < 24)
				near_clip = fmaxf(near_clip, 0.1f);
			mNearClip = near_clip;
			mbFrustumDirty = 1;
			mbProjectionMatrixDirty = 1;
		}
	}

	inline void SetMaxBokehBufferVertexAmount(float value)
	{
		float v2;
		v2 = fminf(value, 1.0f);
		if (v2 > -0.0f)
			this->mMaxBokehBufferVertexAmount = v2;
		else
			mMaxBokehBufferVertexAmount = 0;
	}

	inline void SetHFOVScale(float scale)
	{
		Camera* v2;
		float v3;

		v2 = this;
		if (scale != mHFOVScale)
		{
			mHFOVScale = scale;
			v3 = tanf((float)(scale * this->mHFOV) * 0.0087266462f);//pi/360
			v2->mbFrustumDirty = 1;
			v2->mbProjectionMatrixDirty = 1;
			v2->mFocalLength = 0.5f / v3;
		}
	}

	inline void SetHFOV(float fov_degrees)
	{
		Camera* v2;
		float v3;

		v2 = this;
		if (fov_degrees != this->mHFOV)
		{
			this->mHFOV = fov_degrees;
			v3 = tanf((float)(fov_degrees * this->mHFOVScale) * 0.0087266462f);
			v2->mbFrustumDirty = 1;
			v2->mbProjectionMatrixDirty = 1;
			v2->mFocalLength = 0.5f / v3;
		}
	}

	inline void SetFarClip(float far_clip)
	{
		if (far_clip != this->mFarClip)
		{
			mFarClip = far_clip;
			mbFrustumDirty = 1;
			mbProjectionMatrixDirty = 1;
		}
	}

	inline void SetProjectionMatrix(const Matrix4& proj){
		mCachedWorldMatrix = mCachedViewMatrix = Matrix4::Identity();
		mCachedProjectionMatrix = proj;
		mbWorldTransformDirty = mbViewMatrixDirty = mbFrustumDirty = mbProjectionMatrixDirty = false;
	}

	inline void SetFXRadialBlurTintIntensity(float tintIntensity)
	{
		float v2; // xmm1_4@1

		v2 = fmaxf(tintIntensity, 2.f);
		if (v2 < 2.0f)
			this->mFXRadialBlurTintIntensity = v2;
		else
			this->mFXRadialBlurTintIntensity = 2.0f;
	}

	inline void SetFXRadialBlurTint(const Color& tint)
	{
		this->mFXRadialBlurTint = tint;
	}

	inline void SetFXRadialBlurScale(float scale)
	{
		this->mFXRadialBlurScale = fmaxf(scale, 0.0f);
	}

	inline void SetFXRadialBlurOutRadius(float radius)
	{
		float v2;
		float v3;

		v2 = 1.0f;
		v3 = fmaxf(radius, 0.0f);
		if (v3 < 1.0f)
			v2 = v3;
		if (v2 > this->mFXRadialBlurInnerRadius)
			this->mFXRadialBlurOuterRadius = v2;
	}

	inline void SetFXRadialBlurIntensity(float intensity)
	{
		float v2;

		v2 = fmaxf(intensity, 0.0f);
		if (v2 < 2.0f)
			this->mFXRadialBlurIntensity = v2;
		else
			this->mFXRadialBlurIntensity = 2.0f;
	}

	inline void SetFXRadialBlurInRadius(float radius)
	{
		float v2;
		float v3;

		v2 = 1.0f;
		v3 = fmaxf(radius, 0.0f);
		if (v3 < 1.0f)
			v2 = v3;
		if (v2 < this->mFXRadialBlurOuterRadius)
			this->mFXRadialBlurInnerRadius = v2;
	}

	inline void SetFXRadialBlurActive(bool active)
	{
		this->mFXRadialBlurActive = active;
	}

	inline void SetFXMotionBlurRotationThresholdActive(bool active)
	{
		this->mFXMotionBlurThresholdRotActive = active;
	}

	inline void SetFXMotionBlurRotationThreshold(float threshold)
	{
		this->mFXMotionBlurRotationThreshold = threshold;
	}

	inline void SetFXMotionBlurMovementThresholdActive(bool active)
	{
		this->mFXMotionBlurThresholdActive = active;
	}

	inline void SetFXMotionBlurMovementThreshold(float threshold)
	{
		this->mFXMotionBlurThreshold = threshold;
	}

	inline void SetFXMotionBlurIntensity(float strength)
	{
		this->mFXMotionBlurStrength = strength;
	}

	inline void SetFXMotionBlurDelay(bool delay)
	{
		this->mMotionBlurDelay = delay;
	}

	inline void SetFXMotionBlurActive(bool active)
	{
		this->mFXMotionBlurActive = active;
	}

	inline void SetFXLevelsWhite(float white)
	{
		this->mFXLevelsWhite = white;
	}

	inline void SetFXLevelsIntensity(float intensity)
	{
		this->mFXLevelsIntensity = intensity;
	}

	inline void SetFXLevelsBlack(float black)
	{
		this->mFXLevelsBlack = black;
	}

	inline void SetFXLevelsActive(bool active)
	{
		this->mFXLevelsActive = active;
	}

	inline void SetFXColorOpacity(float opacity)
	{
		if (opacity != this->mFXColorOpacity)
		{
			this->mFXColorOpacity = opacity;
		}
	}

	inline void SetFXColorActive(bool active)
	{
		if (active != this->mFXColorActive)
		{
			this->mFXColorActive = active;
		}
	}

	inline void SetFXColor(const Color& color)
	{
		if (this->mFXColor.r != color.r
			|| this->mFXColor.g != color.g
			|| this->mFXColor.b != color.b
			|| this->mFXColor.a != color.a)
		{
			this->mFXColor = color;
		}
	}

	inline void SetExposure(float v)
	{
		this->mExposure = v;
	}

	inline void SetDOFNearMax(float DOFNearMax)
	{
		this->mDOFNearMax = DOFNearMax;
	}

	inline void SetDOFNearFallOff(float DOFNearRamp)
	{
		this->mDOFNearRamp = DOFNearRamp;
	}

	inline void SetDOFNear(float DOFNear)
	{
		this->mDOFNear = DOFNear;
	}

	inline void SetDOFFarMax(float DOFFarMax)
	{
		this->mDOFFarMax = DOFFarMax;
	}

	inline void SetDOFFarFallOff(float DOFFarRamp)
	{
		this->mDOFFarRamp = DOFFarRamp;
	}

	inline void SetDOFFar(float DOFFar)
	{
		this->mDOFFar = DOFFar;
	}

	inline void SetDOFEnabled(bool isEnabled)
	{
		this->mbDOFEnabled = isEnabled;
	}

	inline void SetDOFDebug(float v)
	{
		this->mDOFDebug = v;
	}

	inline void SetDOFCoverageBoost(float v)
	{
		this->mDOFCoverageBoost = v;
	}

	inline void SetBokehMinSize(float v)
	{
		this->mBokehMinSize = v;
	}

	inline void SetBokehMaxSize(float v)
	{
		this->mBokehMaxSize = v;
	}

	inline void SetBokehFalloff(float v)
	{
		this->mBokehFalloff = v;
	}

	inline void SetBokehBrightnessThreshold(float v)
	{
		this->mBokehBrightnessThreshold = v;
	}

	inline void SetBokehBrightnessDeltaThreshold(float v)
	{
		this->mBokehBrightnessDeltaThreshold = v;
	}

	inline void SetBokehBlurThreshold(float v)
	{
		this->mBokehBlurThreshold = v;
	}

	inline void SetBokehAberrationOffsetsY(const Vector3& v)
	{
		this->mBokehAberrationOffsetsY = v;
	}

	inline void SetBokehAberrationOffsetsX(const Vector3& v)
	{
		this->mBokehAberrationOffsetsX = v;
	}

	inline void SetAspectRatio(float aspect)
	{
		if (aspect != this->mAspectRatio)
		{
			mAspectRatio = aspect;
			mbFrustumDirty = 1;
			mbProjectionMatrixDirty = 1;
		}
	}

	inline void SetAllowInvertDepth(bool v)
	{
		if (mbAllowInvertedDepth != v)
		{
			mbAllowInvertedDepth = v;
			mbProjectionMatrixDirty = 1;
		}
	}

	inline void SetAllowBlending(bool v)
	{
		if (mbAllowBlending != v)
			mbAllowBlending = v;
	}

#define CGETTER(var, fn, type) inline type fn(){ return var; }

	CGETTER(mUseHQDOF, GetUseHQDOF, bool);
	CGETTER(mUseBokeh, GetUseBokeh, bool);
	CGETTER(mFXRadialBlurTintIntensity, GetFXRadialBlurTintIntensity, float);
	CGETTER(mFXRadialBlurTint, GetFXRadialBlurTint, Color);
	CGETTER(mFXRadialBlurScale, GetFXRadialBlurScale, float);
	CGETTER(mFXRadialBlurOuterRadius, GetFXRadialBlurOutRadius, float);
	CGETTER(mFXRadialBlurIntensity, GetFXRadialBlurIntensity, float);
	CGETTER(mFXRadialBlurInnerRadius, GetFXRadialBlurInRadius, float);
	CGETTER(mFXRadialBlurActive, GetFXRadialBlurActive, bool);
	CGETTER(mFXMotionBlurThresholdRotActive, GetFXMotionBlurThresholdRotationActive, bool);
	CGETTER(mFXMotionBlurRotationThreshold, GetFXMotionBlurRotationThreshold, float);
	CGETTER(mFXMotionBlurThresholdActive, GetFXMotionBlurThresholdActive, bool);
	CGETTER(mFXMotionBlurThreshold, GetFXMotionBlurThreshold, float);
	CGETTER(mFXMotionBlurStrength, GetFXMotionBlurIntensity, float);
	CGETTER(mFXMotionBlurActive, GetFXMotionBlurActive, bool);
	CGETTER(mDOFNearMax, GetDOFNearMax, float);
	CGETTER(mDOFNear, GetDOFNear, float);
	CGETTER(mDOFNearRamp, GetDOFNearFallOff, float);
	CGETTER(mDOFFarMax, GetDOFFarMax, float);
	CGETTER(mDOFFar, GetDOFFar, float);
	CGETTER(mDOFFarRamp, GetDOFFarFallOff, float);
	CGETTER(mDOFDebug, GetDOFDebug, float);
	CGETTER(mDOFCoverageBoost, GetDOFCoverageBoost, float);
	CGETTER(mhBokehTexture, GetBokenTexture, Handle<T3Texture>);
	CGETTER(mBokehMinSize, GetBokehMinSize, float);
	CGETTER(mBokehMaxSize, GetBokehMaxSize, float);
	CGETTER(mBokehFalloff, GetBokehFalloff, float);
	CGETTER(mBokehBrightnessThreshold, GetBokehBrightnessThreshold, float);

	CGETTER(mBokehBrightnessDeltaThreshold, GetBokehBrightnessDeltaThreshold, float);
	CGETTER(mBokehBlurThreshold, GetBokehBlurThreshold, float);
	CGETTER(mBokehAberrationOffsetsY, GetBokehAberrationOffsetsY, Vector3);
	CGETTER(mBokehAberrationOffsetsX, GetBokehAberrationOffsetsX, Vector3);
	CGETTER(mAspectRatio, GetAspectRatio, float);

	inline void GetDOFParameters(float& outDOFFar, float& outDOFNear, float& outDOFFarRamp, float& outDOFNearRamp, float& outDOFFarMax, float& outDOFNearMax, float& outDOFDebug, float& outCoverageBoost)
	{
		outDOFFar = this->mDOFFar;
		outDOFNear = this->mDOFNear;
		outDOFFarRamp = this->mDOFFarRamp;
		outDOFNearRamp = this->mDOFNearRamp;
		outDOFFarMax = this->mDOFFarMax;
		outDOFNearMax = this->mDOFNearMax;
		outDOFDebug = this->mDOFDebug;
		outCoverageBoost = this->mDOFCoverageBoost;
	}

};