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
#include "World/Physics/DsPhysicsCreator.h"

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;



DsFieldObj::DsFieldObj(DsSys& sys, DsPhysicsWorld& world)
	: m_sys(sys)
	, m_name()
	, m_pAnimation(NULL)
	, m_actorId()
	, m_world(world)
	, m_pAnimEventCallback(NULL)
	, m_pComponentSystem(NULL)
	, m_reqestIsInit(false)
	, m_isCompleteInit(false)
	, m_hitName()
	, m_isRequestDirectAnim(false)
{
	
}

DsFieldObj::~DsFieldObj()
{
	m_actorId = m_world.DeleteActor(m_actorId);
	delete m_pAnimEventCallback; m_pAnimEventCallback = NULL;
	delete m_pAnimation; m_pAnimation = NULL;
	delete m_pComponentSystem; m_pComponentSystem = NULL;

	m_sys.RefResource().UnregisterItem<DsHitRes>(m_hitName.c_str());

}

//virtual
void DsFieldObj::Initialize(const DsFieldInitInfo& initInfo)
{
	m_name = initInfo.name;
	m_hitName = initInfo.hitName;

	m_pComponentSystem = new DsComponentSystem(*this, m_sys);
	DS_ASSERT(m_pComponentSystem, "メモリ確保失敗");

	m_pAnimation = new DsAnimation(initInfo.animName.c_str(), m_sys.RefRender().RefDrawCom(), m_sys.RefResource());
	DS_ASSERT(m_pAnimation, "メモリ確保失敗");
	m_pAnimEventCallback = new DsAnimEventCallback(*this, m_sys.RefResource());
	DS_ASSERT(m_pAnimEventCallback, "メモリ確保失敗");
	m_pAnimEventCallback->Initialize(initInfo.animName.c_str());

	const DsHitRes* pHitRes = m_sys.RefResource().RegisterItem<DsHitRes>(m_hitName.c_str());
	if (pHitRes){
		m_actorId = DsPhysicsCreator::Create(m_world, *pHitRes, initInfo.physicsType, initInfo.pos, initInfo.ang, m_name.c_str());
	}
	else{
		DS_LOG("オブジェ初期化情報が無かったのでオブジェは作られませんでした %s", m_name.c_str());
	}

	if (m_pAnimation){
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
		m_pAnimation->RegisterDraw();
	}

	m_reqestIsInit = true;
	m_isCompleteInit = true;
}

//virtual 
void DsFieldObj::Update(double dt)
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
void DsFieldObj::SetPosition(const DsVec3d& pos)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		const double y = pActor->GetPosition().y;
		const DsVec3d offset = DsVec3d(0, pActor->RefAabb().GetMax().y-y, 0);
		pActor->SetPosition(pos);

		if (m_pAnimation)
		{
			m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
		}
	}
}
//virtual
void DsFieldObj::SetRotation(const DsMat33d& rot)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		pActor->SetRotation(rot);
	}

	if (m_pAnimation)
	{
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
	}
}

//virtual
DsVec3d DsFieldObj::GetPosition() const
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		const double y = pActor->GetPosition().y;
		const DsVec3d offset = DsVec3d(0, -pActor->RefAabb().GetMax().y-y, 0);
		return pActor->GetPosition();
	}
	else
	{
		return DsVec3d::Zero();
	}
}

//virtual
DsMat33d DsFieldObj::GetRotation() const
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
			DsEquipComponent* pEquip = pComSys->GetEquip();
			if (pEquip) {
				DsAnimation* pAnim = pEquip->GetWep();
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


//virtual 
void DsFieldObj::DbgDraw(DsLib::DsDrawCommand& com)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor)
	{
		//メッシュは描画モデルと被るので弾く
		if (pActor->GetType() != DsActor::RIGID_MESH)
		{
			pActor->SetColor(DsVec4d(1, 1, 1, 0));
			pActor->SetLineColor(DsVec4d(0, 0, 0, 0));
			pActor->Draw(com);
		}
	}
}