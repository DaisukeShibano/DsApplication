#include "DsAppPch.h"
#ifndef _DS_EQUIP_COMPONENT_
#include "World/Component/Equip/DsEquipComponent.h"
#endif
//‘¼‚Ìƒwƒbƒ_
#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif
#ifndef _DS_FIELD_OBJ_H_
#include "World/Field/DsFieldObj.h"
#endif
#ifndef _DS_COMPONENT_SYSTEM_
#include "World/Component/DsComponentSystem.h"
#endif
#ifndef _DS_ITEM_BOX_COMPONENT_
#include "World/Component/Item/DsItemBoxComponent.h"
#endif
#ifndef _DS_ATTACH_COMPONENT_
#include "World/Component/Attach/DsAttachComponent.h"
#endif

using namespace DsLib;
using namespace DsApp;


static int MAX_WEP_NUM = 1;


DsEquipComponent::DsEquipComponent()
	: m_wepIndex(0)
	, m_pWep(NULL)
{
}

DsEquipComponent::~DsEquipComponent()
{
}

//virtual
bool DsEquipComponent::Update(const COMPONENT_UPDATE_ARG& arg)
{
	bool isChange = false;
	const DsActionRequest* pActReq = arg.owner.GetActionRequest();
	if (pActReq) {
		if (pActReq->IsAction(ACTION_TYPE::CHANGE_WEP)) {
			m_wepIndex = (++m_wepIndex) % MAX_WEP_NUM;
			isChange = true;
		}
	}

	if (isChange) {
		const DsComponentSystem* pComSys = arg.owner.GetComponentSystem();
		if (pComSys) {
			const DsItemBoxComponent* pItemBox = pComSys->GetItemBox();
			if (pItemBox) {
				const DsItemBox::ITEM* pItem = pItemBox->GetWepItem(m_wepIndex);
				if (pItem) {
					delete m_pWep;
					//m_pWep = new DsAnimation();
				}
			}
		}
	}

	//í’“‚È‚Ì‚Åí‚Étrue
	return true;
}
