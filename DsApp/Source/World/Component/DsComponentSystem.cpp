#include "DsAppPch.h"
#ifndef _DS_COMPONENT_SYSTEM_
#include "World/Component/DsComponentSystem.h"
#endif
//他のヘッダ
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
#include "World/Component/Effect/DsHitEffectComponent.h"

using namespace DsApp;

DsComponentSystem::DsComponentSystem(DsFieldObj& owner, DsLib::DsSys& sys, DsPhysics::DsPhysicsWorld& physWorld, DsGameSys* pGameSys)
	: m_components()
	, m_oneShotComponents()
	, m_owner(owner)
	, m_sys(sys)
	, m_physWorld(physWorld)
	, m_pGameSys(pGameSys)
{
}

DsComponentSystem::~DsComponentSystem()
{
	for (auto t : m_components) {
		for (auto c : t.second) {
			delete c.second;
		}
		t.second.clear();
	}
	m_components.clear();

	for(DsComponent* pC : m_oneShotComponents){
		delete pC;
	}
	m_oneShotComponents.clear();
}

void DsComponentSystem::Update(double dt)
{
	const COMPONENT_UPDATE_ARG arg(dt, m_owner, m_sys, m_physWorld, m_pGameSys);

	{//キー管理コンポーネント更新
		struct _KEY_PAIR
		{
			std::type_index type;
			KEY key;
		};

		std::vector<_KEY_PAIR> eraseList;
		for (auto t : m_components) {
			for (auto c : t.second) {
				const bool isAlive = c.second->Update(arg);
				if (!isAlive) {
					eraseList.push_back({ t.first, c.first });
				}
			}
		}

		for (const _KEY_PAIR& key : eraseList) {
			DsComponent* pCom = m_components[key.type][key.key];
			delete pCom;
			m_components[key.type].erase(key.key);
			//クラスの種類別なのでそこまで増えないはず。
			//それよりもメモリの確保解放が頻繁に走る方がいやなのでeraseはしない
			//if (m_components[key.type].empty()) {
			//	m_components.erase(key.type);
			//}
		}
	}

	{//ワンショットコンポートネント更新
		auto it = m_oneShotComponents.begin();
		while (it != m_oneShotComponents.end()) {
			const bool isAlive = (*it)->Update(arg);
			if (!isAlive) {
				it = m_oneShotComponents.erase(it);
			}
			else {
				++it;
			}
		}
	}
}

template<class CLASS>
CLASS* DsComponentSystem::_CreateGetComponent(ds_int64 key)
{
	CLASS* pComponent = NULL;
	const KEY k( key);
	KeyMap& map = m_components[typeid(CLASS)];
	auto it = map.find(k);
	if (map.end() == it) {
		CLASS* pNew = new CLASS();
		DS_ASSERT(pNew, "メモリ確保失敗");
		map[k] = pNew;
		pComponent = pNew;
	}
	else {
		//slotNo=-1は毎回生成
		pComponent = dynamic_cast<CLASS*>(it->second);
	}

	return pComponent;
}

template<class CLASS>
CLASS* DsComponentSystem::_CreateOneShotComponent(ds_int64 key)
{
	CLASS* pComponent = new CLASS();
	DS_ASSERT(pComponent, "メモリ確保失敗");
	m_oneShotComponents.push_back(pComponent);
	return pComponent;
}

template<class CLASS>
CLASS* DsComponentSystem::_GetComponent(ds_int64 key)const
{
	CLASS* pComponent = NULL;
	const KEY k(key);
	auto itt = m_components.find(typeid(CLASS));
	if (m_components.end() != itt) {
		auto itc = itt->second.find(k);
		if (itt->second.end() != itc) {
			pComponent = dynamic_cast<CLASS*>(itc->second);
		}
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

void DsComponentSystem::RequestHitEffect(ds_int64 key, int effectId, const DsVec3d& hitPos, DsVec3d hitDir)
{
	DsHitEffectComponent* pComponent = _CreateOneShotComponent<DsHitEffectComponent>(key);
	if (pComponent) {
		pComponent->Request(effectId, hitPos, hitDir);
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
	return _GetComponent<DsEquipComponent>( (ds_uint64)(&m_owner) );
}

