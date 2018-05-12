#include "DsAppPch.h"
#ifndef _DS_ITEM_BOX_COMPONENT_
#include "World/Component/Item/DsItemBoxComponent.h"
#endif

using namespace DsApp;

DsItemBoxComponent::DsItemBoxComponent()
	:m_wepItems()
{
}

DsItemBoxComponent::~DsItemBoxComponent()
{
}

void DsItemBoxComponent::AddWep(const std::string& dispName, const std::string& resName)
{

	m_wepItems.push_back(ITEM(dispName, resName));
}

const DsItemBoxComponent::ITEM* DsItemBoxComponent::GetWepItem(int index)const
{
	const ITEM* ret = NULL;
	if (index < m_wepItems.size()) {
		ret = &m_wepItems[index];
	}
	return ret;
}
