#include "DsPhysicsPch.h"
#include "Collision/DsCollisionResult.h"
#include "DsPhysicsWorld.h"
#include "Collision/DsCollisionContext.h"
#include "Collision/DsCollisionDetection.h"

using namespace DsPhysics;

//衝突点を整理する
void DsCollisionResult::ModefyInfo()
{
	DsVec3d sumNormal = DsVec3d::Zero();
	for (int i = 0; i < m_colCounter; ++i)
	{
		sumNormal += m_colNormal[i];
	}
	//衝突方向足した結果なくなった
	if ((sumNormal.Length() < 0.01) && (0<m_colCounter))
	{
		*this = DsCollisionResult::Average(*this);
		//この平均点でもう一回当たり判定できないかな
	}
}

//衝突点の情報を得る
DsCollisionResult::ColInfos& DsCollisionResult::MakeInfos()
{
	m_infos.clear();

	//集めるActor。1か2どちらでもいい
	DsActorId& collect = m_ownerId_1[0];

	for (int i = 0; i < m_colCounter; ++i)
	{
		if (collect == m_ownerId_1[i])
		{
			ColInfo info(m_colPos[i], m_colNormal[i], m_depth[i], m_ownerId_1[i], m_ownerId_2[i]);
			m_infos.push_back(info);//一回コピーしちゃうんだよなー
		}
	}

	//衝突点があるのに、一つも収集されなかったらもう一個の方で試してみる
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