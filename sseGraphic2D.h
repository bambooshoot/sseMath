#ifndef SSEGRAPHIC2D
#define SSEGRAPHIC2D

#include <sseBase.h>
#include <sseVec.h>
#include <sseMatrix.h>

#include <math.h>
#include <vector>
#include <algorithm>

BEGIN_SSE_MATH_NAME

struct Line2D // line in 2D space is similar with plane in 3D space
{
	FVec2 n;
	float d;
	Line2D() :d(1) { n.SetVector(0, 1); };
};

struct Circle2D
{
	FVec2 p;
	float r;
	Circle2D() :r(1) {};
	bool In(const FVec2 & bp) const
	{
		return (bp - p).Length2() <= r*r;
	}
};

struct Edge2D
{
	FVec2 p[2];
	Edge2D() {};
	Edge2D(const FVec2 &a, const FVec2 &b) { p[0] = a; p[1] = b; };
};

struct Triangle2D
{
	FVec2 p[3];
	Triangle2D()
	{
		p[0].SetPoint(0, 0);
		p[1].SetPoint(1, 0);
		p[2].SetPoint(0, 1);
	};
	u_int MaxEdge() const
	{
		float l[3];
		for (int i = 0; i < 3; ++i)
			l[i] = EdgeLen(i);

		float *maxL = std::max_element(l,l+3);
		return (u_int)(maxL - l);
	}
	float EdgeLen(const u_int edgeId) const
	{
		return (p[edgeId] - p[(edgeId + 1) % 3]).Length();
	}
	void EdgeSplit2Triangles(Triangle2D tri2Ds[2], const u_int edgeId, const float ratio) const
	{
		u_int p3Id = (edgeId + 2) % 3;
		u_int p1Id = edgeId;
		u_int p2Id = (edgeId + 1) % 3;

		tri2Ds[0].p[p3Id] = p[p3Id];
		tri2Ds[0].p[p1Id] = p[p1Id];
		tri2Ds[0].p[p2Id] = p[p1Id] * (1 - ratio) + p[p2Id] * ratio;

		tri2Ds[1].p[p3Id] = p[p3Id];
		tri2Ds[1].p[p2Id] = p[p2Id];
		tri2Ds[1].p[p1Id] = tri2Ds[0].p[p2Id];
	}
	FVec2 MeanP() const
	{
		return (p[0] + p[1] + p[2]) * 0.3333f;
	}
	float Area() const
	{
		if (EdgeLen(0) < 1e-10f || EdgeLen(1) < 1e-10f || EdgeLen(2) < 1e-10f)
			return 0;

		FVec2 e0(p[1] - p[0]);
		FVec2 e1(p[2] - p[0]);
		return abs(e0.Cross(e1))*0.5f;
	}
	FVec3 Weight3(const FVec2 & px)
	{
		Matrix33 mat33(	p[0].x(),p[0].y(),1,
						p[1].x(),p[1].y(),1,
						p[2].x(),p[2].y(),1);
		mat33.Inverse();
		FVec3 px3(px);
		px3.z() = 1;
		return mat33*px3;
	}
};

Line2D MakeLine2DWithP2(FVec2 & p0, FVec2 & p1);
float DistanceToLine2D(Line2D & line, FVec2 & p);
bool Point2DInTriangle2D(Triangle2D & tri, FVec2 & p);
bool Circle2D_X_Edge2D(Circle2D & circle, FVec2 & p0, FVec2 & p1);
bool Circle2D_X_Triangle2D(Circle2D & circle, Triangle2D & tri);
bool Line2D_X_Line2D(FVec2 & xP, Line2D & l0, Line2D & l1);
bool Edge2D_X_Edge2D(FVec2 & xP, Edge2D & e0, Edge2D & e1);

END_SSE_MATH_NAME

#endif