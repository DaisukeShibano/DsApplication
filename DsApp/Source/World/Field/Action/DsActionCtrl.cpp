#include "DsAppPch.h"
#ifndef _DS_ACTION_CTRL_
#include "World/Field/Action/DsActionCtrl.h"
#endif
//���̃w�b�_
#include "World/Field/Action/DsChrState.h"
#include "Res/Param/DsChrParam.h"


using namespace DsApp;


DsActionCtrl::DsActionCtrl(DsActionRequest& actReq, const DsAnimEventFlags& animFlags, const std::vector<DsLib::DsAnimClip*>& animClip, const DsChrParam& param)
	: m_pASCtrl(NULL)
	, m_state()
	, m_pCurrentAnim(NULL)
{
	m_pASCtrl = new DsLib::DsActionStateCtrl;
	DS_ASSERT(m_pASCtrl, "�������m�ێ��s");

	//�A�j���f�[�^����X�e�[�g�ɊY��������̂�������
	for (DsAnimClip* pAnim : animClip) {
		//�X�e�[�g�ɊY������A�j�����������̂ŃX�e�[�g����
		DsChrState::INIT_ARG arg(pAnim, actReq, m_state, animFlags, CHR_STATE::IDLE);//�����ł͂܂��X�e�[�g�͕�����Ȃ��̂Ń_�~�[
		DsChrState* pState = DsChrState::CreateIns(arg, pAnim->RefAnimName(), param.GetStateGraphId());
		if (pState) {
			m_state[pState->GetMyState()] = pState;
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