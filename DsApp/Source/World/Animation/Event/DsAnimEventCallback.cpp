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
	m_resource.UnregisterItem(m_pRes);
	m_pRes = NULL;
}

void DsAnimEventCallback::Initialize(const DsAnimRes& animRes)
{
	//m_owner.GetAnim()->
	std::string resName = animRes.RefName();
	for (size_t i = resName.length(); 0 <= i; ++i) {
		if ('.' == resName[i]) {
			resName.replace(i, resName.length() - i + i, ".devt");
		}
	}
	m_pRes = static_cast<DsAnimEventTrackRes*>(m_resource.RegisterItem(resName.c_str(), DsAnimEventTrackResFactory()));
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

	const double current = pClip->GetLocalTime();
	
	const DS_ANIM_ET_PARAM* pParams = NULL;
	int paramNum = 0;

	if (m_pRes && m_pRes->GetData()) {
		std::string curAnimName = pClip->RefAnimName();
		const DS_ANIM_ET_DATA* pData = m_pRes->GetData();
		for (int i = 0; i < pData->paramSetNum; ++i) {
			std::string name = pData->pParamSet[i].pAnimName;
			if (name == curAnimName) {
				pParams = pData->pParamSet[i].pParams;
				paramNum = pData->pParamSet[i].paramNum;
				break;
			}
		}
	}

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
		for (int i = 0; i < static_cast<int>(ACTION_TYPE::NUM); ++i) {
			pReq->SetCancel(static_cast<ACTION_TYPE>(i));
		}
	}
}
