#include "DsAppPch.h"
#ifndef _DS_COMPONENT_SYSTEM_
#include "World/Component/DsComponentSystem.h"
#endif
//ëºÇÃÉwÉbÉ_
#ifndef _DS_TRACE_EFFECT_COMPONENT_
#include "World/Component/Effect/DsTraceEffectComponent.h"
#endif
#ifndef _DS_SOUND_EFFECT_COMPONENT_
#include "World/Component/Effect/DsSoundEffectComponent.h"
#endif
#ifndef _DS_EQUIP_COMPONENT_
#include "World/Component/Equip/DsEquipComponent.h"
#endif
#ifndef _DS_ITEM_BOX_COMPONENT_
#include "World/Component/Item/DsItemBoxComponent.h"
#endif
#ifndef _DS_ATTACH_COMPONENT_
#include "World/Component/Attach/DsAttachComponent.h"
#endif
#include "World/Component/LockOn/DsLockOnComponent.h"
#include "World/Component/Damage/DsDamageComponent.h"

using namespace DsApp;

DsComponentSystem::DsComponentSystem(DsFieldObj& owner, DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& physWorld, DsGameSys* pGameSys)
	: m_components()
	, m_owner(owner)
	, m_sys(sys)
	, m_physWorld(physWorld)
	, m_pGameSys(pGameSys)
{
}

DsComponentSystem::~DsComponentSystem()
{
	for (auto c : m_components) {
		delete c.second;
	}
	m_components.clear();
}

void DsComponentSystem::Update(double dt)
{
	const COMPONENT_UPDATE_ARG arg(dt, m_owner, m_sys, m_physWorld, m_pGameSys);

	std::vector<KEY> eraseList;
	for (auto c : m_components) {
		const bool isAlive = c.second->Update(arg);
		if (!isAlive) {
			eraseList.push_back(c.first);
		}
	}
	for (const KEY& key : eraseList) {
		DsComponent* pCom = m_components[key];
		delete pCom;
		m_components.erase(key);
	}
}

template<class CLASS>
CLASS* DsComponentSystem::_CreateGetComponent(ds_int64 key)
{
	CLASS* pComponent = NULL;
	const KEY k(typeid(CLASS), key);
	auto it = m_components.find(k);
	if (m_components.end() == it) {
		CLASS* pNew = new CLASS();
		DS_ASSERT(pNew, "ÉÅÉÇÉäämï€é∏îs");
		m_components[k] = pNew;
		pComponent = pNew;
	}
	else {
		pComponent = dynamic_cast<CLASS*>(it->second);
	}
	
	return pComponent;
}

template<class CLASS>
CLASS* DsComponentSystem::_GetComponent(ds_int64 key)const
{
	CLASS* pComponent = NULL;
	const KEY k(typeid(CLASS), key);
	auto it = m_components.find(k);
	if (m_components.end() != it) {
		pComponent = dynamic_cast<CLASS*>(it->second);
	}
	return pComponent;
}

void DsComponentSystem::RequestTraceEffect(ds_int64 key, int effectId, int dmypolyId0, int dmypolyId1)
{
	DsTraceEffectComponent* pComponent = _CreateGetComponent<DsTraceEffectComponent>(key);
	if (pComponent) {
		pComponent->Request(effectId, dmypolyId0, dmypolyId1);
	}
}

void DsComponentSystem::RequestSoundEffect(ds_int64 key, int soundId, int dmypolyId)
{
	DsSoundEffectComponent* pComponent = _CreateGetComponent<DsSoundEffectComponent>(key);
	if (pComponent) {
		pComponent->Request(soundId, dmypolyId);
	}
}

void DsComponentSystem::RequestEquip()
{
	_CreateGetComponent<DsEquipComponent>((ds_uint64)(&m_owner));
}

void DsComponentSystem::RequestItemBox()
{
	_CreateGetComponent<DsItemBoxComponent>((ds_uint64)(&m_owner));
}

void DsComponentSystem::RequestAttachWithUpdate(const DsMat44d& target, DsAttachEntity* pMove, double dt)
{
	DsAttachComponent* pComponent = _CreateGetComponent<DsAttachComponent>((ds_uint64)(&m_owner));
	if (pComponent) {
		pComponent->Request(target, pMove);
		const COMPONENT_UPDATE_ARG arg(dt, m_owner, m_sys, m_physWorld, m_pGameSys);
		pComponent->Update(arg);
		pComponent->Request(target, pMove);
	}
}

void DsComponentSystem::RequestLockOnPoint()
{
	DsLockOnComponent* pComponent = _CreateGetComponent<DsLockOnComponent>((ds_uint64)(&m_owner));
	if (pComponent) {
		pComponent->Request();
	}
}

void DsComponentSystem::RequestDamage(ds_int64 key, int damageId, int dmypolyId0, int dmypolyId1)
{
	DsDamageComponent* pComponent = _CreateGetComponent<DsDamageComponent>((ds_uint64)(&m_owner));
	if (pComponent) {
		pComponent->Request(damageId, dmypolyId0, dmypolyId1);
	}
}

DsItemBoxComponent * DsComponentSystem::GetItemBox()const
{
	return _GetComponent<DsItemBoxComponent>( (ds_uint64)(&m_owner) );
}

DsEquipComponent* DsComponentSystem::GetEquip() const
{
	return _GetComponent<DsEquipComponent>((ds_uint64)(&m_owner));
}

