// This file was written by Lucas Saragosa. The code derives from Telltale Games' Engine.
// I do not intend to take credit for it, however; Im the author of this interpretation of 
// the engine and require that if you use this code or library, you give credit to me and
// the amazing Telltale Games.

#ifndef _OBJ_INTRINS
#define _OBJ_INTRINS

#define COERCE(X,TYPE) *((TYPE*)&(X))

#include "../Meta.hpp"
#include <math.h>
#include <cmath>

struct alignas(4) Color {

	inline Color() { r = g = b = a = 0.f; }

	inline Color(float _R, float _G, float _B, float _A) {
		r = _R;
		b = _B;
		g = _G;
		a = _A;
	}

	float r, g, b, a;

	inline bool operator<(const Color& rhs) const {
		if (r == rhs.r) {
			if (g == rhs.g) {
				if (b == rhs.b) {
					if (a == rhs.a) {
						return false;
					}
					else {
						return a <= rhs.a;
					}
				}
				else {
					return b <= rhs.b;
				}
			}
			else {
				return g <= rhs.g;
			}
		}
		else {
			return r <= rhs.r;
		}
	}

	inline Color RGBToRGBM(float ex, float scale) {//2,6
		Color ret{};
		float exponented = powf(scale, ex);
		float RED = r;
		float exponeted = exponented;
		float GREEN = g;
		float BLUE = b;
		float maxRGB = 1.0f;
		float SCALECOPY = powf(RED, 1.0f / ex);
		float green_to_exp = powf(GREEN, 1.0f / ex);
		float v13 = powf(BLUE, 1.0 / ex) * (float)(1.0f / scale);
		maxRGB = (float)(1.0f / scale) * SCALECOPY;
		if (maxRGB >= 1.0f)
			maxRGB = 1.0f;
		maxRGB *= 255.0f;
		maxRGB = std::ceilf(maxRGB);
		float v16 = maxRGB * 0.0039215689f;// 1/255
		if (v16 <= 1e-6f)
			return Color::Black;
		ret.a = v16;
		ret.r = (float)(1.0f / v16) * (float)(SCALECOPY * (float)(1.0 / scale));
		ret.g = (float)(1.0f / v16) * (float)(green_to_exp * (float)(1.0 / scale));
		ret.b = (float)(1.0f / v16) * v13;
		return ret;
	}

	inline Color RGBMToRGB(float ex, float scale) {
		return Color(powf(a * r * scale, ex), powf(a * g * scale, ex), powf(a * b * scale, ex), 1.0f);
	}

	inline Color GammaToLinear() {
		return Color(powf(r, 2.2f), powf(g, 2.2f), powf(b, 2.2f), a);
	}

	inline Color LinearToGamma() {
		return Color(powf(r, 1.0f / 2.2f), powf(g, 1.0f / 2.2f), powf(b, 1.0f / 2.2f), a);
	}

	inline Color operator*=(float mult) {
		return Color(r * mult, g * mult, b * mult, a * mult);
	}

	inline Color operator/=(float mult) {
		return operator*=(1.0f / mult);
	}

	inline Color operator+=(float mult) {
		return Color(r + mult, g + mult, b + mult, a + mult);
	}

	inline Color operator-=(float mult) {
		return operator+=(-mult);
	}

	static Color Black;
	static Color White;
	static Color Red;
	static Color Green;
	static Color Blue;
	static Color Cyan;
	static Color Magenta;
	static Color Yellow;
	static Color DarkRed;
	static Color DarkGreen;
	static Color DarkBlue;
	static Color DarkCyan;
	static Color DarkMagenta;
	static Color DarkYellow;
	static Color Gray;
	static Color LightGray;
	static Color NonMetal;

};

template<typename T> struct Curve {//not serialized
	T cf0, cf1, cf2, cf3;
};

struct alignas(4) Vector2 {
	
	static Vector2 Zero;

	union {
		struct {
			union {
				float x;
				float u;
			};
			union {
				float y;
				float v;
			};
		};
		float array[2];
	};

	inline Vector2() { x = y = 0.f;}

	inline Vector2(float _x, float _y) {
		x = _x;
		y = _y;
	}

	inline Vector2(float _all) : x(_all), y(_all) {}

	inline float Magnitude() const {
		float mag = x * x + y * y;
		if (mag < 9.9999997e-21f)
			mag = 0.f;
		else
			mag = std::sqrtf(mag);
		return mag;
	}

	inline void Normalize() {
		float mag = Magnitude();
		if (mag > 0.f)
			mag = 1.0f / mag;
		x *= mag;
		y *= mag;
	}

	inline void Set(float x, float y) {
		this->x = x;
		this->y = y;
	}

	inline Vector2& operator+=(const Vector2& rhs) {
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	inline Vector2& operator-=(const Vector2& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		return *this;
	}

	inline Vector2& operator*=(const Vector2& rhs) {
		x *= rhs.x;
		y *= rhs.y;
		return *this;
	}

	inline Vector2& operator/=(const Vector2& rhs) {
		x /= rhs.x;
		y /= rhs.y;
		return *this;
	}

	inline float operator[](int index) const {
		return index == 0 ? x : y;
	}

	inline Vector2 operator-() const {
		return Vector2(-x, -y);
	}

};

struct Vector4;

struct alignas(4) Vector3 {

	inline operator Vector2() const {
		return Vector2(x, y);
	}

	inline Vector3() { x = y = z = 0.f; }

	inline Vector3(float _all) : x(_all), y(_all), z(_all) {}

	inline Vector3(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}

	inline float operator[](int index) const {
		return index == 0 ? x : index == 1 ? y : z;
	}

	inline Vector3(Vector2 xy, float z) : Vector3(xy.x, xy.y, z) {}

	inline Vector3(float x, Vector2 yz) : Vector3(x, yz.x, yz.y) {}

	float x;
	float y;
	float z;

	static MetaOpResult MetaOperation_SerializeAsync(void* pObj, MetaClassDescription* pDesc, MetaMemberDescription* pCtx, void* pUserData) {
		static_cast<MetaStream*>(pUserData)->Key("X");
		static_cast<MetaStream*>(pUserData)->serialize_float(&static_cast<Vector3*>(pObj)->x);
		static_cast<MetaStream*>(pUserData)->Key("Y");
		static_cast<MetaStream*>(pUserData)->serialize_float(&static_cast<Vector3*>(pObj)->y);
		static_cast<MetaStream*>(pUserData)->Key("Z");
		static_cast<MetaStream*>(pUserData)->serialize_float(&static_cast<Vector3*>(pObj)->z);
		return eMetaOp_Succeed;
	}

	static Vector3 Up;
	static Vector3 Down;
	static Vector3 Left;
	static Vector3 Right;
	static Vector3 Forward;
	static Vector3 Backward;
	static Vector3 VeryFar;
	static Vector3 Zero;
	static Vector3 Identity;

	inline void Set(float x, float y, float z, float w) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	inline float Magnitude() const {
		float mag = x * x + y * y + z * z;
		if (mag < 9.9999997e-21f)
			mag = 0.f;
		else
			mag = std::sqrtf(mag);
		return mag;
	}

	inline float MagnitudeSquared() const {
		float mag = x * x + y * y + z * z;
		if (mag < 9.9999997e-21f)
			mag = 0.f;
		return mag;
	}

	inline void Normalize() {
		float mag = Magnitude();
		if (mag > 0.f)
			mag = 1.0f / mag;
		x *= mag;
		y *= mag;
		z *= mag;
	}

	inline Vector3& operator+=(const Vector3& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		return *this;
	}

	inline Vector3& operator-=(const Vector3& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		return *this;
	}

	inline Vector3& operator*=(const Vector3& rhs) {
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		return *this;
	}

	inline Vector3(Vector2 xy) : Vector3(xy,0.f) {}

	inline Vector3& operator/=(const Vector3& rhs) {
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		return *this;
	}

	inline Vector3(Vector4 xyz);

	inline static Vector3 XProduct(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1[1] * v2[2] - v1[2] * v2[1],
			v1[2] * v2[0] - v1[0] * v2[2],
			v1[0] * v2[1] - v1[1] * v2[0]);
	}

	static inline Vector3 Normalize(Vector3 vec){
		vec.Normalize();
		return vec;
	}

	static inline float Dot(const Vector3& lhs, const Vector3& rhs) {
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}

	static inline Vector3 Cross(const Vector3& v1, const Vector3& v2){
		return XProduct(v1, v2);
	}

	static inline float AngleBetween(const Vector3& lhs, const Vector3& rhs){
		return std::acosf(Dot(lhs, rhs) / std::sqrtf(lhs.MagnitudeSquared() * rhs.MagnitudeSquared()));
	}

	static inline void CompleteOrthonormalBasis(const Vector3& n, Vector3& b1, Vector3& b2) {
		if (n.z < -0.9999999f)
		{
			b1 = Vector3(0.0f, -1.0f, 0.0f);
			b2 = Vector3(-1.0f, 0.0f, 0.0f);
			return;
		}
		const float a = 1.0f / (1.0f + n.z);
		const float b = -n.x * n.y * a;
		b1 = Vector3(1.0f - n.x * n.x * a, b, -n.x);
		b2 = Vector3(b, 1.0f - n.y * n.y * a, -n.y);
	}

	inline Vector3 operator-() const {
		return Vector3(-x, -y, -z);
	}

	inline float Length() const {
		return std::sqrtf(x * x + y * y + z * z);
	}

	inline Vector3(Color c) : x(c.r), y(c.g), z(c.b) {}

};

struct alignas(4) Vector4 {

	inline operator Color() {
		return Color(x, y, z, w);
	}

	static Vector4 Zero;

	inline Vector4(Color c) : x(c.r), y(c.g), z(c.b), w(c.a) {}

	inline bool operator==(const Vector4& rhs){
		return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
	}

	inline Vector4 operator-() const {
		return Vector4(-x, -y, -z, -w);
	}

	inline operator Vector3() const {
		return Vector3(x, y, z);
	}

	inline operator Vector2() const {
		return Vector2(x, y);
	}

	inline Vector4(Vector3 xyz) : Vector4(xyz, 0.f) {}

	inline Vector4(Vector2 xy) : Vector4(xy, 0.f, 0.f) {}

	inline Vector4() { x = y = z = 0.f; w = 0.0f; }

	inline Vector4(float _all) : x(_all), y(_all), z(_all), w(_all) {}

	inline Vector4(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	inline Vector4(float _x, Vector3 yzw) : x(_x), y(yzw.x), z(yzw.y), w(yzw.z) {}

	inline Vector4(Vector3 xyz, float _w) : x(xyz.x), y(xyz.y), z(xyz.z), w(_w) {}

	inline Vector4(Vector2 xy, Vector2 zw) : x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}

	inline Vector4(float _x, float _y, Vector2 zw) : x(_x), y(_y), z(zw.x), w(zw.y) {}

	inline Vector4(Vector2 xy, float _z, float _w) : x(xy.x), y(xy.y), z(_z), w(_w) {}

	inline Vector4(float _x, Vector2 yz, float _w) : x(_x), y(yz.x), z(yz.y), w(_w) {}

	float x;
	float y;
	float z;
	float w;

	inline float operator[](int index) const {
		return index == 0 ? x : index == 1 ? y : index == 2 ? z : w;
	}

	static MetaOpResult MetaOperation_SerializeAsync(void* pObj, MetaClassDescription* pDesc, MetaMemberDescription* pCtx, void* pUserData) {
		static_cast<MetaStream*>(pUserData)->Key("X");
		static_cast<MetaStream*>(pUserData)->serialize_float(&static_cast<Vector4*>(pObj)->x);
		static_cast<MetaStream*>(pUserData)->Key("Y");
		static_cast<MetaStream*>(pUserData)->serialize_float(&static_cast<Vector4*>(pObj)->y);
		static_cast<MetaStream*>(pUserData)->Key("Z");
		static_cast<MetaStream*>(pUserData)->serialize_float(&static_cast<Vector4*>(pObj)->z);
		static_cast<MetaStream*>(pUserData)->Key("W");
		static_cast<MetaStream*>(pUserData)->serialize_float(&static_cast<Vector4*>(pObj)->w);
		return eMetaOp_Succeed;
	}

	inline void Set(float x, float y, float z, float w){
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}

	inline Vector4& operator+=(const Vector4& rhs) {
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}

	inline Vector4& operator-=(const Vector4& rhs) {
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}

	inline Vector4& operator*=(const Vector4& rhs) {
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		z *= rhs.w;
		return *this;
	}

	inline Vector4& operator/=(const Vector4& rhs) {
		x /= rhs.x;
		y /= rhs.y;
		z /= rhs.z;
		w /= rhs.w;
		return *this;
	}

	inline float Magnitude() const {
		float mag = x * x + y * y + z * z + w * w;
		if (mag < 9.9999997e-21f)
			mag = 0.f;
		else
			mag = std::sqrtf(mag);
		return mag;
	}

	inline float MagnitudeSquared() const {
		float mag = x * x + y * y + z * z + w * w;
		if (mag < 9.9999997e-21f)
			mag = 0.f;
		return mag;
	}

};

inline Vector3::Vector3(Vector4 rhs){
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
}

inline Vector2 operator*(const Vector2& lhs, const Vector2& rhs) {
	Vector2 res = lhs;
	res *= rhs;
	return res;
}

inline Vector3 operator*(const Vector3& lhs, const Vector3& rhs) {
	Vector3 res = lhs;
	res *= rhs;
	return res;
}

inline Vector4 operator*(const Vector4& lhs, const Vector4& rhs){
	Vector4 res = lhs;
	res *= rhs;
	return res;
}

inline Vector2 operator/(const Vector2& lhs, const Vector2& rhs) {
	Vector2 res = lhs;
	res /= rhs;
	return res;
}

inline Vector3 operator/(const Vector3& lhs, const Vector3& rhs) {
	Vector3 res = lhs;
	res /= rhs;
	return res;
}

inline Vector4 operator/(const Vector4& lhs, const Vector4& rhs) {
	Vector4 res = lhs;
	res /= rhs;
	return res;
}

inline Vector2 operator-(const Vector2& lhs, const Vector2& rhs) {
	Vector2 res = lhs;
	res -= rhs;
	return res;
}

inline Vector3 operator-(const Vector3& lhs, const Vector3& rhs) {
	Vector3 res = lhs;
	res -= rhs;
	return res;
}

inline Vector4 operator-(const Vector4& lhs, const Vector4& rhs) {
	Vector4 res = lhs;
	res -= rhs;
	return res;
}

inline Vector2 operator+(const Vector2& lhs, const Vector2& rhs) {
	Vector2 res = lhs;
	res += rhs;
	return res;
}

inline Vector3 operator+(const Vector3& lhs, const Vector3& rhs) {
	Vector3 res = lhs;
	res += rhs;
	return res;
}

inline Vector4 operator+(const Vector4& lhs, const Vector4& rhs) {
	Vector4 res = lhs;
	res += rhs;
	return res;
}

struct ColorHDR {
	float r, g, b, intensity;
};

struct BoundingBox {
	Vector3 mMin, mMax;

	inline void Start(const Vector3& v){
		mMax = mMin = v;
	}

	inline void Finalize(){
		if (this->mMax.x < this->mMin.x)
		{
			mMax.x = 0.f;
			mMin.x = 0.f;
		}
		if (this->mMax.y < this->mMin.y)
		{
			mMax.y = 0.f;
			mMin.y = 0.f;
		}
		if (this->mMax.z < this->mMin.z)
		{
			mMax.z = 0.f;
			mMin.z = 0.f;
		}
	}

	inline Vector3 PointOnBox(const Vector3& pt){
		Vector3 result = pt;
		Vector3 p = pt;
		if (p.x >= mMin.x)
			mMin.x = p.x;
		else
			result.x = mMin.x;
		mMin.y = this->mMin.y;
		if (p.y < mMin.y){
			result.y = mMin.y;
			p.y = mMin.y;
		}
		mMin.z = this->mMin.z;
		if (p.z < mMin.z){
			result.z = mMin.z;
			p.z = mMin.z;
		}
		mMax.x = this->mMax.x;
		if (mMin.x > mMax.x)
			result.x = mMax.x;
		mMax.y = this->mMax.y;
		if (p.y > mMax.y)
			result.y = mMax.y;
		mMax.z = this->mMax.z;
		if (p.z > mMax.z)
			result.z = mMax.z;
		return result;
	}

	inline bool LineIntersection(const Vector3& p1, const Vector3& p2, u32 cflags){
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
		float v21;
		float v22;
		float v23;
		float v24;
		float v25;
		float v26;
		float v27;
		float v28;
		float v29;
		float v30;
		bool result;

		v4 = p1.y;
		v5 = p2.y - v4;
		v6 = p1.x;
		v7 = p2.x - p1.x;
		v8 = p1.z;
		v9 = p2.z - v8;
		result = 1;
		if (!(cflags & 0xC)
			|| ((v10 = 1.0f / v7, !(cflags & 4))
				|| (v11 = (float)(mMin.x - v6) * v10,
					v12 = (float)(v11 * v9) + v8,
					v13 = (float)(v11 * v5) + v4,
					v13 < mMin.y)
				|| v13 > mMax.y
				|| v12 < mMin.z
				|| v12 > mMax.z)
			&& (!(cflags & 8)
				|| (v14 = (float)(mMax.x - v6) * v10,
					v15 = (float)(v14 * v9) + v8,
					v16 = (float)(v14 * v5) + v4,
					v16 < mMin.y)
				|| v16 > mMax.y
				|| v15 < mMin.z
				|| v15 > mMax.z))
		{
			if (!(cflags & 3)
				|| ((v17 = 1.0f / v5, !(cflags & 2))
					|| (v18 = (float)(mMin.y - v4) * v17,
						v19 = (float)(v18 * v9) + v8,
						v20 = (float)(v18 * v7) + v6,
						v20 < mMin.x)
					|| v20 > mMax.x
					|| v19 < mMin.z
					|| v19 > mMax.z)
				&& (!(cflags & 1)
					|| (v21 = (float)(mMax.y - v4) * v17,
						v22 = (float)(v21 * v9) + v8,
						v23 = (float)(v21 * v7) + v6,
						v23 < mMin.x)
					|| v23 > mMax.x
					|| v22 < mMin.z
					|| v22 > mMax.z))
			{
				if (!(cflags & 0x30)
					|| ((v24 = 1.0f / v9, !(cflags & 0x10))
						|| (v25 = (float)(mMin.z - v8) * v24,
							v26 = (float)(v25 * v5) + v4,
							v27 = (float)(v25 * v7) + v6,
							v27 < mMin.x)
						|| v27 > mMax.x
						|| v26 < mMin.y
						|| v26 > mMax.y)
					&& (!(cflags & 0x20)
						|| (v28 = (float)(mMax.z - v8) * v24, v29 = (float)(v28 * v7) + v6, v29 < mMin.x)
						|| v29 > mMax.x
						|| (v30 = (float)(v28 * v5) + v4, v30 < mMin.y)
						|| v30 > mMax.y))
				{
					result = 0;
				}
			}
		}
		return result;
	}

	//weird function. no clue why it just does this
	/*inline void AddPoint(const Vector3& v){
		mMin = v;
	}*/

};

struct Sphere {
	Vector3 mCenter;
	float mRadius;

	inline Sphere() : mRadius(0.f), mCenter() {}

};

struct Quaternion {

	inline Quaternion() { x = y = z = 0.f; w = 1.0f; }

	inline Quaternion(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	float x;
	float y;
	float z;
	float w;

	static MetaOpResult MetaOperation_SerializeAsync(void* pObj, MetaClassDescription* pDesc, MetaMemberDescription* pCtx, void* pUserData) {
		static_cast<MetaStream*>(pUserData)->Key("X");
		static_cast<MetaStream*>(pUserData)->serialize_float(&static_cast<Quaternion*>(pObj)->x);
		static_cast<MetaStream*>(pUserData)->Key("Y");
		static_cast<MetaStream*>(pUserData)->serialize_float(&static_cast<Quaternion*>(pObj)->y);
		static_cast<MetaStream*>(pUserData)->Key("Z");
		static_cast<MetaStream*>(pUserData)->serialize_float(&static_cast<Quaternion*>(pObj)->z);
		static_cast<MetaStream*>(pUserData)->Key("W");
		static_cast<MetaStream*>(pUserData)->serialize_float(&static_cast<Quaternion*>(pObj)->w);
		return eMetaOp_Succeed;
	}

	Quaternion(const Quaternion&) = default;

	static Quaternion kIdentity;
	static Quaternion kBackward;
	static Quaternion kUp;
	static Quaternion kDown;

	inline Quaternion Conjugate() const {
		return Quaternion(-x, -y, -z, w);
	}

	inline Quaternion(const Vector3& normalizedAxis, float radians){
		float sine = sinf(radians * 0.5f);
		float cosine = cosf(radians * 0.5f);
		x = normalizedAxis.x * sine;
		y = normalizedAxis.y * sine;
		z = normalizedAxis.z * sine;
		float mag = x * x + y * y + z * z + cosine * cosine;
		if (mag < 9.9999997e-21f)
			mag = 0.f;
		else
			mag = 1.0f / std::sqrtf(mag);
		x *= mag;
		y *= mag;
		z *= mag;
		w = mag < 9.9999997e-21f ? 1.0f : mag * cosine;
	}

	inline void SetEuler(float xrot, float yrot, float zrot){
		float xrot_sin = sinf(xrot * 0.5f);
		float xrot_cos = cosf(xrot * 0.5f);
		float yrot_sin = sinf(yrot * 0.5f);
		float yrot_cos = cosf(yrot * 0.5f);
		float zrot_sin = sinf(zrot * 0.5f);
		float zrot_cos = cosf(zrot * 0.5f);
		x = zrot_cos * yrot_cos * xrot_sin - zrot_sin * yrot_sin * xrot_cos;
		y = xrot_cos * yrot_sin * zrot_cos + xrot_sin * yrot_cos * zrot_sin;
		z = xrot_cos * yrot_cos * zrot_sin - xrot_sin * yrot_sin * zrot_cos;
		w = xrot_cos * yrot_cos * zrot_cos + xrot_sin * yrot_sin * zrot_sin;
		Normalize();
	}

	inline Quaternion& ExponentialMap(Vector3& v){
		float v4 = v.x;
		float v5 = (float)((float)(v4 * v4) + (float)(v.y * v.y)) + (float)(v.z * v.z);
		float v6 = std::sqrtf(v5);
		float v7 = cosf(v6 * 0.5);
		float v8;
		if (v6 >= 0.064996749f)
			v8 = sinf(v6 * 0.5) / v6;
		else
			v8 = 0.5 - (float)(v5 * 0.020833334f);
		x = v4 * v8;
		y = v8 * v.y;
		float v10 = v8 * v.z;
		w = v7;
		z = v10;
		return *this;
	}

	inline Quaternion& ExpMap(Vector3& v){
		return ExponentialMap(v);
	}

	inline Vector3 LogMap(){
		Vector3 v{};
		float mag = x * x + y * y + z * z;
		if(mag >= 0.000001f){
			mag = 2.0f * atan2f(mag, w);
			v.x = mag * x;
			v.y = mag * y;
			v.z = mag * z;
		}else{
			v.x = v.y = v.z = 0.f;
		}
		return v;
	}

	//pass in 1/deltatime to ensure units
	inline Vector3 CalcRotationVelocity(float invDt){
		Vector3 v = LogMap();
		v.x *= invDt;
		v.y *= invDt;
		v.z *= invDt;
		return v;
	}

	inline void SetDirectionXYZ(const Vector3& normalDir){
		float mag = normalDir.Magnitude();
		if (mag < 0.99999899f || mag > 1.000001f) {
			x = y = z = 0.f;
			w = 1.0f;
		}
		else {
			SetEuler(-asinf(normalDir.y), atan2f(normalDir.x, normalDir.z), 0.f);
		}
	}

	inline Quaternion(const Vector3& normalDir) : Quaternion() {
		SetDirectionXYZ(normalDir);
	}

	inline bool IsWellFormed(){
		float t = x * x + y + y + z * z + w * w - 1.0f;
		u32 t1 = COERCE(t, u32) & 0x7FFFFFFF;
		t = COERCE(t1, float);
		if (t <= 0.001f) {
			for(int i = 0; i < 4; i++){
				t1 = *((float*)this) + i;
				if(t1 <= 1.0f)
					continue;
				return false;
			}
		}
		else return false;
		return true;
	}

	inline void GetAxisAngle(Vector3& axisout,float& angleout){
		float cosw = 2.0f*cosf(w);
		if ((cosw <= -0.000001 || cosw >= 0.000001) && (cosw <= 6.2831845 || cosw >= 6.2831864))
		{
			axisout.x = x;
			axisout.y = y;
			axisout.z = z;
			axisout.Normalize();
		}
		else
		{
			axisout.x = 1.0f;
			axisout.y = axisout.z = 0.0f;
		}
	}

	inline void GetEuler(float& xrot, float& yrot, float& zrot){
		float sinr_cosp = 2 * (w * x + y * z);
		float cosr_cosp = 1 - 2 * (x * x + y * y);
		xrot = atan2f(sinr_cosp, cosr_cosp);
		float sinp = std::sqrt(1 + 2 * (w * y - x * z));
		float cosp = std::sqrt(1 - 2 * (w * y - x * z));
		yrot = 2.0f * atan2f(sinp, cosp) - 1.5707963f;
		double siny_cosp = 2 * (w * z + x * y);
		double cosy_cosp = 1 - 2 * (y * y + z * z);
		zrot = atan2f(siny_cosp, cosy_cosp);
	}

	inline void GetEuler(Vector3& out){
		GetEuler(out.x, out.y, out.z);
	}

	inline bool IsNormalized(){
		float v1 = std::sqrtf((float)((float)((float)(this->x * this->x) + (float)(this->y * this->y)) + (float)(this->z * this->z)) + (float)(this->w * this->w));
		return v1 >= 0.94999999f && v1 <= 1.05f;
	}

	inline void Normalize(){
		float mag = (Vector3{ x, y, z }).Magnitude();
		x *= mag;
		y *= mag;
		z *= mag;
	}

	inline Quaternion operator*(const Quaternion& rhs) const {
		Quaternion res{};
		res.x = (x * rhs.w) + (rhs.x * w) + (y * rhs.z) - (z * rhs.y);
		res.y = (y * rhs.w) + (rhs.y * w) + (z * rhs.x) - (rhs.z * x);
		res.z = (z * rhs.w) + (rhs.z * w) + (x * rhs.y) - (y * rhs.x);
		res.w = (w * rhs.w) - (rhs.x * w) - (rhs.y * y) - (rhs.z * z);
		return res;
	}

	inline Quaternion& operator*=(const Quaternion& rhs) {
		*this = operator*(rhs);
		return *this;
	}

	inline static Quaternion NLerp(Quaternion* q0, Quaternion* q, float t)
	{
		float v4; // xmm5_4@1
		float v6; // xmm6_4@1
		float v7; // xmm7_4@1
		float v8; // xmm9_4@1
		float v9; // xmm10_4@1
		float v10; // xmm11_4@1
		Quaternion val{}; // [sp+20h] [bp-78h]@1

		v4 = q->y;
		v6 = q->z;
		v7 = q->w;
		v8 = q0->y;
		v9 = q0->z;
		v10 = q0->w;
		val.x = (float)((float)(q->x - (float)(q0->x * 1.0f)) * t) + (float)(q0->x * 1.0f);
		val.y = (float)((float)(v4 - (float)(v8 * 1.0f)) * t) + (float)(v8 * 1.0f);
		val.z = (float)((float)(v6 - (float)(v9 * 1.0f)) * t) + (float)(v9 * 1.0f);
		val.w = (float)((float)(v7 - (float)(v10 * 1.0f)) * t) + (float)(v10 * 1.0f);
		val.Normalize();
		return val;
	}

	inline static Quaternion Slerp(Quaternion q, Quaternion q0, float t){
		Quaternion res{};
		Quaternion* v4; // rbx@1
		Quaternion* v5; // rdi@1
		float v6; // xmm9_4@1
		float v7; // xmm10_4@1
		float v8; // xmm11_4@1
		float v9; // xmm12_4@1
		float v10; // xmm14_4@1
		float v11; // xmm2_4@1
		float v12; // xmm8_4@1
		float v13; // xmm7_4@1
		float v14; // xmm6_4@2
		float v15; // xmm13_4@2

		v4 = &q0;
		v5 = &res;
		v6 = q.y;
		v7 = t;
		v8 = q0.x;
		v9 = q.z;
		v10 = q.x;
		v11 = (float)((float)((float)(v6 * q0.y) + (float)(q.x * q0.x)) + (float)(v9 * q0.z)) + (float)(q.w * q0.w);
		v12 = q.w;
		v13 = 1.0f - t;
		if (v11 < 0.999999f)
		{
			v14 = acosf(v11);
			v15 = 1.0f / sinf(v14);
			v13 = sinf(v13 * v14) * v15;
			v7 = sinf(v14 * t) * v15;
		}
		v5->x = (float)(v8 * v13) + (float)(v7 * v10);
		v5->y = (float)(v6 * v7) + (float)(v13 * v4->y);
		v5->z = (float)(v13 * v4->z) + (float)(v9 * v7);
		v5->w = (float)(v13 * v4->w) + (float)(v12 * v7);
		return res;
	}

	inline static Quaternion BetweenVector3(const Vector3& va, const Vector3& vb){
		Quaternion res{};
		float v3;
		Quaternion* v4;
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
		Quaternion val{};

		v3 = vb.y;
		v4 = &res;
		v5 = vb.x;
		v6 = vb.z;
		v7 = va.y;
		v8 = va.z;
		v9 = va.x;
		v10 = (float)((float)(vb.x * va.x) + (float)(vb.y * v7)) + (float)(v6 * v8);
		if (v10 < 1.0f)
		{
			if (v10 >= -0.99999899f)
			{
				v16 = std::sqrtf((float)(v10 + v10) + 2.0f);
				val.x = (float)((float)(v6 * v7) - (float)(v8 * v3)) * (float)(1.0f / v16);
				val.y = (float)((float)(v8 * v5) - (float)(v6 * v9)) * (float)(1.0f / v16);
				val.z = (float)((float)(v9 * v3) - (float)(v5 * v7)) * (float)(1.0f / v16);
				val.w = v16 * 0.5f;
				val.Normalize();
				*v4 = val;
			}
			else
			{
				v11 = (float)(v7 * Vector3::Left.z) - (float)(v8 * Vector3::Left.y);
				v12 = (float)(v8 * Vector3::Left.x) - (float)(v9 * Vector3::Left.z);
				v13 = (float)(v9 * Vector3::Left.y) - (float)(v7 * Vector3::Left.x);
				if ((float)((float)((float)(v12 * v12) + (float)(v11 * v11)) + (float)(v13 * v13)) < 0.000001f)
				{
					v11 = (float)(v7 * Vector3::Right.z) - (float)(v8 * Vector3::Right.y);
					v13 = (float)(v9 * Vector3::Right.y) - (float)(v7 * Vector3::Right.x);
					v12 = (float)(v8 * Vector3::Right.x) - (float)(v9 * Vector3::Right.z);
				}
				v14 = (float)((float)(v12 * v12) + (float)(v11 * v11)) + (float)(v13 * v13);
				if (v14 < 9.9999997e-21f)
					v15 = 1.0f;
				else
					v15 = std::sqrtf(v14);
				res.w = -4.37114e-08f;
				res.x = (float)(v11 * (float)(1.0f / v15)) * 1.0f;
				res.y = (float)(v12 * (float)(1.0f / v15)) * 1.0f;
				res.z = (float)(v13 * (float)(1.0f / v15)) * 1.0f;
				res.Normalize();
			}
		}
		else
		{
			res = kIdentity;
		}
		return res;
	}

};

inline Vector3 operator*(const Vector3& vec, const Quaternion& quat){
	Vector3 result{};
	float v3;
	Vector3* v4; 
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
	const Vector3* v = &vec;
	const Quaternion* q = &quat;
	v3 = q->z;
	v4 = &result;
	v5 = q->w;
	v6 = q->y;
	v7 = (float)(v->x * v3) - (float)(v->z * q->x);
	v8 = (float)((float)(v->z * v6) - (float)(v->y * v3)) + (float)((float)(v->z * v6) - (float)(v->y * v3));
	v9 = v7 + v7;
	v10 = (float)((float)(v->y * q->x) - (float)(v->x * v6)) + (float)((float)(v->y * q->x) - (float)(v->x * v6));
	v11 = (float)((float)(v->z * v6) - (float)(v->y * v3)) + (float)((float)(v->z * v6) - (float)(v->y * v3));
	v12 = v10 * v6;
	v13 = v10 * q->x;
	v14 = (float)(v10 * v5) + v->z;
	v15 = (float)(v12 - (float)(v9 * v3)) + (float)((float)(v8 * v5) + v->x);
	v16 = v9;
	v17 = (float)(v9 * q->x) - (float)(v8 * v6);
	result.x = v15;
	v18 = (float)((float)(v11 * v3) - v13) + (float)((float)(v16 * v5) + v->y);
	result.z = v17 + v14;
	result.y = v18;
	return result;
}

struct Transform {

	Quaternion mRot;
	Vector3 mTrans;
	float mPaddingAlign;//to align it since vec3 is 12 bytes and rot is 16, 16 + 16 = 32, a nice one unlike 28

	inline Transform()  : mPaddingAlign(0.f), mRot{}, mTrans{} {}

	inline Transform(Quaternion rotation, Vector3 translation) : mRot(rotation), mTrans(translation), mPaddingAlign(0.f) {}

	inline Transform operator*(const Transform& rhs) const {
		Transform res{};
		res.mRot = mRot * rhs.mRot;
		res.mTrans = mTrans + (rhs.mTrans * mRot);
		return res;
	}

	inline Transform operator/(const Transform& rhs) const {
		Transform res{};
		Quaternion tmpQ{ -rhs.mRot.x,-rhs.mRot.y,-rhs.mRot.z, rhs.mRot.w };
		res.mRot = tmpQ * mRot;
		res.mTrans = (mTrans - rhs.mTrans) * tmpQ;
		return res;
	}

	inline Transform& operator/=(const Transform& rhs) {
		Transform op = operator/(rhs);
		*this = op;
		return *this;
	}

	inline Transform operator~() const {
		Transform ret{};
		ret.mRot = Quaternion{ -mRot.x, -mRot.y,-mRot.z, mRot.w };
		Vector3 tmpV = { -mTrans.x, -mTrans.y, -mTrans.z };
		ret.mTrans = tmpV * ret.mRot;
		return ret;
	}

};

struct LocationInfo {
	String mAttachedAgent;
	Symbol mAttachedNode;
	Transform mInitialLocalTransform;
};

struct ResourceGroupInfo {
	Color mColor;
	long mPriority;
};

struct Polar
{
	float mR;
	float mTheta;
	float mPhi;
};

struct  __declspec(align(8)) Selectable
{
	BoundingBox mBoundingBox;
	int mSelectionOrder;
	bool mbGameSelectable;
};

namespace NavCam {
	enum Mode : i32 {
		eNone = 1,
		eLookAt = 2,
		eOrbit = 3,
		eAnimation_Track = 4,
		eAnimation_Time = 5,
		eAnimation_Pos_ProceduralLookAt = 6,
		eScenePosition = 7,
		eDynamicConversationCamera = 8,
	};

	struct EnumMode : EnumBase {
		Mode mVal;
	};
}

/*
Matrix4.h
Written by Matthew Fisher

a 4x4 transformation matrix structure.
*/

class alignas(16) Matrix4
{
public:
	//
	// Initalization
	//
	Matrix4();
	Matrix4(const Matrix4& M);
	Matrix4(const Vector3& V0, const Vector3& V1, const Vector3& V2);

#ifdef USE_D3D
	Matrix4(const D3DXMATRIX& M);
#endif

	//
	// Assignment
	//
	Matrix4& operator = (const Matrix4& M);

	//
	// Math properties
	//
	float Determinant() const;
	Matrix4 Transpose() const;
	Matrix4 Inverse() const;

	//
	// Vector transforms
	//
	inline Vector3 TransformPoint(const Vector3& point) const
	{
		float w = point.x * _Entries[0][3] + point.y * _Entries[1][3] + point.z * _Entries[2][3] + _Entries[3][3];
		if (w)
		{
			const float invW = 1.0f / w;
			return Vector3((point.x * _Entries[0][0] + point.y * _Entries[1][0] + point.z * _Entries[2][0] + _Entries[3][0]) * invW,
				(point.x * _Entries[0][1] + point.y * _Entries[1][1] + point.z * _Entries[2][1] + _Entries[3][1]) * invW,
				(point.x * _Entries[0][2] + point.y * _Entries[1][2] + point.z * _Entries[2][2] + _Entries[3][2]) * invW);
		}
		else
		{
			return Vector3();
		}
	}

	inline Vector3 TransformNormal(const Vector3& normal) const
	{
		return Vector3(normal.x * _Entries[0][0] + normal.y * _Entries[1][0] + normal.z * _Entries[2][0],
			normal.x * _Entries[0][1] + normal.y * _Entries[1][1] + normal.z * _Entries[2][1],
			normal.x * _Entries[0][2] + normal.y * _Entries[1][2] + normal.z * _Entries[2][2]);
	}

	//
	// Accessors
	//
	inline float* operator [] (int Row)
	{
		return _Entries[Row];
	}
	inline const float* operator [] (int Row) const
	{
		return _Entries[Row];
	}
	inline void SetColumn(UINT Column, const Vector4& Values)
	{
		_Entries[0][Column] = Values.x;
		_Entries[1][Column] = Values.y;
		_Entries[2][Column] = Values.z;
		_Entries[3][Column] = Values.w;
	}
	inline void SetRow(UINT Row, const Vector4& Values)
	{
		_Entries[Row][0] = Values.x;
		_Entries[Row][1] = Values.y;
		_Entries[Row][2] = Values.z;
		_Entries[Row][3] = Values.w;
	}
	inline Vector4 GetColumn(UINT Column)
	{
		Vector4 Result;
		Result.x = _Entries[0][Column];
		Result.y = _Entries[1][Column];
		Result.z = _Entries[2][Column];
		Result.w = _Entries[3][Column];
		return Result;
	}
	inline Vector4 GetRow(UINT Row)
	{
		Vector4 Result;
		Result.x = _Entries[Row][0];
		Result.y = _Entries[Row][1];
		Result.z = _Entries[Row][2];
		Result.w = _Entries[Row][3];
		return Result;
	}

	//
	// Transformation matrices
	//
	static Matrix4 Identity();
	static Matrix4 Scaling(const Vector3& ScaleFactors);
	static Matrix4 Scaling(float ScaleFactor)
	{
		return Scaling(Vector3(ScaleFactor, ScaleFactor, ScaleFactor));
	}
	static Matrix4 Translation(const Vector3& Pos);
	static Matrix4 Rotation(const Vector3& Axis, float Angle, const Vector3& Center);
	static Matrix4 Rotation(const Vector3& Axis, float Angle);
	static Matrix4 Rotation(float Yaw, float Pitch, float Roll);
	static Matrix4 Rotation(const Vector3& Basis1, const Vector3& Basis2, const Vector3& Basis3);
	static Matrix4 RotationX(float Theta);
	static Matrix4 RotationY(float Theta);
	static Matrix4 RotationZ(float Theta);
	static Matrix4 Camera(const Vector3& Eye, const Vector3& Look, const Vector3& Up, const Vector3& Right);
	static Matrix4 LookAt(const Vector3& Eye, const Vector3& At, const Vector3& Up);
	static Matrix4 Orthogonal(float Width, float Height, float ZNear, float ZFar);
	static Matrix4 OrthogonalOffCenter(float left, float right, float bottom, float top, float ZNear, float ZFar);
	static Matrix4 Perspective(float Width, float Height, float ZNear, float ZFar);
	static Matrix4 PerspectiveFov(float FOV, float Aspect, float ZNear, float ZFar);
	static Matrix4 PerspectiveMultiFov(float FovX, float FovY, float ZNear, float ZFar);
	static Matrix4 Face(const Vector3& V0, const Vector3& V1);
	static Matrix4 Viewport(float Width, float Height);
	static Matrix4 ChangeOfBasis(const Vector3& Source0, const Vector3& Source1, const Vector3& Source2, const Vector3& SourceOrigin,
		const Vector3& Target0, const Vector3& Target1, const Vector3& Target2, const Vector3& TargetOrigin);
	static float CompareMatrices(const Matrix4& Left, const Matrix4& Right);

	float _Entries[4][4];
};

Matrix4 operator * (const Matrix4& Left, const Matrix4& Right);
Matrix4 operator * (const Matrix4& Left, float& Right);
Matrix4 operator * (float& Left, const Matrix4& Right);
Matrix4 operator + (const Matrix4& Left, const Matrix4& Right);
Matrix4 operator - (const Matrix4& Left, const Matrix4& Right);

inline Vector4 operator * (const Vector4& Right, const Matrix4& Left)
{
	return Vector4(Right.x * Left[0][0] + Right.y * Left[1][0] + Right.z * Left[2][0] + Right.w * Left[3][0],
		Right.x * Left[0][1] + Right.y * Left[1][1] + Right.z * Left[2][1] + Right.w * Left[3][1],
		Right.x * Left[0][2] + Right.y * Left[1][2] + Right.z * Left[2][2] + Right.w * Left[3][2],
		Right.x * Left[0][3] + Right.y * Left[1][3] + Right.z * Left[2][3] + Right.w * Left[3][3]);
}


struct Plane {

	Vector4 mPlane;

	inline void TransformBy(const Matrix4& lhs) {
		mPlane = mPlane * lhs;
	}

};

enum FRUSTUM_PLANE_INDEX {
	FRUSTUMPLANE_ZNEAR = 0,
	FRUSTUMPLANE_LEFT = 1,
	FRUSTUMPLANE_RIGHT = 2,
	FRUSTUMPLANE_DOWN = 3,
	FRUSTUMPLANE_UP = 4,
	FRUSTUMPLANE_ZFAR = 5,
	FRUSTUMPLANE_COUNT = 6
};

struct Frustum {

	Plane mPlane[FRUSTUMPLANE_COUNT];
	i32 mPlaneCount = 0;

	inline Frustum() : mPlaneCount(6) {}

	inline void TransformBy(const Matrix4& lhs){
		for (i32 i = 0; i < mPlaneCount; i++)
			mPlane[i].TransformBy(lhs);
	}

	//straddles
	inline bool Visible(const BoundingBox& box, bool* pbStraddlesNearPlane){
		Vector3 c[8]{};
		//here we expand bounding box to 8 vertices of a cube.
		float minx = box.mMin.x;
		float minz = box.mMin.y;
		float miny = box.mMin.z;
		float maxx = box.mMax.x;
		float maxz = box.mMax.y;
		float maxy = box.mMax.z;
		c[0] = box.mMin;
		c[1].x = minx;
		c[1].y = miny;
		c[1].z = maxz;
		c[2].x = minx;
		c[2].y = maxy;
		c[2].z = maxz;
		c[3].x = minx;
		c[3].y = maxy;
		c[3].z = minz;
		c[4].x = maxx;
		c[4].y = miny;
		c[4].z = minz;
		c[5].x = maxx;
		c[5].y = miny;
		c[5].z = maxz;
		c[6].x = maxx;
		c[6].y = maxy;
		c[6].z = maxz;
		c[7].x = maxx;
		c[7].y = maxy;
		c[7].z = minz;
		if(pbStraddlesNearPlane){
			if(InternalVisibleTest(c)){
				int i = 0;
				while((Vector3::Dot(Vector3(mPlane[0].mPlane), c[i]) + mPlane[0].mPlane.w) > 0.0f){
					if(++i >= 8){
						*pbStraddlesNearPlane = false;
						return true;
					}
				}
				*pbStraddlesNearPlane = true;
				return true;
			}else{
				*pbStraddlesNearPlane = false;
				return false;
			}
		}
		else return InternalVisibleTest(c);
	}

	inline bool Visible(const BoundingBox& box){
		return Visible(box, 0);
	}

	inline bool InternalVisibleTest(const Vector3 pBoxCorners[8]) const {
		for (i32 i = 0; i < mPlaneCount; i++) {
			for(int j = 0; j < 8; j++){
				if (!((Vector3::Dot(Vector3(mPlane[i].mPlane), pBoxCorners[j]) + mPlane[i].mPlane.w) > 0.0f))
					return false;
			}
		}
		return true;
	}

};

inline Matrix4 MatrixRotation(const Quaternion& q){
	Matrix4 ret{};
	ret._Entries[0][3] = 0.f;
	ret._Entries[1][3] = 0.f;
	ret._Entries[2][3] = 0.f;
	ret._Entries[3][3] = 1.f;
	float _2_times_qz_sq = q.z * (float)(q.z + q.z);
	float v8 = q.w * (q.z+q.z);
	float _2_qz = q.z + q.z;
	float _2_qy = q.y + q.y;
	float v11 = q.w * (q.x + q.x);
	float v12 = q.y * (q.z + q.z);
	float v13 = q.w * (q.y + q.y);
	float v14 = q.y * (q.y + q.y);
	float v15 = 1.f - (q.x * (q.x + q.x));

	ret._Entries[0][0] = (1.f - v14) - _2_times_qz_sq;
	ret._Entries[0][1] = (q.x* _2_qy) + v8;
	ret._Entries[0][2] = (q.x * _2_qz) - v13;

	ret._Entries[1][0] = (q.x * _2_qy) - v8;
	ret._Entries[1][1] = v15 - _2_times_qz_sq;
	ret._Entries[1][2] = v12 + v11;

	ret._Entries[2][0] =  (q.x * _2_qz) + v13;
	ret._Entries[2][1] = v12 - v11;
	ret._Entries[2][2] = v15 - v14;

	ret._Entries[3][0] = 0.f;
	ret._Entries[3][1] = 0.f;
	ret._Entries[3][2] = 0.f;

	return ret;
}

inline Matrix4 MatrixTranslation(const Vector3& Translation){
	Matrix4 ret{};
	ret._Entries[3][0] = Translation.x;
	ret._Entries[3][1] = Translation.y;
	ret._Entries[3][2] = Translation.z;
	ret._Entries[3][3] = 1.0f;
	return ret;
}

inline Matrix4 MatrixTransformation(const Quaternion& rot, const Vector3& Translation) {
	Matrix4 ret = MatrixRotation(rot);
	ret._Entries[3][0] += Translation.x;
	ret._Entries[3][1] += Translation.y;
	ret._Entries[3][2] += Translation.z;
	return ret;
}

inline Matrix4 MatrixTransformation(const Vector3 scale, const Quaternion& rot, const Vector3& Translation) {
	Matrix4 ret = MatrixTransformation(rot, Translation);
	return ret * Matrix4::Scaling(scale);
}

inline Matrix4 MatrixScaling(float ScaleX, float ScaleY, float ScaleZ){
	Matrix4 ret = Matrix4::Identity();
	ret._Entries[0][0] = ScaleX;
	ret._Entries[1][1] = ScaleY;
	ret._Entries[2][2] = ScaleZ;
	return ret;
}

inline Matrix4 MatrixRotationYawPitchRollDegrees(float yaw, float pitch, float roll){
	return Matrix4::RotationX(roll) * Matrix4::RotationY(pitch) * Matrix4::RotationZ(yaw);
}

inline void SHProjectDirectionalLight(/*out*/Color shParam[9], Vector3 direction, Color color, float wrapAround){
	float g; 
	float b; 
	float a; 
	float wra;
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
	float v21;
	float v22;
	float v23;
	float v24;
	float v25;
	float v26;
	float v27;
	float v28;
	float v29;
	float v30;
	float v31;
	float v32;
	float v33;
	float v34;
	float v35;
	float v36;
	float v37;
	float v38;
	float v39;
	float v40;
	float v41;

	g = color.g;
	b = color.b;
	a = color.a;
	wra = wrapAround;
	shParam->r = (float)((float)((float)(color.r * wrapAround) * 0.11764705f) + (float)(color.r * 0.2352941f))
		+ shParam->r;
	shParam->g = (float)((float)((float)(g * wrapAround) * 0.11764705f) + (float)(g * 0.2352941f)) + shParam->g;
	shParam->b = (float)((float)((float)(b * wrapAround) * 0.11764705f) + (float)(b * 0.2352941f)) + shParam->b;
	shParam->a = (float)((float)((float)(a * wrapAround) * 0.11764705f) + (float)(a * 0.2352941f)) + shParam->a;
	v8 = wrapAround * 0.16666669f;
	v9 = direction.y;
	v10 = color.g;
	v11 = (float)((float)(color.b * (float)(0.66666669f - (float)(wrapAround * 0.16666669f))) * 0.70588231f) * v9;
	v12 = (float)(color.a * (float)(0.66666669f - v8)) * 0.70588231f;
	shParam[1].r = (float)((float)((float)((float)(0.66666669f - v8) * color.r) * 0.70588231f) * v9) + shParam[1].r;
	shParam[1].g = (float)((float)((float)((float)(0.66666669f - v8) * v10) * 0.70588231f) * v9) + shParam[1].g;
	shParam[1].b = v11 + shParam[1].b;
	shParam[1].a = (float)(v12 * v9) + shParam[1].a;
	v13 = direction.z;
	v14 = color.b;
	v15 = color.a;
	v16 = color.g * (float)(0.66666669f - v8);
	shParam[2].r = (float)((float)((float)((float)(0.66666669f - v8) * color.r) * 0.70588231f) * v13) + shParam[2].r;
	shParam[2].g = (float)((float)(v16 * 0.70588231f) * v13) + shParam[2].g;
	shParam[2].b = (float)((float)((float)((float)(0.66666669f - v8) * v14) * 0.70588231f) * v13) + shParam[2].b;
	shParam[2].a = (float)((float)((float)(v15 * (float)(0.66666669f - v8)) * 0.70588231f) * v13) + shParam[2].a;
	v17 = direction.x;
	v18 = color.a;
	v19 = (float)((float)((float)(0.66666669f - v8) * color.g) * 0.70588231f) * direction.x;
	v20 = (float)((float)(color.b * (float)(0.66666669f - v8)) * 0.70588231f) * direction.x;
	shParam[3].r = (float)((float)((float)((float)(0.66666669f - v8) * color.r) * 0.70588231f) * direction.x)
		+ shParam[3].r;
	shParam[3].g = v19 + shParam[3].g;
	shParam[3].b = v20 + shParam[3].b;
	shParam[3].a = (float)((float)((float)(0.47058821f - (float)(wra * 0.11764707f)) * v18) * v17) + shParam[3].a;
	v21 = direction.x * direction.y;
	v22 = (float)((float)(color.b * (float)(1.0f - wra)) * 0.88235295f) * v21;
	v23 = (float)((float)(color.g * (float)(1.0f - wra)) * 0.88235295f) * v21;
	v24 = (float)(color.a * (float)(1.0f - wra)) * 0.88235295f;
	shParam[4].r = (float)((float)((float)(color.r * (float)(1.0f - wra)) * 0.88235295f) * v21) + shParam[4].r;
	shParam[4].g = v23 + shParam[4].g;
	shParam[4].b = v22 + shParam[4].b;
	shParam[4].a = (float)(v24 * v21) + shParam[4].a;
	v25 = color.b;
	v26 = direction.y * direction.z;
	v27 = color.a * (float)(1.0f - wra);
	v28 = (float)(color.g * (float)(1.0f - wra)) * 0.88235295f;
	shParam[5].r = (float)((float)((float)(color.r * (float)(1.0f - wra)) * 0.88235295f) * v26) + shParam[5].r;
	shParam[5].g = (float)(v28 * v26) + shParam[5].g;
	shParam[5].b = (float)((float)((float)(v25 * (float)(1.0f - wra)) * 0.88235295f) * v26) + shParam[5].b;
	shParam[5].a = (float)((float)(v27 * 0.88235295f) * v26) + shParam[5].a;
	v29 = direction.z;
	v30 = color.g * (float)(1.0f - wra);
	v31 = color.b * (float)(1.0f - wra);
	v32 = color.a * (float)(1.0f - wra);
	v33 = (float)(direction.z * 0.22058824f) * v29;
	shParam[6].r = (float)((float)(v33 * (float)(color.r * (float)(1.0f - wra)))
		- (float)((float)(color.r * (float)(1.0f - wra)) * 0.073529415f))
		+ shParam[6].r;
	shParam[6].g = (float)((float)((float)((float)(v29 * 0.22058824f) * v29) * v30) - (float)(v30 * 0.073529415f))
		+ shParam[6].g;
	shParam[6].b = (float)((float)(v33 * v31) - (float)(v31 * 0.073529415f)) + shParam[6].b;
	shParam[6].a = (float)((float)((float)((float)(v29 * 0.22058824f) * v29) * v32) - (float)(v32 * 0.073529415f))
		+ shParam[6].a;
	v34 = direction.z * direction.x;
	v35 = (float)(color.g * (float)(1.0f - wra)) * 0.88235295f;
	v36 = (float)(color.b * (float)(1.0f - wra)) * 0.88235295f;
	v37 = (float)(color.a * (float)(1.0f - wra)) * 0.88235295f;
	shParam[7].r = (float)((float)((float)(color.r * (float)(1.0f - wra)) * 0.88235295f) * v34) + shParam[7].r;
	shParam[7].g = (float)(v35 * v34) + shParam[7].g;
	shParam[7].b = (float)(v36 * v34) + shParam[7].b;
	shParam[7].a = (float)(v37 * v34) + shParam[7].a;
	v38 = color.g * (float)(1.0f - wra);
	v39 = color.b * (float)(1.0f - wra);
	v40 = color.a * (float)(1.0f - wra);
	v41 = (float)(direction.x * direction.x) - (float)(direction.y * direction.y);
	shParam[8].r = (float)((float)((float)(color.r * (float)(1.0f - wra)) * 0.22058824f) * v41) + shParam[8].r;
	shParam[8].g = (float)((float)(v38 * 0.22058824f) * v41) + shParam[8].g;
	shParam[8].b = (float)((float)(v39 * 0.22058824f) * v41) + shParam[8].b;
	shParam[8].a = (float)((float)(v40 * 0.22058824f) * v41) + shParam[8].a;
}

inline void SHProjectDirection(float shParam[8], Vector3 direction, const float v, float wrapAround)
{
	float v4;
	float v5;
	float v6;

	v4 = v;
	v5 = (float)(1.0f - wrapAround) * v;
	*shParam = (float)((float)((float)(wrapAround * 0.5f) + 1.0f) * (float)(v * 0.2352941f)) + *shParam;
	v6 = (float)(0.66666669f - (float)(wrapAround * 0.16666669f)) * (float)(v * 0.70588231f);
	shParam[1] = (float)(v6 * direction.y) + shParam[1];
	shParam[2] = (float)(v6 * direction.z) + shParam[2];
	shParam[3] = (float)(v6 * direction.x) + shParam[3];
	shParam[4] = (float)((float)(direction.x * direction.y) * (float)(v5 * 0.88235295f)) + shParam[4];
	shParam[5] = (float)((float)(direction.z * direction.y) * (float)(v5 * 0.88235295f)) + shParam[5];
	shParam[6] = (float)((float)((float)((float)(direction.z * 0.22058824f) * direction.z) * v5)
		- (float)((float)((float)(1.0f - wrapAround) * v4) * 0.073529415f))
		+ shParam[6];
	shParam[7] = (float)((float)(direction.x * direction.z) * (float)(v5 * 0.88235295f)) + shParam[7];
	shParam[8] = (float)((float)((float)(direction.x * direction.x) - (float)(direction.y * direction.y))
		* (float)(v5 * 0.22058824f))
		+ shParam[8];
}

#endif