#include "DsAppPch.h"
#include "World/Component/LockOn/DsLockOnComponent.h"
//他のヘッダ
#include "World/Field/LockOn/DsLockOnPoint.h"
#ifndef _DS_FIELD_OBJ_H_
#include "World/Field/DsFieldObj.h"
#endif
#include "World/Field/LockOn/DsLockOn.h"

using namespace DsApp;

DsLockOnComponent::DsLockOnComponent()
	: m_pLockOnPoint(NULL)
	, m_isRequest(false)
{
	m_pLockOnPoint = new DsLockOnPoint();
	DS_ASSERT(m_pLockOnPoint, "メモリ確保失敗");
}

DsLockOnComponent::~DsLockOnComponent()
{
	delete m_pLockOnPoint; m_pLockOnPoint = NULL;
}

//virtual 
bool DsLockOnComponent::Update(const COMPONENT_UPDATE_ARG& arg)
{
	if (m_pLockOnPoint && arg.pGameSys) {

		if (m_isRequest) {
			const double ownerSize = arg.owner.GetChrSize().y;
			const DsVec3d lockPos = arg.owner.GetPosition() + DsVec3d(0, ownerSize*0.5, 0);//大体真ん中をロックオンする
			m_pLockOnPoint->SetPos(lockPos);

			DsLockOn* pLockOn = arg.pGameSys->GetLockOn();
			if (pLockOn) {
				pLockOn->Register(m_pLockOnPoint);
			}
		}
		else {
			//ロックオン解除
			DsLockOn* pLockOn = arg.pGameSys->GetLockOn();
			if (pLockOn) {
				pLockOn->Unregister(m_pLockOnPoint);
			}
		}
	}

	bool ret = m_isRequest;
	m_isRequest = false;

	return ret;
}

void DsLockOnComponent::Request()
{
	m_isRequest = true;
}