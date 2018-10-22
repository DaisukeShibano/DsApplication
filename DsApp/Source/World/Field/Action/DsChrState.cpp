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

/*--------------------------------------------------------------
ステートファクトリー
---------------------------------------------------------------*/
class DsStateFactory
{
public:
	DsStateFactory():m_pNext(NULL){}
	virtual  DsChrState* Create(const DsChrState::INIT_ARG& arg) { return NULL; }
	void SetNext(DsStateFactory* pNext) { m_pNext = pNext; }
	DsStateFactory* GetNext() const { return m_pNext; }

	virtual std::type_index GetType() const = 0;
private:
	DsStateFactory* m_pNext;
};


DsStateFactory* s_factoryTop = NULL;

#define REGISTER_STATE(ClassName)\
class ClassName##Factory : public DsStateFactory\
{\
public:\
	ClassName##Factory()\
		:DsStateFactory()\
	{\
		if(s_factoryTop){\
			DsStateFactory* pSet = s_factoryTop;\
			while(pSet){\
				DsStateFactory* pNext = pSet->GetNext();\
				if( pNext ){\
					pSet = pNext;\
				}else{\
					pSet->SetNext(this); \
					break; \
				}\
			}\
		}else{\
			s_factoryTop = this;\
		}\
	}\
	virtual ClassName* Create(const DsChrState::INIT_ARG& arg) override { return new ClassName(arg); }\
	virtual std::type_index GetType() const override { return typeid(ClassName); }\
};\
static ClassName##Factory s_##ClassName##Factory;\


/*********************************************************
@brief 共通処理
**********************************************************/
//virtual
void DsChrState::OnActive(double dt)
{
	//現在のステートに設定
	m_nextState = m_myState;


	if (m_pAnimClip) {
		//補完時間をデフォルトに戻す
		m_pAnimClip->ClearInterpolationTime();
	}
}

//virtual
void DsChrState::Update(double dt)
{
	//アニメイベントからアニメ補間秒数設定
	double interpolationTime = 0;
	if (m_animFlags.GetAnimInterpolationTime(interpolationTime)) {
		//ブレンドされるので無効値では上書きしない
		//有効値ならどちらか不定
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
REGISTER_STATE(DsChrStateIdle)


/*********************************************************
@brief 走行
**********************************************************/
class DsChrStateRun : public DsChrState
{
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

		m_nextState = m_myState;

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
REGISTER_STATE(DsChrStateRun)


/*********************************************************
@brief 攻撃１
**********************************************************/
class DsChrStateAttack1 : public DsChrState
{
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

		m_nextState = m_myState;

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
REGISTER_STATE(DsChrStateAttack1)


/*********************************************************
@brief 攻撃２
**********************************************************/
class DsChrStateAttack2 : public DsChrState
{
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

		m_nextState = m_myState;
	
		if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
			m_nextState = CHR_STATE::ATTACK3;
		}
		else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
			m_nextState = CHR_STATE::RUN;
		}
		else if (m_pAnimClip->IsEnd()) {
			m_nextState = CHR_STATE::IDLE;
		}

	};
};
REGISTER_STATE(DsChrStateAttack2)


/*********************************************************
@brief 攻撃３
**********************************************************/
class DsChrStateAttack3 : public DsChrState
{
public:
	DsChrStateAttack3(const INIT_ARG& arg) :DsChrState(arg)
	{
		if (m_pAnimClip) {
			m_pAnimClip->SetLoop(false);
		}
	}

private:
	virtual void Update(double dt) override
	{
		DsChrState::Update(dt);

		m_nextState = m_myState;
	
		if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
			m_nextState = CHR_STATE::ATTACK4;
		}
		else if (m_actReq.IsAction(ACTION_TYPE::MOVE)) {
			m_nextState = CHR_STATE::RUN;
		}
		else if (m_pAnimClip->IsEnd()) {
			m_nextState = CHR_STATE::IDLE;
		}

	};
};
REGISTER_STATE(DsChrStateAttack3)



/*********************************************************
@brief 攻撃４
**********************************************************/
class DsChrStateAttack4 : public DsChrState
{
public:
	DsChrStateAttack4(const INIT_ARG& arg) :DsChrState(arg)
	{
		if (m_pAnimClip) {
			m_pAnimClip->SetLoop(false);
		}
	}

private:
	virtual void Update(double dt) override
	{
		DsChrState::Update(dt);

		m_nextState = m_myState;

		if (m_actReq.IsAction(ACTION_TYPE::ATTACK)) {
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
REGISTER_STATE(DsChrStateAttack4)




static STATE_CLASS_TYPE s_registerClass[] =
{
	{ typeid(DsChrStateIdle), CHR_STATE::IDLE, "idle" },
	{ typeid(DsChrStateRun), CHR_STATE::RUN, "run" },
	{ typeid(DsChrStateAttack1), CHR_STATE::ATTACK1, "Attack1" },
	{ typeid(DsChrStateAttack2), CHR_STATE::ATTACK2, "Attack2" },
	{ typeid(DsChrStateAttack3), CHR_STATE::ATTACK3, "Attack3" },
	{ typeid(DsChrStateAttack4), CHR_STATE::ATTACK4, "Attack4" },
	//{ typeid(DsChrStateDamageF), CHR_STATE::DAMAGE_F, "DamageF" },
	//{ typeid(DsChrStateDamageB), CHR_STATE::DAMAGE_B, "DamageB" },
	//{ typeid(DsChrStateDamageL), CHR_STATE::DAMAGE_L, "DamageL" },
	//{ typeid(DsChrStateDamageR), CHR_STATE::DAMAGE_R, "DamageR" },
};


//static
size_t DsChrState::GetStateClassTypesNum()
{
	return std::size(s_registerClass);
}
//static
STATE_CLASS_TYPE* DsChrState::GetStateClassTypes()
{
	return s_registerClass;
}

//static 
DsChrState* DsChrState::CreateIns(const INIT_ARG& arg)
{
	DsChrState* ret = NULL;

	//クラス一覧から引数のステートに該当する型情報検索
	for (const STATE_CLASS_TYPE& classType : s_registerClass) {
		if (classType.state == arg.myState) {

			//登録されたファクトリーを検索
			DsStateFactory* pFactory = s_factoryTop;
			while (pFactory) {
				if (pFactory->GetType() == classType.type) {
					break;
				}
				pFactory = pFactory->GetNext();
			}

			if (pFactory) {
				ret = pFactory->Create(arg);
			}

			//重複はありえないのでここで終了
			break;
		}
	}

	return ret;
}
