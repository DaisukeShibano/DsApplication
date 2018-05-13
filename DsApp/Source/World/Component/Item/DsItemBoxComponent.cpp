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

void DsItemBoxComponent::AddItem(int itemId)
{

	m_wepItems.push_back(ITEM(itemId));
}

const DsItemBoxComponent::ITEM* DsItemBoxComponent::GetItem(int index)const
{
	const ITEM* ret = NULL;
	if (index < m_items.size()) {
		ret = &m_items[index];
	}
	return ret;
}

void DsItemBoxComponent::AddWep(int itemId)
{

	m_wepItems.push_back(ITEM(itemId));
}

const DsItemBoxComponent::ITEM* DsItemBoxComponent::GetWep(int index)const
{
	const ITEM* ret = NULL;
	if( (0 <= index) && (index < m_wepItems.size()) ){
		ret = &m_wepItems[index];
	}
	return ret;
}
