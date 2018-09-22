#ifndef SSEVEC
#define SSEVEC

#include <sseBase.h>

BEGIN_SSE_MATH_NAME

class FVec4 {
public:
	__m128 v4;

	FVec4() { v4 = _mm_setzero_ps(); };
	FVec4(const __m128 _A) { v4 = _A; };
	FVec4(const float f) { v4=_mm_set1_ps(f); };
	FVec4(const float x, const float y, const float z, const float w) { v4.m128_f32[0] = x; v4.m128_f32[1] = y; v4.m128_f32[2] = z; v4.m128_f32[3] = w; }
	FVec4& operator =(const FVec4 &_A) { v4 = _A.v4; return *this; }

	/* Arithmetic Operators */
	friend FVec4 operator +(const FVec4 &_A, const FVec4 &_B) { return _A.v4 + _B.v4; }
	friend FVec4 operator -(const FVec4 &_A, const FVec4 &_B) { return _A.v4 - _B.v4; }
	friend FVec4 operator *(const FVec4 &_A, const FVec4 &_B) { return _A.v4 * _B.v4; }
	friend FVec4 operator *(const FVec4 &_A, const float  _B) { return _A.v4 * _B; }
	friend FVec4 operator /(const FVec4 &_A, const FVec4 &_B) { return _A.v4 / _B.v4; }
	friend FVec4 operator /(const FVec4 &_A, const float _B)  { return _A.v4 / _B;    }

	FVec4& operator +=(const FVec4 &_A) { v4 = v4 + _A.v4; return *this; }
	FVec4& operator -=(const FVec4 &_A) { v4 = v4 - _A.v4; return *this; }
	FVec4& operator *=(const FVec4 &_A) { v4 = v4 * _A.v4; return *this; }
	FVec4& operator *=(const float  _A) { v4 = v4 * _A;    return *this; }
	FVec4& operator /=(const FVec4 &_A) { v4 = v4 / _A.v4; return *this; }
	FVec4& operator /=(const float  _A) { v4 = v4 / _A;    return *this; }
	float& operator[](const int id) { return v4.m128_f32[id]; }

	float& x() { return v4.m128_f32[0]; }
	float& y() { return v4.m128_f32[1]; }
	float& z() { return v4.m128_f32[2]; }
	float& w() { return v4.m128_f32[3]; }
	const float cValue(const int id) const {return v4.m128_f32[id];}

	void Get(float &x, float &y, float &z, float &w) 
	{ 
		x = v4.m128_f32[0]; 
		y = v4.m128_f32[1];
		z = v4.m128_f32[2];
		w = v4.m128_f32[3];
	}

	float Dot(const FVec4 &_A) 
	{
		__m128 a;
		a = _mm_mul_ps(_A.v4, v4);
		return sum4(a);
	}

	float Length() 
	{ 
		__m128 a = _mm_mul_ps(v4,v4);
		float fa=sum4(a);
		a = _mm_sqrt_ss(_mm_set_ss(fa));
		return a.m128_f32[0];
	}
	float Length2()
	{
		__m128 a = _mm_mul_ps(v4, v4);
		return sum4(a);
	}
};

class FVec3 : public FVec4
{
public:
	FVec3() { v4 = _mm_setzero_ps(); };
	FVec3(const __m128 _A) { v4 = _A; };
	FVec3(const FVec4 & _A) { v4 = _A.v4; };
	FVec3(const FVec3 & _A) { v4 = _A.v4; };
	FVec3(const float x, const float y, const float z, const float w = 0) { v4.m128_f32[0] = x; v4.m128_f32[1] = y; v4.m128_f32[2] = z; v4.m128_f32[3] = w; }
	FVec3(const float a) { v4.m128_f32[0] = a; v4.m128_f32[1] = a; v4.m128_f32[2] = a; v4.m128_f32[3] = 0; }

	FVec3& operator =(const FVec3 &_A) { v4 = _A.v4; return *this; }
	float & operator[](const int id) { return v4.m128_f32[id]; }

	u_int Dim() const { return 3; };
	
	FVec3 Cross(const FVec3 & _B) const { return FVec3( sse_cross3d( v4, _B.v4 ) ); }
	void SetPoint(const float x, const float y, const float z)  { v4.m128_f32[0] = x; v4.m128_f32[1] = y; v4.m128_f32[2] = z; v4.m128_f32[3] = 1; }
	void SetVector(const float x, const float y, const float z) { v4.m128_f32[0] = x; v4.m128_f32[1] = y; v4.m128_f32[2] = z; v4.m128_f32[3] = 0; }
	float Dot(const FVec3 &_A) const
	{
		__m128 a;
		a = _mm_mul_ps(_A.v4, v4);
		return sum3(a);
	}
	float Length() const
	{
		__m128 a = _mm_mul_ps(v4, v4);
		float fa = sum3(a);
		a = _mm_sqrt_ss(_mm_set_ss(fa));
		return a.m128_f32[0];
	}
	float Length2() const
	{
		__m128 a = _mm_mul_ps(v4, v4);
		return sum3(a);
	}
	void Normalize() 
	{
		__m128 l = _mm_set1_ps(Length());
		v4 /= l;
	}
	FVec3 Normalized() const
	{
		FVec3 a(*this);
		__m128 l = _mm_set1_ps(a.Length());
		a.v4 /= l;
		return a;
	}
	void Get(float &x, float &y, float &z)
	{
		x = v4.m128_f32[0];
		y = v4.m128_f32[1];
		z = v4.m128_f32[2];
	}

	/* Arithmetic Operators */
	friend FVec3 operator +(const FVec3 &_A, const FVec3 &_B) { return _A.v4 + _B.v4; }
	friend FVec3 operator -(const FVec3 &_A, const FVec3 &_B) { return _A.v4 - _B.v4; }
	friend FVec3 operator *(const FVec3 &_A, const FVec3 &_B) { return _A.v4 * _B.v4; }
	friend FVec3 operator *(const FVec3 &_A, const float  _B) { return _A.v4 * _B; }
	friend FVec3 operator /(const FVec3 &_A, const FVec3 &_B) { return _A.v4 / _B.v4; }
	friend FVec3 operator /(const FVec3 &_A, const float _B) { return _A.v4 / _B; }
};

INLINE float distance(FVec3 & a, FVec3 &b){ return (a - b).Length(); }
INLINE float distance2(FVec3 & a, FVec3 &b) { return (a - b).Length2(); }

class FVec2 : public FVec4
{
public:
	FVec2() { v4 = _mm_setzero_ps(); };
	FVec2(const __m128 _A) { v4 = _A; };
	FVec2(const FVec4 & _A) { v4 = _A.v4; v4.m128_f32[3] = 0; };
	FVec2(const FVec2 & _A) { v4 = _A.v4; v4.m128_f32[3] = 0; };
	FVec2(const float x, const float y, const float w) { v4.m128_f32[0] = x; v4.m128_f32[1] = y; v4.m128_f32[2] = w; v4.m128_f32[3] = 0;}
	FVec2(const float a) { v4.m128_f32[0] = a; v4.m128_f32[1] = a; v4.m128_f32[2] = 0; v4.m128_f32[3] = 0; }

	u_int Dim() const { return 2; };

	void SetPoint(const float x, const float y) { v4.m128_f32[0] = x; v4.m128_f32[1] = y; v4.m128_f32[2] = 1; v4.m128_f32[3] = 0; }
	void SetVector(const float x, const float y) { v4.m128_f32[0] = x; v4.m128_f32[1] = y; v4.m128_f32[2] = 0; v4.m128_f32[3] = 0; }
	float Cross(const FVec2 & _B) const
	{
		__m128 a, b;
		a = _mm_shuffle_ps(_B.v4, _B.v4, _MM_SHUFFLE(3, 2, 0, 1));
		b = v4 * a;
		a = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 2, 0, 1));
		b -= a ;
		return b.m128_f32[0];
	}
	float& w() { return v4.m128_f32[2]; }
	float& z() { return v4.m128_f32[3]; }
	float Dot(const FVec2 &_A) const
	{
		__m128 a;
		a = _mm_mul_ps(_A.v4, v4);
		return sum2(a);
	}
	float Length2() const
	{
		__m128 a = _mm_mul_ps(v4, v4);
		return sum2(a);
	}
	float Length() const
	{
		__m128 a = _mm_mul_ps(v4, v4);
		float fa = sum2(a);
		a = _mm_sqrt_ss(_mm_set_ss(fa));
		return a.m128_f32[0];
	}
	void Normalize()
	{
		__m128 l = _mm_set1_ps(Length());
		v4 /= l;
	}
	FVec2 Normalized() const
	{
		FVec2 a(*this);
		__m128 l = _mm_set1_ps(a.Length());
		a.v4 /= l;
		return a;
	}
	void Get(float &x, float &y)
	{
		x = v4.m128_f32[0];
		y = v4.m128_f32[1];
	}
};

INLINE FVec3 Point3(const float x, const float y, const float z) { return FVec3(x, y, z, 1); }
INLINE FVec3 Vector3(const float x, const float y, const float z) { return FVec3(x, y, z, 0); }
INLINE FVec2 Point2(const float x, const float y) { return FVec2(x, y, 1); }
INLINE FVec2 Vector2(const float x, const float y) { return FVec2(x, y, 0); }

INLINE FVec4 mmMax(const FVec4 & a, const FVec4 & b) { return mmMax(a.v4,b.v4); }
INLINE FVec4 mmMin(const FVec4 & a, const FVec4 & b) { return mmMin(a.v4, b.v4); }

INLINE FVec3 mmMax(const FVec3 & a, const FVec3 & b) { return mmMax(a.v4, b.v4); }
INLINE FVec3 mmMin(const FVec3 & a, const FVec3 & b) { return mmMin(a.v4, b.v4); }

INLINE FVec2 mmMax(const FVec2 & a, const FVec2 & b) { return mmMax(a.v4, b.v4); }
INLINE FVec2 mmMin(const FVec2 & a, const FVec2 & b) { return mmMin(a.v4, b.v4); }

INLINE bool  parallel(const FVec2 & a, const FVec2 & b) { return 1 - fabsf(a.Dot(b)) < 1e-10f; }
INLINE bool  parallel(const FVec3 & a, const FVec3 & b) { return 1 - fabsf(a.Dot(b)) < 1e-10f; }

INLINE float distance(FVec2 & a, FVec2 &b) { return (a - b).Length(); }
INLINE float distance2(FVec2 & a, FVec2 &b) { return (a - b).Length2(); }

INLINE float angle(FVec3 & a, FVec3 &b) { return acosf(a.Dot(b)); };
INLINE float angle(FVec2 & a, FVec2 &b) { return acosf(a.Dot(b)); };

template<typename VEC>
VEC Complement(const VEC & a)
{
	VEC one(1);
	return one - a;
}

END_SSE_MATH_NAME


#endif