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
				cp *= 1.0f / (float)clusterIds.size();
				centriods.push_back(cp);
			}
		}
	}

	for (auto iter = clusterIdList.begin(); iter != clusterIdList.end(); ) {
		if (iter->empty())
			iter = clusterIdList.erase(iter);
		else
			++iter;
	}
}

END_SSE_MATH_NAME