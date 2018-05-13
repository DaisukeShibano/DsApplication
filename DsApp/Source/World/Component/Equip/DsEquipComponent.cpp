#include "DsAppPch.h"
#ifndef _DS_EQUIP_COMPONENT_
#include "World/Component/Equip/DsEquipComponent.h"
#endif
//他のヘッダ
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
#ifndef _DS_WEP_PARAM_
#include "Res/Param/DsWepParam.h"
#endif

using namespace DsLib;
using namespace DsApp;


static int MAX_WEP_NUM = 1;


//virtual
void DsAnimAttachEntity::SetTransform(const DsMat44d& transform)
{
	if (m_pAnim) {
		m_pAnim->SetRootMatrix(transform.GetPos(), transform.ToMat33());
	}
}

DsEquipComponent::DsEquipComponent()
	: m_wepIndex(0)
	, m_pWep(NULL)
	, m_attachEntity()
{
}

DsEquipComponent::~DsEquipComponent()
{
	delete m_pWep; m_pWep = NULL;
}

//virtual
bool DsEquipComponent::Update(const COMPONENT_UPDATE_ARG& arg)
{
	DsComponentSystem* pComSys = arg.owner.GetComponentSystem();
	if (!pComSys) {
		return true;
	}

	bool isChange = false;
	const DsActionRequest* pActReq = arg.owner.GetActionRequest();
	if (pActReq) {
		if (pActReq->IsAction(ACTION_TYPE::CHANGE_WEP)) {
			m_wepIndex = (++m_wepIndex) % MAX_WEP_NUM;
			isChange = true;
		}
	}

	std::vector<DsMat44d> attachMat;
	if (isChange) {
		delete m_pWep; m_pWep = NULL;
		const DsItemBoxComponent* pItemBox = pComSys->GetItemBox();
		if (pItemBox) {
			const DsItemBox::ITEM* pItem = pItemBox->GetWep(m_wepIndex);
			if (pItem) {
				DsWepParam param(pItem->itemId);
				if (param.IsValid()) {
					if (arg.owner.GetDmypoly(param.GetAttachDmypolyId(), attachMat)) {
						m_pWep = new DsAnimation(param.GetResName(), arg.sys.RefRender().RefDrawCom(), arg.sys.RefResource());
						DS_ASSERT(m_pWep, "メモリ確保失敗");
					}
				}
			}
		}
	}

	if (m_pWep && (!attachMat.empty())) {
		m_attachEntity.SetAnim(m_pWep);
		pComSys->RequestAttach(attachMat[0], &m_attachEntity);
	}

	//常駐なので常にtrue
	return true;
}
