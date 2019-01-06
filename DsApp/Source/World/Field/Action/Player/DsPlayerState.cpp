#include "DsAppPch.h"
#include "World/Field/Action/Player/DsPlayerState.h"
//他のヘッダ
#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif
#include "World/Field/Action/DsChrStateDefine.h"
#include "World/Field/Action/DsActionFlags.h"


using namespace DsApp;



namespace
{

	//ステートグラフID登録
	static const int DS_REGISTER_STATE_GRAPH_ID = DsChrStateDefine::PLAYER_STATE_GRAPH_ID;



	/*********************************************************
	@brief 待機
	**********************************************************/
	class DsChrStateIdle : public DsPlayerState
	{
	public:
		DsChrStateIdle(const INIT_ARG& arg) :DsPlayerState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(true);
			}
		}

	private:
		virtual void Update(double dt) override
		{
			DsPlayerState::Update(dt);

			m_nextState = m_myState;

			//いつでもキャンセル可能
			m_actReq.SetCancelAll();

			if (m_actReq.IsAction(ACTION_TYPE::STEP)) {
				m_nextState = CHR_STATE::STEP_F;
			}
			else if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
				m_nextState = CHR_STATE::ATTACK1;
			}
			else if (m_actReq.IsMove()) {
				m_nextState = CHR_STATE::RUN;
			}
		};
	};
	DS_REGISTER_STATE(DsChrStateIdle)

	/*---------------------------------------------------------
	DsRegisterMyClass
	---------------------------------------------------------*/
	class DsRegisterMyClass : public DsRegisterClass
	{
	public:
		DsRegisterMyClass(std::vector<STATE_CLASS_TYPE>& classes, std::type_index* pTypes, int typeNum)
			:DsRegisterClass(classes, pTypes, typeNum)
		{}
		virtual int GetId() const override
		{
			return DsChrStateDefine::PLAYER_STATE_GRAPH_ID;
		}
	};

}


/*---------------------------------------------------------
DsPlayerState
---------------------------------------------------------*/
//static
void DsPlayerState::Initialize()
{
	//上書きするステートリストを登録します
	static std::vector<STATE_CLASS_TYPE> s_myClasses;
	static std::type_index s_myClassTypes[] =
	{
		typeid(DsChrStateIdle),
	};
	static DsRegisterMyClass s_registerMyClass(s_myClasses, s_myClassTypes, static_cast<int>(std::size(s_myClassTypes)));
	DsChrState::AddRegisterClass(s_registerMyClass);
}
