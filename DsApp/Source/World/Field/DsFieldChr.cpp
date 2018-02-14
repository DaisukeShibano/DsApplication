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
#ifndef _DS_AMIM_CUSTOM_PROPERTY_
#include "Animation/DsAnimCustomProperty.h"
#endif
#ifndef _DS_ANIM_RAGDOLL_MODIFIER_
#include "World/Physics/DsAnimRagdollModifier.h"
#endif
#ifndef _DS_RES_PARAM_DS_RAGDOLL_PARAM_
#include "Res/Param/DsRagdollParam.h"
#endif
#ifndef __DS_APP_COLLISION_FILTER__
#include "World/Physics/DsAppCollisionFilter.h"
#endif
#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif
#ifndef _DS_ACTION_CTRL_
#include "World/Field/Action/DsActionCtrl.h"
#endif

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;



DsFieldChr::DsFieldChr(DsSys& sys, DsPhysicsWorld& world)
	: DsFieldObj(sys, world)
	, m_vel(DsVec3d::Zero())
	, m_ang(DsVec3d::Zero())
	, m_pRagdoll(NULL)
	, m_pAnimRagdollModifier(NULL)
	, m_pActReq(NULL)
	, m_pActCtrl(NULL)
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
		//キャラは跳ね返らないし、摩擦もない
		DsActorMaterial mat;
		mat.m_bundCoef = 0.0;
		mat.m_staticFrictionCoef = 0.0;
		mat.m_kinematicFricCoef = 0.0;
		pActor->SetMaterial(mat);
	}
	
	m_actorId.GetActor()->SetUserData(this);

	//ラグドール
	const DsAnimCustomProperty* pProperty = m_pAnimation->GetCustomProperty();
	DsAnimSkeleton* pSkeleton = m_pAnimation->GetSkeleton();

	//地形あたりは内部のものは全て当たらない
	m_actorId.GetActor()->SetCollisionFilter(DsAppCollisionFilter::CalcFilterInsideAllGroup());

	if (pProperty && pSkeleton) {
		m_pRagdoll = new DsRagdoll(pProperty->ragdollParamIds, *pSkeleton, m_world, this, GetPosition(),GetRotation());
		DS_ASSERT(m_pRagdoll, "メモリ確保失敗");

		for (DsRagdollParts& parts : m_pRagdoll->RefParts()) {
			DsRagdollParam param(parts.ragdollParamId);
			parts.damperA = param.GetDamperA();
			parts.damperV = param.GetDamperV();
			parts.mass = param.GetMass();
			parts.collisionFilter = DsAppCollisionFilter::CalcFilterInside(param.GetCollisionGroup());
			//parts.collisionFilter = DsAppCollisionFilter::CalcFilterAllOne();//あらぶるので一時的に何にも当たらないように
			m_pRagdoll->SetParam(parts);
		}

		m_pAnimRagdollModifier = new DsAnimRagdollModifier(*m_pRagdoll);
		DS_ASSERT(m_pAnimRagdollModifier, "メモリ確保失敗");

		m_pAnimation->SetAnimSkeletonModifier(m_pAnimRagdollModifier);
	}

	//アクション
	m_pActReq = _CreareActionRequest();
	m_pActCtrl = new DsActionCtrl(*m_pActReq, m_pAnimation->RefAnimClips());
	DS_ASSERT(m_pActCtrl, "メモリ確保失敗");
	m_pAnimation->RequestPlayAnim(m_pActCtrl->GetCurrentAnim());
}

//virtual
DsActionRequest* DsFieldChr::_CreareActionRequest()
{
	DsActionRequest* ret = new DsActionRequest();
	DS_ASSERT(ret, "メモリ確保失敗");
	return ret;
}

//virtual 
void DsFieldChr::Update(double dt)
{
	m_pActReq->Update(dt);
	m_pActCtrl->Update(dt);
	m_pAnimation->RequestPlayAnim(m_pActCtrl->GetCurrentAnim());

	DsActor* pActor = m_actorId.GetActor();
	if (pActor)
	{
		//y軸はグローバル。x軸はy軸回転後
		const DsMat33d rot = DsMat33d::RotateY(m_ang.y)*DsMat33d::RotateX(m_ang.x);
		m_vel = rot*m_vel;

		const double velGain = 600.0f;//加速ゲイン
		const DsVec3d targetVelF = (m_vel - pActor->GetVelocity())*velGain;
		pActor->AddForce(DsVec3d(targetVelF.x, 0, targetVelF.z));

		DsFieldObj::Update(dt);
		if (m_pAnimation)
		{
			//剛体の底がキャラ座標になるように剛体は持ち上げられてる。
			//モデルデータの足元座標は原点になってる。なのでキャラ座標そのままでいい
			//姿勢は剛体ではなく、m_ang
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
		const double y = pActor->GetPosition().y;
		const DsVec3d offset = DsVec3d(0, -(pActor->RefAabb().GetMax().y-y), 0);
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
		const double y = pActor->GetPosition().y;
		const DsVec3d offset = DsVec3d(0, pActor->RefAabb().GetMax().y-y, 0);
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

