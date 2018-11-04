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
#ifndef _DS_MODEL_
#include "Model/DsModel.h"
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
#include "Res/Param/DsChrParam.h"


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
	, m_pProxy(NULL)
{
}

DsFieldChr::~DsFieldChr()
{
	delete m_pProxy; m_pProxy = NULL;
	delete m_pRagdoll; m_pRagdoll = NULL;
}

//virtual
void DsFieldChr::Initialize(const DsFieldInitInfo& initInfo)
{
	m_pProxy = new DsChrProxy(m_world, this);
	DS_ASSERT(m_pProxy, "�������m�ێ��s");
	//FieldObj��Initialize�ł����`���񂪂Ƃ�Ȃ��̂ō��͒��ł�
	const DsMat33d rot = DsMat33d::RotateX(initInfo.ang.x)*DsMat33d::RotateY(initInfo.ang.y)*DsMat33d::RotateZ(initInfo.ang.z);
	const double height = 1.5;
	m_pProxy->Initialize(0.25, height, 20, initInfo.pos + DsVec3d(0, height*0.5, 0), rot);
	m_pProxy->SetCollisionFilter(DsAppCollisionFilter::CalcFilterInsideAllGroup());

	//���̏�����
	DsFieldObj::Initialize(initInfo);

	//DsFieldObj::Initialize�ł͍��̂̒��S���A�j���̒��S�ɂȂ��Ă���̂ōēx�L�����p��_�ɃZ�b�g������
	SetPosition(initInfo.pos);
	m_ang = initInfo.ang;
	m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());

	//���O�h�[��
	const DsAnimCustomProperty* pProperty = m_pAnimation->GetCustomProperty();
	DsAnimSkeleton* pSkeleton = m_pAnimation->GetSkeleton();

	if (pProperty && pSkeleton) {
		m_pRagdoll = new DsRagdoll(pProperty->ragdollParamIds, *pSkeleton, m_world, this, GetPosition(),GetRotation());
		DS_ASSERT(m_pRagdoll, "�������m�ێ��s");

		for (DsRagdollParts& parts : m_pRagdoll->RefParts()) {
			DsRagdollParam param(parts.ragdollParamId);
			parts.damperA = param.GetDamperA();
			parts.damperV = param.GetDamperV();
			parts.mass = param.GetMass();
			//parts.collisionFilter = DsAppCollisionFilter::CalcFilterInside(param.GetCollisionGroup());
			parts.collisionFilter = DsAppCollisionFilter::CalcFilterAllOne();//���O�h�[���͂�ꕨ�����g���ĂȂ��̂ŉ��ɂ�������Ȃ��������ȁB�_���[�W��������̂Ƃ��J�v�Z���ŏ\��
			m_pRagdoll->SetParam(parts);
		}

		m_pAnimRagdollModifier = new DsAnimRagdollModifier(*m_pRagdoll);
		DS_ASSERT(m_pAnimRagdollModifier, "�������m�ێ��s");

		m_pAnimation->SetAnimSkeletonModifier(m_pAnimRagdollModifier);
	}

	//�A�N�V����
	DsChrParam param(m_name);
	if (!param.IsValid()) {
		param = DsChrParam(0);
		//���c�[���ȂǂŔz�u�O�̃C���X�^���X����邱�Ƃ�����̂Ń_�~�[��p�ӂ�����
	}

	m_pActReq = _CreareActionRequest();
	m_pActCtrl = new DsActionCtrl(*m_pActReq, *GetActionFlags(), m_pAnimation->RefAnimClips(), param);
	DS_ASSERT(m_pActCtrl, "�������m�ێ��s");

	//�����A�j��
	m_pAnimation->RequestPlayAnim(m_pActCtrl->GetCurrentAnim());
}

//virtual
DsActionRequest* DsFieldChr::_CreareActionRequest()
{
	DsActionRequest* ret = new DsActionRequest();
	DS_ASSERT(ret, "�������m�ێ��s");
	return ret;
}

//virtual 
void DsFieldChr::Update(double dt)
{
	//y���̓O���[�o���Bx����y����]��
	const DsMat33d rot = DsMat33d::RotateY(m_ang.y)*DsMat33d::RotateX(m_ang.x);
	m_vel = rot * m_vel;

	DsFieldObj::Update(dt);
	
	//AnimEvent�̌�
	m_pActReq->Update(dt);
	m_pActCtrl->Update(dt);


	if (!IsRequestDirectAnim()) {
		//�X�e�[�g����Đ��A�j������
		m_pAnimation->RequestPlayAnim(m_pActCtrl->GetCurrentAnim());
	}

	DsVec3d move = DsVec3d::Zero();
	if (m_pAnimation){
		m_pAnimation->SetRootMatrix(GetPosition(), DsMat33d::RotateY(m_ang.y));
		move = rot * m_pAnimation->GetDeltaMotion();
	}

	m_pProxy->Drive(dt, move);
}

//virtual
DsVec3d DsFieldChr::GetPosition() const
{
	return m_pProxy->GetPosition();
}

//virtual
DsMat33d DsFieldChr::GetRotation() const
{
	const DsMat33d rot = DsMat33d::RotateY(m_ang.y)*DsMat33d::RotateX(m_ang.x);
	return rot;
}

//virtual
void DsFieldChr::SetPosition(const DsVec3d& pos)
{
	m_pProxy->SetPosition(pos);
	if (m_pAnimation)
	{
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
	}
}
//virtual
void DsFieldChr::SetRotation(const DsMat33d& rot)
{
	const DsVec3d forward = DsVec3d(rot.GetAxisZ().x, 0.0, rot.GetAxisZ().z);
	const double dot = DsVec3d::Dot(DsVec3d::GetZ(), forward);
	const double ang = acos( max(0.0, min(dot, 1.0)) );
	m_ang.y = ang;
}

//virtual
DsVec3d DsFieldChr::GetChrSize() const
{
	DsVec3d ret = DsVec3d::Zero();
	//��̂̃T�C�Y
	if (m_pProxy) {
		const DsAabb aabb = m_pProxy->GetAabb();
		ret = aabb.GetMax() - aabb.GetMin();
	}
	return ret;
}





//virtual 
void DsFieldChr::DbgDraw(DsLib::DsDrawCommand& com)
{
	m_pProxy->DbgDraw(com);
}



//virtual
void DsFieldChr::DbgSetStatic(bool isStatic)
{
	m_pProxy->DbgSetStatic(isStatic);
}

//virtual
void DsFieldChr::DbgSetGravity(bool isGravity)
{
	m_pProxy->DbgSetGravity(isGravity);
}

//virtual
void DsFieldChr::DbgSetDrawWireFrame(bool isWireFrame)
{
	m_pProxy->DbgSetDrawWireFrame(isWireFrame);
}