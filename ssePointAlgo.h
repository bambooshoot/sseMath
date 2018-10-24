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
	std::vector<uint> clusterPointNums;
	totalW = 1.0f / totalW;
	float remainder = 0;
	uint pointNum2 = 0;
	for (auto & w : weightList) {
		w *= totalW;
		float fCurPointNum = w*pointNum + remainder;
		float curPointNum = floorf(fCurPointNum);
		remainder = fCurPointNum - curPointNum;
		uint uCurPointNum = (uint)curPointNum;
		pointNum2 += uCurPointNum;
		if (pointNum2 <= pointNum)
			clusterPointNums.push_back(uCurPointNum);
		else
			clusterPointNums.push_back(uCurPointNum - (pointNum2 - pointNum));
	}

	std::vector<VEC> centriods;
	VEC cp;
	cp = points[0];

	std::vector<uint> idList;
	for (uint i = 0; i < points.size(); ++i) {
		idList.push_back(i);
	}

	uint cpNum = (uint)clusterPointNums.size();
	std::vector<uint>::iterator beginIter = idList.begin();

	for (uint i = 0; i < cpNum; ++i) {
		if (i < cpNum - 1) {
			std::nth_element(beginIter, beginIter + clusterPointNums[i], idList.end(), [&](const uint id0, const uint id1) {
				return (points[id0] - cp).Length() < (points[id1] - cp).Length();
			});
			
			cp.SetZero();
			for (auto idIter = beginIter; idIter != beginIter + clusterPointNums[i]; ++idIter) {
				cp += points[*idIter];
			}
			cp /= (float)clusterPointNums[i];
			centriods.push_back(cp);

			beginIter += clusterPointNums[i];
			cp = points[*beginIter];
		}
		else {
			cp.SetZero();
			for (auto idIter = beginIter; idIter != idList.end(); ++idIter) {
				cp += points[*idIter];
			}
			cp /= (float)clusterPointNums[i];
			centriods.push_back(cp);
		}
	}

	clusterIdList.clear();
	KMeanClusterByCentriods(clusterIdList, points, centriods, iterNum);
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