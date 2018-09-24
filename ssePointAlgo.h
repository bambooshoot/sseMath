#pragma once

#include <random>
#include <sseBase.h>
#include <sseBox.h>

BEGIN_SSE_MATH_NAME

template<typename VEC>
void KMeanCluster(std::vector<std::vector<uint>> & clusterIdList, const std::vector<VEC> & points, const uint k, const uint iterNum)
{
	if (k >= points.size()) {
		clusterIdList.resize(points.size());
		for (uint i = 0; i < clusterIdList.size(); ++i) {
			clusterIdList[i].push_back(i);
		}
		return;
	}
<<<<<<< HEAD

	std::vector<VEC> centriods;
	VEC cp = points[0];
	uint clusterPointNum = points.size() / k;
	uint lastClusterPointNum = points.size() - clusterPointNum*(k-1);
	std::vector<uint> idList;
	for (uint i = 0; i < points.size(); ++i) {
		idList.push_back(i);
	}

	std::vector<uint>::iterator beginIter = idList.begin();
	for (uint i = 0; i < k; ++i) {
		uint curClusterPointNum = (i < k - 1) ? clusterPointNum : lastClusterPointNum;

		std::nth_element(beginIter, beginIter + curClusterPointNum, idList.end(), [&](const uint a,const uint b) {
			return (cp - points[a]).Length2() < (cp - points[b]).Length2();
		});
		
		cp.SetZero();
		for (uint j = 0; j < curClusterPointNum; ++j) {
			cp += points[idList[*(beginIter + j)]];
		}
		cp /= (float)curClusterPointNum;
		centriods.push_back(cp);
		if (i < k - 1) {
			beginIter += curClusterPointNum;
			cp = points[*beginIter];
		}
	}

	float minDist,curDist2;
	uint clusterId, curI, pId;
=======
	std::default_random_engine generator;
	std::uniform_real_distribution<float> dis(0, 1);

	Box box;
	for (auto & p : points)
		box.Extend(p);


	auto RadomPoint = [&]() {
		VEC cp;
		cp.x() = dis(generator);
		cp.y() = dis(generator);
		cp.z() = dis(generator);
		return Lerp(box.min, box.max, cp);
	};

	std::vector<VEC> centriods;
	VEC cp;
	for (uint i = 0; i < k; ++i) {
		centriods.push_back(RadomPoint());
	}

	float minDist,curDist2;
	uint clusterId, curI, pId, cpId;
>>>>>>> 1ad6131e6877d64f61c717839a46d35027de7a43
	
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
<<<<<<< HEAD
		centriods.clear();
		for (auto & clusterIds : clusterIdList) {
			if (clusterIds.empty()) {
				continue;
			}
			else {
				VEC cp;
				for (auto pId : clusterIds) {
					cp += points[pId];
				}
=======
		cpId = 0;
		centriods.clear();
		for (auto & clusterIds : clusterIdList) {
			for (auto pId : clusterIds) {
				cp += points[pId];
			}

			if (clusterIds.empty()) {
				centriods.push_back(RadomPoint());
			}
			else {
>>>>>>> 1ad6131e6877d64f61c717839a46d35027de7a43
				cp *= 1.0f / (float)clusterIds.size();
				centriods.push_back(cp);
			}
		}
	}

<<<<<<< HEAD
	auto cpIter = centriods.begin();
	for (auto iter = clusterIdList.begin(); iter != clusterIdList.end(); ) {
		if (iter->empty()) {
			iter = clusterIdList.erase(iter);
			cpIter = centriods.erase(cpIter);
		}
		else
			++iter;
	}

	cpIter = centriods.begin();
	for (auto iter = clusterIdList.begin(); iter != clusterIdList.end(); ++iter, ++cpIter) {
		std::nth_element(iter->begin(), iter->begin() + 1, iter->end(), [&](const uint a, const uint b) {
			return (*cpIter - points[a]).Length2() < (*cpIter - points[b]).Length2();
		});
	}
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

=======
	for (auto iter = clusterIdList.begin(); iter != clusterIdList.end(); ) {
		if (iter->empty())
			iter = clusterIdList.erase(iter);
		else
			++iter;
	}
}

>>>>>>> 1ad6131e6877d64f61c717839a46d35027de7a43
END_SSE_MATH_NAME