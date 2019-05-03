#include "DsAppPch.h"
#ifndef _DS_SOUND_EFFECT_COMPONENT_
#include "World/Component/Effect/DsSoundEffectComponent.h"
#endif
//‘¼‚Ìƒwƒbƒ_

using namespace DsApp;


//–¢ŽÀ‘•

DsSoundEffectComponent::DsSoundEffectComponent()
	: m_soundId(0)
	, m_dmypolyId(0)
{
}

DsSoundEffectComponent::~DsSoundEffectComponent()
{
}

//virtual
bool DsSoundEffectComponent::Update(COMPONENT_UPDATE_RESULT& result, const COMPONENT_UPDATE_ARG& arg)
{
	//–¢ŽÀ‘•
	return false;
}

void DsSoundEffectComponent::Request(int soundId, int dmypolyId)
{
	m_soundId = soundId;
	m_dmypolyId = dmypolyId;
}