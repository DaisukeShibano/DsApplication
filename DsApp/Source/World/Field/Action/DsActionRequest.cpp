#include "DsAppPch.h"
#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif

using namespace DsApp;

DsActionRequest::DsActionRequest(const DsLib::DsSys& sys)
	: m_key(sys.RefKeyboard())
	, m_moveVec(DsVec3d::Zero())
{
}

DsActionRequest::~DsActionRequest()
{
}

void DsActionRequest::Update(double dt)
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

DsVec3d DsActionRequest::GetMoveVec()const
{
	return m_moveVec;
}
ACTION_TYPE DsActionRequest::GetAction() const
{
	const double len = m_moveVec.Length();
	const bool isMove = (0.01 < len);
	if (isMove) {
		return ACTION_TYPE::MOVE;
	}
	else {
		return ACTION_TYPE::IDLE;
	}
}