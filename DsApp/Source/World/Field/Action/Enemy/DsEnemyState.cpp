#include "DsAppPch.h"
#include "World/Field/Action/Enemy/DsEnemyState.h"
#include "World/Field/Action/DsActionRequest.h"

#include "World/Animation/Event/DsAnimEventFlags.h"

using namespace DsApp;


/*
���̃X�e�[�g�̌��ߕ��Fm_nextState��enum��������
���̃X�e�[�g�ւ̐i�܂����FIsNext ��true��Ԃ��悤�ɂ���
�A�j���̃��[�v�ݒ�͂����ł���Ă���
�c�[���ɏo�������b�g���Ȃ��̂ŃR�[�h�ɒ��ڏ���
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
		DS_ASSERT( (NULL==s_factory[static_cast<int>(state)]), "�L�����X�e�[�g[%d]�̏d���o�^", state);\
		s_factory[static_cast<int>(state)] = this;\
	}\
	virtual ClassName* Create(const DsEnemyState::INIT_ARG& arg) override { return new ClassName(arg); }\
};\
static ClassName##Factory s_##ClassName##Factory;\


/*********************************************************
@brief ���ʏ���
**********************************************************/
//virtual
void DsEnemyState::OnActive(double dt)
{
	//���݂̃X�e�[�g�ɐݒ�
	m_nextState = GetMyState();


	if (m_pAnimClip) {
		//�⊮���Ԃ��f�t�H���g�ɖ߂�
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
@brief �ҋ@
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

		//���ł��L�����Z���\
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
@brief ���s
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

		//���ł��L�����Z���\
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
@brief �U���P
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
			//2�i�ڍU��
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
@brief �U���Q
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
			//1�i�ڍU��
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
	DS_ASSERT(pFactory, "�o�^����Ă��Ȃ��X�e�[�g[%d]�𐶐����悤�Ƃ��܂���", arg.myState);
	ret = pFactory->Create(arg);
	
	return ret;
}
