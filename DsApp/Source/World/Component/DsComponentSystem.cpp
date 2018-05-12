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

using namespace DsApp;

DsComponentSystem::DsComponentSystem(DsFieldObj& owner, DsLib::DsSys& sys)
	: m_components()
	, m_owner(owner)
	, m_sys(sys)
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
	const COMPONENT_UPDATE_ARG arg(dt, m_owner, m_sys);

	std::vector<KEY> eraseList;
	for (auto c : m_components) {
		const bool isAlive = c.second->Update(arg);
		if (!isAlive) {
			delete c.second;
			eraseList.push_back(c.first);
		}
	}
	for (const KEY& key : eraseList) {
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

DsItemBoxComponent * DsComponentSystem::GetItemBox()const
{
	return _GetComponent<DsItemBoxComponent>( (ds_uint64)(&m_owner) );
}