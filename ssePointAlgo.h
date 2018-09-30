#pragma once

#include <random>
#include <algorithm>
#include <sseBase.h>
#include <sseBox.h>

BEGIN_SSE_MATH_NAME

template<typename VEC>
void KMeanClusterByCentriods(std::vector<std::vector<uint>> & clusterIdList, const std::vector<VEC> & points, std::vector<VEC> & centriods, const uint iterNum)
{
	float minDist, curDist2;
	uint clusterId, curI, pId;

	for (uint iterI = 0; iterI < iterNum; ++iterI) {
		clusterIdList.clear();
		clusterIdList.resize(centriods.size());
		pId = 0;
		for (auto & p : points) {
			minDist = 1e10f;
			curI = 0;
			for (auto & cp : centriods) {
				curDist2 = (p - cp).Length2();
				if (minDist > curDist2) {
					minDist = curDist2;
					clusterId = curI;
				}
				++curI;
			}
			clusterIdList[clusterId].push_back(pId++);
		}

		//caculate centroids
		auto cIter = centriods.begin();
		for (auto & clusterIds : clusterIdList) {
			if (clusterIds.empty()) {
				continue;
			}
			else {
				cIter->SetZero();
				for (auto pId : clusterIds) {
					*cIter += points[pId];
				}
				*cIter *= 1.0f / (float)clusterIds.size();
			}
			++cIter;
		}
	}

	for (auto iter = clusterIdList.begin(); iter != clusterIdList.end(); ) {
		if (iter->empty())
			iter = clusterIdList.erase(iter);
		else
			++iter;
	}
}

template<typename VEC>
void KMeanCluster(std::vector<std::vector<uint>> & clusterIdList, const std::vector<VEC> & points, const uint k, 
	std::vector<float> &weightList,const uint iterNum=3)
{
	if (k >= points.size()) {
		clusterIdList.resize(points.size());
		for (uint i = 0; i < clusterIdList.size(); ++i) {
			clusterIdList[i].push_back(i);
		}
		return;
	}

	float totalW=0;
	for (auto w : weightList) {
		totalW += w;
	}
	
	uint pointNum = (uint)points.size();
	float fPointNum = (float)pointNum;
	float fOnePointPiece = 1.0f / fPointNum;

	totalW = 1.0f / totalW;
	for (auto & w : weightList) {
		w *= totalW;
	}

	std::vector<VEC> centriods;
	VEC cp;
	cp = points[0];
	std::vector<int> clusterPointNums;
	int curNum,accNum = 0;
	float weightRemainder = 0, fCurNum;
	for (uint i = 0; i < k; ++i) {
		fCurNum = fPointNum*weightList[i] + weightRemainder;
		weightRemainder = fCurNum - floorf(fCurNum);
		curNum = clamp<int>((int)(fCurNum), 0, pointNum - k);
		if (curNum == 0)
			continue;

		clusterPointNums.push_back(curNum);
		accNum += curNum;
	}
	accNum = std::max<int>(0,pointNum - accNum);
	clusterPointNums.back() += accNum;

	std::vector<uint> idList;
	for (uint i = 0; i < points.size(); ++i) {
		idList.push_back(i);
	}

	clusterIdList.clear();
	clusterIdList.resize(clusterPointNums.size());
	std::vector<uint>::iterator beginIter = idList.begin();
	uint ck=(uint)clusterPointNums.size();
	for (uint i = 0; i < ck; ++i) {
		uint curClusterPointNum = clusterPointNums[i];

		std::nth_element(beginIter, beginIter + curClusterPointNum, idList.end(), [&](const uint a,const uint b) {
			return (cp - points[a]).Length2() < (cp - points[b]).Length2();
		});

		for (uint j = 0; j < curClusterPointNum; ++j) {
			clusterIdList[i].push_back(*(beginIter+j));
		}
		
		//cp.SetZero();
		//for (uint j = 0; j < curClusterPointNum; ++j) {
		//	cp += points[idList[*(beginIter + j)]];
		//}
		//cp /= (float)curClusterPointNum;
		//
		//centriods.push_back(cp);
		if (i < ck - 1) {
			beginIter += curClusterPointNum;
			cp = points[*beginIter];
		}
	}

	//KMeanClusterByCentriods(clusterIdList, points, centriods, iterNum);
}


//template<typename VEC>
//void KMeanCluster(std::vector<std::vector<uint>> & clusterIdList, const std::vector<VEC> & points, const uint k, const uint iterNum)
//{
//	if (k >= points.size()) {
//		clusterIdList.resize(points.size());
//		for (uint i = 0; i < clusterIdList.size(); ++i) {
//			clusterIdList[i].push_back(i);
//		}
//		return;
//	}
//	std::default_random_engine generator;
//	std::uniform_real_distribution<float> dis(0, 1);
//
//	Box box;
//	for (auto & p : points)
//		box.Extend(p);
//
//
//	auto RadomPoint = [&]() {
//		VEC cp;
//		cp.x() = dis(generator);
//		cp.y() = dis(generator);
//		cp.z() = dis(generator);
//		return Lerp(box.min, box.max, cp);
//	};
//
//	std::vector<VEC> centriods;
//	for (uint i = 0; i < k; ++i) {
//		centriods.push_back(RadomPoint());
//	}
//
//	float minDist, curDist2;
//	uint clusterId, curI, pId, cpId;
//
//	for (uint iterI = 0; iterI < iterNum; ++iterI) {
//		clusterIdList.clear();
//		clusterIdList.resize(centriods.size());
//		pId = 0;
//		for (auto & p : points) {
//			minDist = 1e10f;
//			curI = 0;
//			for (auto & cp : centriods) {
//				curDist2 = (p - cp).Length2();
//				if (minDist > curDist2) {
//					minDist = curDist2;
//					clusterId = curI;
//				}
//				++curI;
//			}
//			clusterIdList[clusterId].push_back(pId++);
//		}
//
//		//caculate centroids
//		centriods.clear();
//		for (auto & clusterIds : clusterIdList) {
//			if (clusterIds.empty()) {
//				centriods.push_back(RadomPoint());
//			}
//			else {
//				VEC cp;
//				for (auto pId : clusterIds) {
//					cp += points[pId];
//				}
//				cp *= 1.0f / (float)clusterIds.size();
//				centriods.push_back(cp);
//			}
//		}
//	}
//
//	for (auto iter = clusterIdList.begin(); iter != clusterIdList.end(); ) {
//		if (iter->empty())
//			iter = clusterIdList.erase(iter);
//		else
//			++iter;
//	}
//}

END_SSE_MATH_NAME