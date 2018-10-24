#ifndef TRIANGLETREE
#define TRIANGLETREE

#include <sseBase.h>
#include <sseBox.h>
#include <sseGraphic.h>

#include <set>

BEGIN_SSE_MATH_NAME

struct TriangleAndBox
{
	Triangle	tri;
	Box			box;
};

class TriangleTreeNode
{
public:
	enum Type {
		kBranch = 0,
		kLeaf = 1
	};
	Type type;
	TriangleTreeNode(const Type _type) :type(_type) {}
};

class TriangleTreeBranch : public TriangleTreeNode
{
public:
	int axis;
	float  d;
	TriangleTreeNode * children[2];

	TriangleTreeBranch():TriangleTreeNode(kBranch) {};
	TriangleTreeBranch(const float _d, const int _axis) :TriangleTreeNode(kBranch) { axis = _axis; d = _d; };
};

class TriangleTreeLeaf : public TriangleTreeNode
{
public:
	std::vector<TriangleAndBox> triAndBoxes;

	TriangleTreeLeaf() :TriangleTreeNode(kLeaf) {};
	TriangleTreeLeaf(std::vector<Triangle> & _tris) :TriangleTreeNode(kLeaf)
	{
		TriangleAndBox triAndBox;
		for (auto & curTri : _tris) {
			triAndBox.box.Reset();
			triAndBox.box.Extend(curTri.p[0]);
			triAndBox.box.Extend(curTri.p[1]);
			triAndBox.box.Extend(curTri.p[2]);
			triAndBox.tri = curTri;
			triAndBoxes.push_back(triAndBox);
		}
	};
};

class TriangleTree
{
public:
	Box rootBox;
	std::vector<TriangleTreeNode*> pNodeList;

	~TriangleTree()
	{
		clear();
	}

	void clear()
	{
		for (auto iter = pNodeList.begin(); iter != pNodeList.end(); ++iter) {
			if ((*iter)->type == TriangleTreeNode::kBranch)
				delete (TriangleTreeBranch*)*iter;
			else
				delete (TriangleTreeLeaf*)*iter;
		}
			
	}

	TriangleTreeNode* rootNode() { return pNodeList[0]; }

	void buildBranch(TriangleTreeBranch * pParNode, std::vector<Triangle>& tris, const int leafContain, const int levelDepth);
	float meanAxisD(std::vector<Triangle>& tris, const int axisId);
	Box calcBox(std::vector<Triangle>& tris);
	TriangleTreeNode* makeANode(std::vector<Triangle>& tris, const int leafContain, const int levelDepth);

	void searchBranch(TriangleTreeNode* pNode, std::vector<Triangle*>& foundTris, Box & box);

	void Build(std::vector<Triangle>& tris,const int leafContain=4,const int levelDepth=5);
	void SearchByBox(std::vector<Triangle*>& foundTris, Box & box);

};


END_SSE_MATH_NAME

#endif
