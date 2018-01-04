#include "DsAppPch.h"
#ifndef _DS_ACTION_CTRL_
#include "World/Field/Action/DsActionCtrl.h"
#endif

#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif
#ifndef _DS_CHR_STATE_
#include "World/Field/Action/DsChrState.h"
#endif
using namespace DsApp;



DsActionCtrl::DsActionCtrl(const DsLib::DsSys& sys)
	: m_pASCtrl(NULL)
	, m_pActReq(NULL)
	, m_state()
{
	m_pASCtrl = new DsLib::DsActionStateCtrl;
	DS_ASSERT(m_pASCtrl, "ƒƒ‚ƒŠŠm•ÛŽ¸”s");
	m_pActReq = new DsActionRequest(sys);
	DS_ASSERT(m_pActReq, "ƒƒ‚ƒŠŠm•ÛŽ¸”s");


}

DsActionCtrl::~DsActionCtrl()
{
	delete m_pASCtrl; m_pASCtrl = NULL;
	delete m_pActReq; m_pActReq = NULL;
	for (auto& state : m_state) {
		delete state.second;
	}
	m_state.clear();
}

void DsActionCtrl::Update(double dt)
{
	if (m_pActReq) {
		m_pActReq->Update(dt);
	}
	if (m_pASCtrl) {
		m_pASCtrl->Update(dt);
	}
}