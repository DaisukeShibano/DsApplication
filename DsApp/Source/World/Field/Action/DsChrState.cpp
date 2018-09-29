#include "DsAppPch.h"
#ifndef _DS_CHR_STATE_
#include "World/Field/Action/DsChrState.h"
#endif

#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif

#include "World/Animation/Event/DsAnimEventFlags.h"

using namespace DsApp;


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


/*********************************************************
@brief 共通処理
**********************************************************/
//virtual
void DsChrState::OnActive(double dt)
{
	//現在のステートに設定
	m_nextState = GetMyState();


	if (m_pAnimClip) {
		//補完時間をデフォルトに戻す
		m_pAnimClip->ClearInterpolationTime();
	}
}

//virtual
void DsChrState::Update(double dt)
{
	double interpolationTime = 0;
	if (m_animFlags.GetAnimInterpolationTime(interpolationTime)) {
		if (m_pAnimClip) {
			m_pAnimClip->SetInterpolationTime(interpolationTime);
		}
	}
}

//virtual
void DsChrState::OnDeactive(double dt)
{
	
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
		DsChrState::Update(dt);

		m_nextState = GetMyState();

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
		DsChrState::Update(dt);

		m_nextState = GetMyState();

		//いつでもキャンセル可能
		m_actReq.SetCancelAll();

		if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
			m_nextState = CHR_STATE::ATTACK1;
		}
		else if (!m_actReq.IsMove()) {
			m_nextState = CHR_STATE::IDLE;
		}
	};
};
REGISTER_STATE(DsChrStateRun, CHR_STATE::RUN)


/*********************************************************
@brief 攻撃１
**********************************************************/
class DsChrStateAttack1 : public DsChrState
{
	virtual CHR_STATE GetMyState() const override { return CHR_STATE::ATTACK1; }
public:
	DsChrStateAttack1(const INIT_ARG& arg) :DsChrState(arg)
	{
		if (m_pAnimClip) {
			m_pAnimClip->SetLoop(false);
		}
	}

private:
	virtual void Update(double dt) override
	{
		DsChrState::Update(dt);

		m_nextState = GetMyState();

		if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
			//2段目攻撃
			m_nextState = CHR_STATE::ATTACK2;
		}
		else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
			m_nextState = CHR_STATE::RUN;
		}
		else if (m_pAnimClip->IsEnd()) {
			m_nextState = CHR_STATE::IDLE;
		}

	};
};
REGISTER_STATE(DsChrStateAttack1, CHR_STATE::ATTACK1)


/*********************************************************
@brief 攻撃２
**********************************************************/
class DsChrStateAttack2 : public DsChrState
{
	virtual CHR_STATE GetMyState() const override { return CHR_STATE::ATTACK2; }
public:
	DsChrStateAttack2(const INIT_ARG& arg) :DsChrState(arg)
	{
		if (m_pAnimClip) {
			m_pAnimClip->SetLoop(false);
		}
	}

private:
	virtual void Update(double dt) override
	{
		DsChrState::Update(dt);

		m_nextState = GetMyState();
	
		if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
			//1段目攻撃
			m_nextState = CHR_STATE::ATTACK1;
		}
		else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
			m_nextState = CHR_STATE::RUN;
		}
		else if (m_pAnimClip->IsEnd()) {
			m_nextState = CHR_STATE::IDLE;
		}

	};
};
REGISTER_STATE(DsChrStateAttack2, CHR_STATE::ATTACK2)



//static 
DsChrState* DsChrState::CreateIns(const INIT_ARG& arg)
{
	DsChrState* ret = NULL;
	StateFactory* pFactory = s_factory[static_cast<int>(arg.myState)];
	DS_ASSERT(pFactory, "登録されていないステート[%d]を生成しようとしました", arg.myState);
	ret = pFactory->Create(arg);
	
	return ret;
}
