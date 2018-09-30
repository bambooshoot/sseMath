#ifndef GLGRAPHIC
#define GLGRAPHIC

#include <glut.h>
#include <sseGraphic.h>
#include <sseTriangleTree.h>
#include <ImathRandom.h>

BEGIN_SSE_MATH_NAME

void DrawPoint(FVec3 & p)
{
	glBegin(GL_POINTS);
	glVertex3f(p[0],p[1],p[2]);
	glEnd();
}

void DrawPoint2D(FVec2 & p)
{
	glBegin(GL_POINTS);
	glVertex2f(p[0], p[1]);
	glEnd();
}

void DrawPlane(Plane & pln,const float width,const int gridSubdiv)
{
	FVec3 yAxis(0.0f, 1.0f, 0.0f, 0.0f);
	if (yAxis.Dot(pln.n))
		yAxis.SetVector(1,0,0);
	FVec3 xAxis=pln.n.Cross(yAxis);
	xAxis.Normalize();
	FVec3 zAxis = xAxis.Cross(yAxis);
	zAxis.Normalize();
	TransformMatrix3D tMat;
	tMat.SetAxis(xAxis, 0);
	tMat.SetAxis(yAxis, 1);
	tMat.SetAxis(zAxis, 2);
	tMat.SetTranslate(pln.n*pln.d);
	float * pfv = new float[3 * gridSubdiv * gridSubdiv];
	float gridStep = width / (gridSubdiv - 1);
	FVec3 curP;
	for (int zi = 0; zi < gridSubdiv; ++zi) {
		for (int xi = 0; xi < gridSubdiv; ++xi) {
			curP.SetPoint(-width / 2 + gridStep*xi, -width /2 + gridStep*zi, 0);
			curP *= tMat;
			float &x = pfv[3 * (xi + gridSubdiv*zi)];
			float &y = pfv[3 * (xi + gridSubdiv*zi) + 1];
			float &z = pfv[3 * (xi + gridSubdiv*zi) + 2];
			curP.Get(x, y, z);
		}
	}
	
	for (int zi = 0,i=0; zi < gridSubdiv; ++zi) {
		glBegin(GL_LINE_STRIP);
		for (int xi = 0; xi < gridSubdiv; ++xi,++i) {
			glVertex3f(pfv[3 * i], pfv[3 * i + 1], pfv[3 * i + 2]);
		}
		glEnd();
	}

	for (int xi = 0; xi < gridSubdiv; ++xi) {
		glBegin(GL_LINE_STRIP);
		for (int yi = 0; yi < gridSubdiv; ++yi) {
			int i = yi * gridSubdiv + xi;
			glVertex3f(pfv[3 * i], pfv[3 * i + 1], pfv[3 * i + 2]);
		}
		glEnd();
	}

	delete[] pfv;
}

void DrawSphere(Sphere & sph)
{
	glPushMatrix();
	glTranslatef(sph.p.x(), sph.p.y(), sph.p.z());
	glutWireSphere(sph.r,16,16);
	glPopMatrix();
}

void DrawTriangle(Triangle & tri)
{
	glBegin(GL_LINE_LOOP);
	glVertex3f(tri.p[0].x(), tri.p[0].y(), tri.p[0].z());
	glVertex3f(tri.p[1].x(), tri.p[1].y(), tri.p[1].z());
	glVertex3f(tri.p[2].x(), tri.p[2].y(), tri.p[2].z());
	glEnd();

}

void DrawLine2D(Line2D & line)
{
	TransformMatrix2D tMat;
	tMat.SetAxis(FVec2(line.n.y(), -line.n.x(), 0), 0);
	tMat.SetAxis(line.n, 1);
	tMat.SetTranslate(line.n*line.d);

	FVec2 p0(-1, 0, 1);
	FVec2 p1(1, 0, 1);
	p0 *= tMat;
	p1 *= tMat;

	glBegin(GL_LINES);
	glVertex2f(p0.x(), p0.y());
	glVertex2f(p1.x(), p1.y());
	glEnd();
}

void DrawEdge2D(Edge2D & e)
{
	glBegin(GL_LINES);
	glVertex2f(e.p[0][0], e.p[0][1]);
	glVertex2f(e.p[1][0], e.p[1][1]);
	glEnd();
}

void DrawCircle2D(Circle2D & circle)
{
	glPushMatrix();
	glTranslatef(circle.p.x(), circle.p.y(), circle.p.z());
	const float segment = 36;
	const float step = PI2 / segment;

	glBegin(GL_LINE_LOOP);
	for (float fi = 0; fi < segment; ++fi)
		glVertex2f(cosf(fi*step)*circle.r, sinf(fi*step)*circle.r);
	glEnd();
	glPopMatrix();
}

void DrawTriangle2D(Triangle2D & tri)
{
	glBegin(GL_LINE_LOOP);
	glVertex2f(tri.p[0].x(), tri.p[0].y());
	glVertex2f(tri.p[1].x(), tri.p[1].y());
	glVertex2f(tri.p[2].x(), tri.p[2].y());
	glEnd();
}

void DrawBox(Box & box)
{
	FVec3 & min = box.min;
	FVec3 & max = box.max;
	FVec3 fBoxP[8] = {	FVec3(min[0],min[1],min[2]),
						FVec3(max[0],min[1],min[2]),
						FVec3(max[0],max[1],min[2]),
						FVec3(min[0],max[1],min[2]),

						FVec3(min[0],min[1],max[2]),
						FVec3(max[0],min[1],max[2]),
						FVec3(max[0],max[1],max[2]),
						FVec3(min[0],max[1],max[2]) };

	glBegin(GL_LINE_LOOP);
	glVertex3f(fBoxP[0][0], fBoxP[0][1], fBoxP[0][2]);
	glVertex3f(fBoxP[1][0], fBoxP[1][1], fBoxP[1][2]);
	glVertex3f(fBoxP[2][0], fBoxP[2][1], fBoxP[2][2]);
	glVertex3f(fBoxP[3][0], fBoxP[3][1], fBoxP[3][2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(fBoxP[4][0], fBoxP[4][1], fBoxP[4][2]);
	glVertex3f(fBoxP[5][0], fBoxP[5][1], fBoxP[5][2]);
	glVertex3f(fBoxP[6][0], fBoxP[6][1], fBoxP[6][2]);
	glVertex3f(fBoxP[7][0], fBoxP[7][1], fBoxP[7][2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(fBoxP[0][0], fBoxP[0][1], fBoxP[0][2]);
	glVertex3f(fBoxP[3][0], fBoxP[3][1], fBoxP[3][2]);
	glVertex3f(fBoxP[7][0], fBoxP[7][1], fBoxP[7][2]);
	glVertex3f(fBoxP[4][0], fBoxP[4][1], fBoxP[4][2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(fBoxP[1][0], fBoxP[1][1], fBoxP[1][2]);
	glVertex3f(fBoxP[5][0], fBoxP[5][1], fBoxP[5][2]);
	glVertex3f(fBoxP[6][0], fBoxP[6][1], fBoxP[6][2]);
	glVertex3f(fBoxP[2][0], fBoxP[2][1], fBoxP[2][2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(fBoxP[3][0], fBoxP[3][1], fBoxP[3][2]);
	glVertex3f(fBoxP[2][0], fBoxP[2][1], fBoxP[2][2]);
	glVertex3f(fBoxP[6][0], fBoxP[6][1], fBoxP[6][2]);
	glVertex3f(fBoxP[7][0], fBoxP[7][1], fBoxP[7][2]);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(fBoxP[0][0], fBoxP[0][1], fBoxP[0][2]);
	glVertex3f(fBoxP[4][0], fBoxP[4][1], fBoxP[4][2]);
	glVertex3f(fBoxP[5][0], fBoxP[5][1], fBoxP[5][2]);
	glVertex3f(fBoxP[1][0], fBoxP[1][1], fBoxP[1][2]);
	glEnd();
};

void DrawTriangleNode(TriangleTreeNode* pNode,Box & box, Imath::Rand32 & rand)
{
	glColor3f(rand.nextf(), rand.nextf(), rand.nextf());
	if (pNode->type == TriangleTreeNode::kBranch) {
		Box lBox, rBox;
		TriangleTreeBranch * pBranch = (TriangleTreeBranch *)pNode;
		box.AxisSplit(lBox, rBox, pBranch->d, pBranch->axis);
		//DrawBox(lBox);
		//DrawBox(rBox);
		DrawTriangleNode(pBranch->children[0], lBox, rand);
		DrawTriangleNode(pBranch->children[1], rBox, rand);
	}
	else {
		TriangleTreeLeaf * pLeaf = (TriangleTreeLeaf*)pNode;
		for (auto iter = pLeaf->triAndBoxes.begin(); iter != pLeaf->triAndBoxes.end(); ++iter)
			DrawTriangle(iter->tri);
	}
}
void DrawTriangleTree(TriangleTree & tree)
{
	Imath::Rand32 rand;
	auto rootNode=tree.rootNode();
	Box box = tree.rootBox;
	DrawBox(box);
	DrawTriangleNode(rootNode, box, rand);
}

void DrawKdTree3D(KdTree<FVec3> & tree,std::vector<FVec3> & pList)
{
	DrawBox(tree.box);

	struct StackParam
	{
		u_int id;
		Box box;
		StackParam(const u_int _id, const Box _box) :id(_id) { box = _box; };
	};
	std::vector<StackParam> stackParamList;
	stackParamList.push_back(StackParam(0,tree.box));
	
	while (stackParamList.size() > 0) {
		StackParam   param = stackParamList.back();
		auto pNode = tree.pNodeList[param.id];

		u_int dimId = pNode->dim;
		Box lBox, rBox;
		lBox = param.box;
		lBox.max[dimId] = pList[pNode->dataId][dimId];

		rBox = param.box;
		rBox.min[dimId] = pList[pNode->dataId][dimId];

		float p4[4][3];
		u_int dim[3];
		
		dim[0] = dimId;
		dim[1] = (dimId + 1) % 3;
		dim[2] = (dimId + 2) % 3;

		p4[0][dim[0]] = lBox.max[dim[0]];
		p4[0][dim[1]] = lBox.min[dim[1]];
		p4[0][dim[2]] = lBox.min[dim[2]];

		p4[1][dim[0]] = lBox.max[dim[0]];
		p4[1][dim[1]] = lBox.max[dim[1]];
		p4[1][dim[2]] = lBox.min[dim[2]];

		p4[2][dim[0]] = lBox.max[dim[0]];
		p4[2][dim[1]] = lBox.max[dim[1]];
		p4[2][dim[2]] = lBox.max[dim[2]];

		p4[3][dim[0]] = lBox.max[dim[0]];
		p4[3][dim[1]] = lBox.min[dim[1]];
		p4[3][dim[2]] = lBox.max[dim[2]];

		glBegin(GL_LINE_LOOP);
		for (int i = 0; i < 4; ++i)
			glVertex3f(p4[i][0], p4[i][1], p4[i][2]);
		glEnd();

		stackParamList.pop_back();
		if (pNode->childrenId[0] != 0)
			stackParamList.push_back(StackParam(pNode->childrenId[0], lBox));

		if (pNode->childrenId[1] != 0)
			stackParamList.push_back(StackParam(pNode->childrenId[1], rBox));
	}
}

END_SSE_MATH_NAME

#endif
