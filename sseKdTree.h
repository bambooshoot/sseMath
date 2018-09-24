#ifndef SSEKDTREE
#define SSEKDTREE

#include <functional>
#include <vector>
#include <algorithm>
#include <memory>
#include <FloatSort.h>

#include <sseBase.h>
#include <sseVec.h>
#include <sseBox.h>
#include <sseMatrix.h>

BEGIN_SSE_MATH_NAME

class KdTreeNode
{
public:
	u_int dim;
	u_int dataId,childrenId[2];

	KdTreeNode() : dim(0), dataId(0) { memset(childrenId,0,sizeof(u_int)*2); };
	KdTreeNode(u_int _dim, u_int _id)
	: dim(_dim), dataId(_id){ memset(childrenId, 0, sizeof(u_int) * 2); };
	//bool IsLeaf() { return childrenId[0] == 0 && childrenId[1]; }
};

using KdTreeNodePointer = std::shared_ptr<KdTreeNode>;

template <class Vec>
class KdTree
{
public:
	Box box;
	std::vector<KdTreeNodePointer> pNodeList;

	~KdTree() { Clear(); };
	void Clear()
	{
		pNodeList.clear();

		box.Reset();
	}
	void addANode(const u_int _parentId, const u_int _dim, const u_int _dataId, const u_int _chidSide) 
	{
		KdTreeNodePointer pNode(new KdTreeNode(_dim, _dataId));
		if (_parentId != UINT_MAX)
			pNodeList[_parentId]->childrenId[_chidSide] = pNodeList.size();

		pNodeList.push_back(pNode);
	}
	void Build(std::vector<Vec> & pList)
	{
		Clear();

		std::vector<u_int> idList(pList.size());
		u_int i = 0;
		for (u_int & ii : idList)
			ii = i++;

		for (auto p : pList) {
			box.Extend(p);
		}

		struct StackParam
		{
			u_int beginId;
			u_int endId;
			u_int parentId;
			u_int childSide;
			Box   box;
			StackParam(	const u_int _beginId,
						const u_int _endId,
						const u_int _parentId,
						const u_int _childSide,
						const Box   & _box) 
			: beginId(_beginId), endId(_endId), parentId(_parentId), childSide(_childSide) { box = _box; };
		};

		std::vector< StackParam > paramStack;
		paramStack.push_back( StackParam(0, (u_int)idList.size(), UINT_MAX, 0, box) );

		std::vector<u_int> parentIdList;

		//FILE* fileId = fopen("d:/test.txt", "w");

		while (paramStack.size()>0) {
			StackParam param = paramStack.back();	// please note param here can not be reference variable.  
													// otherwise below line would get wrong data after pop_back

			u_int dimId = param.box.MaxAxis();

			u_int curIdListLen = param.endId - param.beginId;
			if (curIdListLen == 1) {
				addANode(param.parentId, dimId, idList[param.beginId], param.childSide);
				paramStack.pop_back();
				continue;
			}

			auto beginIter = idList.begin() + param.beginId;
			auto endIter = idList.begin() + param.endId;

			std::sort(beginIter, endIter, [&](u_int & a,u_int & b) {
				return pList[a][dimId] < pList[b][dimId];
			});
			
			if (curIdListLen == 2) {
				addANode(param.parentId,	 dimId, idList[param.beginId],   param.childSide);
				addANode(pNodeList.size()-1, dimId, idList[param.beginId+1], 1);
				paramStack.pop_back();
				continue;
			}
			else if (curIdListLen == 3) {
				addANode(param.parentId, dimId, idList[param.beginId+1], param.childSide);
				u_int parId = pNodeList.size()-1;
				addANode(parId, dimId, idList[param.beginId], 0);
				addANode(parId, dimId, idList[param.beginId+2], 1);
				paramStack.pop_back();
				continue;
			}

			u_int curMidId = curIdListLen / 2;

			//fprintf(fileId,"beginid %i endId %i\n",param.beginId,param.endId);
			//fflush(fileId);

			//add a node
			u_int gMidId = curMidId + param.beginId;
			u_int curNodeId = pNodeList.size();
			addANode(param.parentId, dimId, idList[gMidId], param.childSide);
			Box lBox, rBox;
			param.box.AxisSplit(lBox,rBox,pList[gMidId][dimId],dimId);
			paramStack.pop_back(); // here it is

			paramStack.push_back(StackParam(param.beginId,   gMidId, curNodeId, 0, lBox));
			paramStack.push_back(StackParam(gMidId + 1, param.endId, curNodeId, 1, rBox));
		}
		//fclose(fileId);
	}
	void Search(std::vector<u_int> & foundIdList, Vec p, const float radius, const std::vector<Vec> & pList)
	{
		foundIdList.clear();
		Box boxWithRad=box;
		
		for (int i = 0; i < 3; ++i) {
			boxWithRad.min[i] -= radius;
			boxWithRad.max[i] += radius;
		}
			
		if (!boxWithRad.In(p))
			return;

		for (int i = 0; i < 3; ++i) {
			boxWithRad.min[i] = p.cValue(i) - radius;
			boxWithRad.max[i] = p.cValue(i) + radius;
		}

		std::vector<u_int> cadidateIdList;
		std::vector<u_int> nodeIdList;
		nodeIdList.push_back(0);

		std::vector<Box> boxList;
		boxList.push_back(box);
		while (nodeIdList.size() > 0) {
			u_int curNodeId=nodeIdList.back();
			KdTreeNodePointer pCurNode = pNodeList[curNodeId];
			Vec curNodePos = pList[pCurNode->dataId];
			if (boxWithRad.In(curNodePos))
				cadidateIdList.push_back(pCurNode->dataId);

			Box & curBox = boxList.back();
			FloatSort<u_int> fSort;
			u_int dimId = pCurNode->dim;
			
			float nodeSplittingValue = curNodePos[dimId];
			float curSampleValue = p[dimId];
			Box lBox, rBox;
			curBox.AxisSplit(lBox, rBox, nodeSplittingValue, dimId);

			nodeIdList.pop_back();
			boxList.pop_back();

			if (pCurNode->childrenId[0] != 0) {
				if (boxWithRad.In(lBox)) {
					nodeIdList.push_back(pCurNode->childrenId[0]);
					boxList.push_back(lBox);
				}
			}

			if (pCurNode->childrenId[1] != 0) {
				if (boxWithRad.In(rBox)) {
					nodeIdList.push_back(pCurNode->childrenId[1]);
					boxList.push_back(rBox);
				}
			}
		}
		
		float radius2 = radius*radius;
		for (u_int id : cadidateIdList) {
			if ((p - pList[id]).Length2() < radius2)
				foundIdList.push_back(id);
		}
	}
};

template<typename VEC>
const int SearchNearestPoint(KdTree<VEC> p_tree, const VEC & p, const float radius, const std::vector<VEC> pList)
{
	std::vector<u_int> idList;
	p_tree.Search(idList, p, radius, pList);
	if (idList.empty())
		return -1;

	auto iter = std::min_element(idList.begin(), idList.end(), [&](const u_int a, const u_int b) {
		return (p - pList[a]).Length2() < (p - pList[b]).Length2();
	});
	return *iter;
}

END_SSE_MATH_NAME

#endif