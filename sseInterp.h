#pragma once
#include <sseVec.h>

BEGIN_SSE_MATH_NAME

float Lerp(const float a, const float b, const float ratio)
{
	__m128 x0,x1;
	x0 = mmFill_F(a,b);
	x1 = mmFill_F(1-ratio, ratio);
	x0 *= x1;
	return sum2(x0);
}

void Lerp(float & out0, float & out1, const float a0, const float a1, const float b0, const float b1, const float ratio0, const float ratio1)
{
	__m128 x0, x1;
	x0 = mmFill_F(a0, a1, b0, b1);
	x1 = mmFill_F(1 - ratio0, 1 - ratio1, ratio0, ratio1);
	x0 *= x1;
	x0 = sum_01_23(x0);
	out0 = x0.m128_f32[0];
	out1 = x0.m128_f32[1];
}

template<typename VEC>
VEC Lerp(const VEC & a, const VEC & b, const float ratio)
{
	return a*(1 - ratio) + a*ratio;
}

template<typename VEC>
VEC Lerp(const VEC & a, const VEC & b, const VEC & ratioV)
{
	return a*Complement(ratioV) + b*ratioV;
}

END_SSE_MATH_NAME