#include <sseTriangleScatter.h>

BEGIN_SSE_MATH_NAME
void UniformScatterPointsOnTriangle3DList(std::vector<FVec3>& outPoints, const std::vector<Triangle>& tris, const u_int pointCount)
{
	std::vector<float> areaList;
	float totalArea = 0, curArea;
	for (auto & curTri : tris) {
		sseMath::Matrix3DTo2D mat322 = Mat322FromTriangle(curTri);
		curArea = curTri.Area();
		totalArea += curArea;
		areaList.push_back(curArea);
	}

	totalArea = (float)pointCount / totalArea;
	int i = 0;
	u_int locPointCount;
	std::vector<FVec3> pointList;
	FVec3 weight3;
	auto triIter = tris.begin();
	for (auto & curArea : areaList) {
		locPointCount = (u_int)roundf(curArea*totalArea);
		if (locPointCount > 0) {
			pointList.clear();
			UniformScatterPointsOnTriangle<FVec3,Triangle>(pointList, *triIter, locPointCount);
			for (auto & finP : pointList) {
				outPoints.push_back(finP);
			}
		}
		++triIter;
	}
}
//void UniformScatterPointsOnTriangle3DList(std::vector<FVec3>& outPoints, const std::vector<Triangle>& tris, const u_int pointCount) 
//{
//	std::vector<Triangle2D> tri2DList;
//	std::vector<float> areaList;
//	float totalArea=0, curArea;
//	for (auto & curTri : tris) {
//		sseMath::Matrix3DTo2D mat322 = Mat322FromTriangle(curTri);
//		sseMath::Triangle2D tri2D = ProjTriangleFrom3D22D(mat322, curTri);
//		tri2DList.push_back(tri2D);
//		curArea = tri2D.Area();
//		totalArea += curArea;
//		areaList.push_back(curArea);
//	}
//
//	totalArea = (float)pointCount / totalArea;
//	int i = 0;
//	u_int locPointCount;
//	std::vector<FVec2> point2DList;
//	FVec3 weight3,finP;
//	auto tri2DIter = tri2DList.begin();
//	auto triIter = tris.begin();
//	for (auto & curArea : areaList) {
//		locPointCount = (u_int)roundf(curArea*totalArea);
//		if (locPointCount > 0) {
//			point2DList.clear();
//			UniformScatterPointsOnTriangle<Triangle>(point2DList, *tri2DIter, locPointCount);
//			for (auto & p2d : point2DList) {
//				weight3 = tri2DIter->Weight3(p2d);
//				finP = triIter->WeightPoint(weight3);
//				outPoints.push_back(finP);
//			}
//		}
//		++tri2DIter;
//		++triIter;
//	}
//}
END_SSE_MATH_NAME