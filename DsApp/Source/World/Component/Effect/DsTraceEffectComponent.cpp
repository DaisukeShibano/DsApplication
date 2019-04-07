#include "DsAppPch.h"
#ifndef _DS_TRACE_EFFECT_COMPONENT_
#include "World/Component/Effect/DsTraceEffectComponent.h"
#endif
//他のヘッダ
#ifndef _DS_FIELD_OBJ_H_
#include "World/Field/DsFieldObj.h"
#endif

using namespace DsLib;
using namespace DsApp;

DsTraceEffectComponent::DsTraceEffectComponent()
	: m_pEmitter(NULL)
	, m_pSys(NULL)
	, m_effectId(-1)
	, m_dmypolyId0(-1)
	, m_dmypolyId1(-1)
	, m_isRequest(false)
{
}

DsTraceEffectComponent::~DsTraceEffectComponent()
{
	if (m_pSys && m_pEmitter) {
		m_pSys->RefRender().RefParticleRender().UnRegister(*m_pEmitter);
	}
	delete m_pEmitter; m_pEmitter = NULL;
}

void DsTraceEffectComponent::Request(int effectId, int dmypolyId0, int dmypolyId1)
{
	m_effectId = effectId;
	m_dmypolyId0 = dmypolyId0;
	m_dmypolyId1 = dmypolyId1;
	m_isRequest = true;
}

//virtual
bool DsTraceEffectComponent::Update(const COMPONENT_UPDATE_ARG& arg)
{
	bool ret = true;

	if (m_isRequest) {
		if (!m_pEmitter) {
			m_pEmitter = new DsTraceParticleEmitter();
			DS_ASSERT(m_pEmitter, "メモリ確保失敗");
			arg.sys.RefRender().RefParticleRender().Register(*m_pEmitter);
			m_pSys = &arg.sys;
		}

		bool isDmypolyOk= false;
		DsMat44d mat0 = DsMat44d::Identity();
		DsMat44d mat1 = DsMat44d::Identity();
		if (arg.owner.GetDmypoly(m_dmypolyId0, mat0, DMYPOLY_SLOT::WEP) &&
			arg.owner.GetDmypoly(m_dmypolyId1, mat1, DMYPOLY_SLOT::WEP)) {
			m_pEmitter->RequestEmit(mat0.GetPos(), mat1.GetPos());
		}
	}
	else {
		bool isEmpty = true;
		if (m_pEmitter) {
			isEmpty = m_pEmitter->IsEmpty();
		}
		if (isEmpty) {
			ret = false;//エフェクト出し切ったので終了。
		}
	}

	if (m_pEmitter) {
		m_pEmitter->Update(arg.dt);
	}

	m_isRequest = false;

	if (!ret) {
		arg.sys.RefRender().RefParticleRender().UnRegister(*m_pEmitter);
		delete m_pEmitter; m_pEmitter = NULL;
	}

	return ret;
}