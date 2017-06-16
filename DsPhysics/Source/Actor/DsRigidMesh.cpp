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



namespace
{
	DsVec3d _GetMaxVector(const DsVec3d* pVec, const int vn)
	{
		double maxX = 0.0;
		double maxY = 0.0;
		double maxZ = 0.0;

		for (int i = 0; vn > i; ++i)
		{
			maxX = (maxX > fabs(pVec[i].x)) ? maxX : fabs(pVec[i].x);
			maxY = (maxY > fabs(pVec[i].y)) ? maxY : fabs(pVec[i].y);
			maxZ = (maxZ > fabs(pVec[i].z)) ? maxZ : fabs(pVec[i].z);
		}

		const DsVec3d ret = { maxX*2.0, maxY*2.0, maxZ*2.0 };
		return ret;
	}

	DsVec3d _GetCenterOfGravity(const DsVec3d* v, const int vertexNum)
	{
		DsVec3d sum = { 0.0, 0.0, 0.0 };
		for (int vn = 0; vertexNum > vn; ++vn)
		{
			sum += v[vn];
		}
		const DsVec3d ave = { sum.x / vertexNum, sum.y / vertexNum, sum.z / vertexNum };
		return ave;
	}
}



DsRigidMesh::DsRigidMesh(const DsActorId& id, const char* name)
	: DsRigidBody(id, name)
	, m_pAabbTree(NULL)
{
}

DsRigidMesh::~DsRigidMesh()
{
	delete m_pAabbTree; m_pAabbTree = NULL;
	delete m_pCollisionContext; m_pCollisionContext = NULL;
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

	m_pCollisionContext = new DsCollisionContext(m_geomInfo.pVertex, m_geomInfo.vn, m_geomInfo.pFace, m_geomInfo.fn,
		m_geomInfo.pLine, m_geomInfo.ln, GetId(), m_physicsInfo.centerOfGravity, NULL, //1�t���O���_�̓p�t�H�[�}���X�̂��߂��Ȃ��B
		m_sideSize, m_pAabbTree, &m_aabb, GetRotation());
}

void DsRigidMesh::_Update(const DsVec3d& deltaPos, const DsMat33d& deltaRot)
{
	if (m_option.isStatic && (!m_isForceUpdate) && (!m_isForceRotation))
	{
		return;
	}

	DsRigidGeometryInfo& gi = m_geomInfo;
	DsRigidPhysicsInfo& pi = m_physicsInfo;

	//�����p�����f�̂��߉�]������L����
	if (m_option.isRotation || m_isForceRotation)
	{
		//�p���X�V
		{
			pi.rotation = deltaRot * pi.rotation;
		}
	}
	
	const bool isChangeRot = m_isForceRotation || (!deltaRot.IsNearEqual(DsMat33d::Identity()));
	if (isChangeRot || m_isForceUpdate)
	{
		//���_��]
		for (int vn = 0; gi.vn > vn; ++vn)
		{
			gi.pVertex[vn] = pi.rotation * gi.pVertexOriginal[vn];
		}

		//�����e���\���X�V
		{
			const DsMat33d invR = DsMat33d::Inverse(deltaRot);
			const DsMat33d rotI = (deltaRot * pi.mass.inertia) * invR;
			pi.mass.inertia = rotI;
		}
		//�S���p�����e���\���t�s��
		{
			const DsMat33d&& invRI = DsMat33d::Inverse(pi.mass.inertia);
			pi.massInv[0] = 1.0 / pi.mass.mass;
			pi.massInv[1] = invRI[0]; pi.massInv[2] = invRI[1]; pi.massInv[3] = invRI[2];
			pi.massInv[4] = invRI[3]; pi.massInv[5] = invRI[4]; pi.massInv[6] = invRI[5];
			pi.massInv[7] = invRI[6]; pi.massInv[8] = invRI[7]; pi.massInv[9] = invRI[8];
		}

		//�@���X�V
		for (int fn = 0; gi.fn > fn; ++fn)
		{
			const DsVec3d rotN = deltaRot * gi.pFace[fn].normal;//�Ȃ�t�����Ɉ�񋁂ߒ������ق������������B
			gi.pFace[fn].normal = DsVec3d::Normalize(rotN);//���K���͖��t�����Ȃ��Ă��ǂ��C������
		}

		//�ʒu�X�V
		{
			pi.pos += deltaPos;
		}

		//���_�ړ�
		for (int vn = 0; gi.vn > vn; ++vn)
		{
			gi.pVertex[vn] = pi.pos + gi.pVertex[vn];
		}
	}
	else if (!deltaPos.IsNearZero())
	{
		//�ʒu�X�V
		{
			pi.pos += deltaPos;
		}

		//���_�ړ�
		for (int vn = 0; gi.vn > vn; ++vn)
		{
			gi.pVertex[vn] += deltaPos;
		}
	}

	//�d�S�X�V
	//pi.centerOfGravity = _GetCenterOfGravity(gi.pVertex, gi.vn);
	pi.centerOfGravity = GetPosition();
	
	//AABB
	{
		if (isChangeRot)
		{
			DsVec3d maxLen = DsVec3d::Zero();
			for (int i = 0; i < m_geomInfo.vn; ++i)
			{
				const DsVec3d len = gi.pVertex[i] - pi.centerOfGravity;
				maxLen.x = max(maxLen.x, fabs(len.x));
				maxLen.y = max(maxLen.y, fabs(len.y));
				maxLen.z = max(maxLen.z, fabs(len.z));
			}
			m_aabb.Setup(maxLen.x, maxLen.y, maxLen.z, pi.centerOfGravity);
		}
		else
		{
			//��]���ĂȂ��Ȃ璆�S���W�����X�V
			m_aabb.SetPos(pi.centerOfGravity);
		}
	}

	m_isForceUpdate = false;
	m_isForceRotation = false;
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