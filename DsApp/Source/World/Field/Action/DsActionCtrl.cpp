#include "DsAppPch.h"
#ifndef _DS_ACTION_CTRL_
#include "World/Field/Action/DsActionCtrl.h"
#endif

#ifndef _DS_ACTION_REQUEST_
#include "World/Field/Action/DsActionRequest.h"
#endif
#ifndef _DS_CHR_STATE_
#include "World/Field/Action/DsChrState.h"
#endif
using namespace DsApp;



DsActionCtrl::DsActionCtrl(const DsLib::DsSys& sys, const std::vector<DsAnimClip*>& animClip)
	: m_pASCtrl(NULL)
	, m_pActReq(NULL)
	, m_state()
	, m_pCurrentAnim(NULL)
{
	m_pASCtrl = new DsLib::DsActionStateCtrl;
	DS_ASSERT(m_pASCtrl, "�������m�ێ��s");
	m_pActReq = new DsActionRequest(sys);
	DS_ASSERT(m_pActReq, "�������m�ێ��s");


	struct CREATE_INFO
	{
		const CHR_STATE state;
		const char* animTypeName;
	};
	const CREATE_INFO createInfo[] =
	{
		{ CHR_STATE::IDLE, "idle"},
		{ CHR_STATE::RUN, "run" },
	};

	for (const CREATE_INFO& info : createInfo) {
		//�A�j���f�[�^����X�e�[�g�ɊY��������̂�������
		for (DsAnimClip* pAnim : animClip) {
			if ( std::string::npos != pAnim->RefAnimName().find(info.animTypeName) ) {
				//�X�e�[�g�ɊY������A�j�����������̂ŃX�e�[�g����
				DsChrState::INIT_ARG arg(pAnim, *m_pActReq, m_state);
				DsChrState* pState = DsChrState::CreateIns(info.state, arg);
				DS_ASSERT(pState, "�������s��");
				m_state[info.state] = pState;
				break;
			}
		}
	}

	//�����X�e�[�g�ݒ�
	auto it = m_state.find(CHR_STATE::IDLE);
	if (it != m_state.end()) {
		m_pASCtrl->RegisterActiveNode(it->second);
	}
}

DsActionCtrl::~DsActionCtrl()
{
	delete m_pASCtrl; m_pASCtrl = NULL;
	delete m_pActReq; m_pActReq = NULL;
	
	for (auto& state : m_state) {
		delete state.second;
	}
	m_state.clear();
}

void DsActionCtrl::Update(double dt)
{
	//���N�G�X�g�̍X�V
	if (m_pActReq) {
		m_pActReq->Update(dt);
	}

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