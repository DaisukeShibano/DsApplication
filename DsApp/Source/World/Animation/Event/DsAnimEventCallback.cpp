#include "DsAppPch.h"
#ifndef _DS_ANIM_EVENT_CALLBACK_
#include "World/Animation/Event/DsAnimEventCallback.h"
#endif
//他のヘッダ
#ifndef _DS_ANIM_EVENT_TRACK_RES_
#include "Res/DsAnimEventTrackRes.h"
#endif
#ifndef _DS_ANIM_EVENT_TRACK_FORMAT_
#include "Res/DsAnimEventTrackFormat.h"
#endif
#ifndef _DS_FIELD_OBJ_H_
#include "World/Field/DsFieldObj.h"
#endif
#ifndef _DS_COMPONENT_SYSTEM_
#include "World/Component/DsComponentSystem.h"
#endif
#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif

#include "World/Animation/Event/DsAnimEventFlags.h"



using namespace DsLib;
using namespace DsApp;

DsAnimEventCallback::DsAnimEventCallback(DsFieldObj& owner, DsLib::DsResource& resource)
	: m_preLocalTime(0)
	, m_owner(owner)
	, m_resource(resource)
	, m_pRes(NULL)
{

}

DsAnimEventCallback::~DsAnimEventCallback()
{
	m_pRes = m_resource.UnregisterItem<DsAnimEventTrackRes>(m_pRes);
}

void DsAnimEventCallback::Initialize(const char* pAnimResName)
{
	std::string resName = pAnimResName;
	for (int i = static_cast<int>(resName.length())-1; 0 <= i; --i) {
		if ('.' == resName[i]) {
			resName.replace(i, resName.length() - i + i, ".devt");
		}
	}
	m_pRes = m_resource.RegisterItem<DsAnimEventTrackRes>(resName.c_str());
}

void DsAnimEventCallback::Call()
{
	DsAnimation* pAnim = m_owner.GetAnim();
	if (!pAnim) {
		return;
	}
	const DsAnimClip* pClip = pAnim->GetPlayAnim();
	if (!pClip) {
		return;
	}

	//毎フレームクリア系のフラグクリア
	DsAnimEventFlags* pFlags = m_owner.GetAnimEventFlags();
	if (pFlags) {
		pFlags->Clear();
	}
	
	const DS_ANIM_ET_PARAM* pParams = NULL;
	int paramNum = 0;

	if (m_pRes && m_pRes->GetData()) {
		const std::string curAnimName = pClip->RefAnimName();
		const DS_ANIM_ET_DATA* pData = m_pRes->GetData();
		for (int i = 0; i < pData->paramSetNum; ++i) {
			const std::string name = pData->pParamSet[i].pAnimName;
			if (name == curAnimName) {
				pParams = pData->pParamSet[i].pParams;
				paramNum = pData->pParamSet[i].paramNum;
				break;
			}
		}
	}

	const double current = pClip->GetLocalTime();
	//アニメ時間の期間中のイベントを発行
	for (int i = 0; i < paramNum; ++i) {
		if ((m_preLocalTime <= pParams[i].endTime ) &&
			(pParams[i].startTime <= current)) {
			_Call(pParams[i]);
		}
	}

	m_preLocalTime = current;
}


//ここに新規イベントを追加していく
void DsAnimEventCallback::_Call(const DS_ANIM_ET_PARAM& param)
{
	const DS_ANIM_ET_ACTION_TYPE type = static_cast<DS_ANIM_ET_ACTION_TYPE>(param.paramType);
	switch (type)
	{
	case DS_ANIM_ET_ACTION_TYPE::TRACE_EFFECT:
		_TraceEffect(param.pEffect);
		break;
	case DS_ANIM_ET_ACTION_TYPE::SOUND_EFFECT:
		break;
	case DS_ANIM_ET_ACTION_TYPE::DAMAGE:
		_Damage(param.pDamage);
		break;
	case DS_ANIM_ET_ACTION_TYPE::CANCEL_ACTION_TIMING:
		_Cancel(param.pCancel);
		break;
	case DS_ANIM_ET_ACTION_TYPE::ANIM_INTERPOLATION:
		_Interpolation(param.pAnimInterpolation);
		break;
	case DS_ANIM_ET_ACTION_TYPE::ACTION_INPUT:
		_ActionInput(param.pActionInput);
		break;
	case DS_ANIM_ET_ACTION_TYPE::NO_HIT_DAMAGE:
		break;
	case DS_ANIM_ET_ACTION_TYPE::DISABLE_TURN:
		break;
	case DS_ANIM_ET_ACTION_TYPE::DISABLE_GRAVITY:
		break;
	case DS_ANIM_ET_ACTION_TYPE::SUPER_ARMOR:
		break;
	default:
		break;
	}
}


void DsAnimEventCallback::_TraceEffect(const DS_ANIM_ET_TRACE_EFFECT* pParam)
{
	DsComponentSystem* pComSys =m_owner.GetComponentSystem();
	if (pComSys) {
		const ds_uint64 key = (ds_uint64)(pParam);
		pComSys->RequestTraceEffect(key, pParam->effectId, pParam->dmyPolyId[0], pParam->dmyPolyId[1]);
	}
}

void DsAnimEventCallback::_SoundEffect(const DS_ANIM_ET_SOUND_EFFECT* pParam)
{
	DsComponentSystem* pComSys = m_owner.GetComponentSystem();
	if (pComSys) {
		const ds_uint64 key = (ds_uint64)(pParam);
		pComSys->RequestSoundEffect(key, pParam->soundId, pParam->dmyPolyId);
	}
}

void DsAnimEventCallback::_Damage(const DS_ANIM_ET_DAMAGE* pParam)
{
}

void DsAnimEventCallback::_Cancel(const DS_ANIM_ET_CANCEL_ACTION_TIMING* pParam)
{
	DsActionRequest* pReq = m_owner.GetActionRequest();
	if (pReq) {
		pReq->SetCancel(pParam->action);
	}
}

void DsAnimEventCallback::_Interpolation(const struct DS_ANIM_ET_ANIM_INTERPOLATION* pParam)
{
	DsAnimEventFlags* pFalgs = m_owner.GetAnimEventFlags();
	if (pFalgs) {
		pFalgs->SetAnimInterpolationTime(pParam->time);
	}
}

void DsAnimEventCallback::_ActionInput(const struct DS_ANIM_ET_ACTION_INPUT* pParam)
{
	DsActionRequest* pReq = m_owner.GetActionRequest();
	if (pReq) {
		pReq->SetInput(pParam->actionType);
	}
}
void DsAnimEventCallback::_NoHitDamage(const struct DS_ANIM_ET_NO_HIT_DAMAGE* pParam)
{

}

void DsAnimEventCallback::_DisableTurn(const struct DS_ANIM_ET_DISABLE_TURN* pParam)
{

}

void DsAnimEventCallback::_DisableGravity(const struct DS_ANIM_ET_DISABLE_GRAVITY* pParam)
{

}

void DsAnimEventCallback::_SuperArmor(const struct DS_ANIM_ET_SUPER_ARMOR* pParam)
{

}