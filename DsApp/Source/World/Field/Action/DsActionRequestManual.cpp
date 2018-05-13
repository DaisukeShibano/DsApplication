#include "DsAppPch.h"
#ifndef _DS_ACTION_REQUEST_MANUAL_
#include "World/Field/Action/DsActionRequestManual.h"
#endif

using namespace DsApp;

DsActionRequestManual::DsActionRequestManual(const DsLib::DsSys& sys)
	: m_key(sys.RefKeyboard())
	, m_moveVec(DsVec3d::Zero())
	, m_moveDir{}
	, m_request(0)
	, m_requestPre(0)
	, m_requestToggle(0)
	, m_cancel(0)
	, m_action(0)
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


	if (m_key.IsPush('r') || m_key.IsPush('R')) {
		SetRequest(ACTION_TYPE::CHANGE_WEP);
	}


	//アクションの更新
	_UpdateAction();
}

void DsActionRequestManual::_UpdateAction()
{
	m_requestToggle = m_request & (~m_requestPre);
	m_requestPre = m_request;
	m_action = m_requestToggle & m_cancel;
	m_request = 0;
	m_cancel = 0;
}

//virtual
DsVec3d DsActionRequestManual::GetMoveVec()const
{
	return m_moveVec;
}

//virtual
bool DsActionRequestManual::IsAction(ACTION_TYPE type)const
{
	return (0ULL != (m_action & (1ULL << static_cast<ds_uint64>(type))));
}

//virtual
void DsActionRequestManual::SetRequest(ACTION_TYPE type)
{
	m_request |= 1ULL << static_cast<ds_uint64>(type);
}

//virtual
void DsActionRequestManual::SetCancel(ACTION_TYPE type)
{
	m_cancel |= 1ULL << static_cast<ds_uint64>(type);
}

//virtual
void DsActionRequestManual::SetCancelAll()
{
	m_cancel = 0xFFFFFFFFFFFFFFFFULL;
}