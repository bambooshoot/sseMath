#pragma once

#include <sseVec.h>
#include <sseMatrix.h>

BEGIN_SSE_MATH_NAME

class Quaternion
{
public:
	FVec3 v;
	float w;
	Quaternion() = default;
	Quaternion(const float x, const float y, const float z, const float _w)
	{
		v.x() = x; v.y() = x; v.z() = x; w = _w;
	}
	Quaternion(const FVec3 &_v, const float _w)
	{
		v = _v;
		w = _w;
	}
	//void SetAxisAngle(const FVec3 &axis, const float angle)
	//{
	//	float halfAngle = angle*0.5f;
	//	v = axis*sin(halfAngle);
	//	w = cos(halfAngle);
	//}

	//float Length() const
	//{
	//	return sqrtf(v.Dot(v) + w*w);
	//}
	//Quaternion Conjugate() const
	//{
	//	return Quaternion(-v.cValue(0), -v.cValue(1), -v.cValue(2), w);
	//}
	//Quaternion Invert() const
	//{
	//	return Conjugate() / Length();
	//}
	Quaternion operator * (const Quaternion & b) const
	{
		Quaternion c;
		c.v = v.Cross(b.v) + v*b.w + b.v*w;
		c.w = w*b.w - v.Dot(b.v);
		return c;
	}
	Quaternion operator * (const float f) const
	{
		FVec3 v1 = v*f;
		return Quaternion(v1, w*f);
	}
	Quaternion operator *= (const Quaternion &b)
	{
		v = v.Cross(b.v) + v*b.w + b.v*w;
		w = w*b.w - v.Dot(b.v);
		return *this;
	}
	Quaternion operator / (const float f) const
	{
		float invF = 1.f / f;
		return *this * invF;
	}
	
	FVec3 RotateVector(const FVec3 & v) const
	{
		Quaternion p(v, 0);
		Quaternion inv(*this);
		inv.v *= -1;
		Quaternion p1 = *this * p * inv;
		return p1.v.Normalized();
	}

	Quaternion setRotation(const FVec3 &from, const FVec3 &to)
	{
		FVec3 f0 = from.Normalized();
		FVec3 t0 = to.Normalized();

		if (f0.Dot(t0) >= 0) {
			setRotationInternal(f0, t0, *this);
		}
		else {
			FVec3 h0 = (f0 + t0).Normalized();

			if (h0.Dot(h0) != 0)
			{
				setRotationInternal(f0, h0, *this);

				Quaternion q;
				setRotationInternal(h0, t0, q);

				*this *= q;
			}
			else
			{
				w = 0;

				FVec3 f02 = f0 * f0;

				const float f02x = f02.cValue(0);
				const float f02y = f02.cValue(1);
				const float f02z = f02.cValue(2);

				if (f02x <= f02y && f02x <= f02z)
					v = f0.Cross(FVec3(1, 0, 0)).Normalized();
				else if (f02y <= f02z)
					v = f0.Cross(FVec3(0, 1, 0)).Normalized();
				else
					v = f0.Cross(FVec3(0, 0, 1)).Normalized();
			}
		}

		return *this;
	}
	void setRotationInternal(const FVec3 &f0, const FVec3 &t0, Quaternion &q)
	{
		FVec3 h0 = (f0 + t0).Normalized();
		q.w = f0.Dot(h0);
		q.v = f0.Cross(h0);
	}
	Matrix44
		Quaternion::toMatrix44() const
	{
		const float & r = w;
		const float x = v.cValue(0);
		const float y = v.cValue(1);
		const float z = v.cValue(2);
		return Matrix44(1 - 2 * (y * y + z * z),
			2 * (x * y + z * r),
			2 * (z * x - y * r),
			0,
			2 * (x * y - z * r),
			1 - 2 * (z * z + x * x),
			2 * (y * z + x * r),
			0,
			2 * (z * x + y * r),
			2 * (y * z - x * r),
			1 - 2 * (y * y + x * x),
			0,
			0,
			0,
			0,
			1);
	}
};

END_SSE_MATH_NAME