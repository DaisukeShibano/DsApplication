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
	class ClassName##Factory : public StateFactory\
	{\
	public:\
		ClassName##Factory()\
		{\
			DS_ASSERT( (NULL==s_factory[static_cast<int>(state)]), "キャラステート[%d]の重複登録", state);\
			s_factory[static_cast<int>(state)] = this;\
		}\
		virtual ClassName* Create(const DsChrState::INIT_ARG& arg) override { return new ClassName(arg); }\
	};\
	static ClassName##Factory s_##ClassName##Factory;\


	namespace
	{
		bool _IsMove(const DsActionRequest& req)
		{
			const double len = req.GetMoveVec().LengthSq();
			const bool isMove = (0.0001 < len);
			return isMove;
		}
	}

	/*********************************************************
	@brief 待機
	**********************************************************/
	class DsChrStateIdle : public DsChrState
	{
		virtual CHR_STATE GetMyState() const override { return CHR_STATE::IDLE; }
	public:
		DsChrStateIdle(const INIT_ARG& arg):DsChrState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(true);
			}
		}
		
	private:
		virtual void Update(double dt) override 
		{
			m_nextState = GetMyState();

			//いつでもキャンセル可能
			m_actReq.SetCancelAll();

			if (_IsMove(m_actReq)) {
				m_nextState = CHR_STATE::RUN;
			}
		};
	};
	REGISTER_STATE(DsChrStateIdle, CHR_STATE::IDLE)

	/*********************************************************
	@brief 走行
	**********************************************************/
	class DsChrStateRun : public DsChrState
	{
		virtual CHR_STATE GetMyState() const override { return CHR_STATE::RUN; }
	public:
		DsChrStateRun(const INIT_ARG& arg) :DsChrState(arg)
		{
			if (m_pAnimClip) {
				m_pAnimClip->SetLoop(true);
			}
		}

	private:
		virtual void Update(double dt) override
		{
			m_nextState = GetMyState();

			//いつでもキャンセル可能
			m_actReq.SetCancelAll();

			if (!_IsMove(m_actReq)) {
				m_nextState = CHR_STATE::IDLE;
			}
		};
	};
	REGISTER_STATE(DsChrStateRun, CHR_STATE::RUN)
}



//static 
DsChrState* DsChrState::CreateIns(const INIT_ARG& arg)
{
	DsChrState* ret = NULL;
	StateFactory* pFactory = s_factory[static_cast<int>(arg.myState)];
	DS_ASSERT(pFactory, "登録されていないステート[%d]を生成しようとしました", arg.myState);
	ret = pFactory->Create(arg);
	
	return ret;
}
