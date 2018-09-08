#include "DsPhysicsPch.h"
//�����̃w�b�_
#include "DsChrProxy.h"
//���̃w�b�_
#include "Actor/DsRigidCapsule.h"
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif


using namespace DsPhysics;


DsChrProxy::DsChrProxy(DsPhysicsWorld& world)
	: m_world(world)
	, m_actorId()
	, m_pos(DsVec3d::Zero())
{
}

DsChrProxy::~DsChrProxy()
{
}

void DsChrProxy::Initialize(double r, double height, double mass, DsVec3d pos, DsMat33d rot)
{
	//height�͑S��
	height = max(r * 2.0, height);
	height *= 0.5;
	height -= r;
	height = max(height, 0.0);
	DsRigidCapsule::DsRigidCapsuleFactory factory(r, height, mass, "ChrProxy");
	factory.InitPos(pos);
	factory.InitRot(DsMat33d::Identity());//�J�v�Z���ɉ�]�͗v��Ȃ�
	m_actorId = m_world.CreateActor(factory);
	m_actorId.GetActor()->SetMaterial(DsActorMaterial::Aluminum());
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

void DsChrProxy::SetUserData(void *pData)
{
	DsActor* pActor = m_actorId.GetActor();
	if (pActor) {
		pActor->SetUserData(pData);
	}
}

void DsChrProxy::SetCollisionFilter(DsCollisionFilter filter)
{
	DsActor* pActor = m_actorId.GetActor();
	if (pActor) {
		pActor->SetCollisionFilter(filter);
	}
}

void DsChrProxy::Drive(DsVec3d move)
{

}