#include "DsAppPch.h"
#include "World/Component/Effect/DsHitEffectComponent.h"
//他のヘッダ

using namespace DsApp;



DsHitEffectComponent::DsHitEffectComponent()
	: DsComponent()
	, m_isRequest(false)
	, m_effectId(-1)
	, m_hitPos(DsVec3d::Zero())
	, m_hitDir(DsVec3d::Zero())
	, m_pSys(NULL)
	, m_pEmitter(NULL)
{
}

DsHitEffectComponent::~DsHitEffectComponent()
{
	if (m_pSys) {
		m_pSys->RefRender().RefParticleRender().UnRegister(*m_pEmitter);
	}
	delete m_pEmitter; m_pEmitter = NULL;
}

//virtual
bool DsHitEffectComponent::Update(COMPONENT_UPDATE_RESULT& outResult, const COMPONENT_UPDATE_ARG& arg)
{
	bool ret = true;

	if (m_isRequest) {
		if (!m_pEmitter) {
			m_pEmitter = new DsBloodParticleEmitter();
			DS_ASSERT(m_pEmitter, "メモリ確保失敗");
			arg.sys.RefRender().RefParticleRender().Register(*m_pEmitter);
			m_pSys = &arg.sys;
		}

		m_pEmitter->RequestEmit(m_hitPos, m_hitDir);

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

void DsHitEffectComponent::Request(int effectId, const DsVec3d& hitPos, DsVec3d hitDir)
{
	m_isRequest = true;
	m_effectId = effectId;
	m_hitPos = hitPos;
	m_hitDir = hitDir;
}