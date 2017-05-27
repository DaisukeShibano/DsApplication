#include "DsPhysicsPch.h"
#include "Collision/DsCollisionResult.h"
#include "DsPhysicsWorld.h"
#include "Collision/DsCollisionContext.h"
#include "Collision/DsCollisionDetection.h"

using namespace DsPhysics;

//�Փ˓_�𐮗�����
void DsCollisionResult::ModefyInfo()
{
	DsVec3d sumNormal = DsVec3d::Zero();
	for (int i = 0; i < m_colCounter; ++i)
	{
		sumNormal += m_colNormal[i];
	}
	//�Փ˕������������ʂȂ��Ȃ���
	if ((sumNormal.Length() < 0.01) && (0<m_colCounter))
	{
		*this = DsCollisionResult::Average(*this);
		//���̕��ϓ_�ł�����񓖂��蔻��ł��Ȃ�����
	}
}

//�Փ˓_�̏��𓾂�
DsCollisionResult::ColInfos& DsCollisionResult::MakeInfos()
{
	m_infos.clear();

	//�W�߂�Actor�B1��2�ǂ���ł�����
	DsActorId& collect = m_ownerId_1[0];

	for (int i = 0; i < m_colCounter; ++i)
	{
		if (collect == m_ownerId_1[i])
		{
			ColInfo info(m_colPos[i], m_colNormal[i], m_depth[i], m_ownerId_1[i], m_ownerId_2[i]);
			m_infos.push_back(info);//���R�s�[�����Ⴄ�񂾂�ȁ[
		}
	}

	//�Փ˓_������̂ɁA������W����Ȃ������������̕��Ŏ����Ă݂�
	if (m_infos.empty() && (0 < m_colCounter ))
	{
	}

	if(0){
		DsVec3d sumNormal = DsVec3d::Zero();
		for each(const ColInfo& info in m_infos)
		{
			sumNormal += info.colNormal;
		}
		if ( (sumNormal.Length() < 0.1) && (0 < m_infos.size()) )
		{
			DsVec3d avePos = DsVec3d::Zero();
			for each(const ColInfo& info in m_infos)
			{
				avePos += info.colPos;
			}
			avePos /= static_cast<double>(m_infos.size());
		}
	}


	return m_infos;
}