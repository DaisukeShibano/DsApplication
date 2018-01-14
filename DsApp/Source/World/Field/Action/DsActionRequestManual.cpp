#include "DsAppPch.h"
#ifndef _DS_ACTION_REQUEST_MANUAL_
#include "World/Field/Action/DsActionRequestManual.h"
#endif

using namespace DsApp;

DsActionRequestManual::DsActionRequestManual(const DsLib::DsSys& sys)
	: m_key(sys.RefKeyboard())
	, m_moveVec(DsVec3d::Zero())
{
	for (double& val : m_moveDir) {
		val = 0;
	}
}

DsActionRequestManual::~DsActionRequestManual()
{
}

//virtual
void DsActionRequestManual::Update(double dt)
{
	const double vel = 2000.0*dt;
	if (m_key.IsPush(DsKeyboard::FORWARD))
	{
		m_moveDir[0] += vel;
		m_moveDir[0] = min(1.0, m_moveDir[0]);
	}
	else
	{
		m_moveDir[0] -= vel;
		m_moveDir[0] = max(0.0, m_moveDir[0]);
	}

	if (m_key.IsPush(DsKeyboard::BACK))
	{
		m_moveDir[1] -= vel;
		m_moveDir[1] = max(-1.0, m_moveDir[1]);
	}
	else
	{
		m_moveDir[1] += vel;
		m_moveDir[1] = min(0.0, m_moveDir[1]);
	}

	if (m_key.IsPush(DsKeyboard::LEFT))
	{
		m_moveDir[2] += vel;
		m_moveDir[2] = min(1.0, m_moveDir[2]);
	}
	else
	{
		m_moveDir[2] -= vel;
		m_moveDir[2] = max(0.0, m_moveDir[2]);
	}
	if (m_key.IsPush(DsKeyboard::RIGHT))
	{
		m_moveDir[3] -= vel;
		m_moveDir[3] = max(-1.0, m_moveDir[3]);
	}
	else
	{
		m_moveDir[3] += vel;
		m_moveDir[3] = min(0.0, m_moveDir[3]);
	}

	m_moveVec.z = m_moveDir[0] + m_moveDir[1];
	m_moveVec.x = m_moveDir[2] + m_moveDir[3];
}

//virtual
DsVec3d DsActionRequestManual::GetMoveVec()const
{
	return m_moveVec;
}