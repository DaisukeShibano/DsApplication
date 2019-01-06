#include "DsAppPch.h"
#ifndef _DS_ACTION_REQUEST_MANUAL_
#include "World/Field/Action/DsActionRequestManual.h"
#endif

using namespace DsApp;

DsActionRequestManual::DsActionRequestManual(const DsLib::DsSys& sys)
	: m_key(sys.RefKeyboard())
	, m_mouse(sys.RefMouse())
	, m_moveVec(DsVec3d::Zero())
	, m_moveDir{}
	, m_request(0)
	, m_requestPre(0)
	, m_requestTrigger(0)
	, m_cancel(0)
	, m_input(0)
	, m_inputSave(0)
	, m_actionTrigger(0)
	, m_actionContinue(0)
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

	if (IsMove()) {
		SetRequest(ACTION_TYPE::MOVE);
	}

	if (m_key.IsPush(' ')) {
		SetRequest(ACTION_TYPE::STEP);
	}

	if (m_key.IsPush('r') || m_key.IsPush('R')) {
		SetRequest(ACTION_TYPE::CHANGE_WEP);
	}

	if (m_mouse.GetClickState() == DS_LEFT_CLICK) {
		SetRequest(ACTION_TYPE::ATTACK);
	}

	//アクションの更新
	_UpdateAction();
}

void DsActionRequestManual::_UpdateAction()
{
	//リクエスト
	m_requestTrigger = m_request & (~m_requestPre);//押した瞬間
	m_requestPre = m_request;

	//入力保持
	if (0 != m_input) {
		//入力のバーが続いていれば保持
		m_inputSave |= m_input & m_requestTrigger;
	}
	else {
		//バーが切れればクリア
		m_inputSave = 0;
	}

	//入力保持はキャンセルタイミングが来たらキャンセル可能
	//キャンセルタイミングはいつでもキャンセル可能
	const ds_uint64 enableAction = (m_inputSave & m_cancel) | (m_requestTrigger & m_cancel);

	//アクション
	m_actionTrigger = enableAction;
	m_actionContinue = (m_request & m_cancel) | enableAction;//押しっぱなしでもtrue

	//クリア
	m_request = 0;
	m_cancel = 0;
	m_input = 0;
}

//virtual
DsVec3d DsActionRequestManual::GetMoveVec()const
{
	return m_moveVec;
}

//virtual
bool DsActionRequestManual::IsAction(ACTION_TYPE type)const
{
	if (ACTION_TYPE::MOVE == type) {
		return (0ULL != (m_actionContinue & (1ULL << static_cast<ds_uint64>(type))));
	}
	else {
		return (0ULL != (m_actionTrigger & (1ULL << static_cast<ds_uint64>(type))));
	}
}

//virtual
bool DsActionRequestManual::IsMove()const
{
	//キャンセルタイミングは見ない
	return 0.0001 < GetMoveVec().LengthSq();
}

//virtual
void DsActionRequestManual::SetRequest(ACTION_TYPE type)
{
	m_request |= 1ULL << static_cast<ds_uint64>(type);
}

//virtual
void DsActionRequestManual::SetCancel(int type)
{
	if (0 <= type) {
		m_cancel |= 1ULL << static_cast<ds_uint64>(type);
	}
	else {
		SetCancelAll();
	}
}

//virtual
void DsActionRequestManual::SetCancelAll()
{
	m_cancel = 0xFFFFFFFFFFFFFFFFULL;
}

//virtual
void DsActionRequestManual::SetInput(int type)
{
	if (0 <= type) {
		m_input |= 1ULL << static_cast<ds_uint64>(type);
	}
	else {
		m_input = 0xFFFFFFFFFFFFFFFFULL;
	}
}

