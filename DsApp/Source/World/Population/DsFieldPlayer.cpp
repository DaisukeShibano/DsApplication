#include "DsAppPch.h"
#ifndef _DS_FIELD_PLAYER_H_
#include "World/Population/DsFieldPlayer.h"
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

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;



DsFieldPlayer::DsFieldPlayer(DsSys& sys, DsPhysicsWorld& world)
	: DsFieldChr(sys, world)
	, m_cam(sys.RefCam())
	, m_mouse(sys.RefMouse())
	, m_window(sys.RefWindow())
	, m_actReq(sys)
{
}

DsFieldPlayer::~DsFieldPlayer()
{
}

//virtual
void DsFieldPlayer::Initialize(const InitInfo& initInfo)
{
	DsFieldChr::Initialize(initInfo);
	const DsVec3d pos = DsVec3d(0, 1.5f, -3.5f) + GetPosition();
	m_cam.SetPos(pos);
	m_cam.SetRot(GetRotation());
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
		const DsVec3d chrOffSet = DsVec3d::ToVec3(0, 1.5f, 0);//�J�����I�t�Z�b�g����
		const DsVec3d toChr = ToVec3f(GetPosition()) + chrOffSet - m_cam.GetPos();
		const double toChrLen = toChr.Length();

		//if (toChrLen > fllowLen)
		{
			//�J�����ƃL�����̐���̃L��������fllowLen�����ꂽ�ʒu���J�����̖ڕW�ʒu�B�ɂ���ƒႢ�ʒu�ɂȂ��Ă��܂��Ƃ�������̂ł��̓L�����̓���ɌŒ�
			DsVec3d camTargetPos = ToVec3f(GetPosition()) - DsVec3d::Normalize(toChr)*fllowLen;
			camTargetPos.y = static_cast<float>(GetPosition().y) + chrOffSet.y;

			const DsVec3d targetDir = camTargetPos - m_cam.GetPos();
			const DsVec3d camVel = targetDir * static_cast<float>(dt) * 2.0f;//�L�����[�J�����ʒu��ۂ悤��
			m_cam.SetPos(camVel + m_cam.GetPos());
		}

		if (0.1 < m_vel.Length())
		{
			DsVec3d chrMoveDir = DsVec3d::Normalize(DsVec3d::ToVec3(toChr.x, 0, toChr.z));
			DsVec3d camDir = m_cam.GetRot().GetAxisZ();
			camDir = DsVec3d::Normalize(DsVec3d::ToVec3(camDir.x, 0, camDir.z));
			const DsVec3d axis = DsVec3d::Cross(camDir, chrMoveDir);
			double ang = acos(max(-1.0, min(1.0, DsVec3d::Dot(chrMoveDir, camDir))));
			if (0.001 < fabs(ang))
			{
				const double dAng = ang * dt * 10.0f;
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
		const float moveVel = 6.0f;
		const DsMat33d mat = m_cam.GetRot();
		const DsVec3d rotAxisX = mat.GetAxisX();
		const DsVec3d rotAxisY = mat.GetAxisY();
		const DsMat33d rotMat = DsMat33d::RotateVec(rotAxisY*drag3.x*moveVel) * 
			DsMat33d::RotateVec(rotAxisX*drag3.y*moveVel*(-1.0f));
		m_cam.SetRot(rotMat*mat);

		//�L�����𒆐S�����悤�ɂ���
		DsVec3d toCam = m_cam.GetPos() - GetPosition();
		toCam = rotMat*toCam;
		m_cam.SetPos(GetPosition() + toCam);
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