#include "DsAppPch.h"
#include "World/Field/Action/Enemy/DsEnemyState.h"
#include "World/Field/Action/DsActionRequest.h"

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
	virtual  DsEnemyState* Create(const DsEnemyState::INIT_ARG& arg) { return NULL; }
};

static StateFactory* s_factory[static_cast<int>(ENEMY_STATE::NUM)];

#define REGISTER_STATE(ClassName, state)\
class ClassName##Factory : public StateFactory\
{\
public:\
	ClassName##Factory()\
	{\
		DS_ASSERT( (NULL==s_factory[static_cast<int>(state)]), "キャラステート[%d]の重複登録", state);\
		s_factory[static_cast<int>(state)] = this;\
	}\
	virtual ClassName* Create(const DsEnemyState::INIT_ARG& arg) override { return new ClassName(arg); }\
};\
static ClassName##Factory s_##ClassName##Factory;\


/*********************************************************
@brief 共通処理
**********************************************************/
//virtual
void DsEnemyState::OnActive(double dt)
{
	//現在のステートに設定
	m_nextState = GetMyState();


	if (m_pAnimClip) {
		//補完時間をデフォルトに戻す
		m_pAnimClip->ClearInterpolationTime();
	}
}

//virtual
void DsEnemyState::Update(double dt)
{
	double interpolationTime = 0;
	if (m_animFlags.GetAnimInterpolationTime(interpolationTime)) {
		if (m_pAnimClip) {
			m_pAnimClip->SetInterpolationTime(interpolationTime);
		}
	}
}

//virtual
void DsEnemyState::OnDeactive(double dt)
{
	
}



/*********************************************************
@brief 待機
**********************************************************/
class DsEnemyStateIdle : public DsEnemyState
{
	virtual ENEMY_STATE GetMyState() const override { return ENEMY_STATE::IDLE; }
public:
	DsEnemyStateIdle(const INIT_ARG& arg):DsEnemyState(arg)
	{
		if (m_pAnimClip) {
			m_pAnimClip->SetLoop(true);
		}
	}
		
private:
	virtual void Update(double dt) override 
	{
		DsEnemyState::Update(dt);

		m_nextState = GetMyState();

		//いつでもキャンセル可能
		m_actReq.SetCancelAll();

		if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
			m_nextState = ENEMY_STATE::ATTACK1;
		}
		else if (m_actReq.IsMove()) {
			m_nextState = ENEMY_STATE::RUN;
		}
	};
};
REGISTER_STATE(DsEnemyStateIdle, ENEMY_STATE::IDLE)

/*********************************************************
@brief 走行
**********************************************************/
class DsEnemyStateRun : public DsEnemyState
{
	virtual ENEMY_STATE GetMyState() const override { return ENEMY_STATE::RUN; }
public:
	DsEnemyStateRun(const INIT_ARG& arg) :DsEnemyState(arg)
	{
		if (m_pAnimClip) {
			m_pAnimClip->SetLoop(true);
		}
	}

private:
	virtual void Update(double dt) override
	{
		DsEnemyState::Update(dt);

		m_nextState = GetMyState();

		//いつでもキャンセル可能
		m_actReq.SetCancelAll();

		if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
			m_nextState = ENEMY_STATE::ATTACK1;
		}
		else if (!m_actReq.IsMove()) {
			m_nextState = ENEMY_STATE::IDLE;
		}
	};
};
REGISTER_STATE(DsEnemyStateRun, ENEMY_STATE::RUN)


/*********************************************************
@brief 攻撃１
**********************************************************/
class DsEnemyStateAttack1 : public DsEnemyState
{
	virtual ENEMY_STATE GetMyState() const override { return ENEMY_STATE::ATTACK1; }
public:
	DsEnemyStateAttack1(const INIT_ARG& arg) :DsEnemyState(arg)
	{
		if (m_pAnimClip) {
			m_pAnimClip->SetLoop(false);
		}
	}

private:
	virtual void Update(double dt) override
	{
		DsEnemyState::Update(dt);

		m_nextState = GetMyState();

		if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
			//2段目攻撃
			m_nextState = ENEMY_STATE::ATTACK2;
		}
		else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
			m_nextState = ENEMY_STATE::RUN;
		}
		else if (m_pAnimClip->IsEnd()) {
			m_nextState = ENEMY_STATE::IDLE;
		}

	};
};
REGISTER_STATE(DsEnemyStateAttack1, ENEMY_STATE::ATTACK1)


/*********************************************************
@brief 攻撃２
**********************************************************/
class DsEnemyStateAttack2 : public DsEnemyState
{
	virtual ENEMY_STATE GetMyState() const override { return ENEMY_STATE::ATTACK2; }
public:
	DsEnemyStateAttack2(const INIT_ARG& arg) :DsEnemyState(arg)
	{
		if (m_pAnimClip) {
			m_pAnimClip->SetLoop(false);
		}
	}

private:
	virtual void Update(double dt) override
	{
		DsEnemyState::Update(dt);

		m_nextState = GetMyState();
	
		if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
			//1段目攻撃
			m_nextState = ENEMY_STATE::ATTACK1;
		}
		else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
			m_nextState = ENEMY_STATE::RUN;
		}
		else if (m_pAnimClip->IsEnd()) {
			m_nextState = ENEMY_STATE::IDLE;
		}

	};
};
REGISTER_STATE(DsEnemyStateAttack2, ENEMY_STATE::ATTACK2)



//static 
DsEnemyState* DsEnemyState::CreateIns(const INIT_ARG& arg)
{
	DsEnemyState* ret = NULL;
	StateFactory* pFactory = s_factory[static_cast<int>(arg.myState)];
	DS_ASSERT(pFactory, "登録されていないステート[%d]を生成しようとしました", arg.myState);
	ret = pFactory->Create(arg);
	
	return ret;
}
