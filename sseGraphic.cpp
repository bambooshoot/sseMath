#include <sseGraphic.h>

BEGIN_SSE_MATH_NAME

Triangle2D ProjTriangleFrom3D22D(const Matrix3DTo2D & mat322, const Triangle & tri)
{
	Triangle2D tri2D;
	tri2D.p[1] = mat322 * tri.p[1];
	tri2D.p[2] = mat322 * tri.p[2];
	return tri2D;
};

Plane MakePlaneWithTriangle(Triangle & tri)
{
	FVec3 ab = tri.p[1] - tri.p[0];
	FVec3 ac = tri.p[2] - tri.p[0];
	FVec3 n = ab.Cross(ac);
	n.Normalize();
	Plane plane;
	plane.n = n;
	plane.d = n.Dot(tri.p[0]);
	return plane;
};

Matrix3DTo2D Mat322FromTriangle(const Triangle & tri)
{
	FVec3 xAxis, yAxis, zAxis;
	xAxis = tri.p[1] - tri.p[0];
	yAxis = tri.p[2] - tri.p[0];
	zAxis = xAxis.Cross(yAxis);
	yAxis = zAxis.Cross(xAxis);
	xAxis.Normalize();
	yAxis.Normalize();
	return Matrix3DTo2D(xAxis, yAxis);
};

bool Sphere_X_Plane(float & d, FVec3 & xP, Sphere & sph, Plane & plane)
{
	d = sph.p.Dot(plane.n);
	FVec3 nV = plane.n*d;
	xP = sph.p - nV;
	return fabsf(d) <= sph.r;
};

bool Sphere_X_Triangle(FVec3 & xP, Sphere & sph, Triangle & tri)
{
	Plane plane = MakePlaneWithTriangle(tri);
	float d;
	if (!Sphere_X_Plane(d, xP, sph, plane))
		return false;

	Circle2D circle;
	circle.r = sqrtf(sph.r*sph.r - d*d);
	Matrix3DTo2D mat322 = Mat322FromTriangle(tri);
	circle.p = mat322*xP;

	Triangle2D tri2D = ProjTriangleFrom3D22D(mat322, tri);

	// problem projected to 2D space becomes intersect between circle2d and triangle2d
	Circle2D_X_Triangle2D(circle, tri2D);
	return true;
};
void Cut3TrianglesByX(Triangle subTris[3], FVec3 xp[2], int sortedTriId[3], Triangle & tri)
{
	subTris[0].p[0] = tri.p[sortedTriId[0]];
	subTris[0].p[1] = xp[0];
	subTris[0].p[2] = xp[1];

	subTris[1].p[0] = tri.p[sortedTriId[1]];
	subTris[1].p[1] = tri.p[sortedTriId[2]];
	subTris[1].p[2] = xp[0];

	subTris[2].p[0] = tri.p[sortedTriId[2]];
	subTris[2].p[1] = xp[1];
	subTris[2].p[2] = xp[0];
}

AAPlane_X_Triangle_Type AAPlane_X_Triangle(FVec3 xp[2], int sortedTriId[3], Triangle & tri, const float axisD, const int axisId)
// sortedTriId
// 1___2
// x\__|x
//   \ |
//    \|
//	   0
{
	FloatSort<int> fsort;
	float axisXList[4] = { tri.p[0].v4.m128_f32[axisId], tri.p[1].v4.m128_f32[axisId], tri.p[2].v4.m128_f32[axisId], axisD };
	int idList[4] = { 0,1,2,3 };
	fsort.Sort(axisXList, idList, 4);
	FVec3 *p0, *p1, *p2;
	AAPlane_X_Triangle_Type xFlag;
	if (fsort[0].data == 3) {
		return AAPlane_X_Triangle_Type::kRight;
	}
	else if (fsort[3].data == 3) {
		return AAPlane_X_Triangle_Type::kLeft;
	}
	else if (fsort[1].data == 3) {
		p0 = &tri.p[fsort[0].data];
		p1 = &tri.p[fsort[2].data];
		p2 = &tri.p[fsort[3].data];
		sortedTriId[0] = fsort[0].data;
		sortedTriId[1] = fsort[2].data;
		sortedTriId[2] = fsort[3].data;
		xFlag = AAPlane_X_Triangle_Type::k1X2;
	}
	else {
		p0 = &tri.p[fsort[3].data];
		p1 = &tri.p[fsort[1].data];
		p2 = &tri.p[fsort[0].data];
		sortedTriId[0] = fsort[3].data;
		sortedTriId[1] = fsort[1].data;
		sortedTriId[2] = fsort[0].data;
		xFlag = AAPlane_X_Triangle_Type::k2X1;
	}

	float w0 = (axisD - p0->v4.m128_f32[axisId]) / (p1->v4.m128_f32[axisId] - p0->v4.m128_f32[axisId]);
	float w1 = (axisD - p0->v4.m128_f32[axisId]) / (p2->v4.m128_f32[axisId] - p0->v4.m128_f32[axisId]);
	xp[0].v4.m128_f32[axisId] = axisD;
	xp[1].v4.m128_f32[axisId] = axisD;
	int axis1 = (axisId + 1) % 3;
	int axis2 = (axisId + 2) % 3;
	FVec4 f4(p0->v4.m128_f32[axis1], p0->v4.m128_f32[axis2], p0->v4.m128_f32[axis1], p0->v4.m128_f32[axis2]);
	FVec4 f4a(p1->v4.m128_f32[axis1], p1->v4.m128_f32[axis2], p2->v4.m128_f32[axis1], p2->v4.m128_f32[axis2]);
	FVec4 fw2(w0, w0, w1, w1), fw1(1.0f);
	f4 *= fw1 - fw2;
	f4a *= fw2;
	f4 += f4a;
	f4.Get(xp[0].v4.m128_f32[axis1], xp[0].v4.m128_f32[axis2], xp[1].v4.m128_f32[axis1], xp[1].v4.m128_f32[axis2]);
	return xFlag;
};

bool Triangle_X_Triangle(FVec3 & xp0, FVec3 & xp1, Triangle & tri0, Triangle & tri1)
{
	Triangle * tris[] = { &tri0,&tri1 };
	Matrix22 A;
	Plane tri_planes[2];
	tri_planes[0] = MakePlaneWithTriangle(tri0);
	tri_planes[1] = MakePlaneWithTriangle(tri1);
	if (fabsf(tri_planes[0].n.Dot(tri_planes[1].n)) <= 1e-10f)
		return false;

	FVec2 b(tri_planes[0].d, tri_planes[1].d, 0);
	FVec3 p[2][2];
	float NP0, NP1, w;

	const int indices[3][2] = { { 0,1 },{ 1,2 },{ 2,0 } };
	const int triId[2][2] = { { 0,1 },{ 1,0 } };
	int validFlag[2] = { -1,-1 };
	for (int j = 0; j < 2; ++j) {
		for (int i = 0; i < 3; ++i) {
			FVec3 & segP0 = tris[triId[j][0]]->p[indices[i][0]];
			FVec3 & segP1 = tris[triId[j][0]]->p[indices[i][1]];
			Plane & pln = tri_planes[triId[j][1]];
			NP0 = pln.n.Dot(segP0);
			NP1 = pln.n.Dot(segP1);
			w = (pln.d - NP1) / (NP0 - NP1);

			if (w >= 0 && w <= 1) {
				if (validFlag[j] == -1) {
					p[j][0] = segP0*w + segP1*(1 - w);
				}
				else {
					p[j][1] = segP0*w + segP1*(1 - w);
				}
				validFlag[j] = i;
			}
		}
	}

	if (validFlag[0] == -1 || validFlag[1] == -1)
		return false;

	FVec3 v0, v1, v2;
	v0 = p[0][1] - p[0][0];
	v1 = p[1][0] - p[0][0];
	v2 = p[1][1] - p[0][0];

	float f0, f1, f2;
	f0 = v0.Length();
	v0.Normalize();
	f1 = v1.Dot(v0);
	f2 = v2.Dot(v0);

	struct EP
	{
		int edgeId;
		FVec3 *pp;
	};

	EP epList[4];
	epList[0].edgeId = 0;
	epList[0].pp = &p[0][0];

	epList[1].edgeId = 0;
	epList[1].pp = &p[0][1];

	epList[2].edgeId = 1;
	epList[2].pp = &p[1][0];

	epList[3].edgeId = 1;
	epList[3].pp = &p[1][1];

	float sortflist[4] = { 0,f0,f1,f2 };

	FloatSort<EP> fsort;
	fsort.Sort(sortflist, epList, 4);

	if ((fsort[0].data.edgeId != fsort[1].data.edgeId)
		&& (fsort[2].data.edgeId != fsort[3].data.edgeId)) {
		xp0 = *fsort[1].data.pp;
		xp1 = *fsort[2].data.pp;
		return true;
	}
	return false;
};

END_SSE_MATH_NAME