#include "DsAppPch.h"
#ifndef _DS_ACTION_REQUEST_MANUAL_
#include "World/Field/Action/DsActionRequestManual.h"
#endif

using namespace DsApp;

DsActionRequestManual::DsActionRequestManual(const DsLib::DsSys& sys)
	: m_key(sys.RefKeyboard())
	, m_moveVec(DsVec3d::Zero())
	, m_moveDir{}
	, m_action{}
	, m_actionPre{}
{

}

DsActionRequestManual::~DsActionRequestManual()
{
}

//virtual
void DsActionRequestManual::Update(double dt)
{
	const double vel = 2000.0*dt;
	if (m_key.IsPush('w')|| m_key.IsPush('W'))
	{
		m_moveDir[0] += vel;
		m_moveDir[0] = min(1.0, m_moveDir[0]);
	}
	else
	{
		m_moveDir[0] -= vel;
		m_moveDir[0] = max(0.0, m_moveDir[0]);
	}

	if (m_key.IsPush('s')|| m_key.IsPush('S'))
	{
		m_moveDir[1] -= vel;
		m_moveDir[1] = max(-1.0, m_moveDir[1]);
	}
	else
	{
		m_moveDir[1] += vel;
		m_moveDir[1] = min(0.0, m_moveDir[1]);
	}

	if (m_key.IsPush('a')|| m_key.IsPush('A'))
	{
		m_moveDir[2] += vel;
		m_moveDir[2] = min(1.0, m_moveDir[2]);
	}
	else
	{
		m_moveDir[2] -= vel;
		m_moveDir[2] = max(0.0, m_moveDir[2]);
	}
	if (m_key.IsPush('d')|| m_key.IsPush('D'))
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


	//アクションの更新
	_UpdateAction(ACTION_TYPE::CHANGE_WEP, m_key.IsPush('r') || m_key.IsPush('R'));
}

void DsActionRequestManual::_UpdateAction(ACTION_TYPE type, bool isReq)
{
	const int idx = static_cast<int>(type);
	if (idx < static_cast<int>(ACTION_TYPE::NUM)) {
		m_action[idx] = isReq && (!m_actionPre[idx]);
		m_actionPre[idx] = isReq;
	}
}

//virtual
DsVec3d DsActionRequestManual::GetMoveVec()const
{
	return m_moveVec;
}

//virtual
bool DsActionRequestManual::IsAction(ACTION_TYPE type)const
{
	bool ret = false;
	const int idx = static_cast<int>(type);
	if (idx < static_cast<int>(ACTION_TYPE::NUM)) {
		ret = m_action[idx];
	}
	return ret;
}