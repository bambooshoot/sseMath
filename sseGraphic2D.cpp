#include <sseGraphic2D.h>

BEGIN_SSE_MATH_NAME

Line2D MakeLine2DWithP2(FVec2 & p0, FVec2 & p1)
{
	Line2D line;
	FVec2 v = p1 - p0;
	v.Normalize();
	line.n.x() = v.y();
	line.n.y() = -v.x();
	line.d = p0.Dot(line.n);
	return line;
}

float DistanceToLine2D(Line2D & line, FVec2 & p)
{
	return (p - line.n*line.d).Dot(line.n);
}

bool Point2DInTriangle2D(Triangle2D & tri, FVec2 & p)
{
	Line2D line[3];
	line[0] = MakeLine2DWithP2(tri.p[0], tri.p[1]);
	line[1] = MakeLine2DWithP2(tri.p[1], tri.p[2]);
	line[2] = MakeLine2DWithP2(tri.p[2], tri.p[0]);
	float d0 = DistanceToLine2D(line[0], p);
	float d1 = DistanceToLine2D(line[1], p);
	float d2 = DistanceToLine2D(line[2], p);
	__m128 d3 = _mm_set_ps(0, d2, d1, d0);
	__m128 o = _mm_setzero_ps();
	o = _mm_cmple_ps(d3, o);
	return o.m128_u32[0] && o.m128_u32[1] && o.m128_u32[2];
}

bool Circle2D_X_Edge2D(Circle2D & circle, FVec2 & p0, FVec2 & p1)
{
	FVec2 v = p1 - p0;
	float dot0 = (circle.p - p0).Dot(v);
	float dot1 = (circle.p - p1).Dot(v);
	if (dot0 * dot1 < 0)
	{
		Line2D line = MakeLine2DWithP2(p0, p1);
		float d = DistanceToLine2D(line, circle.p);
		return d * d <= circle.r * circle.r;
	}
	return false;
}

bool Circle2D_X_Triangle2D(Circle2D & circle, Triangle2D & tri)
{
	//FVec3 d2[4];
	//d2[0] = tri.p[0] - circle.p;
	//d2[1] = tri.p[1] - circle.p;
	//d2[2] = tri.p[2] - circle.p;
	//d2[0] *= d2[0];
	//d2[1] *= d2[1];
	//d2[2] *= d2[2];

	//_MM_TRANSPOSE4_PS(d2[0].v4, d2[1].v4, d2[2].v4, d2[3].v4);
	//d2[0] += d2[1] + d2[2];
	//__m128 r2 = _mm_set1_ps(circle.r);
	//r2 *= r2;
	//d2[3].v4 = _mm_cmple_ps(d2[0].v4, r2);
	//if (d2[3].v4.m128_u32[0] || d2[3].v4.m128_u32[1] || d2[3].v4.m128_u32[2]) // at least a vertex of triangle is in circle
	//	return true;

	for (int i = 0; i < 3; ++i) {
		if (circle.In(tri.p[i]))
			return true;
	}

	if (Point2DInTriangle2D(tri, circle.p))
		return true;

	// whether does a edge interest with circle
	if (Circle2D_X_Edge2D(circle, tri.p[0], tri.p[1]))
		return true;

	if (Circle2D_X_Edge2D(circle, tri.p[1], tri.p[2]))
		return true;

	if (Circle2D_X_Edge2D(circle, tri.p[2], tri.p[0]))
		return true;

	return false;
}

bool Line2D_X_Line2D(FVec2 & xP, Line2D & l0, Line2D & l1)
{
	if (parallel(l0.n, l1.n))
		return false;

	Matrix22 nMat(l0.n.x(), l1.n.x(), l0.n.y(), l1.n.y());
	nMat.Inverse();
	xP = nMat*FVec2(l0.d, l1.d, 0);
	return true;
}
bool Edge2D_X_Edge2D(FVec2 & xP, Edge2D & e0, Edge2D & e1)
{
	Line2D line0 = MakeLine2DWithP2(e0.p[0], e0.p[1]);
	Line2D line1 = MakeLine2DWithP2(e1.p[0], e1.p[1]);

	float d[4];
	d[0] = distance2(e0.p[0], e1.p[0]);
	d[1] = distance2(e0.p[0], e1.p[1]);
	d[2] = distance2(e0.p[1], e1.p[0]);
	d[3] = distance2(e0.p[1], e1.p[1]);
	std::nth_element(d, d + 2, d + 4);
	if (d[0] < 1e-10f && d[1] < 1e-10f)
		return true;
	else if (d[0] < 1e-10f && d[1] > 1e-10f)
		return false;

	//FVec2 xP;
	if (Line2D_X_Line2D(xP, line0, line1)) {
		FVec2 ev0, ev1;
		ev0 = e0.p[1] - e0.p[0];
		ev1 = e1.p[1] - e1.p[0];
		float ev0l = ev0.Length();
		float ev1l = ev1.Length();

		ev0.Normalize();
		ev1.Normalize();

		float dot0 = (xP - e0.p[0]).Dot(ev0);
		float dot1 = (xP - e1.p[0]).Dot(ev1);
		if (dot0 > 0 && dot0 < ev0l && dot1>0 && dot1 < ev1l)
			return true;
	}

	return false;
}

END_SSE_MATH_NAME