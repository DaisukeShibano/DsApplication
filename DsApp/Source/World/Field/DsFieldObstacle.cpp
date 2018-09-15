#include "DsAppPch.h"
//Ž©•ª‚Ìƒwƒbƒ_
#include "World/Field/DsFieldObstacle.h"
//‘¼‚Ìƒwƒbƒ_
#ifndef _DS_ANIM_RES_
#include "Res/DsAnimRes.h"
#endif
#ifndef _DS_HITRES_H_
#include "Res/DsHitRes.h"
#endif
#ifndef _DS_MAP_RES_H_
#include "Res/DsMapRes.h"
#endif
#ifndef _DS_MODEL_
#include "Model/DsModel.h"
#endif
#ifndef _DS_ANIM_EVENT_CALLBACK_
#include "World/Animation/Event/DsAnimEventCallback.h"
#endif
#ifndef _DS_COMPONENT_SYSTEM_
#include "World/Component/DsComponentSystem.h"
#endif
#ifndef _DS_EQUIP_COMPONENT_
#include "World/Component/Equip/DsEquipComponent.h"
#endif

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;



DsFieldObstacle::DsFieldObstacle(DsSys& sys, DsPhysicsWorld& world)
	: DsFieldObj(sys, world)
	, m_actorId()
{
	
}

DsFieldObstacle::~DsFieldObstacle()
{
	m_actorId = m_world.DeleteActor(m_actorId);
}



//virtual
void DsFieldObstacle::SetPosition(const DsVec3d& pos)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		const double y = pActor->GetPosition().y;
		const DsVec3d offset = DsVec3d(0, pActor->RefAabb().GetMax().y-y, 0);
		pActor->SetPosition(pos);

		if (m_pAnimation)
		{
			m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
		}
	}
}
//virtual
void DsFieldObstacle::SetRotation(const DsMat33d& rot)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		pActor->SetRotation(rot);
	}

	if (m_pAnimation)
	{
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
	}
}

//virtual
DsVec3d DsFieldObstacle::GetPosition() const
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		const double y = pActor->GetPosition().y;
		const DsVec3d offset = DsVec3d(0, -pActor->RefAabb().GetMax().y-y, 0);
		return pActor->GetPosition();
	}
	else
	{
		return DsVec3d::Zero();
	}
}

//virtual
DsMat33d DsFieldObstacle::GetRotation() const
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		return pActor->GetRotation();
	}
	else
	{
		return DsMat33d::Identity();
	}
}

DsActor* DsFieldObstacle::GetActor()
{
	return m_world.GetActor(m_actorId);
}

const DsActor* DsFieldObstacle::GetActor() const
{
	return m_world.GetActor(m_actorId);
}
