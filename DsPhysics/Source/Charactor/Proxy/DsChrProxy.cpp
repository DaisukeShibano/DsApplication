#include "DsPhysicsPch.h"


#ifndef _DS_PHYSICS_H_
#include "DsPhysics.h"
#endif
#ifndef _DS_CHR_PROXY_H_
#include "Charactor/Proxy/DsChrProxy.h"
#endif
#ifndef __DS_ACTOR_MATERIAL__
#include "Actor/DsActorMaterial.h"
#endif //__DS_ACTOR_MATERIAL__


using namespace DsPhysics;

DsChrProxy::DsChrProxy()
	: m_actorId()
	, m_vel(DsVec3d::Zero())
	, m_ang(DsVec3d::Zero())
{
	DsPhysicsWorld* pWorld = DsPhysicsManager::GetDefaultWorld();
	if (pWorld)
	{
		DsVec3d vertex[8];
		DsRigidBodyCube::GetVertex(vertex, 0.6, 1.4, 0.6);
		DsRigidBodyCube::DsRigidBodyFactory factory(vertex, 1.0, "�L�����v���L�V");
		factory.InitPos(DsVec3d(0, 0, 0));
		factory.InitRot(DsMat33d::Identity());
		factory.SetOption(DsActor::Option::ChrProxy());
		m_actorId = pWorld->CreateActor(factory);

		DsActorMaterial material;
		material.m_bundCoef = 0.00001;//�L�����͒��˕Ԃ����肵�Ȃ��̂ŁB
		m_actorId.GetActor()->SetMaterial(material);
	}
}

DsChrProxy::~DsChrProxy()
{
	DsPhysicsWorld* pWorld = DsPhysicsManager::GetDefaultWorld();
	if (pWorld)
	{
		m_actorId = pWorld->DeleteActor(m_actorId);
	}
}

void DsChrProxy::Update(double dt)
{
	DsActor* pActor = m_actorId.GetActor();
	if (pActor)
	{
		//y���̓O���[�o���Bx����y����]��
		const DsMat33d rot = DsMat33d::RotateY(m_ang.y)*DsMat33d::RotateX(m_ang.x);
		m_vel = rot*m_vel;
		pActor->SetExVelocity(m_vel);
	}
}

const DsVec3d DsChrProxy::GetPos() const
{
	const DsActor* pActor = m_actorId.GetActor();
	if (pActor)
	{
		return pActor->GetPosition()+DsVec3d::Get(0, -0.7, 0);//���̂̒��S���W�ɂȂ��Ă�̂ŁA��ɂ��邽�ߔ��a��������
	}
	return DsVec3d::Zero();
}

const DsVec3d DsChrProxy::GetVel() const
{
	return m_vel;
}

const DsMat33d& DsChrProxy::GetRot() const
{
	const DsActor* pActor = m_actorId.GetActor();
	if (pActor)
	{
		return pActor->GetRotation();
	}
	static DsMat33d dummy = DsMat33d::Identity();
	return dummy;
}