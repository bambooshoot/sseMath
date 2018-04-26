#ifndef SSEVEC
#define SSEVEC

#include <sseBase.h>

BEGIN_SSE_MATH_NAME

INLINE __m128 operator +(const __m128 & a, const __m128 & b) {	return _mm_add_ps(a, b); }
INLINE __m128 operator -(const __m128 & a, const __m128 & b) { return _mm_sub_ps(a, b); }
INLINE __m128 operator *(const __m128 & a, const __m128 & b) { return _mm_mul_ps(a, b); }
INLINE __m128 operator *(const __m128 & a, const float  & b) { return _mm_mul_ps(a, _mm_set1_ps(b)); }
INLINE __m128 operator /(const __m128 & a, const __m128 & b) { return _mm_div_ps(a, b); }
INLINE __m128 operator /(const __m128 & a, const float  & b) { return _mm_div_ps(a, _mm_set1_ps(b)); }

INLINE __m128 operator +=(__m128 & a, const __m128 & b) { a = a + b; return a; }
INLINE __m128 operator -=(__m128 & a, const __m128 & b) { a = a - b; return a; }
INLINE __m128 operator *=(__m128 & a, const __m128 & b) { a = a * b; return a; }
INLINE __m128 operator *=(__m128 & a, const float    b) { a = a * b; return a; }
INLINE __m128 operator /=(__m128 & a, const __m128 & b) { a = a / b; return a; }
INLINE __m128 operator /=(__m128 & a, const float    b) { a = a / b; return a; }

INLINE float sum4(const __m128 & _a)
{
	return _a.m128_f32[0] + _a.m128_f32[1] + _a.m128_f32[2] + _a.m128_f32[3];
}

__m128 sse_cross3d(const __m128 & _a, const __m128 & _b)
{
	__m128 a, b, c;

	a = _mm_shuffle_ps(_a, _a, _MM_SHUFFLE(3, 0, 2, 1));
	b = _mm_shuffle_ps(_b, _b, _MM_SHUFFLE(3, 1, 0, 2));
	c = a * b;

	a = _mm_shuffle_ps(_a, _a, _MM_SHUFFLE(3, 1, 0, 2));
	b = _mm_shuffle_ps(_b, _b, _MM_SHUFFLE(3, 0, 2, 1));
	c -= a * b;
	
	return c;
}

class FVec4 {
public:
	__m128 v4;

	FVec4() { v4 = _mm_setzero_ps(); };
	FVec4(__m128 _A) { v4 = _A; };
	FVec4(const float x, const float y, const float z, const float w) { v4.m128_f32[0] = x; v4.m128_f32[1] = y; v4.m128_f32[2] = z; v4.m128_f32[3] = w; }
	FVec4& operator =(const FVec4 &_A) { memcpy(this, &_A, sizeof(FVec4)); return *this; }

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

	float& x() { return v4.m128_f32[0]; }
	float& y() { return v4.m128_f32[1]; }
	float& z() { return v4.m128_f32[2]; }
	float& w() { return v4.m128_f32[3]; }

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
};

class FVec3 : public FVec4
{
public:
	FVec3() { v4 = _mm_setzero_ps(); };
	FVec3(__m128 _A) { v4 = _A; v4.m128_f32[3] = 0; };
	FVec3(const float x, const float y, const float z) { v4.m128_f32[0] = x; v4.m128_f32[1] = y; v4.m128_f32[2] = z; v4.m128_f32[3] = 0; }
	FVec3 Cross(const FVec3 & _B) { return FVec3( sse_cross3d( v4, _B.v4 ) ); }
};

class FVec2 : public FVec4
{
public:
	FVec2() { v4 = _mm_setzero_ps(); };
	FVec2(__m128 _A) { v4 = _A; v4.m128_f32[2] = 0; v4.m128_f32[3] = 0; };
	FVec2(const float x, const float y) { v4.m128_f32[0] = x; v4.m128_f32[1] = y; }
	float Cross(const FVec2 & _B)
	{
		__m128 a, b;
		a = _mm_shuffle_ps(_B.v4, _B.v4, _MM_SHUFFLE(3, 2, 0, 1));
		b = v4 * a;
		a = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 2, 0, 1));
		b -= a ;
		return b.m128_f32[0];
	}
};

END_SSE_MATH_NAME


#endif