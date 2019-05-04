#include "DsAppPch.h"
#ifndef _DS_FIELD_OBJ_H_
#include "World/Field/DsFieldObj.h"
#endif
#ifndef _DS_ANIM_RES_
#include "Res/DsAnimRes.h"
#endif
#ifndef _DS_HITRES_H_
#include "Res/DsHitRes.h"
#endif
#ifndef _DS_MAP_RES_H_
#include "Res/DsMapRes.h"
#endif
#ifndef _DS_MODEL_
#include "Model/DsModel.h"
#endif
#ifndef _DS_ANIM_EVENT_CALLBACK_
#include "World/Animation/Event/DsAnimEventCallback.h"
#endif
#ifndef _DS_COMPONENT_SYSTEM_
#include "World/Component/DsComponentSystem.h"
#endif
#ifndef _DS_EQUIP_COMPONENT_
#include "World/Component/Equip/DsEquipComponent.h"
#endif
#include "World/Field/Action/DsActionFlags.h"


using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;



DsFieldObj::DsFieldObj(DsSys& sys, DsPhysicsWorld& world)
	: m_sys(sys)
	, m_name()
	, m_pAnimation(NULL)
	, m_world(world)
	, m_pAnimEventCallback(NULL)
	, m_pComponentSystem(NULL)
	, m_pAnimEventFlags(NULL)
	, m_pGameSys(NULL)
	, m_exVelocity(DsVec3d::Zero())
	, m_reqestIsInit(false)
	, m_isCompleteInit(false)
	, m_isRequestDirectAnim(false)
{
	
}

DsFieldObj::~DsFieldObj()
{
	delete m_pAnimEventCallback; m_pAnimEventCallback = NULL;
	delete m_pAnimation; m_pAnimation = NULL;
	delete m_pComponentSystem; m_pComponentSystem = NULL;
	delete m_pAnimEventFlags; m_pAnimEventFlags = NULL;
}

//virtual
void DsFieldObj::Initialize(const DsFieldInitInfo& initInfo)
{
	m_name = initInfo.name;
	m_pGameSys = initInfo.pGameSys;

	m_pComponentSystem = new DsComponentSystem(*this, m_sys, m_world, initInfo.pGameSys);
	DS_ASSERT(m_pComponentSystem, "メモリ確保失敗");

	m_pAnimation = new DsAnimation(initInfo.animName.c_str(), m_sys.RefRender().RefDrawCom(), m_sys.RefResource());
	DS_ASSERT(m_pAnimation, "メモリ確保失敗");
	m_pAnimEventCallback = new DsAnimEventCallback(*this, m_sys.RefResource());
	DS_ASSERT(m_pAnimEventCallback, "メモリ確保失敗");
	m_pAnimEventCallback->Initialize(initInfo.animName.c_str());
	m_pAnimEventFlags = new DsActionFlags();
	DS_ASSERT(m_pAnimEventFlags, "メモリ確保失敗");

	if (m_pAnimation){
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
		m_pAnimation->RegisterDraw();
	}

	m_reqestIsInit = true;
	m_isCompleteInit = true;
}

//virtual 
void DsFieldObj::Update1(double dt)
{
	//毎フレーム頭でクリア系のフラグクリア
	DsActionFlags* pFlags = GetActionFlags();
	if (pFlags) {
		pFlags->ClearFirst();
	}
}

//virtual 
void DsFieldObj::Update2(double dt)
{
	if (m_pAnimation){
		if (m_isRequestDirectAnim) {
			dt = 0;
		}
		m_pAnimation->Update(dt);
		m_isRequestDirectAnim = false;
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
	}

	if (m_pAnimEventCallback) {
		m_pAnimEventCallback->Call();
	}

	if (m_pComponentSystem) {
		m_pComponentSystem->Update(dt);
	}
}

//virtual
void DsFieldObj::Update3(double dt)
{
	//毎フレーム最後でクリア系のフラグクリア
	DsActionFlags* pFlags = GetActionFlags();
	if (pFlags) {
		pFlags->ClearLast();
	}
}

bool DsFieldObj::GetDmypoly(int id, std::vector<DsMat44d>& outMat, DMYPOLY_SLOT slot)const
{
	bool ret = false;
	
	const DsModel* pModel = NULL;

	if (DMYPOLY_SLOT::MAIN_BODY == slot) {//本体モデルから
		const DsAnimation* pAnim = GetAnim();
		if (pAnim) {
			pModel = pAnim->GetModel();
		}
	}
	else if(DMYPOLY_SLOT::WEP == slot){//武器モデルから
		const DsComponentSystem* pComSys = GetComponentSystem();
		if (pComSys) {
			const DsEquipComponent* pEquip = pComSys->GetEquip();
			if (pEquip) {
				const DsAnimation* pAnim = pEquip->GetWep();
				if (pAnim) {
					pModel = pAnim->GetModel();
				}
			}
		}
	}

	if (pModel) {
		ret = pModel->GetDmypoly(id, outMat);
	}

	return ret;
}

bool DsFieldObj::GetDmypoly(int id, DsMat44d& outMat, DMYPOLY_SLOT slot)const
{
	bool ret = false;
	std::vector<DsMat44d> tmp;
	if (GetDmypoly(id, tmp, slot)) {
		outMat = tmp[0];
		ret = true;
	}
	return ret;
}

//virtual
void DsFieldObj::SetRequestAnim(std::string name)
{
	if (m_pAnimation) {
		for (DsAnimClip* pClip : m_pAnimation->RefAnimClips()) {
			if (name == pClip->RefAnimName()) {
				m_pAnimation->RequestPlayAnim(pClip);
				m_isRequestDirectAnim = true;
				break;
			}
		}
	}
}
