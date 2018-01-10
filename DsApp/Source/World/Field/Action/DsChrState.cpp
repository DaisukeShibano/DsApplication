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


	
	class StateFactory
	{
	public:
		virtual  DsChrState* Create(const DsChrState::INIT_ARG& arg) { return NULL; }
	};

	static StateFactory* s_factory[static_cast<int>(CHR_STATE::NUM)];

	#define REGISTER_STATE(ClassName, state)\
	class ClassNameFactory : public StateFactory\
	{\
	public:\
		ClassNameFactory()\
		{\
			s_factory[static_cast<int>(state)] = this;\
		}\
		virtual ClassName* Create(const DsChrState::INIT_ARG& arg) override { new ClassName(arg); }\
	};\
	static ClassNameFactory m_ins;\
	virtual CHR_STATE GetMyState() const override { return state; }\



	/*********************************************************
	@brief 待機
	**********************************************************/
	class DsChrStateIdle : public DsChrState
	{
		REGISTER_STATE(DsChrStateIdle, CHR_STATE::IDLE)
	public:
		DsChrStateIdle(const INIT_ARG& arg):DsChrState(arg)
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
	};
	
	/*********************************************************
	@brief 走行
	**********************************************************/
	class DsChrStateRun : public DsChrState
	{
		REGISTER_STATE(DsChrStateRun, CHR_STATE::RUN)
	public:
		DsChrStateRun(const INIT_ARG& arg) :DsChrState(arg)
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
	};

}



//static 
DsChrState* DsChrState::CreateIns(CHR_STATE state, const INIT_ARG& arg)
{
	DsChrState* ret = NULL;
	StateFactory* pFactory = s_factory[static_cast<int>(state)];
	if (pFactory) {
		ret = pFactory->Create(arg);
	}
	return ret;
}
