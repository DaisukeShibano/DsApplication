#include "DsAppPch.h"
#include "World/Component/KnockBack/DsKnockBackComponent.h"
//‘¼‚Ìƒwƒbƒ_
#include "World/Field/DsFieldObj.h"


using namespace DsApp;



DsKnockBackComponent::DsKnockBackComponent()
	: m_isRequest(false)
	, m_targetMove(DsVec3d::Zero())
	, m_targetTime(0.0)
	, m_localTime(0.0)
{
}

DsKnockBackComponent::~DsKnockBackComponent()
{
}

//virtual
bool DsKnockBackComponent::Update(const COMPONENT_UPDATE_ARG& arg)
{
	bool ret = true;

	const DsVec3d vel = (0.0 < m_targetTime) ? (m_targetMove / m_targetTime) : (DsVec3d::Zero());
	arg.owner.SetExVelocity(vel);

	m_localTime += arg.dt;
	if (m_targetTime < m_localTime) {
		ret = false;
	}

	return ret;
}

void DsKnockBackComponent::Request(const DsVec3d& move, double time)
{
	m_isRequest = true;
	m_targetMove = move;
	m_targetTime = time;
	m_localTime = 0.0;
}