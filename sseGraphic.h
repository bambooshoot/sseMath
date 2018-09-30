#ifndef SSEGRAPHIC
#define SSEGRAPHIC

#include <sseBase.h>
#include <sseVec.h>
#include <sseMatrix.h>

#include <math.h>
#include <vector>
#include <algorithm>
#include <FloatSort.h>
#include <sseGraphic2D.h>

BEGIN_SSE_MATH_NAME

struct Plane
{
	FVec3 n; //normalized vector
	float d; //distance from orginal to plane
	Plane() :d(1) { n.SetVector(1,1,1); };
};
struct Box;

struct Sphere
{
	FVec3 p;
	float r;
	Sphere():r(1) {};
};

struct Edge
{
	FVec3 p[2];
};

class Triangle
{
public:
	FVec3 p[3];
	Triangle() 
	{ 
		p[0].SetPoint(1, 0, 0);
		p[1].SetPoint(0, 1, 0);
		p[2].SetPoint(0, 0, 1);
	};
	Triangle(const FVec3 &p0, const FVec3 &p1, const FVec3 &p2)
	{
		Set(p0, p1, p2);
	}
	void Set(const FVec3 &p0, const FVec3 &p1, const FVec3 &p2)
	{
		p[0] = p0; p[1] = p1; p[2] = p2;
	}
	u_int MaxEdge() const
	{
		float l[3];
		for (int i = 0; i < 3; ++i)
			l[i] = EdgeLen(i);

		float *maxL = std::max_element(l, l + 3);
		return (u_int)(maxL - l);
	}
	float EdgeLen(const u_int edgeId) const
	{
		return (p[edgeId] - p[(edgeId + 1) % 3]).Length();
	}
	void EdgeSplit2Triangles(Triangle tris[2], const u_int edgeId, const float ratio) const
	{
		u_int p1Id = edgeId;
		u_int p2Id = (edgeId + 1) % 3;
		u_int p3Id = (edgeId + 2) % 3;

		tris[0].p[p1Id] = p[p1Id];
		tris[0].p[p2Id] = p[p1Id] * (1 - ratio) + p[p2Id] * ratio;
		tris[0].p[p3Id] = p[p3Id];

		tris[1].p[p1Id] = tris[0].p[p2Id];
		tris[1].p[p2Id] = p[p2Id];
		tris[1].p[p3Id] = p[p3Id];
	}
	FVec3 MeanP() const
	{
		return (p[0] + p[1] + p[2]) * 0.3333f;
	}
	float MeanD(const int axisId) const
	{
		return (p[0].cValue(axisId) + p[1].cValue(axisId) + p[2].cValue(axisId)) / 3;
	}
	float Area() const
	{
		if (EdgeLen(0) < 1e-10f || EdgeLen(1) < 1e-10f || EdgeLen(2) < 1e-10f)
			return 0;

		FVec3 e0(p[1] - p[0]);
		FVec3 e1(p[2] - p[0]);
		return abs(e0.Cross(e1).Length())*0.5f;
	}
	void Tesselate(std::vector<Triangle> & subTriList) const
	{
		Triangle tri;
		tri.Set(p[0], (p[0] + p[1])*0.5f, (p[0] + p[2])*0.5f);
		subTriList.push_back(tri);

		tri.Set(p[1], (p[1] + p[2])*0.5f, (p[1] + p[0])*0.5f);
		subTriList.push_back(tri);

		tri.Set(p[2], (p[2] + p[0])*0.5f, (p[2] + p[1])*0.5f);
		subTriList.push_back(tri);

		tri.Set((p[0] + p[1])*0.5f, (p[1] + p[2])*0.5f, (p[2] + p[0])*0.5f);
		subTriList.push_back(tri);
	}
	FVec3 WeightPoint(const FVec3 & w) const {
		return p[0] * w.cValue(0) + p[1] * w.cValue(1) + p[2] * w.cValue(2);
	}
	FVec3 Weight3(const FVec3 & px)
	{
		Matrix33 mat33(	p[0].x(), p[0].y(), p[0].z(),
						p[1].x(), p[1].y(), p[1].z(),
						p[2].x(), p[2].y(), p[2].z() );
		mat33.Inverse();
		FVec3 px3(px);
		return mat33*px3;
	}
	bool operator < (const Triangle & tri) const
	{
		return Area() < tri.Area();
	}
};

Triangle2D ProjTriangleFrom3D22D(const Matrix3DTo2D & mat322, const Triangle & tri);
Plane MakePlaneWithTriangle(Triangle & tri);
Matrix3DTo2D Mat322FromTriangle(const Triangle & tri);
bool Sphere_X_Plane(float & d, FVec3 & xP, Sphere & sph, Plane & plane);
bool Sphere_X_Triangle(FVec3 & xP, Sphere & sph, Triangle & tri);
bool Sphere_X_Box(const Sphere & sph, Box & abox);
void Cut3TrianglesByX(Triangle subTris[3], FVec3 xp[2], int sortedTriId[3], Triangle & tri);

enum AAPlane_X_Triangle_Type
{
	kLeft = -1,
	kRight = 1,
	k1X2 = 12,
	k2X1 = 21
};
AAPlane_X_Triangle_Type AAPlane_X_Triangle(FVec3 xp[2], int sortedTriId[3], Triangle & tri, const float axisD, const int axisId);
bool Triangle_X_Triangle(FVec3 & xp0, FVec3 & xp1, Triangle & tri0, Triangle & tri1);

END_SSE_MATH_NAME

#endif