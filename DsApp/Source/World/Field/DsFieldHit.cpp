#include "DsAppPch.h"
#ifndef _DS_FIELD_HIT_H_
#include "World/Field/DsFieldHit.h"
#endif

#ifndef _DS_ANIM_RES_
#include "Res/DsAnimRes.h"
#endif
#ifndef _DS_HITRES_H_
#include "Res/DsHitRes.h"
#endif
#ifndef _DS_MODEL_
#include "Model/DsModel.h"
#endif
#ifndef __DS_APP_COLLISION_FILTER__
#include "World/Physics/DsAppCollisionFilter.h"
#endif

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;

DsFieldHit::DsFieldHit(DsSys& sys, DsPhysicsWorld& world)
	: DsFieldObj(sys, world)
{
	
}

DsFieldHit::~DsFieldHit()
{
}

//virtual
void DsFieldHit::Initialize(const DsFieldInitInfo& initInfo)
{
	DsFieldObj::Initialize(initInfo);

	//’nŒ`‚ ‚½‚è‚Í“à•”‚Ì‚à‚Ì‚Í‘S‚Ä“–‚½‚ç‚È‚¢
	m_actorId.GetActor()->SetCollisionFilter(DsAppCollisionFilter::CalcFilterGroup(COLLISION_GROUP::HIT));
}

//virtual 
void DsFieldHit::Update(double dt)
{
	DsFieldObj::Update(dt);
}

//virtual
void DsFieldHit::SetPosition(const DsVec3d& pos)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		pActor->SetPosition(pos);
	}

	if (m_pAnimation)
	{
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
	}
}
//virtual
void DsFieldHit::SetRotation(const DsMat33d& rot)
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
DsVec3d DsFieldHit::GetPosition() const
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		return pActor->GetPosition();
	}
	else
	{
		return DsVec3d::Zero();
	}
}

//virtual
DsMat33d DsFieldHit::GetRotation() const
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


//virtual 
void DsFieldHit::DbgDraw(DsLib::DsDrawCommand& com)
{
	DsFieldObj::DbgDraw(com);
}