#include "DsPhysicsPch.h"
#ifndef _DS_RIGID_MESH_H_
#include "Actor/DsRigidMesh.h"
#endif
#ifndef _DS_ANIM_MODEL_
#include "Animation/DsAnimModel.h"
#endif
#ifndef __DS_BOUNDING_TREE_BASE__
#include "Collision/BoundingTree/DsBoundingTreeAabb.h"
#endif

using namespace DsLib;
using namespace DsPhysics;



DsRigidMesh::DsRigidMesh(const DsActorId& id, const char* name)
	: DsRigidBody(id, name)
	, m_pAabbTree(NULL)
{
}

DsRigidMesh::~DsRigidMesh()
{
	delete m_pAabbTree; m_pAabbTree = NULL;
	delete m_pCollisionGeometry; m_pCollisionGeometry = NULL;
}

void DsRigidMesh::Create(const DsAnimModel& animModel)
{
	m_geomInfo.vn = animModel.GetVertexNum();
	m_geomInfo.pVertex = new DsVec3d[m_geomInfo.vn];
	m_geomInfo.pVertexOriginal = new DsVec3d[m_geomInfo.vn];
	for (int i = 0; i < m_geomInfo.vn; ++i)
	{
		m_geomInfo.pVertex[i] = animModel.GetVertex()[i];
		m_geomInfo.pVertexOriginal[i] = animModel.GetVertex()[i];
	}

	m_geomInfo.fn = animModel.GetFaceNum();
	m_geomInfo.pFace = new DsQuad[m_geomInfo.fn];
	std::vector<DsQuad> addFaces;
	for (int i = 0; i < m_geomInfo.fn; ++i)
	{
		const int vn = animModel.GetFace()[i].vn;
		
		if(3 == vn)
		{
			m_geomInfo.pFace[i].vn = vn;
			for (int vi = 0; vi < vn; ++vi)
			{
				m_geomInfo.pFace[i].index[vi] = animModel.GetFace()[i].pIndex[vi];
			}
			//�@�������߂�BanimModel�̂��g���ꍇ�͒��_�@���Ȃ̂Œ���
			const int* tmpIdx = animModel.GetFace()[i].pIndex;
			const DsVec4d* tmpV = animModel.GetVertex();
			m_geomInfo.pFace[i].normal = DsVec3d::CalcNormal(tmpV[tmpIdx[0]], tmpV[tmpIdx[1]], tmpV[tmpIdx[2]]);

		}
		else if ( 4 <= vn)
		{
			//4�p�`�ȏ�͖ʂ𕪊�����B4�p�`����͕��ʂłȂ��Ȃ�\��������̂ŁB
			
			//3�p�`�܂ł�if(3 == vn)�Ɠ����B
			m_geomInfo.pFace[i].vn = 3;
			for (int vi = 0; vi < 3; ++vi)
			{
				m_geomInfo.pFace[i].index[vi] = animModel.GetFace()[i].pIndex[vi];
			}
			//�@�������߂�BanimModel�̂��g���ꍇ�͒��_�@���Ȃ̂Œ���
			const int* tmpIdx = animModel.GetFace()[i].pIndex;
			const DsVec4d* tmpV = animModel.GetVertex();
			m_geomInfo.pFace[i].normal = DsVec3d::CalcNormal(tmpV[tmpIdx[0]], tmpV[tmpIdx[1]], tmpV[tmpIdx[2]]);
			

			//3�߂̒��_�ȍ~����ʂ𕪊��B���ԂȂ���������ɂ͂O�Ԗڂ��Ō�ɂ���B3�ԖځA4�ԖځA�O�ԖځA�̂悤�ɕ���������
			int overVn = vn - m_geomInfo.pFace[i].vn;
			int curIndex = m_geomInfo.pFace[i].vn-1;
			while (overVn != 0)
			{
				//�O��̒��_�A�O��+1�A�O�Ԗڂ̒��_�A�ƂȂ�悤��3�p�`�����

				//�c�蒸�_���ɂ���ĂS�p�`���R�p�`�����߂�
				const int addFaceVn = 1;//�R�p�`�Œ�

				DsQuad addFace;
				addFace.vn = addFaceVn + 2;
				for (int vi = 0; vi < addFace.vn-1; ++vi)//�ǉ���-1�܂ł̒��_�ŐV�����ʂ����B�Ō�̒��_�͂O�ԌŒ�
				{
					addFace.index[vi] = animModel.GetFace()[i].pIndex[curIndex+vi];
				}
				addFace.index[addFace.vn - 1] = animModel.GetFace()[i].pIndex[0];
				addFace.normal = DsVec3d::CalcNormal(m_geomInfo.pVertex[addFace.index[0]], m_geomInfo.pVertex[addFace.index[1]], m_geomInfo.pVertex[addFace.index[2]]);
				addFaces.push_back(addFace);

				curIndex += addFaceVn;
				overVn -= addFaceVn;
				DS_ASSERT((0 <= overVn), "�ʕ����Ɏ��s");//�c�蒸�_�͂��傤�ǂO�ɂȂ�͂�
			}
		}
	}

	//�����ʂ��������̂ŁA�ʃ��T�C�Y
	if(!addFaces.empty())
	{
		const size_t newSize = m_geomInfo.fn + addFaces.size();
		DsQuad* newFace = new DsQuad[newSize];
		for (int fi = 0; fi < m_geomInfo.fn; ++fi)
		{
			newFace[fi] = m_geomInfo.pFace[fi];
		}
		for (unsigned int fi = 0; fi < addFaces.size(); ++fi)
		{
			newFace[m_geomInfo.fn + fi] = addFaces[fi];
		}
		delete m_geomInfo.pFace;
		m_geomInfo.pFace = newFace;
		m_geomInfo.fn = static_cast<int>(newSize);
	}

	//�ʂƖʂ̔��肪����ΕK�v�Ȃ��͂��B
	m_geomInfo.ln = 0;
	m_geomInfo.pLine = 0;

	{
		DsRigidPhysicsInfo& pi = m_physicsInfo;

		//mass
		{//�����e���\���͗�����
			const double density = 2700.0;//�A���~��2.7Mg/m~3 �炵��
			const DsVec3d maxLength = _GetMaxVector(m_geomInfo.pVertex, m_geomInfo.vn);
			const double mass = density*maxLength.x*maxLength.y*maxLength.z;
			const double bias = 1.0;
			const double M = m_option.isStatic ? DsMathUtil::DS_INFINITY_D : mass;
			const double Ixx = M / 3.0 * (maxLength.y*maxLength.y + maxLength.z*maxLength.z) * bias;
			const double Iyy = M / 3.0 * (maxLength.x*maxLength.x + maxLength.z*maxLength.z) * bias;
			const double Izz = M / 3.0 * (maxLength.x*maxLength.x + maxLength.y*maxLength.y) * bias;
			
			const double maxMass = 1000;
			pi.mass.inertia = DsMat33d::Identity();
			pi.mass.inertia[0] = min(Ixx, maxMass);
			pi.mass.inertia[4] = min(Iyy, maxMass);
			pi.mass.inertia[8] = min(Izz, maxMass);
			pi.mass.mass = min(M, maxMass);
		}

		//�����p��
		pi.rotation = DsMat33d::Identity();

		//�����ʒu
		pi.pos.Set(0.0, 0.0, 0.0);

		//�d�S�ʒu
		DsRigidGeometryInfo& gi = m_geomInfo;
		//pi.centerOfGravity = _GetCenterOfGravity(m_geomInfo.pVertex, m_geomInfo.vn);
		pi.centerOfGravity = GetPosition();

		//AABB
		DsVec3d maxLen = DsVec3d::Zero();
		for (int i = 0; i < m_geomInfo.vn; ++i)
		{
			const DsVec3d len = gi.pVertex[i] - pi.centerOfGravity;
			maxLen.x = max(maxLen.x, fabs(len.x));
			maxLen.y = max(maxLen.y, fabs(len.y));
			maxLen.z = max(maxLen.z, fabs(len.z));
		}
		m_aabb.Setup(maxLen.x, maxLen.y, maxLen.z, pi.centerOfGravity);
		m_sideSize.x = maxLen.x;
		m_sideSize.y = maxLen.y;
		m_sideSize.z = maxLen.z;
	}

	{//AABB�c���[�����B
		std::vector<int> faceIdx;
		for (int i = 0; i < m_geomInfo.fn; ++i){
			faceIdx.push_back(i);
		}
		const int depth = (m_option.isStatic) ? (DsBoundingTreeAabb::DEFAULT_DEPTH) : (0);//�������������̂ɑΉ����ĂȂ�
		m_pAabbTree = new DsBoundingTreeAabb(*this, m_physicsInfo.pos, m_sideSize, m_geomInfo.pFace, m_geomInfo.fn, m_geomInfo.pVertex, m_geomInfo.vn, faceIdx, depth);
		m_pAabbTree->ConstructTree();
	}

	//�ÓI�I�u�W�F�ł��ŏ��̈�񂾂�Update���Ȃ��Ə����p�������f����Ȃ�
	m_isForceUpdate = true;
	m_isForceRotation = true;

	_Update(m_initPos, m_initRot);

	m_pCollisionGeometry = new DsCollisionGeometry(m_geomInfo.pVertex, m_geomInfo.vn, m_geomInfo.pFace, m_geomInfo.fn,
		m_geomInfo.pLine, m_geomInfo.ln, GetId(), m_physicsInfo.centerOfGravity, NULL, //1�t���O���_�̓p�t�H�[�}���X�̂��߂��Ȃ��B
		m_sideSize, m_pAabbTree, &m_aabb, GetRotation());
}

//virtual 
void DsRigidMesh::Draw(DsDrawCommand& com)
{
	const DsVec3d* pVertex = m_geomInfo.pVertex;
	const int faceNum = m_geomInfo.fn;

	com.SetColor(m_dbgColor);
	for (int fIdx = 0; faceNum > fIdx; ++fIdx)
	{
		const DsQuad& face = m_geomInfo.pFace[fIdx];
		if (face.vn == 3)
		{
			com.DrawPolygon(pVertex[face.index[0]], pVertex[face.index[1]], pVertex[face.index[2]]);
		}
		else if (face.vn == 4)
		{
			com.DrawQuad(pVertex[face.index[0]], pVertex[face.index[1]], pVertex[face.index[2]], pVertex[face.index[3]]);
		}
	}
}

//////////////////////factory//////////////////////////
DsActor* DsRigidMesh::DsRigidMeshFactory::CreateIns(const DsActorId& id) const
{
	DsRigidMesh* pRet = new DsRigidMesh(id, m_name.c_str());
	if (pRet)
	{
		pRet->m_option = m_initOption;
		pRet->m_initPos = m_initPos;
		pRet->m_initRot = m_initRot;
		pRet->Create(m_anim);
		return pRet;
	}
	else
	{
		return NULL;
	}
}