#include "DsPhysicsPch.h"
//自分のヘッダ
#include "ChrProxy/DsChrProxy.h"
//他のヘッダ
#include "Actor/DsRigidCapsule.h"
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif


using namespace DsPhysics;


DsChrProxy::DsChrProxy(DsPhysicsWorld& world, void* pOwner)
	: m_world(world)
	, m_actorId()
	, m_pos(DsVec3d::Zero())
	, m_pOwner(pOwner)
	, m_filter(0)
	, m_height(0)
	, m_radius(0)
	, m_isGround(false)
{
}

DsChrProxy::~DsChrProxy()
{
}

void DsChrProxy::Initialize(double r, double height, double mass, DsVec3d pos, DsMat33d rot)
{
	m_height = height;
	m_radius = r;

	//heightは全長
	height = max(r * 2.0, height);
	height *= 0.5;
	height -= r;
	height = max(height, 0.0);
	DsRigidCapsule::DsRigidCapsuleFactory factory(r, height, mass, "ChrProxy");
	factory.InitPos(pos);
	factory.InitRot(DsMat33d::Identity());//カプセルに回転は要らない
	factory.SetOption(DsActor::Option::ChrProxy());
	m_actorId = m_world.CreateActor(factory);
	m_actorId.GetActor()->SetUserData(m_pOwner);

	//キャラは跳ね返らないし、摩擦もない
	DsActorMaterial m;
	m.m_bundCoef = 0.0;
	m.m_staticFrictionCoef = 0.0;
	m.m_kinematicFricCoef = 0.0;
	m_actorId.GetActor()->SetMaterial(m);
}

void DsChrProxy::SetPosition(const DsVec3d& pos)
{
	m_pos = pos;
	DsActor* pActor = m_actorId.GetActor();
	if (pActor) {
		const DsAabb aabb = pActor->RefAabb();
		const double halfLen = (aabb.GetMax().y - aabb.GetMin().y)*0.5;
		pActor->SetPosition(pos + DsVec3d(0, halfLen, 0));
	}
}

DsAabb DsChrProxy::GetAabb() const
{
	DsAabb ret;
	const DsActor* pActor = m_actorId.GetActor();
	if (pActor) {
		ret = pActor->RefAabb();
	}
	return ret;
}

void DsChrProxy::SetCollisionFilter(DsCollisionFilter filter)
{
	m_filter = filter;
	DsActor* pActor = m_actorId.GetActor();
	if (pActor) {
		pActor->SetCollisionFilter(filter);
	}
}

void DsChrProxy::Drive(double dt, DsVec3d move)
{
	DsActor* pActor = m_actorId.GetActor();
	if (!pActor) {
		return;
	}

	//高さあわせ
	{
		const double upHeight = 0.3;
		const double downHeight = 0.0;
		const DsVec3d upStart = m_pos + DsVec3d::Up()*m_height;
		const DsVec3d upEnd = m_pos + DsVec3d::Up()*(m_height + upHeight);
		DsVec3d upHitPos = upEnd;
		if (m_isGround) {
			m_world.SphereCast(upStart, upEnd, m_radius, m_filter, m_pOwner, &upHitPos);
			pActor->SetPosition(pActor->GetPosition() + (upHitPos - upStart));//浮かせる
		}

		m_world.DriveActor(dt, m_actorId, move);

		const DsVec3d downStart = DsVec3d(pActor->GetPosition().x, pActor->RefAabb().GetMin().y, pActor->GetPosition().z);//進んだ後のカプセルの底
		const DsVec3d downEnd = (m_isGround) ? (downStart - DsVec3d::Up()*(upHeight + downHeight)) : (downStart);
		DsVec3d downHitPos = downEnd;
		const bool isGround = m_world.SphereCast(downStart, downEnd, m_radius, m_filter, m_pOwner, &downHitPos);
		if (m_isGround) {
			pActor->SetPosition(pActor->GetPosition() + (downHitPos - downStart));//沈ませる
		}

		m_pos = downHitPos;

		m_isGround = isGround;
	}
}