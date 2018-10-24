#include <sseTriangleTree.h>

BEGIN_SSE_MATH_NAME

float TriangleTree::meanAxisD(std::vector<Triangle>& tris, const int axisId)
{
	float inv = 1.f / (float)tris.size();
	float d = 0;
	for (auto tri : tris)
	{
		d += tri.MeanD(axisId)*inv;
	}
	return d;
}

Box TriangleTree::calcBox(std::vector<Triangle>& tris)
{
	Box box;
	for (auto tri : tris)
	{
		box.Extend(tri.p[0]);
		box.Extend(tri.p[1]);
		box.Extend(tri.p[2]);
	}
	return box;
}

TriangleTreeNode* TriangleTree::makeANode(std::vector<Triangle>& tris, const int leafContain, const int levelDepth)
{
	TriangleTreeNode* pRootNode;
	if (tris.size() > leafContain && levelDepth>0) {
		Box cbox = calcBox(tris);
		int axisId = cbox.MaxAxis();
		float d = meanAxisD(tris, axisId);
		pRootNode = new TriangleTreeBranch(d, axisId);
		pNodeList.push_back(pRootNode);
	}
	else {
		pRootNode = new TriangleTreeLeaf(tris);
		pNodeList.push_back(pRootNode);
	}
	return pRootNode;
}

void TriangleTree::buildBranch(TriangleTreeBranch * pParNode, std::vector<Triangle>& tris, const int leafContain, const int levelDepth)
{
	std::vector<Triangle> leftTris, rightTris;
	for (Triangle tri : tris)
	{
		FVec3 xp[2];
		int sortedId[3];
		AAPlane_X_Triangle_Type side = AAPlane_X_Triangle(xp, sortedId, tri, pParNode->d, pParNode->axis);
		if (side == AAPlane_X_Triangle_Type::kLeft)
			leftTris.push_back(tri);
		else if (side == AAPlane_X_Triangle_Type::kRight)
			rightTris.push_back(tri);
		else {
			Triangle cutTris[3];
			Cut3TrianglesByX(cutTris, xp, sortedId, tri);
			if (side == AAPlane_X_Triangle_Type::k1X2) {
				if(cutTris[0].Area()>1e-10f)
					leftTris.push_back(cutTris[0]);

				if (cutTris[1].Area()>1e-10f)
					rightTris.push_back(cutTris[1]);
				
				if (cutTris[2].Area()>1e-10f)
					rightTris.push_back(cutTris[2]);
			}
			else {
				if (cutTris[0].Area()>1e-10f)
					rightTris.push_back(cutTris[0]);

				if (cutTris[1].Area()>1e-10f)
					leftTris.push_back(cutTris[1]);

				if (cutTris[2].Area()>1e-10f)
					leftTris.push_back(cutTris[2]);
			}
		}
	}

	pParNode->children[0] = makeANode(leftTris, leafContain, levelDepth);
	pParNode->children[1] = makeANode(rightTris, leafContain, levelDepth);

	if (pParNode->children[0]->type == TriangleTreeNode::kBranch)
		buildBranch((TriangleTreeBranch*)pParNode->children[0], leftTris, leafContain, levelDepth - 1);

	if (pParNode->children[1]->type == TriangleTreeNode::kBranch)
		buildBranch((TriangleTreeBranch*)pParNode->children[1], rightTris, leafContain, levelDepth - 1);
}

void TriangleTree::Build(std::vector<Triangle>& tris, const int leafContain, const int levelDepth)
{
	clear();

	rootBox = calcBox(tris);
	TriangleTreeNode * pRootNode = makeANode(tris, leafContain, levelDepth);
	if (pRootNode->type == TriangleTreeNode::kBranch)
		buildBranch((TriangleTreeBranch*)pRootNode, tris, leafContain, levelDepth - 1);
}
void TriangleTree::SearchByBox(std::vector<Triangle*>& foundTris, Box & box)
{
	if (!rootBox.In(box))
		return;

	auto pNode = rootNode();
	foundTris.clear();
	searchBranch(pNode, foundTris, box);
}

void TriangleTree::searchBranch(TriangleTreeNode* pNode, std::vector<Triangle*>& foundTris, Box & box)
{
	if (pNode->type == TriangleTreeNode::kLeaf) {
		TriangleTreeLeaf * pLeaf = (TriangleTreeLeaf*)pNode;
		for (auto & triAndBox : pLeaf->triAndBoxes)
			if(triAndBox.box.In(box))
				foundTris.push_back(&triAndBox.tri);
		return;
	}

	// branch
	TriangleTreeBranch * pBranch = (TriangleTreeBranch*)pNode;
	auto side = box.X_AAPlane(pBranch->d, pBranch->axis);
	if (side == Box::kBoth) {
		searchBranch(pBranch->children[0], foundTris, box);
		searchBranch(pBranch->children[1], foundTris, box);
	}
	else if (side == Box::kLeft)
		searchBranch(pBranch->children[0], foundTris, box);
	else
		searchBranch(pBranch->children[1], foundTris, box);
}

END_SSE_MATH_NAME