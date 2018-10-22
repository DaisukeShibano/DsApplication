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

/*--------------------------------------------------------------
�X�e�[�g�t�@�N�g���[
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
@brief ���ʏ���
**********************************************************/
//virtual
void DsChrState::OnActive(double dt)
{
	//���݂̃X�e�[�g�ɐݒ�
	m_nextState = m_myState;


	if (m_pAnimClip) {
		//�⊮���Ԃ��f�t�H���g�ɖ߂�
		m_pAnimClip->ClearInterpolationTime();
	}
}

//virtual
void DsChrState::Update(double dt)
{
	//�A�j���C�x���g����A�j����ԕb���ݒ�
	double interpolationTime = 0;
	if (m_animFlags.GetAnimInterpolationTime(interpolationTime)) {
		//�u�����h�����̂Ŗ����l�ł͏㏑�����Ȃ�
		//�L���l�Ȃ�ǂ��炩�s��
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
REGISTER_STATE(DsChrStateIdle)


/*********************************************************
@brief ���s
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
REGISTER_STATE(DsChrStateRun)


/*********************************************************
@brief �U���P
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
REGISTER_STATE(DsChrStateAttack1)


/*********************************************************
@brief �U���Q
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
@brief �U���R
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
@brief �U���S
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

	//�N���X�ꗗ��������̃X�e�[�g�ɊY������^��񌟍�
	for (const STATE_CLASS_TYPE& classType : s_registerClass) {
		if (classType.state == arg.myState) {

			//�o�^���ꂽ�t�@�N�g���[������
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

			//�d���͂��肦�Ȃ��̂ł����ŏI��
			break;
		}
	}

	return ret;
}
