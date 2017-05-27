#include "DsPch.h"
#ifndef _DS_CHR_UNIT_H_
#include "Charactor/DsChrUnit.h"
#endif

#ifndef _DS_SYS_H_
#include "System/DsSys.h"
#endif
#ifndef _DS_ACT_REQ_H_
#include "Animation/ActionRequest/DsActReq.h"
#endif
#ifndef _DS_ANIMATION_H_
#include "Animation/DsAnimation.h"
#endif
#ifndef _DS_RENDER_H_
#include "Graphics/Render/DsRender.h"
#endif
#ifndef _DS_DRAW_COMMAND_H_
#include "Graphics/Command/DsDrawCommand.h"
#endif
#ifndef _DS_LIGHT_MAN_H_
#include "Graphics/Light/DsLightMan.h"
#endif
#ifndef _DS_LIGHT_H_
#include "Graphics/Light/DsLight.h"
#endif

using namespace DsLib;

DsChrUnit::DsChrUnit(DsSys& sys, const DsAnimRes& anim)
	: m_pActReq(NULL)
	, m_pAnimation(NULL)
{
	m_pActReq = new DsActReq(sys);
	m_pAnimation = new DsAnimation(*m_pActReq, anim, sys.RefRender().RefDrawCom());
}

DsChrUnit::DsChrUnit(DsActReq* pAct, const DsAnimRes& anim)
	: m_pActReq(pAct)
	, m_pAnimation(NULL)
{
	m_pAnimation = new DsAnimation(*m_pActReq, anim, DsDbgSys::GetIns().RefDrawCom());
}

DsChrUnit::DsChrUnit(DsSys& sys)
	: m_pActReq(NULL)
	, m_pAnimation(NULL)
{
	m_pActReq = new DsActReq(sys);
	m_pAnimation = new DsAnimation(*m_pActReq);
}

DsChrUnit::~DsChrUnit()
{
	delete m_pActReq;
	m_pActReq = NULL;

	delete m_pAnimation;
	m_pAnimation = NULL;
}

void DsChrUnit::Update(double dt)
{
	m_pActReq->Update(dt);
	m_pAnimation->Update(dt);
	DsLightMan::GetIns().GetLight(0).Enable();
	DsLightMan::GetIns().GetLight(0).SetPos(ToVec3f(m_pAnimation->GetPosition()));
	DsLightMan::GetIns().GetLight(0).SetDir(ToVec3f(m_pAnimation->GetRotation().GetAxisZ()));
}

void DsChrUnit::RegisterDraw(DsDrawCommand& com)
{
	//–{‹C•`‰æ‚É“o˜^
	m_pAnimation->RegisterDraw();
}

void DsChrUnit::SetRootMatrix(const DsVec3d& p, const DsMat33d& r)
{
	m_pAnimation->SetRootMatrix(p, r);
}

