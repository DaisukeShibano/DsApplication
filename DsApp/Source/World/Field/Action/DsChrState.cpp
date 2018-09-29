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
���̃X�e�[�g�̌��ߕ��Fm_nextState��enum��������
���̃X�e�[�g�ւ̐i�܂����FIsNext ��true��Ԃ��悤�ɂ���
�A�j���̃��[�v�ݒ�͂����ł���Ă���
�c�[���ɏo�������b�g���Ȃ��̂ŃR�[�h�ɒ��ڏ���
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
		DS_ASSERT( (NULL==s_factory[static_cast<int>(state)]), "�L�����X�e�[�g[%d]�̏d���o�^", state);\
		s_factory[static_cast<int>(state)] = this;\
	}\
	virtual ClassName* Create(const DsChrState::INIT_ARG& arg) override { return new ClassName(arg); }\
};\
static ClassName##Factory s_##ClassName##Factory;\


/*********************************************************
@brief ���ʏ���
**********************************************************/
//virtual
void DsChrState::OnActive(double dt)
{
	//���݂̃X�e�[�g�ɐݒ�
	m_nextState = GetMyState();


	if (m_pAnimClip) {
		//�⊮���Ԃ��f�t�H���g�ɖ߂�
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
@brief �ҋ@
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

		//���ł��L�����Z���\
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
@brief ���s
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

		//���ł��L�����Z���\
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
@brief �U���P
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
			//2�i�ڍU��
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
@brief �U���Q
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
			//1�i�ڍU��
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
	DS_ASSERT(pFactory, "�o�^����Ă��Ȃ��X�e�[�g[%d]�𐶐����悤�Ƃ��܂���", arg.myState);
	ret = pFactory->Create(arg);
	
	return ret;
}
