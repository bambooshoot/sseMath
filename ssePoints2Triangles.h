#ifndef SSEPOINTS2TRIANGLES
#define SSEPOINTS2TRIANGLES

#include <sseBase.h>
#include <sseVec.h>
#include <sseMatrix.h>

#include <math.h>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <algorithm>
#include <FloatSort.h>
#include <sseGraphic2D.h>
#include <sseKdTree.h>

BEGIN_SSE_MATH_NAME

struct EdgeId
{
	u_int id[2];
	EdgeId() {};
	EdgeId(const u_int a, const u_int b) { id[0] = a; id[1] = b; };
};

struct TriangleId
{
	u_int id[3];
	TriangleId() {};
	TriangleId(const u_int a, const u_int b,const u_int c) { id[0] = a; id[1] = b; id[2] = c; };
	friend bool operator == (const TriangleId &a, const TriangleId &b) {
		bool flag=true;
		for (u_int bId : b.id)
			flag &= std::find(a.id, a.id + 3, bId) != a.id+3;
		return flag;
	}
};

FVec3 NormalFittingByPoints(std::vector<u_int> & idList,
							std::vector<FVec3> & pList)
	// plane function cx+by+az+d=0
	// divid d on both sides the function is equal to c/d*x+b/d*y+c/d*z+1=0
	// let A=a/d B=b/d C=c/d the function becomes Ax+By+Cz+1=0
	// Through above transfering variable d is hidden behind A, B and C
	// and the problem would be solved by least-squared with Matrix 3x3
{
	FVec3 v;
	for (u_int id : idList) {
		v += pList[id];
	}

	Matrix33 mat;
	
	FVec3 v0 = v*v.x();
	mat.SetVec(v0, 0);
	
	v0 = v*v.y();
	mat.SetVec(v0, 1);

	v0 = v*v.z();
	mat.SetVec(v0, 2);

	mat.Inverse();

	return mat*v;
}

void MakeTrianglesWithPoints(	std::vector<EdgeId> & e2List,
								std::vector<u_int> & idList, 
								std::vector<FVec3> & pList,
								const u_int samplePId)
{
	//FVec3 n = NormalFittingByPoints(idList, pList);
	FVec3 n=pList[samplePId];
	n.Normalize();
	Matrix3DTo2D mat32 = Mat322FromNormal(n);

	std::map<u_int,FVec2> p2Map;

	for (u_int id:idList) {
		p2Map[id] = mat32*pList[id];
	}

	std::list<EdgeId> e2List2;
	Edge2D edge;
	for (u_int id : idList) {
		FVec2 curP2 = p2Map[id];
		for (u_int id2 : idList) {
			if (id == id2)
				continue;

			edge.p[0] = curP2;
			edge.p[1] = p2Map[id2];
			bool xFlag = false;
			for ( auto eIter = e2List2.begin();
				eIter != e2List2.end();
				++eIter ) {

				FVec2 xp;
				if (Edge2D_X_Edge2D(xp, edge, Edge2D(p2Map[eIter->id[0]], p2Map[eIter->id[1]]))) {
					xFlag = true;
					break;
				}
			}

			if (xFlag)
				continue;

			e2List2.push_back(EdgeId(id, id2));
		}
	}

	for (auto edgeId : e2List2) {
		e2List.push_back(edgeId);
	}
}

void Points2Triangles(	std::vector<EdgeId> & e2List,
						std::vector<TriangleId> & triIds,
						std::vector<FVec3> & pList,
						const float radius)
{
	triIds.clear();
	std::vector<EdgeId> edgeIdList;

	KdTree<FVec3> tree;
	tree.Build(pList);

	std::set<u_int> pIdSet;
	for (int i = 0; i < pList.size(); ++i)
		pIdSet.insert(i);

	while(pIdSet.size()>0){
		u_int sampleId = *pIdSet.begin();
		FVec3 curP = pList[sampleId];
		std::vector<u_int> foundIdList;
		tree.Search(foundIdList, curP, radius, pList);

		if (foundIdList.size() == 0)
			continue;

		// local triangulation
		MakeTrianglesWithPoints(e2List, foundIdList,pList, sampleId);

		for (u_int id : foundIdList) {
			pIdSet.erase(id);
		}
	}

	//std::map< u_int, std::vector<u_int> > ueMap;
	//for (EdgeId & edgeId : e2List) {
	//	ueMap[edgeId.id[0]].push_back(edgeId.id[1]);
	//	ueMap[edgeId.id[1]].push_back(edgeId.id[0]);
	//}

	//std::vector<TriangleId> triIdList;
	//for (auto & idPair : ueMap) {
	//	u_int id0 = idPair.first;
	//	for (u_int id1 : idPair.second) {
	//		for (u_int id2 : ueMap[id1]) {
	//			for (u_int id3 : ueMap[id2]) {
	//				if (id0 == id3) {
	//					TriangleId triId(id0, id1, id2);
	//					//if( std::find(triIdList.begin(), triIdList.end(), triId) == triIdList.end() )
	//					triIdList.push_back(triId);
	//				}
	//			}
	//		}
	//	}
	//}

	//for (auto & triId : triIdList) {
	//	locTriIds.push_back(triId);
	//}
};

END_SSE_MATH_NAME

#endif