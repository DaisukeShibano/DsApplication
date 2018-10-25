#include "DsAppPch.h"
#include "World/Field/Action/DsChrState.h"
//���̃w�b�_
#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif
#include "World/Field/Action/DsChrStateDefine.h"
#include "World/Animation/Event/DsAnimEventFlags.h"

using namespace DsApp;


/*
���̃X�e�[�g�̌��ߕ��Fm_nextState��enum��������
���̃X�e�[�g�ւ̐i�܂����FIsNext ��true��Ԃ��悤�ɂ���
�A�j���̃��[�v�ݒ�͂����ł���Ă���
�c�[���ɏo�������b�g���Ȃ��̂ŃR�[�h�ɒ��ڏ���
*/


namespace
{

	//�X�e�[�g�O���tID�o�^
	static const int DS_REGISTER_STATE_GRAPH_ID = DsChrStateDefine::CHR_STATE_GRAPH_ID;
}


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


namespace
{
	/*********************************************************
	@brief �ҋ@
	**********************************************************/
	class DsChrStateIdle : public DsChrState
	{
	public:
		DsChrStateIdle(const INIT_ARG& arg) :DsChrState(arg)
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
	DS_REGISTER_STATE(DsChrStateIdle)


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
	DS_REGISTER_STATE(DsChrStateRun)


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
	DS_REGISTER_STATE(DsChrStateAttack1)


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
	DS_REGISTER_STATE(DsChrStateAttack2)


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
	DS_REGISTER_STATE(DsChrStateAttack3)



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
	DS_REGISTER_STATE(DsChrStateAttack4)


		//�f�t�H���g�N���X���
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
	//�h���N���X�o�^�R���e�i
	static std::map<int, DsRegisterClass*> s_registerClassContainer;
	//�X�e�[�g�C���X�^���X�����@
	static DsStateFactory* s_factoryTop = NULL;
}

//static
DsStateFactory* DsChrState::GetFactoryTop()
{
	return s_factoryTop;
}
//static
void DsChrState::SetFactoryTop(DsStateFactory* pFactory)
{
	s_factoryTop = pFactory;
}


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
DsChrState* DsChrState::CreateIns(INIT_ARG arg, const std::string& animName, const int id )
{
	DsChrState* ret = NULL;

	auto it = s_registerClassContainer.find(id);
	if (it == s_registerClassContainer.end()) {
		DS_ASSERT(false, "ID[%d]�͑��݂��܂���", id);
	}
	else {
		const std::vector<STATE_CLASS_TYPE>& registerClass = it->second->RefRegisterClasses();
		//�N���X�ꗗ��������̃X�e�[�g�ɊY������^��񌟍�
		for (const STATE_CLASS_TYPE& classType : registerClass) {
			if (animName == classType.pName) {

				//�o�^���ꂽ�t�@�N�g���[������
				DsStateFactory* pFactory = DsChrState::GetFactoryTop();
				DsStateFactory* pBaseFactory = NULL;
				while (pFactory) {
					if (pFactory->GetType() == classType.type) {
						if (pFactory->GetId() == id) {
							pBaseFactory = NULL;//���ʏ����͂Ȃ�
							break;//�ړI�̂��̂��������̂ŏI���
						}
						else if (pFactory->GetId() == DsChrStateDefine::CHR_STATE_GRAPH_ID) {
							//�Ƃ肠�������ʂ̂͂������̂ł�������Ă���
							pBaseFactory = pFactory;
							//�܂��T���͑�����
						}
					}
					pFactory = pFactory->GetNext();
				}

				if ((!pFactory) && pBaseFactory) {
					//���ʂ̂����Ȃ������̂ł�����̗p
					pFactory = pBaseFactory;
				}

				if (pFactory) {
					arg.myState = classType.state;//�����ŃX�e�[�g�m��
					ret = pFactory->Create(arg);
					break;
				}

			}
		}
	}

	return ret;
}

//static
void DsChrState::AddRegisterClass(DsRegisterClass& registerClass)
{
	const int id = registerClass.GetId();
	auto it = s_registerClassContainer.find(id);
	if (it == s_registerClassContainer.end()) {
		s_registerClassContainer[id] = &registerClass;
		registerClass.Register();
	}
	else {
		DS_ASSERT(false, "�L�����X�e�[�g�o�^ID���d�����Ă��܂� %d", id);
	}
}


/*--------------------------------------------------------------
�N���X�o�^
---------------------------------------------------------------*/
void DsRegisterClass::Register()
{
	if (m_registerClasses.empty()) {

		STATE_CLASS_TYPE* pClassTypes = DsChrState::GetStateClassTypes();
		for (int i = 0; i < DsChrState::GetStateClassTypesNum(); ++i) {
			STATE_CLASS_TYPE copy = pClassTypes[i];

			//���̃N���X���Ɠ������̂�h���̃N���X���̒�����T��
			std::string srcName = copy.type.name();

			for (int typeIdx = 0; typeIdx < m_overrideTypesNum; ++typeIdx) {
				const std::type_index& typeIndex = m_pOverwriteTypes[typeIdx];
				std::string myName = typeIndex.name();

				//namespace����菜�����N���X��
				auto getClassName = [](std::string name)->std::string
				{
					const std::string token = "::";
					auto pos = name.rfind(token);
					if (pos == std::string::npos) {
						return name;//namespace���Ȃ��̂ł��̂܂�
					}
					else {
						std::string ret = name.c_str() + pos + token.length();
						return ret;
					}
				};
			

				std::string cName1 = getClassName(myName);
				std::string cName2 = getClassName(srcName);

				if(cName1 == cName2){
					//�h���ɓ����N���X�����������̂Ō^���㏑��
					copy.type = typeIndex;
					break;
				}
			}
			m_registerClasses.push_back(copy);
		}
	}
}

