#pragma once

#include <vector>
#include <sseBase.h>
#include <sseVec.h>
#include <sseMatrix.h>
#include <sseQuaternion.h>
#include <sseBox.h>

#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MQuaternion.h>
#include <maya/MMatrix.h>

BEGIN_SSE_MATH_NAME

class Bone
{
private:
	TransformMatrix3D mat44;

public:
	void SetMatrix(const TransformMatrix3D & locMat)
	{
		mat44 = locMat;
	}
	void AccumMatrix(TransformMatrix3D & worldMat) const
	{
		worldMat *= mat44;
	}
	void TransformBy(const TransformMatrix3D & transMat)
	{
		mat44 *= transMat;
	}
};

//inline TransformMatrix3D MMatrixToTransformMatrix(const MMatrix & m_rotMat)
//{
//	return TransformMatrix3D((float)m_rotMat(0, 0), (float)m_rotMat(0, 1), (float)m_rotMat(0, 2), (float)m_rotMat(0, 3),
//		(float)m_rotMat(1, 0), (float)m_rotMat(1, 1), (float)m_rotMat(1, 2), (float)m_rotMat(1, 3),
//		(float)m_rotMat(2, 0), (float)m_rotMat(2, 1), (float)m_rotMat(2, 2), (float)m_rotMat(2, 3),
//		(float)m_rotMat(3, 0), (float)m_rotMat(3, 1), (float)m_rotMat(3, 2), (float)m_rotMat(3, 3));
//}
//inline MMatrix TransformMatrixToMMatrix(TransformMatrix3D & transMat)
//{
//	float m44[4][4] = { {transMat(0, 0), transMat(0, 1), transMat(0, 2), transMat(0, 3)},
//						{transMat(1, 0), transMat(1, 1), transMat(1, 2), transMat(1, 3)},
//						{transMat(2, 0), transMat(2, 1), transMat(2, 2), transMat(2, 3)},
//						{transMat(3, 0), transMat(3, 1), transMat(3, 2), transMat(3, 3)} };
//	return MMatrix(m44);
//}
class BoneChain
{
private:
	std::vector<Bone> bones;
	std::vector<FVec3> joints;

	void AddJointPoint(const FVec3 & _pos)
	{
		FVec3 pos1 = _pos;
		pos1.EnsurePoint();

		FVec3 pos1_local = pos1;
		
		Bone bone;
		if (!joints.empty()) {
			TransformMatrix3D worldMat,invWorldMat;
			if(bones.size() > 0)
				worldMat = WorldMatrix((u_int)bones.size() - 1);

			invWorldMat = worldMat.Inverse();
			pos1_local *= invWorldMat;

			FVec3 pos0 = joints.back();

			FVec3 zAxis = pos1_local - pos0;
			zAxis.EnsureVector();
			zAxis.Normalize();

			Quaternion quat;
			quat.setRotation(FVec3(0, 0, 1), zAxis);

			TransformMatrix3D rotMat = quat.toMatrix44();

			TransformMatrix3D  offsetMat;
			offsetMat.SetAxis( pos0, 3 );

			rotMat = offsetMat * rotMat;

			bone.SetMatrix(rotMat);

			rotMat = worldMat * rotMat;
			rotMat = rotMat.Inverse();
			pos1 *= rotMat;
			
			bones.push_back(bone);
		}
		joints.push_back(pos1);
		
	}

public:
	void Clear()
	{
		bones.clear();
		joints.clear();
	}
	u_int JointCount() const
	{
		return (int)joints.size();
	}
	u_int BoneCount() const
	{
		return (int)bones.size();
	}
	Box BBox() const
	{
		Box box;
		for (const FVec3 & p : joints) {
			box.Extend(p);
		}
		return box;
	}
	void GetWorldJointList(std::vector<FVec3> &wpList) const
	{
		wpList.clear();
		wpList.push_back(joints[0]);
		TransformMatrix3D worldMat;
		auto jointIter = joints.begin() + 1;
		for (auto & bone : bones) {
			bone.AccumMatrix(worldMat);
			wpList.push_back(worldMat * (*jointIter));
			++jointIter;
		}
	}
	FVec3 GetWorldJoint(const int jointIndex) const
	{
		if (jointIndex == 0)
			return joints[0];
		
		TransformMatrix3D worldMat;
		if (jointIndex == bones.size())
			worldMat = WorldMatrix(jointIndex - 2);
		else
			worldMat = WorldMatrix(jointIndex - 1);
		return worldMat*joints[jointIndex];
	}
	TransformMatrix3D WorldMatrix(const int boneIndex) const
	{
		TransformMatrix3D mat44;
		if (boneIndex >= bones.size())
			return mat44;

		int curIndex = 0;
		while (curIndex <= boneIndex) {
			bones[curIndex++].AccumMatrix(mat44);
		}
		return mat44;
	}
	void Build(const std::vector<FVec3> & tarJoints)
	{
		Clear();
		for (const FVec3 & p : tarJoints) {
			AddJointPoint(p);
		}
	}
	void TransformToEveryBone(std::vector<FVec3> & vList, const FVec3 & v)
	{
		FVec3 v1;
		vList.clear();
		TransformMatrix3D worldMat;
		for (auto & bone : bones) {
			bone.AccumMatrix(worldMat);
			v1 = worldMat * v;
			vList.push_back(v1);
		}
	}
	
	void TransformTo(const std::vector<FVec3> & tarJoints)
	{
		auto tar_joint0Iter = tarJoints.begin();
		auto tar_joint1Iter = tarJoints.begin() + 1;
		auto src_joint0Iter = joints.begin();
		FVec3 p0 = *src_joint0Iter;
		*src_joint0Iter = *tar_joint0Iter;

		FVec3 tarP0_local, tarP1_local, zAxis0(0, 0, 1), offsetV;

		TransformMatrix3D worldMatrix, invWorldMatrix, offsetMat, rotMat, localMat;
		for (u_int bId = 0; bId < bones.size(); ++bId) {
			auto & bone = bones[bId];
			bone.AccumMatrix(worldMatrix);

			invWorldMatrix = worldMatrix.Inverse();
			tarP0_local = invWorldMatrix * p0;
			tarP0_local = invWorldMatrix * *tar_joint0Iter;
			tarP1_local = invWorldMatrix * *tar_joint1Iter;

			FVec3 zAxis = tarP1_local - tarP0_local;
			zAxis.Normalize();

			Quaternion quat;
			quat = quat.setRotation(zAxis0, zAxis);
			rotMat = quat.toMatrix44();

			offsetMat.SetAxis(tarP0_local,3);

			localMat = offsetMat * rotMat;

			worldMatrix *= localMat;

			bone.TransformBy(localMat);

			invWorldMatrix = worldMatrix.Inverse();
			tarP1_local = invWorldMatrix * *tar_joint1Iter;

			++tar_joint0Iter;
			++tar_joint1Iter;
			++src_joint0Iter;

			*src_joint0Iter = tarP1_local;
		}
	}
	float Length() const
	{
		std::vector<FVec3> pList;
		GetWorldJointList(pList);

		float len = 0;
		for (int i = 0; i < bones.size(); ++i) {
			len += (pList[i+1] - pList[i]).Length();
		}
		return len;
	}
	FVec3 LocalSpace(const u_int id, const FVec3 & sampleP)
	{
		TransformMatrix3D mat = WorldMatrix(id);
		mat=mat.Inverse();
		return mat*sampleP;
	}
	FVec3 WorldSpace(const u_int id, const FVec3 & sampleP)
	{
		TransformMatrix3D mat = WorldMatrix(id);
		return mat*sampleP;
	}
	void NearestP2W(u_int id2[2], float w2[2], const FVec3 & sampleP) const
	{
		struct DistAndId {
			u_int id;
			float d;
		};
		std::vector<DistAndId> dAndIdList;
		u_int i = 0;
		std::vector<FVec3> pList;
		GetWorldJointList(pList);
		for (uint i = 0; i < pList.size() - 1; ++i) {
			auto & curP = pList[i];
			dAndIdList.push_back({ i,(sampleP - curP).Length2() });
			++i;
		}
		std::nth_element(dAndIdList.begin(), dAndIdList.begin() + 1, dAndIdList.end(), [&](const DistAndId & a, const DistAndId & b) {
			return a.d < b.d;
		});
		u_int nId = dAndIdList[0].id;

		auto & curP = pList[nId];

		FVec3 v;
		float invLen;
		float dotVN;
		FVec3 n;

		auto & p2 = pList[nId + 1];
		n = p2 - curP;
		invLen = 1.0f / n.Length();

		n.Normalize();
		v = sampleP - curP;
		dotVN = v.Dot(n);

		if (dotVN >= 0) {
			if (nId == pList.size() - 2) {
				id2[0] = nId - 1;
				id2[1] = nId;
				w2[0] = 0;
				w2[1] = 1;
			}
			else {
				id2[0] = nId;
				id2[1] = nId + 1;
				w2[1] = dotVN*invLen;
				w2[0] = 1 - w2[1];
			}
		}
		else {
			if (nId == 0) {
				id2[0] = nId;
				id2[1] = nId + 1;
				w2[0] = 1;
				w2[1] = 0;
			}
			else {
				auto & p0 = pList[nId - 1];
				n = curP - p0;
				invLen = 1.0f / n.Length();

				n.Normalize();
				v = sampleP - p0;
				dotVN = v.Dot(n);

				id2[0] = nId - 1;
				id2[1] = nId;
				w2[1] = dotVN*invLen;
				w2[0] = 1 - w2[1];
			}
		}
	}
};

END_SSE_MATH_NAME