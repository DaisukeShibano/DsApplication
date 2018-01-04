#include "DsAppPch.h"
#ifndef _DS_CHR_STATE_
#include "World/Field/Action/DsChrState.h"
#endif

#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif

using namespace DsApp;

namespace
{
	/*
	次のステートの決め方：m_nextStateにenumを代入する
	次のステートへの進ませ方：IsNext をtrueを返すようにする
	アニメのループ設定はここでやっている
	ツールに出すメリットがないのでコードに直接書く
	*/


	/*********************************************************
	@brief 待機
	**********************************************************/
	class DsChrStateIdle : public DsChrState
	{
	public:
		DsChrStateIdle(StateMap& allState, DsLib::DsAnimClip* pAnimClip, const DsActionRequest& actReq):DsChrState(allState, pAnimClip, actReq)
		{
			m_nextState = GetMyState();
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(true);
			}
		}
		
	private:
		virtual void Update(double dt) override 
		{
			if (m_actReq.GetAction() == ACTION_TYPE::MOVE) {
				m_nextState = CHR_STATE::RUN;
			}
			else {
				m_nextState = GetMyState();
			}
			
		};

	public:
		virtual CHR_STATE GetMyState() const override { return CHR_STATE::IDLE; }

	};

	/*********************************************************
	@brief 走行
	**********************************************************/
	class DsChrStateRun : public DsChrState
	{
	public:
		DsChrStateRun(StateMap& allState, DsLib::DsAnimClip* pAnimClip, const DsActionRequest& actReq) :DsChrState(allState, pAnimClip, actReq)
		{
			m_nextState = GetMyState();
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(true);
			}
		}

	private:
		virtual void Update(double dt) override
		{
			if (m_actReq.GetAction() == ACTION_TYPE::MOVE) {
				m_nextState = GetMyState();
			}
			else {
				m_nextState = CHR_STATE::IDLE;
			}

		};

	public:
		virtual CHR_STATE GetMyState() const override { return CHR_STATE::RUN; }

	};

}

#define NEW_CHR_STATE(state, arg1, arg2, arg3) new DsChrState##state(arg1, arg2, arg3)

//static 
DsChrState* DsChrState::CreateIns(CHR_STATE state, StateMap& allState, DsLib::DsAnimClip* pAnimClip, const DsActionRequest& actReq)
{
	return NEW_CHR_STATE(state, allState, pAnimClip, actReq);
}
