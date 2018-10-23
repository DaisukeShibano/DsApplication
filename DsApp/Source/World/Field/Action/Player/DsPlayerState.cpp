#include "DsAppPch.h"
#include "World/Field/Action/Player/DsPlayerState.h"
//他のヘッダ
#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif

using namespace DsApp;



/*********************************************************
@brief 待機
**********************************************************/
class DsChrStateIdle : public DsChrState
{
public:
	DsChrStateIdle(const INIT_ARG& arg) :DsChrState(arg)
	{
		if (m_pAnimClip) {
			m_pAnimClip->SetLoop(true);
		}
	}

private:
	virtual void Update(double dt) override
	{
		DsChrState::Update(dt);

		m_nextState = m_myState;

		//いつでもキャンセル可能
		m_actReq.SetCancelAll();

		if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
			m_nextState = CHR_STATE::ATTACK1;
		}
		else if (m_actReq.IsMove()) {
			m_nextState = CHR_STATE::RUN;
		}
	};
};
DS_REGISTER_STATE(DsChrStateIdle)


std::vector<STATE_CLASS_TYPE> s_myClasses;
std::type_index s_myClassTypes[]=
{
	typeid(DsChrStateIdle),
};



class DsRegisterMyClass : DsRegisterClass
{
public:
	DsRegisterMyClass(std::vector<STATE_CLASS_TYPE>& classes, std::type_index* pTypes, int typeNum)
		:DsRegisterClass(classes, pTypes, typeNum)
	{
		DsChrState::AddRegisterClass(*this);
	}

	virtual int GetId() const override
	{
		return 0;//define.hなどで纏める
	}
};
static DsRegisterMyClass s_registerMyClass(s_myClasses, s_myClassTypes, static_cast<int>(std::size(s_myClassTypes)));