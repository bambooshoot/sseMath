#ifndef SSEBOX
#define SSEBOX

#include <sseBase.h>
#include <sseVec.h>
#include <sseMatrix.h>
#include <sseGraphic.h>
#include <FloatSort.h>

BEGIN_SSE_MATH_NAME

struct Box
{
	enum PlaneSide
	{
		kBoth = 0,
		kLeft = 1,
		kRight = 2
	};

	FVec3 min, max;
	Box() { Reset(); };
	Box(const FVec3 & _min, const FVec3 & _max) { min = _min; max = _max; };
	Box(const Box & box) { min = box.min; max = box.max; };
	Box(Triangle & tri)
	{
		Reset();
		Extend(tri.p[0]);
		Extend(tri.p[1]);
		Extend(tri.p[2]);
	}
	void Reset()
	{
		min.SetPoint(1e10f, 1e10f, 1e10f);
		max.SetPoint(-1e10f, -1e10f, -1e10f);
	}
	float Length(const int axisId)
	{
		return max[axisId] - min[axisId];
	}
	int MaxAxis()
	{
		float len[3];
		len[0] = Length(0);
		len[1] = Length(1);
		len[2] = Length(2);
		int axisList[3] = { 0,1,2 };
		FloatSort<int> fsort;
		fsort.Sort(len, axisList, 3);
		return fsort[2].data;
	}
	void Extend(const FVec3 &p)
	{
		min = mmMin(min, p);
		max = mmMax(max, p);
	}
	void Extend(const float radius)
	{
		min.x() -= radius;
		min.y() -= radius;
		min.z() -= radius;
		max.x() += radius;
		max.y() += radius;
		max.z() += radius;
	}
	bool In(const FVec3 & p)
	{
		__m128 chk = min.v4 <= p.v4 && p.v4 <= max.v4;
		return chk.m128_i32[0] && chk.m128_i32[1] && chk.m128_i32[2];
	}
	bool In(Box & box)
	{
		FloatSort<int> fsort;
		int boxFlag[4] = { 0,0,1,1 };
		int inFlag = 0;
		for (int i = 0; i < 3; ++i) {
			float xList[4] = { box.min[i],box.max[i],min[i],max[i] };
			fsort.Sort(xList, boxFlag, 4);
			if (fsort[0].data != fsort[1].data)
				++inFlag;
		}
		return inFlag == 3;
	}
	PlaneSide X_AAPlane(const float axisD, const int axisId)
	{
		FloatSort<int> fsort;
		float xList[3] = { axisD,min[axisId],max[axisId] };
		int flag[3] = { 0,1,1 };
		fsort.Sort(xList, flag, 3);
		if (fsort[0].data == 0)
			return kRight;
		else if (fsort[1].data == 0)
			return kBoth;
		return kLeft;
	}
	void AxisSplit(Box &lBox, Box &rBox, const float axisD, const int axisId)
	{
		lBox = *this;
		lBox.max[axisId] = axisD;

		rBox = *this;
		rBox.min[axisId] = axisD;
	}
}; 

END_SSE_MATH_NAME

#endif