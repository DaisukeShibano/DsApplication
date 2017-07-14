#include "DsAppPch.h"
#ifndef _DS_FIELD_CHR_H_
#include "World/Field/DsFieldChr.h"
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



DsFieldChr::DsFieldChr(DsSys& sys, DsPhysicsWorld& world)
	: DsFieldObj(sys, world)
	, m_vel(DsVec3d::Zero())
	, m_ang(DsVec3d::Zero())
{

}

DsFieldChr::~DsFieldChr()
{
}

//virtual
void DsFieldChr::Initialize(const DsFieldInitInfo& initInfo)
{
	DsFieldObj::Initialize(initInfo);

	DsActor* pActor = m_world.GetActor(m_actorId);
	{
		//�L�����͒��˕Ԃ�Ȃ����A���C���Ȃ�
		DsActorMaterial mat;
		mat.m_bundCoef = 0.0;
		mat.m_staticFrictionCoef = 0.0;
		mat.m_kinematicFricCoef = 0.0;
		pActor->SetMaterial(mat);
	}
}

//virtual 
void DsFieldChr::Update(double dt)
{
	DsActor* pActor = m_actorId.GetActor();
	if (pActor)
	{
		//y���̓O���[�o���Bx����y����]��
		const DsMat33d rot = DsMat33d::RotateY(m_ang.y)*DsMat33d::RotateX(m_ang.x);
		m_vel = rot*m_vel;

		const double velGain = 200.0f;//�����Q�C��
		const DsVec3d targetVelF = (m_vel - pActor->GetVelocity())*velGain;
		pActor->AddForce(DsVec3d(targetVelF.x, 0, targetVelF.z));

		DsFieldObj::Update(dt);
		if (m_pAnimation)
		{
			//���̂̒ꂪ�L�������W�ɂȂ�悤�ɍ��͎̂����グ���Ă�B
			//���f���f�[�^�̑������W�͌��_�ɂȂ��Ă�B�Ȃ̂ŃL�������W���̂܂܂ł���
			//�p���͍��̂ł͂Ȃ��Am_ang
			m_pAnimation->SetRootMatrix(GetPosition(), DsMat33d::RotateY(m_ang.y));
		}
	}
}

//virtual
DsVec3d DsFieldChr::GetPosition() const
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		const DsVec3d offset = DsVec3d(0, -pActor->RefAabb().GetMax().y, 0);
		return pActor->GetPosition() + offset;
	}
	else
	{
		return DsVec3d::Zero();
	}
}

//virtual
DsMat33d DsFieldChr::GetRotation() const
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
void DsFieldChr::SetPosition(const DsVec3d& pos)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		const DsVec3d offset = DsVec3d(0, pActor->RefAabb().GetMax().y, 0);
		pActor->SetPosition(pos+offset);
	}

	if (m_pAnimation)
	{
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
	}
}
//virtual
void DsFieldChr::SetRotation(const DsMat33d& rot)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		pActor->SetRotation(rot);
		if (m_pAnimation)
		{
			m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
		}
	}
}


//virtual 
void DsFieldChr::DbgDraw(DsLib::DsDrawCommand& com)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		pActor->SetColor(DsVec4d(1, 1, 1, 0));
		pActor->SetLineColor(DsVec4d(0, 0, 0, 0));
		pActor->Draw(com);
	}
}

