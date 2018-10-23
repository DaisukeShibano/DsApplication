#include "DsAppPch.h"
#ifndef _DS_ACTION_CTRL_
#include "World/Field/Action/DsActionCtrl.h"
#endif
//他のヘッダ
#include "World/Field/Action/DsChrState.h"

using namespace DsApp;


DsActionCtrl::DsActionCtrl(DsActionRequest& actReq, const DsAnimEventFlags& animFlags, const std::vector<DsLib::DsAnimClip*>& animClip)
	: m_pASCtrl(NULL)
	, m_state()
	, m_pCurrentAnim(NULL)
{
	m_pASCtrl = new DsLib::DsActionStateCtrl;
	DS_ASSERT(m_pASCtrl, "メモリ確保失敗");


	STATE_CLASS_TYPE* pCreateInfo = DsChrState::GetStateClassTypes();
	for (size_t idx = 0; idx < DsChrState::GetStateClassTypesNum(); ++idx) {
		const STATE_CLASS_TYPE& info = pCreateInfo[idx];
		//アニメデータからステートに該当するものを見つける
		for (DsAnimClip* pAnim : animClip) {
			if ( std::string::npos != pAnim->RefAnimName().find(info.pName) ) {
				//ステートに該当するアニメを見つけたのでステート生成
				DsChrState::INIT_ARG arg(pAnim, actReq, m_state, animFlags, info.state);
				DsChrState* pState = DsChrState::CreateIns(arg);
				DS_ASSERT(pState, "ステート作成失敗");
				m_state[info.state] = pState;
				break;
			}
		}
	}

	//初期ステート設定
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
	//ステートの更新
	if (m_pASCtrl) {
		m_pASCtrl->Update(dt);
	}

	//カレントアニメの更新
	std::vector<DsASNode*> node = m_pASCtrl->GetActiveNode();
	if (!node.empty()) {
		const DsChrState* pState = static_cast<DsChrState*>(node[0]);
		m_pCurrentAnim = pState->GetAnimClip();
	}
}