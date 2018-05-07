#include "DsAppPch.h"
#ifndef _DS_COMPONENT_SYSTEM_
#include "World/Component/DsComponentSystem.h"
#endif
//ëºÇÃÉwÉbÉ_
#ifndef _DS_TRACE_EFFECT_COMPONENT_
#include "World/Component/DsTraceEffectComponent.h"
#endif

using namespace DsApp;

DsComponentSystem::DsComponentSystem()
	: m_components()
{
}

DsComponentSystem::~DsComponentSystem()
{
	for (auto c : m_components) {
		delete c.second;
	}
	m_components.clear();
}

void DsComponentSystem::Update(const COMPONENT_UPDATE_ARG& arg)
{
	std::vector<ds_uint64> eraseList;
	for (auto c : m_components) {
		const bool isAlive = c.second->Update(arg);
		if (!isAlive) {
			delete c.second;
			eraseList.push_back(c.first);
		}
	}
	for (ds_uint64 key : eraseList) {
		m_components.erase(key);
	}
}

void DsComponentSystem::RequestTraceEffect(ds_int64 key, int effectId, int dmypolyId0, int dmypolyId1)
{
	DsTraceEffectComponent* pComponent = NULL;
	auto it = m_components.find(key);
	if (m_components.end() == it) {
		DsTraceEffectComponent* pNew = new DsTraceEffectComponent();
		DS_ASSERT(pNew, "ÉÅÉÇÉäämï€é∏îs");
		m_components[key] = pNew;
		pComponent = pNew;
	}
	else {
		pComponent = dynamic_cast<DsTraceEffectComponent*>(it->second);
	}

	if (pComponent) {
		pComponent->Request(effectId, dmypolyId0, dmypolyId1);
	}
}