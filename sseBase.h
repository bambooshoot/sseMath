#ifndef SSEBASE
#define SSEBASE

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <xmmintrin.h> 
#include <limits>

namespace sseMath {}

#define BEGIN_SSE_MATH_NAME namespace sseMath {
#define END_SSE_MATH_NAME }

BEGIN_SSE_MATH_NAME

#ifndef PI
#define PI		3.1415926f
#define PI_HALF	3.1415926f*0.5f
#define PI2		(3.1415926f*2.0f)
#endif

using u_int = unsigned int;

#define MAX_FLOAT std::numeric_limits<float>::max()
#define MIN_FLOAT std::numeric_limits<float>::min()
#define MAX_UINT  std::numeric_limits<u_int>::max()

template<typename T>
T clamp(const T & val, const T & lo, const T & hi)
{
	return std::min(std::max(val, lo), hi);
}



#define MEM_ALIGN16 __declspec(align(16))
#define INLINE __forceinline

INLINE __m128 operator +(const __m128 & a, const __m128 & b) { return _mm_add_ps(a, b); }
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

INLINE __m128 operator <=(const __m128 & a, const __m128 & b) { return _mm_cmple_ps(a, b); }
INLINE __m128 operator < (const __m128 & a, const __m128 & b) { return _mm_cmplt_ps(a, b); }
INLINE __m128 operator >=(const __m128 & a, const __m128 & b) { return _mm_cmpge_ps(a, b); }
INLINE __m128 operator > (const __m128 & a, const __m128 & b) { return _mm_cmpgt_ps(a, b); }
INLINE __m128 operator ==(const __m128 & a, const __m128 & b) { return _mm_cmpeq_ps(a, b); }
INLINE __m128 operator !=(const __m128 & a, const __m128 & b) { return _mm_cmpneq_ps(a, b); }
INLINE __m128 operator ||(const __m128 & a, const __m128 & b) { return _mm_or_ps(a, b); }
INLINE __m128 operator &&(const __m128 & a, const __m128 & b) { return _mm_and_ps(a, b); }

INLINE __m128 mmMax(const __m128 & a, const __m128 & b) { return _mm_max_ps(a, b); }
INLINE __m128 mmMin(const __m128 & a, const __m128 & b) { return _mm_min_ps(a, b); }

INLINE __m128 mmFill_F(const float f0, const float f1 = 0.0f, const float f2 = 0.0f, const float f3 = 0.0f) { return _mm_set_ps(f3, f2, f1, f0); }
INLINE __m128 mmFill_F4(const float f0) { return _mm_set_ps1(f0); }

INLINE float sum4(const __m128 & _a)
{
	return _a.m128_f32[0] + _a.m128_f32[1] + _a.m128_f32[2] + _a.m128_f32[3];
}

INLINE __m128 sum_01_23(const __m128 & _a)
{
	__m128 x0, x1;
	x0 = mmFill_F(_a.m128_f32[0], _a.m128_f32[1]);
	x1 = mmFill_F(_a.m128_f32[2], _a.m128_f32[3]);
	x0 += x1;
	return x0;
}

INLINE __m128 sum_02_13(const __m128 & _a)
{
	__m128 x0, x1;
	x0 = mmFill_F(_a.m128_f32[0], _a.m128_f32[2]);
	x1 = mmFill_F(_a.m128_f32[1], _a.m128_f32[3]);
	x0 += x1;
	return x0;
}

INLINE float sum3(const __m128 & _a)
{
	return _a.m128_f32[0] + _a.m128_f32[1] + _a.m128_f32[2];
}

INLINE float sum2(const __m128 & _a)
{
	return _a.m128_f32[0] + _a.m128_f32[1];
}

INLINE __m128 sse_cross3d(const __m128 & _a, const __m128 & _b)
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

END_SSE_MATH_NAME

#endif