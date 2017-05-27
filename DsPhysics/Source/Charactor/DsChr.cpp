#include "DsPhysicsPch.h"
#ifndef _DS_CHR_
#include "Charactor/DsChr.h"
#endif

#ifndef _DS_CHR_UNIT_H_
#include "Charactor/DsChrUnit.h"
#endif
#ifndef _DS_ANIMATION_H_
#include "Animation/DsAnimation.h"
#endif
#ifndef _DS_AMIM_SKELTON_
#include "Animation/DsAnimSkelton.h"
#endif
#ifndef _DS_SKINMESH_
#include "Animation/SkinMesh/DsSkinMesh.h"
#endif
#ifndef _DS_CHR_PROXY_H_
#include "Charactor/Proxy/DsChrProxy.h"
#endif
#ifndef _DS_ACT_REQ_H_
#include "Animation/ActionRequest/DsActReq.h"
#endif
#ifndef _DS_CAMERA_H_
#include "Graphics/Camera/DsCamera.h"
#endif


using namespace DsLib;
using namespace DsPhysics;

DsChr::DsChr()
	: m_pChrUnit(NULL)
	, m_isInit(false)
	, m_pProxy(NULL)
	, m_pCam(NULL)
{
}

DsChr::~DsChr()
{
	delete m_pChrUnit; m_pChrUnit = NULL;
	delete m_pProxy; m_pProxy = NULL;
}

void DsChr::Initialize(DsSys& sys)
{
	if (!m_isInit)
	{
		m_pChrUnit = new DsChrUnit(sys);
		m_pChrUnit->RegisterDraw(sys.RefRender().RefDrawCom());
		m_pProxy = new DsChrProxy();
		m_pCam = &sys.RefCam();
	}
}

void DsChr::Update(double dt)
{
	{//キャラ座標の更新
		const DsVec3d moveVec = ToMat33d(m_pCam->GetRot())*DsVec3d::Normal(m_pChrUnit->GetActReq()->GetMoveVec());
		const DsVec3d chrDir = DsMat33d::RotateY(m_pProxy->GetAng().y)*DsVec3d::GetZ();
		const DsVec3d axis = DsVec3d::Cross(chrDir, moveVec);
		const double dAng = acos(max(-1.0, min(1.0, DsVec3d::Dot(chrDir, moveVec))));
		const double angVel = DegToRad(720.0)*dt*0.001;
		double targetY;

		if (0.001 < moveVec.Length())
		{
			if (angVel < dAng)
			{
				if (0 < axis.y)
				{
					targetY = m_pProxy->GetAng().y + angVel;
				}
				else
				{
					targetY = m_pProxy->GetAng().y - angVel;
				}
				m_pProxy->SetAng(0, targetY);
			}
		}

		const double vel = (m_pChrUnit->GetActReq()->GetMoveVec().Length() / 3.0) * 0.4 * dt;
		m_pProxy->SetVel(DsVec3d::Get(0, 0, vel));
	}
	
	m_pChrUnit->Update(dt);
	m_pProxy->Update(dt);

	const DsMat33d animRot = DsMat33d::RotateY(m_pProxy->GetAng().y);
	const DsVec3d animPos = m_pProxy->GetPos();
	m_pChrUnit->SetRootMatrix(animPos, animRot);
	_UpdateCam(dt);
}

void DsChr::_UpdateCam(double dt)
{
	const float fllowLen = 3.5f;//カメラとの距離
	const DsVec3f chrOffSet = DsVec3f::Get(0, 1.5f, 0);//カメラオフセット高さ
	const DsVec3f toChr = ToVec3f(m_pProxy->GetPos()) + chrOffSet - m_pCam->GetPos();
	const float toChrLen = toChr.Length();

	//if (toChrLen > fllowLen)
	{
		//カメラとキャラの線上のキャラからfllowLen分離れた位置がカメラの目標位置。にすると低い位置になってしまうときがあるのでｙはキャラの頭上に固定
		DsVec3f camTargetPos = ToVec3f(m_pProxy->GetPos()) - DsVec3f::Normal(toChr)*fllowLen;
		camTargetPos.y = static_cast<float>(m_pProxy->GetPos().y) + chrOffSet.y;

		const DsVec3f targetDir = camTargetPos - m_pCam->GetPos();
		const DsVec3f camVel = targetDir * static_cast<float>(dt) * 0.001f * 2.0f;//キャラーカメラ位置を保つように
		m_pCam->SetPos(camVel + m_pCam->GetPos());
	}
	
	if (0.1 < m_pProxy->GetVel().Length())
	{
		DsVec3f chrMoveDir = DsVec3f::Normal(DsVec3f::Get(toChr.x, 0, toChr.z));
		DsVec3f camDir = m_pCam->GetRot().GetAxisZ();
		camDir = DsVec3f::Normal(DsVec3f::Get(camDir.x, 0, camDir.z));
		const DsVec3f axis = DsVec3f::Cross( camDir, chrMoveDir );
		float ang = acosf( max(-1.0f, min(1.0f, DsVec3f::Dot(chrMoveDir, camDir))) );
		if (0.001 < fabs(ang))
		{
			const float dAng = ang * static_cast<float>(dt) *0.001f * 10.0f;
			const DsMat33f dr = DsMat33f::RotateVec(axis*dAng);
			m_pCam->SetRot(dr*m_pCam->GetRot());
		}
	}
	
}

void DsChr::DbgDrawSkelton()
{
	if (m_pChrUnit->GetAnimation()->GetSkelton())
	{
		m_pChrUnit->GetAnimation()->GetSkelton()->DbgDraw();
	}
}

void DsChr::DbgDrawSkinMesh()
{
	m_pChrUnit->GetAnimation()->GetSkinMesh()->DbgDraw();
}