#include "DsAppPch.h"
#include "World/Component/Effect/DsHitEffectComponent.h"

using namespace DsApp;



DsHitEffectComponent::DsHitEffectComponent()
	: m_isRequest(false)
	, m_effectId(-1)
	, m_hitPos(DsVec3d::Zero())
	, m_hitDir(DsVec3d::Zero())
{
}

DsHitEffectComponent::~DsHitEffectComponent()
{
}


//virtual
bool DsHitEffectComponent::Update(const COMPONENT_UPDATE_ARG& arg)
{
	m_isRequest = false;
	return false;
}


void DsHitEffectComponent::Request(int effectId, const DsVec3d& hitPos, DsVec3d hitDir)
{
	m_isRequest = true;
	m_effectId = effectId;
	m_hitPos = hitPos;
	m_hitDir = hitDir;
}