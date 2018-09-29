#include "DsAppPch.h"
#ifndef _DS_ACTION_CTRL_
#include "World/Field/Action/DsActionCtrl.h"
#endif

#ifndef _DS_CHR_STATE_
#include "World/Field/Action/DsChrState.h"
#endif
using namespace DsApp;


static const char* IDEL_ANIM_NAME = "idle";
static const char* RUN_ANIM_NAME = "run";
static const char* RUN_ATTACK1_NAME = "Attack1";
static const char* RUN_ATTACK2_NAME = "Attack2";


DsActionCtrl::DsActionCtrl(DsActionRequest& actReq, const DsAnimEventFlags& animFlags, const std::vector<DsLib::DsAnimClip*>& animClip)
	: m_pASCtrl(NULL)
	, m_state()
	, m_pCurrentAnim(NULL)
{
	m_pASCtrl = new DsLib::DsActionStateCtrl;
	DS_ASSERT(m_pASCtrl, "�������m�ێ��s");


	struct CREATE_INFO
	{
		const CHR_STATE state;
		const char* animTypeName;
	};
	const CREATE_INFO createInfo[] =
	{
		{ CHR_STATE::IDLE, IDEL_ANIM_NAME },
		{ CHR_STATE::RUN, RUN_ANIM_NAME },
		{ CHR_STATE::ATTACK1, RUN_ATTACK1_NAME },
		{ CHR_STATE::ATTACK2, RUN_ATTACK2_NAME },
	};

	for (const CREATE_INFO& info : createInfo) {
		//�A�j���f�[�^����X�e�[�g�ɊY��������̂�������
		for (DsAnimClip* pAnim : animClip) {
			if ( std::string::npos != pAnim->RefAnimName().find(info.animTypeName) ) {
				//�X�e�[�g�ɊY������A�j�����������̂ŃX�e�[�g����
				DsChrState::INIT_ARG arg(pAnim, actReq, m_state, animFlags, info.state);
				DsChrState* pState = DsChrState::CreateIns(arg);
				DS_ASSERT(pState, "�X�e�[�g�쐬���s");
				m_state[info.state] = pState;
				break;
			}
		}
	}

	//�����X�e�[�g�ݒ�
	auto it = m_state.find(CHR_STATE::IDLE);
	if (it != m_state.end()) {
		m_pASCtrl->RegisterActiveNode(it->second);
		m_pCurrentAnim = it->second->GetAnimClip();
	}
}

DsActionCtrl::~DsActionCtrl()
{
	delete m_pASCtrl; m_pASCtrl = NULL;
	
	for (auto& state : m_state) {
		delete state.second;
	}
	m_state.clear();
}

void DsActionCtrl::Update(double dt)
{
	//�X�e�[�g�̍X�V
	if (m_pASCtrl) {
		m_pASCtrl->Update(dt);
	}

	//�J�����g�A�j���̍X�V
	std::vector<DsASNode*> node = m_pASCtrl->GetActiveNode();
	if (!node.empty()) {
		const DsChrState* pState = static_cast<DsChrState*>(node[0]);
		m_pCurrentAnim = pState->GetAnimClip();
	}
}