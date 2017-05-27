#include "DsPch.h"
#ifndef _DS_ACT_REQ_H_
#include "Animation/ActionRequest/DsActReq.h"
#endif

#ifndef _DS_SYS_H_
#include "System/DsSys.h"
#endif
#ifndef _DS_KEYBOARD_H_
#include "Keyboard/DsKeyboard.h"
#endif

using namespace DsLib;

DsActReq::DsActReq(const DsSys& sys)
	: m_key(sys.RefKeyboard())
	, m_moveVec(DsVec3d::Zero())
{
	for each(double& dir in m_moveDir)
	{
		dir = 0.0;
	}
}

DsActReq::~DsActReq()
{

}

//virtual
void DsActReq::Update(double dt)
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
bool DsActReq::IsMove() const
{
	const double len = m_moveVec.Length();
	const bool ret = (0.01 < len);
	return ret;
}