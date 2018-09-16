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

	if (pActor->RefOption().isStatic) {
		return;
	}

	//これ有効だと破片とかも上ってしまう
	//スフィアキャストのコリジョンフィルタを設定できるようにする
	const bool isFlatMove = true;

	const double radius = m_radius * 0.1;//外側のカプセルよりも小さく
	const double diam = radius * 2.0;

	//高さあわせ
	if(isFlatMove){
		const double upHeight = 0.1;
		const double downHeight = 0.0;
		const DsVec3d upStart = m_pos + DsVec3d::Up()*m_height;
		const DsVec3d upEnd = upStart + DsVec3d::Up()*upHeight;
		DsVec3d upHitPos = upEnd;
		DsVec3d upVec = DsVec3d::Zero();
		if (m_isGround) {
			m_world.SphereCast(upStart, upEnd, radius, m_filter, m_pOwner, &upHitPos);
			upVec = upHitPos - upStart;
			pActor->SetPosition(pActor->GetPosition() + upVec);//浮かせる
		}

		m_world.DriveActor(dt, m_actorId, move);

		const DsVec3d downStart = DsVec3d(pActor->GetPosition().x, pActor->RefAabb().GetMin().y + diam, pActor->GetPosition().z);//進んだ後のカプセルの底
		const DsVec3d downEnd = (m_isGround) ? (downStart - upVec - DsVec3d::Up()*(downHeight + diam)) : (downStart - DsVec3d::Up()*diam);
		DsVec3d downHitPos = downEnd;
		const bool isGround = m_world.SphereCast(downStart, downEnd, radius, m_filter, m_pOwner, &downHitPos);
		if (m_isGround) {
			const DsVec3d tmpP = pActor->GetPosition();
			const double down = downHitPos.y - (downStart.y - diam);
			pActor->SetPosition(DsVec3d(tmpP.x, tmpP.y + down, tmpP.z));//沈ませる
		}

		m_pos = downHitPos;

		m_isGround = true;
	}
	else {
		m_world.DriveActor(dt, m_actorId, move);

		const DsVec3d downStart = DsVec3d(pActor->GetPosition().x, pActor->RefAabb().GetMin().y + radius * 2.0, pActor->GetPosition().z);
		const DsVec3d downEnd = downStart - DsVec3d::Up()*radius*2.0;

		m_pos = downEnd;

		m_isGround = m_world.SphereCast(downStart, downEnd, radius, m_filter, m_pOwner, NULL);
	}
}


void DsChrProxy::DbgDraw(DsLib::DsDrawCommand& com)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor){
		pActor->SetColor(DsVec4d(1, 1, 1, 0));
		pActor->SetLineColor(DsVec4d(0, 0, 0, 0));
		pActor->Draw(com);
	}
}

void DsChrProxy::DbgSetStatic(bool isStatic)
{
	m_actorId.GetActor()->RefOption().isStatic = isStatic;
}

void DsChrProxy::DbgSetGravity(bool isGravity)
{
	m_actorId.GetActor()->RefOption().isGravity = isGravity;
}

void DsChrProxy::DbgSetDrawWireFrame(bool isWireFrame)
{
	m_actorId.GetActor()->RefOption().isDrawWireFrame = isWireFrame;
}