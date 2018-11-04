#include "DsAppPch.h"
#ifndef _DS_FIELD_PLAYER_H_
#include "World/Field/DsFieldPlayer.h"
#endif
//���̃w�b�_
#ifndef _DS_ANIM_RES_
#include "Res/DsAnimRes.h"
#endif
#ifndef _DS_HITRES_H_
#include "Res/DsHitRes.h"
#endif
#ifndef _DS_MODEL_
#include "Model/DsModel.h"
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
#ifndef _DS_ACTION_REQUEST_MANUAL_
#include "World/Field/Action/DsActionRequestManual.h"
#endif
#ifndef _DS_COMPONENT_SYSTEM_
#include "World/Component/DsComponentSystem.h"
#endif
#ifndef _DS_ITEM_BOX_COMPONENT_
#include "World/Component/Item/DsItemBoxComponent.h"
#endif
#include "World/Field/LockOn/DsLockOn.h"
#include "World/Field/Action/DsActionFlags.h"



using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;



DsFieldPlayer::DsFieldPlayer(DsSys& sys, DsPhysicsWorld& world)
	: DsFieldChr(sys, world)
	, m_cam(sys.RefCam())
	, m_mouse(sys.RefMouse())
	, m_window(sys.RefWindow())
	, m_pGameSys(NULL)
	, m_isLockOn(false)
	, m_isReqLockOn(false)
{
}

DsFieldPlayer::~DsFieldPlayer()
{
}

//virtual
void DsFieldPlayer::Initialize(const DsFieldInitInfo& initInfo)
{
	m_pGameSys = initInfo.pGameSys;

	DsFieldChr::Initialize(initInfo);
	const DsVec3d pos = DsVec3d(0, 1.5f, -3.5f) + GetPosition();
	m_cam.SetPos(pos);
	m_cam.SetRot(GetRotation());
	
	//�v���C���[���葕��
	DsComponentSystem* pComSys = GetComponentSystem();
	if (pComSys) {
		pComSys->RequestItemBox();
		pComSys->RequestEquip();

		//����������
		DsItemBox* pItemBox = pComSys->GetItemBox();
		if (pItemBox) {
			pItemBox->AddItem(0);
		}
	}
}


//virtual
DsActionRequest* DsFieldPlayer::_CreareActionRequest()
{
	DsActionRequest* ret = new DsActionRequestManual(m_sys);
	DS_ASSERT(ret, "�������m�ێ��s");
	return ret;
}

//virtual 
void DsFieldPlayer::Update(double dt)
{
	{//�L�������W�̍X�V

		//���b�N�I�������ɐ���
		double angVelDeg = 1440.0;
		DsLockOn* pLockOn = (m_pGameSys) ? (m_pGameSys->GetLockOn()) : (NULL);
		const DsActionFlags* pFlags = GetActionFlags();
		DsVec3d lockOnDir;
		bool isLockOn = false;
		if (pFlags && pLockOn) {
			DsVec3d lockOnPos;
			if (pFlags->IsLockOnTurn() && m_isLockOn && pLockOn->GetLockOnPos(lockOnPos)) {
				const DsVec3d toTarget = lockOnPos - GetPosition();
				const DsVec3d toTargetXZ = DsVec3d::Normalize(DsVec3d(toTarget.x, 0, toTarget.z));
				lockOnDir = toTargetXZ;
				angVelDeg = 430.0;
				isLockOn = true;
			}
		}

		//�J������Y����]�ʂ����߂�
		const DsVec3d camFlatZ = DsVec3d::Normalize(DsVec3d(m_cam.GetRot().GetAxisZ().x, 0, m_cam.GetRot().GetAxisZ().z));
		const DsVec3d camRotCross = DsVec3d::Normalize(DsVec3d::Cross(DsVec3d::GetZ(), camFlatZ));
		const double camRotYang = acos(Clamp(DsVec3d::Dot(DsVec3d::GetZ(), camFlatZ), -1.0, 1.0));
		const DsMat33d camRotY = DsMat33d::RotateVec(camRotCross*camRotYang);

		//���͈ړ��ʂ��J��������̍��W�ɕϊ����A�L�����̖ڕW�p�x�ɂ���
		const DsVec3d moveVec = (isLockOn) ? (lockOnDir) : (camRotY * DsVec3d::Normalize(m_pActReq->GetMoveVec()));
		const DsVec3d chrDir = DsMat33d::RotateY(m_ang.y)*DsVec3d::GetZ();
		const DsVec3d axis = DsVec3d::Cross(chrDir, moveVec);
		const double dAng = DsACos(DsVec3d::Dot(chrDir, moveVec));
		double angVel = DegToRad(angVelDeg)*dt;
		double targetY;

		if (0.00001 < moveVec.Length())
		{
			//if (angVel < dAng)
			{
				if (0.0 < axis.y)
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

		const double vel = (moveVec.Length()) * 0.3 * dt * 1000.0;
		m_vel.Set(0, 0, vel);
	}

	DsFieldChr::Update(dt);

	_UpdateCam(dt);

	

}


void DsFieldPlayer::_UpdateCam(double dt)
{
	const DsVec3d chrOffSet = DsVec3d::ToVec3(0, 1.5, 0);//�J�����I�t�Z�b�g����
	const DsVec3d chrOffSetLock = DsVec3d::ToVec3(0, 1.2, 0);//���b�N���J�����I�t�Z�b�g����
	const DsVec3d chrBasePos = GetPosition() + chrOffSet;
	const DsVec3d chrBasePosLock = GetPosition() + chrOffSetLock;
	const double fllowLen = 3.5;//�J�����Ƃ̋���

	DsLockOn* pLockOn = (m_pGameSys) ? (m_pGameSys->GetLockOn()) : (NULL);
	DsVec3d lockOnPos;
	if (pLockOn) {
		const bool isInutLockOn = m_sys.RefKeyboard().IsPush('F');
		const bool isLockOn = isInutLockOn && (!m_isReqLockOn);
		m_isReqLockOn = isInutLockOn;
	
		pLockOn->Update(dt, m_cam.GetPos());

		if (isLockOn) {
			m_isLockOn = !m_isLockOn;
		}

		if (isLockOn) {
			pLockOn->LockOn(m_cam.GetPos(), m_cam.GetRot().GetAxisZ());
		}

		if (m_isLockOn) {
			if ( !pLockOn->GetLockOnPos(lockOnPos) ) {
				m_isLockOn = false;//�͈͓��ɒN�����Ȃ������̂ŉ���
			}
		}
	}

	if (!m_isLockOn)
	{
		//�J�����Ǐ]
		{
			const DsVec3d toChr = chrBasePos - m_cam.GetPos();
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

				DsMat33d camRot = m_cam.GetRot();
				DsMat33d yFix;
				{
					const DsVec3d chr = DsVec3d::Normalize(DsVec3d(toChr.x, 0, toChr.z));
					const DsVec3d cam = DsVec3d::Normalize(DsVec3d(camDir.x, 0, camDir.z));
					double angY = DsVec3d::Dot(chr, cam);
					angY = acos(max(-1.0, min(1.0, angY)));
					const DsVec3d axisY = DsVec3d::Cross(cam, chr);
					camRot = DsMat33d::RotateVec(axisY * angY * dt*10.0)*camRot;
					yFix = DsMat33d::RotateVec(axisY*angY)*camRot;//y����]����v�������J�����p��
				}
				{
					double angX = DsVec3d::Dot(yFix.GetAxisZ(), chrMoveDir);
					angX = acos(max(-1.0, min(1.0, angX)));
					const DsVec3d axisX = DsVec3d::Cross(yFix.GetAxisZ(), chrMoveDir);
					camRot = DsMat33d::RotateVec(axisX*angX*dt*10.0)*camRot;
				}
				m_cam.SetRot(camRot);
			}
		}

		//�E�N���b�N�ŉ�]
		if (m_mouse.GetClickState() == DsMouseClickState::DS_LEFT_CLICK)
		{
			{
				const DsVec2i drag2i = m_mouse.GetDragMove();
				const DsVec3d drag3 = m_window.GetNormalizeScreenCoord(DsVec2i::ToVec2(-drag2i.x, -drag2i.y));//���ʂ������Ă�Ɖ��肵�ĉE��n�ɂȂ�悤�ɁB
				const double moveVel = 2.0;
				const DsMat33d mat = m_cam.GetRot();
				const DsVec3d rotAxisX = mat.GetAxisX();
				const DsVec3d rotAxisY = mat.GetAxisY();

				const DsMat33d rotMatY = DsMat33d::RotateVec(DsVec3d::GetY()*drag3.x*moveVel);
				const DsMat33d rotMatX = rotMatY * DsMat33d::RotateVec(rotAxisX*drag3.y*moveVel*(-1.0));
				const DsMat33d rotMat = rotMatY * rotMatX;
				m_cam.SetRot(rotMat*mat);

				//�����_�𒆐S�����悤�ɂ���
				DsVec3d toLook = m_cam.GetRot().GetAxisZ() * (GetPosition() - m_cam.GetPos()).Length();
				DsVec3d toCam = -toLook;
				DsVec3d toCamDr = rotMat * toCam;
				m_cam.SetPos(m_cam.GetPos() + (toCamDr - toCam));
			}
		}
	}
	else {//���b�N�I����

		const DsVec3d camDir = m_cam.GetRot().GetAxisZ();
		const DsVec3d camDirXZ = DsVec3d::Normalize( DsVec3d(camDir.x, 0, camDir.z) );
		const DsVec3d toTarget = DsVec3d::Normalize(lockOnPos - chrBasePosLock);
		const DsVec3d toTargetXZ = DsVec3d::Normalize( DsVec3d(toTarget.x, 0, toTarget.z) );

		//Y������
		const DsVec3d axis = DsVec3d::Cross(camDirXZ, toTargetXZ);
		const double angY = DsACos( DsVec3d::Dot(camDirXZ, toTargetXZ) );
		const double velY = min(1.0, dt * 10.0);
		const double dY = (0.0 < axis.y) ? (angY * velY) : (-angY * velY);
		const DsMat33d rotY = DsMat33d::RotateY(dY);
		m_cam.SetRot(rotY*m_cam.GetRot());

		//X��
		const double camDirAngX = atan2(camDir.y, sqrt(camDir.x*camDir.x + camDir.z*camDir.z));
		const double toTargetAngX = atan2(toTarget.y, sqrt(toTarget.x*toTarget.x + toTarget.z*toTarget.z));
		const double velX = min(1.0, dt * 10.0);
		const double angX = (camDirAngX - toTargetAngX)*velX;
		const DsMat33d rotX = DsMat33d::RotateAxis(m_cam.GetRot().GetAxisX(), angX);
		m_cam.SetRot(rotX*m_cam.GetRot());

		//�L�����̌��ɍs���悤�ɃJ�������ړ�����
		const DsVec3d targetPos = toTarget * (-fllowLen) + chrBasePosLock;
		const DsVec3d diffPos = targetPos - m_cam.GetPos();
		const DsVec3d dMove = diffPos * (dt * 10.0);
		m_cam.SetPos(m_cam.GetPos() + dMove);
	}

	{//Z����]�𐅕��ɕۂ�
		const DsVec3d tmpX = DsVec3d::Normalize(DsVec3d::Cross(DsVec3d::GetY(), m_cam.GetRot().GetAxisZ()));
		const DsVec3d rotAxis = DsVec3d::Normalize(DsVec3d::Cross(m_cam.GetRot().GetAxisX(), tmpX));
		const double ang = acos(max(-1.0, min(1.0, DsVec3d::Dot(m_cam.GetRot().GetAxisX(), tmpX))));
		m_cam.SetRot(DsMat33d::RotateVec(rotAxis*ang)*m_cam.GetRot());
	}

}


//virtual 
void DsFieldPlayer::DbgDraw(DsLib::DsDrawCommand& com)
{
	DsFieldChr::DbgDraw(com);
}