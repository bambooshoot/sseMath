#ifndef SSEMATRIX
#define SSEMATRIX
#include <sseBase.h>
#include <sseVec.h>


BEGIN_SSE_MATH_NAME

class Matrix44
{
public:
	__m128 f[4];
	Matrix44() { Indentify(); }
	void Indentify()
	{
		f[0] = _mm_set_ps(0, 0, 0, 1);
		f[1] = _mm_set_ps(0, 0, 1, 0);
		f[2] = _mm_set_ps(0, 1, 0, 0);
		f[3] = _mm_set_ps(1, 0, 0, 0);
	}
	Matrix44(const float x00, const float x01, const float x02, const float x03,
		const float x10, const float x11, const float x12, const float x13,
		const float x20, const float x21, const float x22, const float x23,
		const float x30, const float x31, const float x32, const float x33)
	{
		f[0] = _mm_set_ps(x03, x02, x01, x00);
		f[1] = _mm_set_ps(x13, x12, x11, x10);
		f[2] = _mm_set_ps(x23, x22, x21, x20);
		f[3] = _mm_set_ps(x33, x32, x31, x30);
	}
	Matrix44(const Matrix44 & matB)
	{
		f[0] = matB.f[0];
		f[1] = matB.f[1];
		f[2] = matB.f[2];
		f[3] = matB.f[3];
	}

	__m128 operator * (const __m128 &_B)
	{
		__m128 c;
		c = f[0] * _B.m128_f32[0]
			+ f[1] * _B.m128_f32[1]
			+ f[2] * _B.m128_f32[2]
			+ f[3] * _B.m128_f32[3];

		return c;
	}

	FVec4 operator * (const FVec4 & _B)
	{
		return FVec4((*this) * _B.v4);
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
	Matrix44 operator *= (const Matrix44 & _B)
	{
		Matrix44 matA;
		matA.f[0] = (*this) * _B.f[0];
		matA.f[1] = (*this) * _B.f[1];
		matA.f[2] = (*this) * _B.f[2];
		matA.f[3] = (*this) * _B.f[3];
		*this = matA;
		return matA;
	}
	void Transpose()
	{
		_MM_TRANSPOSE4_PS(f[0], f[1], f[2], f[3]);
	}
	float & operator()(const u_int colI, const u_int rowI)
	{
		return f[colI].m128_f32[rowI];
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

class Matrix33
{
public:
	__m128 f[3];
	Matrix33() { Indentify(); }
	Matrix33(const Matrix44 & m44)
	{
		f[0] = m44.f[0];
		f[1] = m44.f[1];
		f[2] = m44.f[2];
		f[0].m128_f32[3] = 0;
		f[1].m128_f32[3] = 0;
		f[2].m128_f32[3] = 0;
	}
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
	void SetVec(FVec3 v, const int id)
	{
		f[id] = v.v4;
	}
};

class TransformMatrix3D : public Matrix44
{
public:
	TransformMatrix3D() :Matrix44() {};
	TransformMatrix3D(const float x00, const float x01, const float x02, const float x03,
		const float x10, const float x11, const float x12, const float x13,
		const float x20, const float x21, const float x22, const float x23,
		const float x30, const float x31, const float x32, const float x33)
		: Matrix44(x00, x01, x02, x03,
			x10, x11, x12, x13,
			x20, x21, x22, x23,
			x30, x31, x32, x33) {}
	TransformMatrix3D(const Matrix44 & mat44) 
	{
		*(Matrix44*)this = mat44;
	}
	TransformMatrix3D(const TransformMatrix3D & transMat)
	{
		*(Matrix44*)this = (Matrix44)transMat;
	}
	TransformMatrix3D & operator = (const Matrix33 & mat33)
	{
		f[0] = mat33.f[0];
		f[1] = mat33.f[1];
		f[2] = mat33.f[2];
		f[3] = _mm_set_ps(1, 0, 0, 0);
		return *this;
	}
	TransformMatrix3D & operator = (const Matrix44 & mat44)
	{
		*(Matrix44*)this = mat44;
		return *this;
	}
	FVec3 operator * (const FVec3 & _A) const
	{
		return FVec3(*((Matrix44*)this) * _A.v4);
	}
	TransformMatrix3D operator * (const TransformMatrix3D & _A) const
	{
		return TransformMatrix3D((Matrix44)*this * (Matrix44)_A);
	}
	TransformMatrix3D operator *= (const TransformMatrix3D & _A)
	{
		*(Matrix44*)this = (Matrix44)*this * (Matrix44)_A;
		return *this;
	}
	TransformMatrix3D Inverse()
	{
		Matrix44 s((*this)(1, 1) * (*this)(2, 2) - (*this)(2, 1) * (*this)(1, 2),
			(*this)(2, 1) * (*this)(0, 2) - (*this)(0, 1) * (*this)(2, 2),
			(*this)(0, 1) * (*this)(1, 2) - (*this)(1, 1) * (*this)(0, 2),
			0,

			(*this)(2, 0) * (*this)(1, 2) - (*this)(1, 0) * (*this)(2, 2),
			(*this)(0, 0) * (*this)(2, 2) - (*this)(2, 0) * (*this)(0, 2),
			(*this)(1, 0) * (*this)(0, 2) - (*this)(0, 0) * (*this)(1, 2),
			0,

			(*this)(1, 0) * (*this)(2, 1) - (*this)(2, 0) * (*this)(1, 1),
			(*this)(2, 0) * (*this)(0, 1) - (*this)(0, 0) * (*this)(2, 1),
			(*this)(0, 0) * (*this)(1, 1) - (*this)(1, 0) * (*this)(0, 1),
			0,

			0,
			0,
			0,
			1);

		float r = (*this)(0, 0) * s(0, 0) + (*this)(0, 1) * s(1, 0) + (*this)(0, 2) * s(2, 0);

		if (abs(r) >= 1)
		{
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					s(i, j) /= r;
				}
			}
		}
		else
		{
			float mr = abs(r) / MIN_FLOAT;

			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					if (mr > abs(s(i, j)))
					{
						s(i, j) /= r;
					}
					else
					{
						return Matrix44();
					}
				}
			}
		}

		s(3, 0) = -(*this)(3, 0) * s(0, 0) - (*this)(3, 1) * s(1, 0) - (*this)(3, 2) * s(2, 0);
		s(3, 1) = -(*this)(3, 0) * s(0, 1) - (*this)(3, 1) * s(1, 1) - (*this)(3, 2) * s(2, 1);
		s(3, 2) = -(*this)(3, 0) * s(0, 2) - (*this)(3, 1) * s(1, 2) - (*this)(3, 2) * s(2, 2);

		return s;
	}
	FVec3 Scale()
	{
		__m128 x[3], x1[3];
		x[0] = f[0] * f[0];
		x[1] = f[1] * f[1];
		x[2] = f[2] * f[2];
		x1[0] = _mm_set_ps(0, x[2].m128_f32[0], x[1].m128_f32[0], x[0].m128_f32[0]);
		x1[1] = _mm_set_ps(0, x[2].m128_f32[1], x[1].m128_f32[1], x[0].m128_f32[1]);
		x1[2] = _mm_set_ps(0, x[2].m128_f32[2], x[1].m128_f32[2], x[0].m128_f32[2]);
		x[0] = x1[0] + x1[1] + x1[2];
		x[0] = _mm_sqrt_ps(x[0]);
		return FVec3(x[0]);
	}
	FVec3 Translate()
	{
		return FVec3(f[3].m128_f32[0], f[3].m128_f32[1], f[3].m128_f32[2], 0);
	}
	FVec3 Axis(const int i)
	{
		return FVec3(f[i].m128_f32[0], f[i].m128_f32[1], f[i].m128_f32[2], 0);
	}
	void SetAxis(const FVec3 & ax,const int i)
	{
		f[i] = ax.v4;
	}
	void SetAxis(const float x, const float y, const float z, const float w, const int i)
	{
		f[i] = _mm_set_ps(w, z, y, x);
	}
	void SetTranslate(const FVec3 & t)
	{
		f[3] = t.v4;
		f[3].m128_f32[3] = 1;
	}
	void SetAxisAngle(const FVec3 & axis, float angle)
	{
		FVec3 unit(axis.Normalized());
		float sine = sinf(angle);
		float cosine = cosf(angle);

		f[0] = _mm_set_ps(	unit[0] * unit[0] * (1 - cosine) + cosine,
							unit[0] * unit[1] * (1 - cosine) + unit[2] * sine,
							unit[0] * unit[2] * (1 - cosine) - unit[1] * sine,
							0 );

		f[1] = _mm_set_ps(	unit[0] * unit[1] * (1 - cosine) - unit[2] * sine,
							unit[1] * unit[1] * (1 - cosine) + cosine,
							unit[1] * unit[2] * (1 - cosine) + unit[0] * sine,
							0 );

		f[2] = _mm_set_ps(	unit[0] * unit[2] * (1 - cosine) + unit[1] * sine,
							unit[1] * unit[2] * (1 - cosine) - unit[0] * sine,
							unit[2] * unit[2] * (1 - cosine) + cosine,
							0 );

		f[3] = _mm_set_ps(0, 0, 0, 1);
	}
	void NormalizeAxis()
	{
		__m128 x, xx = _mm_setzero_ps();
		for (int i = 0; i < 3; ++i) {
			x = _mm_set_ps(0, f[2].m128_f32[i], f[1].m128_f32[i], f[0].m128_f32[i]);
			xx += x * x;
		}
		xx = _mm_rsqrt_ps(xx);
		for (int i = 0; i < 3; ++i) {
			f[i] *= xx.m128_f32[i];
		}
	}
};

class Matrix22
{
public:
	__m128 f;
	Matrix22() { Indentify(); }
	Matrix22(const Matrix33 & m33) 
	{
		f.m128_f32[0] = m33.f[0].m128_f32[0]; f.m128_f32[1] = m33.f[0].m128_f32[1];
		f.m128_f32[2] = m33.f[1].m128_f32[0]; f.m128_f32[3] = m33.f[1].m128_f32[1];
	}
	Matrix22(const float x00, const float x01, 
			 const float x10, const float x11 ) 
	{ f.m128_f32[0] = x00; f.m128_f32[1] = x01; f.m128_f32[2] = x10; f.m128_f32[3] = x11; }

	void Indentify() { f.m128_f32[0] = 1; f.m128_f32[1] = 0; f.m128_f32[2] = 0; f.m128_f32[3] = 1; };

	__m128 operator * (const __m128 &_B)
	{
		__m128 b,c;
		b = _mm_shuffle_ps(_B, _B, _MM_SHUFFLE(1, 1, 0, 0));
		c = f*b;
		b = _mm_set_ps(0, 0, c.m128_f32[3], c.m128_f32[2]);
		c = _mm_set_ps(0, 0, c.m128_f32[1], c.m128_f32[0]);
		return c+b;
	}
	FVec2 operator * (const FVec2 & _B)
	{
		return FVec2((*this) * _B.v4);
	}
	void transpose()
	{
		f=_mm_shuffle_ps(f,f,_MM_SHUFFLE(3,1,2,0));
	}
	void Inverse()
	{
		__m128 x = _mm_set1_ps(determinant());
		x = _mm_rcp_ps(x);
		f *= x;
		f = _mm_set_ps(f.m128_f32[0],-f.m128_f32[2],-f.m128_f32[1],f.m128_f32[3]);
	}
	float determinant()
	{
		return f.m128_f32[0] * f.m128_f32[3] - f.m128_f32[1] * f.m128_f32[2];
	}
	void SetAxis(FVec2 & a, const int id)
	{
		f.m128_f32[id * 2] = a.x();
		f.m128_f32[id * 2 + 1] = a.y();
	}
};

class TransformMatrix2D : public Matrix33
{
public:
	TransformMatrix2D() :Matrix33() {}
	TransformMatrix2D(	const float x00, const float x01, const float x02,
						const float x10, const float x11, const float x12,
						const float x20, const float x21, const float x22 )
		: Matrix33(	x00, x01, x02,
					x10, x11, x12,
					x20, x21, x22 ) {}

	TransformMatrix2D & operator = (const Matrix22 & mat22)
	{
		f[0] = _mm_set_ps(0, 0, mat22.f.m128_f32[1], mat22.f.m128_f32[0]);
		f[1] = _mm_set_ps(0, 0, mat22.f.m128_f32[3], mat22.f.m128_f32[2]);
		f[2] = _mm_set_ps(0, 1, 0, 0);
		return *this;
	}
	FVec2 operator * (const FVec2 & _A) const
	{
		return FVec2(*((Matrix33*)this) * _A.v4);
	}
	void Inverse()
	{
		Matrix22 mat22(*(Matrix33*)this);
		mat22.Inverse();
		FVec2 x(-f[2].m128_f32[0], -f[2].m128_f32[1], 0);
		x = mat22*x;
		*this = mat22;
		f[2] = _mm_set_ps(0, 1, x.y(), x.x());
	}
	FVec2 Scale()
	{
		__m128 x[2],x1[2];
		x[0] = f[0] * f[0];
		x[1] = f[1] * f[1];
		x1[0] = _mm_set_ps(0, 0, x[1].m128_f32[0], x[0].m128_f32[0]);
		x1[1] = _mm_set_ps(0, 0, x[1].m128_f32[1], x[0].m128_f32[1]);
		x1[0] += x1[1];
		x[0] = _mm_sqrt_ps(x1[0]);
		return FVec2(x[0]);
	}
	FVec2 Translate()
	{
		return FVec2(f[2].m128_f32[0], f[2].m128_f32[1], 0);
	}
	void SetAxis(const FVec2 & ax, const int i)
	{
		f[i] = ax.v4;
	}
	void SetTranslate(const FVec2 & t)
	{
		f[2] = t.v4;
	}
};

class Matrix3DTo2D
{
public:
	__m128 f[3];
	Matrix3DTo2D() { f[0]=_mm_setzero_ps(); f[1] = _mm_setzero_ps(); f[2] = _mm_setzero_ps(); };
	Matrix3DTo2D(const float x00, const float x01, const float x02,
		const float x10, const float x11, const float x12)
	{
		f[0] = _mm_set_ps(0, 0, x10, x00);
		f[1] = _mm_set_ps(0, 0, x11, x01);
		f[2] = _mm_set_ps(0, 0, x12, x02);
	}
	Matrix3DTo2D(const FVec3 & xAxis, const FVec3 & yAxis)
	{ 
		f[0] = _mm_set_ps(0, 0, yAxis.v4.m128_f32[0], xAxis.v4.m128_f32[0]);
		f[1] = _mm_set_ps(0, 0, yAxis.v4.m128_f32[1], xAxis.v4.m128_f32[1]);
		f[2] = _mm_set_ps(0, 0, yAxis.v4.m128_f32[2], xAxis.v4.m128_f32[2]);
	}
	FVec2 operator * (const FVec3 & _A) const
	{
		__m128 x[3];
		x[0] = _mm_set1_ps(_A.v4.m128_f32[0]);
		x[1] = _mm_set1_ps(_A.v4.m128_f32[1]);
		x[2] = _mm_set1_ps(_A.v4.m128_f32[2]);
		x[0] *= f[0];
		x[1] *= f[1];
		x[2] *= f[2];
		x[0] += x[1] + x[2];
		return FVec2(x[0]);
	}
};

INLINE FVec4 operator *=(FVec4 & a, Matrix44 & b) { a = b * a; return a; };
INLINE FVec3 operator *=(FVec3 & a, TransformMatrix3D & b) { a = b * a; return a; };
INLINE FVec2 operator *=(FVec2 & a, TransformMatrix2D & b) { a = b * a; return a; };

INLINE Matrix3DTo2D Mat322FromNormal(const FVec3 & n)
{
	FVec3 xAxis(1,0,0), yAxis(0,1,0);
	if ( parallel(n, yAxis) ) {
		yAxis = xAxis.Cross(n);
		xAxis = n.Cross(yAxis);
	}
	else {
		xAxis = n.Cross(yAxis);
		yAxis = xAxis.Cross(n);
	}
	xAxis.Normalize();
	yAxis.Normalize();
	Matrix3DTo2D mat32(xAxis,yAxis);
	return mat32;
}

END_SSE_MATH_NAME
#endif
