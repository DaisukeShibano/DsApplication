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
#ifndef _DS_ACTION_REQUEST_MANUAL_
#include "World/Field/Action/DsActionRequestManual.h"
#endif

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;



DsFieldPlayer::DsFieldPlayer(DsSys& sys, DsPhysicsWorld& world)
	: DsFieldChr(sys, world)
	, m_cam(sys.RefCam())
	, m_mouse(sys.RefMouse())
	, m_window(sys.RefWindow())
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
}


//virtual
DsActionRequest* DsFieldPlayer::_CreareActionRequest()
{
	DsActionRequest* ret = new DsActionRequestManual(m_sys);
	DS_ASSERT(ret, "メモリ確保失敗");
	return ret;
}

//virtual 
void DsFieldPlayer::Update(double dt)
{
	//m_actorId.GetActor()->RefOption().isGravity = false;
	{//キャラ座標の更新

		//カメラのY軸回転量を求める
		const DsVec3d camFlatZ = DsVec3d::Normalize(DsVec3d(m_cam.GetRot().GetAxisZ().x, 0, m_cam.GetRot().GetAxisZ().z));
		const DsVec3d camRotCross = DsVec3d::Normalize( DsVec3d::Cross(DsVec3d::GetZ(), camFlatZ));
		const double camRotYang = acos(Clamp(DsVec3d::Dot(DsVec3d::GetZ(), camFlatZ), -1.0, 1.0));
		const DsMat33d camRotY = DsMat33d::RotateVec(camRotCross*camRotYang);

		//入力移動量をカメラからの座標に変換し、キャラの目標角度にする
		const DsVec3d moveVec = camRotY*DsVec3d::Normalize(m_pActReq->GetMoveVec());
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

		const double vel = (moveVec.Length()) * 0.3 * dt * 1000.0;
		m_vel.Set(0, 0, vel);


		if(0.001 < m_vel.LengthSq())
		{//障害物乗り越え
			DsRay ray;
			ray.SetUserData(this);
			ray.SetCollisionFilter(DsAppCollisionFilter::CalcFilterInsideAllGroup());
			ray.Create(GetPosition() + DsVec3d(0, 0.1, 0), GetPosition() + DsVec3d(0, -0.1, 0));
			if (m_world.RayCast_CollectNear(ray)){//レイ重すぎ！！
				//接地している

				const DsMat33d rot = DsMat33d::RotateY(m_ang.y)*DsMat33d::RotateX(m_ang.x);
				const DsVec3d vel = rot*m_vel;

				DsActor* pActor = m_actorId.GetActor();
				if (pActor) {
					const DsVec3d aabbLen = pActor->RefAabb().GetMax();
					const double radius = sqrt(aabbLen.x*aabbLen.x + aabbLen.z*aabbLen.z);
					const DsVec3d targetPos = DsVec3d::Normalize(vel)*radius + vel*dt + GetPosition();//大体の移動先
					const DsVec3d begin = targetPos + DsVec3d::Up()*aabbLen.y*0.5;

					double depth = 0;
					DsVec3d hitPos;
					DsRay ray2;
					ray2.SetUserData(this);
					ray2.SetCollisionFilter(DsAppCollisionFilter::CalcFilterInsideAllGroup());
					ray2.Create(begin, targetPos);
					if (m_world.RayCast_CollectNear(ray2, &depth, &hitPos)) {
						const double upHeight = hitPos.y - GetPosition().y;
						const double rideVel = max(upHeight,0.0)/dt;
						const double velGain = 200.0f;//加速ゲイン
						const double targetVelF = (rideVel - pActor->GetVelocity().y)*velGain;
						pActor->AddForce(DsVec3d(0, targetVelF, 0));

						//DsDbgSys::GetIns().RefDrawCom().SetColor(DsVec3d(1, 0, 0)).DrawSphere(hitPos, 0.1);
					}
				}

			}

		}
	}

	DsFieldChr::Update(dt);
	_UpdateCam(dt);
}


void DsFieldPlayer::_UpdateCam(double dt)
{
	//カメラ追従
	{
		const double fllowLen = 3.5;//カメラとの距離
		const DsVec3d chrOffSet = DsVec3d::ToVec3(0, 1.5, 0);//カメラオフセット高さ
		const DsVec3d toChr = GetPosition() + chrOffSet - m_cam.GetPos();
		const double toChrLen = toChr.Length();

		if (toChrLen > fllowLen)
		{
			//カメラとキャラの線上のキャラからfllowLen分離れた位置がカメラの目標位置。にすると低い位置になってしまうときがあるのでｙはキャラの頭上に固定
			DsVec3d camTargetPos = GetPosition() - DsVec3d::Normalize(toChr)*fllowLen;
			camTargetPos.y = GetPosition().y + chrOffSet.y;

			const DsVec3d targetDir = camTargetPos - m_cam.GetPos();
			const DsVec3d camVel = targetDir * dt * 6.0;//キャラーカメラ位置を保つように
			m_cam.SetPos(camVel + m_cam.GetPos());
		}


		//キャラを中心に捉える
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
				yFix = DsMat33d::RotateVec(axisY*angY)*camRot;//y軸回転を一致させたカメラ姿勢
			}
			{
				double angX = DsVec3d::Dot(yFix.GetAxisZ(), chrMoveDir);
				angX = acos(max(-1.0, min(1.0, angX)));
				const DsVec3d axisX = DsVec3d::Cross(yFix.GetAxisZ(), chrMoveDir);
				camRot = DsMat33d::RotateVec(axisX*angX*dt*10.0)*camRot;
			}
			m_cam.SetRot(camRot);

			//const DsVec3d axis = DsVec3d::Cross(camDir, chrMoveDir);
			//double ang = acos(max(-1.0, min(1.0, DsVec3d::Dot(chrMoveDir, camDir))));
			//if (0.001 < fabs(ang))
			//{
			//	const double dAng = ang * dt * 10.0;
			//	const DsMat33d dr = DsMat33d::RotateVec(axis*dAng);
			//	m_cam.SetRot(dr*m_cam.GetRot());
			//}
		}
	}

	//右クリックで回転
	if (m_mouse.GetClickState() == DsMouseClickState::DS_LEFT_CLICK)
	{
		{
			const DsVec2i drag2i = m_mouse.GetDragMove();
			const DsVec3d drag3 = m_window.GetNormalizeScreenCoord(DsVec2i::ToVec2(-drag2i.x, -drag2i.y));//正面を向いてると仮定して右手系になるように。
			const double moveVel = 2.0;
			const DsMat33d mat = m_cam.GetRot();
			const DsVec3d rotAxisX = mat.GetAxisX();
			const DsVec3d rotAxisY = mat.GetAxisY();

			const DsMat33d rotMatY = DsMat33d::RotateVec(DsVec3d::GetY()*drag3.x*moveVel);
			const DsMat33d rotMatX = rotMatY*DsMat33d::RotateVec(rotAxisX*drag3.y*moveVel*(-1.0));
			const DsMat33d rotMat = rotMatY*rotMatX;
			m_cam.SetRot(rotMat*mat);

			//注視点を中心を回るようにする
			DsVec3d toLook = m_cam.GetRot().GetAxisZ() * (GetPosition() - m_cam.GetPos()).Length();
			DsVec3d toCam = -toLook;
			DsVec3d toCamDr = rotMat*toCam;
			m_cam.SetPos(m_cam.GetPos() + (toCamDr - toCam));
		}
	}

	{//Z軸回転を水平に保つ
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