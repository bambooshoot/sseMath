#ifndef SSEMATRIX
#define SSEMATRIX
#include <sseBase.h>
#include <sseVec.h>

BEGIN_SSE_MATH_NAME

class Matrix44;

class Matrix33
{
public:
	__m128 f[3];
	Matrix33() { Indentify(); }
	Matrix33(const Matrix44 & m44);
	Matrix33(	const float x00, const float x01, const float x02, 
				const float x10, const float x11, const float x12, 
				const float x20, const float x21, const float x22 )
	{
		f[0] = _mm_set_ps(0, x02, x01, x00);
		f[1] = _mm_set_ps(0, x12, x11, x10);
		f[2] = _mm_set_ps(0, x22, x21, x20);
	}
	void Indentify()
	{
		f[0] = _mm_set_ps(0, 0, 0, 1);
		f[1] = _mm_set_ps(0, 0, 1, 0);
		f[2] = _mm_set_ps(0, 1, 0, 0);
	}
	__m128 operator * (const __m128 &_B)
	{
		__m128 c;
		c =   f[0] * _B.m128_f32[0]
			+ f[1] * _B.m128_f32[1]
			+ f[2] * _B.m128_f32[2];

		return c;
	}
	FVec3 operator * (const FVec3 & _B)
	{
		return FVec3( (*this) * _B.v4 );
	}

	void Inverse()
	{
		__m128 x[3];
		x[0] = sse_cross3d(f[1], f[2]);
		x[1] = sse_cross3d(f[2], f[0]);
		x[2] = sse_cross3d(f[0], f[1]);

		float reci_det = 1/determinant();
		x[0] *= reci_det;
		x[1] *= reci_det;
		x[2] *= reci_det;
		f[0] = x[0];
		f[1] = x[1];
		f[2] = x[2];
		transpose();
	}
	void transpose()
	{
		__m128 x[3];
		x[0] = _mm_set_ps(0, f[2].m128_f32[0], f[1].m128_f32[0], f[0].m128_f32[0]);
		x[1] = _mm_set_ps(0, f[2].m128_f32[1], f[1].m128_f32[1], f[0].m128_f32[1]);
		x[2] = _mm_set_ps(0, f[2].m128_f32[2], f[1].m128_f32[2], f[0].m128_f32[2]);
		f[0] = x[0];
		f[1] = x[1];
		f[2] = x[2];
	}
	float determinant()
	{
		//a[0](b[1]c[2] - c[1]b[2]) - b[0](a[1]c[2] - c[1]a[2]) + c[0](a[1]b[2] - b[1]a[2])
		__m128 aa, bb, cc;

		// b[1]c[2] - c[1]b[2]
		// a[1]c[2] - c[1]a[2]
		// a[1]b[2] - b[1]a[2]
		float *a = f[0].m128_f32;
		float *b = f[1].m128_f32;
		float *c = f[2].m128_f32;

		aa = _mm_set_ps(0, a[1], a[1], b[1]);
		bb = _mm_set_ps(0, b[2], c[2], c[2]);
		cc = aa * bb;

		aa = _mm_set_ps(0, b[1], c[1], c[1]);
		bb = _mm_set_ps(0, a[2], a[2], b[2]);
		cc -= aa * bb;

		aa = _mm_set_ps(0,c[0],b[0],a[0]);
		cc *= aa;

		return cc.m128_f32[0] - cc.m128_f32[1] + cc.m128_f32[2];
	}
};

class Matrix44
{
public:
	__m128 f[4];
	Matrix44() {  Indentify(); }
	void Indentify()
	{
		f[0] = _mm_set_ps(0, 0, 0, 1);
		f[1] = _mm_set_ps(0, 0, 1, 0);
		f[2] = _mm_set_ps(0, 1, 0, 0);
		f[3] = _mm_set_ps(1, 0, 0, 0);
	}
	Matrix44(	const float x00, const float x01, const float x02, const float x03,
				const float x10, const float x11, const float x12, const float x13,
				const float x20, const float x21, const float x22, const float x23,
				const float x30, const float x31, const float x32, const float x33 )
	{
		f[0] = _mm_set_ps(x03, x02, x01, x00);
		f[1] = _mm_set_ps(x13, x12, x11, x10);
		f[2] = _mm_set_ps(x23, x22, x21, x20);
		f[3] = _mm_set_ps(x33, x32, x31, x30);
	}

	__m128 operator * (const __m128 &_B)
	{ 
		__m128 c;
		c =   f[0] * _B.m128_f32[0] 
			+ f[1] * _B.m128_f32[1] 
			+ f[2] * _B.m128_f32[2] 
			+ f[3] * _B.m128_f32[3];

		return c;
	}

	FVec4 operator * (const FVec4 & _B)
	{
		return FVec4( (*this) * _B.v4 );
	}

	Matrix44 operator * (const Matrix44 & _B)
	{
		Matrix44 matA;
		matA.f[0] = (*this) * _B.f[0];
		matA.f[1] = (*this) * _B.f[1];
		matA.f[2] = (*this) * _B.f[2];
		matA.f[3] = (*this) * _B.f[3];
		return matA;
	}
	//void Inverse()
	//{

	//}
	//float determinant()
	//{
	//}
	//float trace()
	//{
	//}
};
class TransfromMatrix44 : public Matrix44
{
public:
	TransfromMatrix44 & operator = (const Matrix33 & mat33)
	{
		f[0] = mat33.f[0];
		f[1] = mat33.f[1];
		f[2] = mat33.f[2];
		f[3] = _mm_set_ps(1,0,0,0);
		return *this;
	}
	void Inverse()
	{
		Matrix33 mat33(*this);
		mat33.Inverse();
		FVec3 x(-f[3].m128_f32[0], -f[3].m128_f32[1], -f[3].m128_f32[2]);
		x = mat33*x;
		*this = mat33;
		f[3] = _mm_set_ps(1,x.x(),x.y(),x.z());
	}
	
};

Matrix33::Matrix33(const Matrix44 & m44)
{
	f[0] = m44.f[0];
	f[1] = m44.f[1];
	f[2] = m44.f[2];
	f[0].m128_f32[3] = 0;
	f[1].m128_f32[3] = 0;
	f[2].m128_f32[3] = 0;
}
END_SSE_MATH_NAME
#endif
