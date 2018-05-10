#include "DsAppPch.h"
#ifndef _DS_EQUIP_COMPONENT_
#include "World/Component/Equip/DsEquipComponent.h"
#endif
//‘¼‚Ìƒwƒbƒ_
#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif
using namespace DsApp;


static int MAX_WEP_NUM = 1;


DsEquipComponent::DsEquipComponent()
	: m_wepIndex(0)
{
}

DsEquipComponent::~DsEquipComponent()
{
}

//virtual
bool DsEquipComponent::Update(const COMPONENT_UPDATE_ARG& arg)
{
	const DsActionRequest* pActReq = arg.m_pActReq;
	if (pActReq) {
		if (pActReq->IsAction(ACTION_TYPE::CHANGE_WEP)) {
			m_wepIndex = (++m_wepIndex) % MAX_WEP_NUM;
		}
	}



	//í’“‚È‚Ì‚Åí‚Étrue
	return true;
}
