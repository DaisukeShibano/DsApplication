#include "DsAppPch.h"
#ifndef _DS_FIELD_PLAYER_H_
#include "World/Field/DsFieldPlayer.h"
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
#ifndef _DS_MOUSE_H_
#include "Mouse/DsMouse.h"
#endif
#ifndef _DS_WINDOW_H_
#include "System/DsWindow.h"
#endif
#ifndef _DS_RAGDOLL_
#include "Ragdoll/DsRagdoll.h"
#endif
#ifndef _DS_ANIMATION_H_
#include "Animation/DsAnimation.h"
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

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;



DsFieldPlayer::DsFieldPlayer(DsSys& sys, DsPhysicsWorld& world)
	: DsFieldChr(sys, world)
	, m_cam(sys.RefCam())
	, m_mouse(sys.RefMouse())
	, m_window(sys.RefWindow())
	, m_actReq(sys)
	, m_pRagdoll(NULL)
	, m_pAnimRagdollModifier(NULL)
{
}

DsFieldPlayer::~DsFieldPlayer()
{
	delete m_pRagdoll; m_pRagdoll = NULL;
}

//virtual
void DsFieldPlayer::Initialize(const DsFieldInitInfo& initInfo)
{
	DsFieldChr::Initialize(initInfo);
	const DsVec3d pos = DsVec3d(0, 1.5f, -3.5f) + GetPosition();
	m_cam.SetPos(pos);
	m_cam.SetRot(GetRotation());
	const DsAnimCustomProperty* pProperty = m_pAnimation->GetCustomProperty();
	DsAnimSkeleton* pSkeleton = m_pAnimation->GetSkeleton();
	
	//�n�`������͓����̂��̂͑S�ē�����Ȃ�
	m_actorId.GetActor()->SetCollisionFilter(DsAppCollisionFilter::CalcFilterInsideAllGroup());

	if (pProperty && pSkeleton) {
		m_pRagdoll = new DsRagdoll(pProperty->ragdollParamIds, *pSkeleton, m_world, this);
		DS_ASSERT(m_pRagdoll, "�������m�ێ��s");

		for (DsRagdollParts& parts : m_pRagdoll->RefParts()) {
			DsRagdollParam param(parts.ragdollParamId);
			parts.damperA = param.GetDamperA();
			parts.damperV = param.GetDamperV();
			parts.mass = param.GetMass();
			parts.collisionFilter = DsAppCollisionFilter::CalcFilterInside(param.GetCollisionGroup());
			parts.collisionFilter = DsAppCollisionFilter::CalcFilterAllOne();//����Ԃ�̂ňꎞ�I�ɉ��ɂ�������Ȃ��悤��
			m_pRagdoll->SetParam(parts);
		}

		m_pAnimRagdollModifier = new DsAnimRagdollModifier(*m_pRagdoll);
		DS_ASSERT(m_pAnimRagdollModifier, "�������m�ێ��s");

		m_pAnimation->SetAnimSkeletonModifier(m_pAnimRagdollModifier);
	}

	m_actorId.GetActor()->SetUserData(this);
}

//virtual 
void DsFieldPlayer::Update(double dt)
{
	m_actReq.Update(dt);
	{//�L�������W�̍X�V

		//�J������Y����]�ʂ����߂�
		const DsVec3d camFlatZ = DsVec3d::Normalize(DsVec3d(m_cam.GetRot().GetAxisZ().x, 0, m_cam.GetRot().GetAxisZ().z));
		const DsVec3d camRotCross = DsVec3d::Normalize( DsVec3d::Cross(DsVec3d::GetZ(), camFlatZ));
		const double camRotYang = acos(Clamp(DsVec3d::Dot(DsVec3d::GetZ(), camFlatZ), -1.0, 1.0));
		const DsMat33d camRotY = DsMat33d::RotateVec(camRotCross*camRotYang);

		//���͈ړ��ʂ��J��������̍��W�ɕϊ����A�L�����̖ڕW�p�x�ɂ���
		const DsVec3d moveVec = camRotY*DsVec3d::Normalize(m_actReq.GetMoveVec());
		const DsVec3d chrDir = DsMat33d::RotateY(m_ang.y)*DsVec3d::GetZ();
		const DsVec3d axis = DsVec3d::Cross(chrDir, moveVec);
		const double dAng = acos(Clamp(DsVec3d::Dot(chrDir, moveVec), -1.0, 1.0));
		const double angVel = DegToRad(1440.0)*dt;
		double targetY;

		if (0.00001 < moveVec.Length())
		{
			//if (angVel < dAng)
			{
				if (0 < axis.y)
				{
					targetY = m_ang.y + min(angVel, fabs(dAng));
				}
				else
				{
					targetY = m_ang.y - min(angVel, fabs(dAng));
				}
				m_ang.Set(0, targetY, 0);
			}
		}

		const double vel = (m_actReq.GetMoveVec().Length()) * 0.3 * dt * 1000.0;
		m_vel.Set(0, 0, vel);
	}

	DsFieldChr::Update(dt);
	_UpdateCam(dt);
}


void DsFieldPlayer::_UpdateCam(double dt)
{
	//�J�����Ǐ]
	{
		const double fllowLen = 3.5;//�J�����Ƃ̋���
		const DsVec3d chrOffSet = DsVec3d::ToVec3(0, 1.5, 0);//�J�����I�t�Z�b�g����
		const DsVec3d toChr = GetPosition() + chrOffSet - m_cam.GetPos();
		const double toChrLen = toChr.Length();

		if (toChrLen > fllowLen)
		{
			//�J�����ƃL�����̐���̃L��������fllowLen�����ꂽ�ʒu���J�����̖ڕW�ʒu�B�ɂ���ƒႢ�ʒu�ɂȂ��Ă��܂��Ƃ�������̂ł��̓L�����̓���ɌŒ�
			DsVec3d camTargetPos = GetPosition() - DsVec3d::Normalize(toChr)*fllowLen;
			camTargetPos.y = GetPosition().y + chrOffSet.y;

			const DsVec3d targetDir = camTargetPos - m_cam.GetPos();
			const DsVec3d camVel = targetDir * dt * 6.0;//�L�����[�J�����ʒu��ۂ悤��
			m_cam.SetPos(camVel + m_cam.GetPos());
		}


		//�L�����𒆐S�ɑ�����
		//if (0.1 < m_vel.Length())
		{
			DsVec3d chrMoveDir = DsVec3d::Normalize(DsVec3d::ToVec3(toChr.x, toChr.y/*0*/, toChr.z));
			DsVec3d camDir = m_cam.GetRot().GetAxisZ();
			camDir = DsVec3d::Normalize(DsVec3d::ToVec3(camDir.x, camDir.y/*0*/, camDir.z));
			const DsVec3d axis = DsVec3d::Cross(camDir, chrMoveDir);
			double ang = acos(max(-1.0, min(1.0, DsVec3d::Dot(chrMoveDir, camDir))));
			if (0.001 < fabs(ang))
			{
				const double dAng = ang * dt * 10.0;
				const DsMat33d dr = DsMat33d::RotateVec(axis*dAng);
				m_cam.SetRot(dr*m_cam.GetRot());
			}
		}
	}

	//�E�N���b�N�ŉ�]
	if (m_mouse.GetClickState() == DsMouseClickState::DS_LEFT_CLICK)
	{
		const DsVec2i drag2i = m_mouse.GetDragMove();
		const DsVec3d drag3 = m_window.GetNormalizeScreenCoord(DsVec2i::ToVec2(-drag2i.x, -drag2i.y));//���ʂ������Ă�Ɖ��肵�ĉE��n�ɂȂ�悤�ɁB
		const double moveVel = 6.0;
		const DsMat33d mat = m_cam.GetRot();
		const DsVec3d rotAxisX = mat.GetAxisX();
		const DsVec3d rotAxisY = mat.GetAxisY();
		const DsMat33d rotMat = DsMat33d::RotateVec(rotAxisY*drag3.x*moveVel) * 
			DsMat33d::RotateVec(rotAxisX*drag3.y*moveVel*(-1.0));
		m_cam.SetRot(rotMat*mat);

		//�L�����𒆐S�����悤�ɂ���
		DsVec3d toCam = m_cam.GetPos() - GetPosition();
		DsVec3d toChrSrc = -toCam;
		toCam = rotMat*toCam;
		m_cam.SetPos(m_cam.GetPos() + (toChrSrc+toCam) );
		//�J���������������Ƃɂ�鋗���Ǐ]�������
		//�L�������J�����̒��S�ɂ��Ȃ��ꍇ�ɂ����
	}
}

//virtual 
DsAnimController& DsFieldPlayer::RefAnimController()
{
	return m_actReq;
}



//virtual 
void DsFieldPlayer::DbgDraw(DsLib::DsDrawCommand& com)
{
	DsFieldChr::DbgDraw(com);
}