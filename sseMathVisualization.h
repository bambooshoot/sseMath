#ifndef SSEMATHVISUALIZATION
#define SSEMATHVISUALIZATION

#include <sseBase.h>
#include <sseVec.h>
#include <sseMatrix.h>

BEGIN_SSE_MATH_NAME

void ShowFloat(float f, const char * name)
{
	printf("\nfloat: %s\n %f\n",name,f);
}

void ShowVec4(FVec4 & vec, const char * name)
{
	printf("\nFVec4: %s\n %f %f %f %f\n", name, vec.x(), vec.y(), vec.z(), vec.w());
}

void ShowVec3(FVec3 & vec, const char * name)
{
	printf("\nFVec3: %s\n %f %f %f %f\n", name, vec.x(), vec.y(), vec.z(), vec.w());
}

void ShowVec2(FVec2 & vec, const char * name)
{
	printf("\nFVec2: %s\n %f %f %f %f\n", name, vec.x(), vec.y(), vec.w(), vec.z());
}

void ShowMatrix44(Matrix44 & mat44, const char * name)
{
	printf("\nmat44: %s\n vec 0: %f %f %f %f\n vec 1: %f %f %f %f\n vec 2: %f %f %f %f\n vec 3: %f %f %f %f \n", name,
		mat44.f[0].m128_f32[0], mat44.f[0].m128_f32[1], mat44.f[0].m128_f32[2], mat44.f[0].m128_f32[3],
		mat44.f[1].m128_f32[0], mat44.f[1].m128_f32[1], mat44.f[1].m128_f32[2], mat44.f[1].m128_f32[3],
		mat44.f[2].m128_f32[0], mat44.f[2].m128_f32[1], mat44.f[2].m128_f32[2], mat44.f[2].m128_f32[3],
		mat44.f[3].m128_f32[0], mat44.f[3].m128_f32[1], mat44.f[3].m128_f32[2], mat44.f[3].m128_f32[3]);
}

void ShowTransformMatrix3D(TransformMatrix3D &tMat,const char * name)
{
	printf("\nTransfromMatrix3D: %s\n",name);
	ShowMatrix44(tMat,name);
	FVec3 scale, translate, rotate;
	scale = tMat.Scale();
	translate = tMat.Translate();
	printf("\n Scale %f %f %f\n translate %f %f %f\n",
		scale.x(), scale.y(), scale.z(),
		translate.x(), translate.y(), translate.z());
	printf("\nEnd TransformMatrix3D\n");
}

void ShowMatrix33(Matrix33 & mat33, const char * name)
{
	printf("\nmat33: %s\n %f %f %f %f\n %f %f %f %f\n %f %f %f %f \n ", name,
		mat33.f[0].m128_f32[0], mat33.f[0].m128_f32[1], mat33.f[0].m128_f32[2], mat33.f[0].m128_f32[3],
		mat33.f[1].m128_f32[0], mat33.f[1].m128_f32[1], mat33.f[1].m128_f32[2], mat33.f[1].m128_f32[3],
		mat33.f[2].m128_f32[0], mat33.f[2].m128_f32[1], mat33.f[2].m128_f32[2], mat33.f[2].m128_f32[3]);
}

void ShowTransformMatrix2D(TransformMatrix2D &tMat, const char * name)
{
	printf("\nTransfromMatrix2D: %s\n", name);
	ShowMatrix33(tMat, name);
	FVec2 scale, translate, rotate;
	scale = tMat.Scale();
	translate = tMat.Translate();
	printf("\n Scale %f %f\n translate %f %f\n",
		scale.x(), scale.y(),
		translate.x(), translate.y() );
	printf("\nEnd TransformMatrix2D\n");
}

END_SSE_MATH_NAME

#endif
