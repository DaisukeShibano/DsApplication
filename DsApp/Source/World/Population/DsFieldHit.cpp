#include "DsAppPch.h"
#ifndef _DS_FIELD_HIT_H_
#include "World/Population/DsFieldHit.h"
#endif

#ifndef _DS_ANIM_RES_
#include "Res/DsAnimRes.h"
#endif
#ifndef _DS_HITRES_H_
#include "Res/DsHitRes.h"
#endif
#ifndef _DS_ANIM_MODEL_
#include "Animation/DsAnimModel.h"
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
void DsFieldHit::Initialize(const InitInfo& initInfo)
{
	DsFieldObj::Initialize(initInfo);
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