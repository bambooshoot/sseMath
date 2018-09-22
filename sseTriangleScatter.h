#pragma once

#include <sseBase.h>
#include <sseVec.h>
#include <sseMatrix.h>
#include <sseGraphic.h>
#include <sseGraphic2D.h>
#include <vector>

BEGIN_SSE_MATH_NAME

template<typename VEC,typename TRI>
void UniformScatterPointsOnTriangle(std::vector<VEC>& outPoints, const TRI & tri, const u_int pointCount)
{
	if (pointCount == 1) {
		outPoints.push_back(tri.MeanP());
		return;
	}

	u_int leftPointCount = pointCount / 2;
	u_int rightPointCount = pointCount - leftPointCount;
	float leftRatio = (float)leftPointCount / (float)pointCount;
	u_int maxEdgeId = tri.MaxEdge();
	TRI tris[2];
	tri.EdgeSplit2Triangles(tris, maxEdgeId, leftRatio);
	UniformScatterPointsOnTriangle(outPoints, tris[0], leftPointCount);
	UniformScatterPointsOnTriangle(outPoints, tris[1], rightPointCount);
}
void UniformScatterPointsOnTriangle3DList(std::vector<FVec3>& outPoints, const std::vector<Triangle>& tris, const u_int pointCount);

END_SSE_MATH_NAME