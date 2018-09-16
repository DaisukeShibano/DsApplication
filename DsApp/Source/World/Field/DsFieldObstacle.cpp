#include "DsAppPch.h"
//自分のヘッダ
#include "World/Field/DsFieldObstacle.h"
//他のヘッダ
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



DsFieldObstacle::DsFieldObstacle(DsSys& sys, DsPhysicsWorld& world)
	: DsFieldObj(sys, world)
	, m_actorId()
{
	
}

DsFieldObstacle::~DsFieldObstacle()
{
	m_actorId = m_world.DeleteActor(m_actorId);
	m_sys.RefResource().UnregisterItem<DsHitRes>(m_hitName.c_str());
}

//virtual
void DsFieldObstacle::Initialize(const DsFieldInitInfo& initInfo)
{
	m_hitName = initInfo.hitName;
	const DsHitRes* pHitRes = m_sys.RefResource().RegisterItem<DsHitRes>(m_hitName.c_str());
	if (pHitRes) {
		m_actorId = DsPhysicsCreator::Create(m_world, *pHitRes, initInfo.physicsType, initInfo.pos, initInfo.ang, initInfo.name);
	}
	else {
		DS_LOG("オブジェ初期化情報が無かったのでオブジェは作られませんでした %s", m_name.c_str());
	}

	DsFieldObj::Initialize(initInfo);
}

//virtual
void DsFieldObstacle::SetPosition(const DsVec3d& pos)
{
	//底を原点にする必要もうなくない？

	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor){
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
void DsFieldObstacle::SetRotation(const DsMat33d& rot)
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor){
		pActor->SetRotation(rot);
	}

	if (m_pAnimation){
		m_pAnimation->SetRootMatrix(GetPosition(), GetRotation());
	}
}

//virtual
DsVec3d DsFieldObstacle::GetPosition() const
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor){
		const double y = pActor->GetPosition().y;
		const DsVec3d offset = DsVec3d(0, -pActor->RefAabb().GetMax().y-y, 0);
		return pActor->GetPosition();
	}
	else{
		return DsVec3d::Zero();
	}
}

//virtual
DsMat33d DsFieldObstacle::GetRotation() const
{
	DsActor* pActor = m_world.GetActor(m_actorId);
	if (pActor){
		return pActor->GetRotation();
	}
	else{
		return DsMat33d::Identity();
	}
}


//virtual 
void DsFieldObstacle::DbgDraw(DsLib::DsDrawCommand& com)
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