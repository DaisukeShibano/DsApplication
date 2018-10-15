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
static const char* ATTACK1_NAME = "Attack1";
static const char* ATTACK2_NAME = "Attack2";
static const char* ATTACK3_NAME = "Attack3";
static const char* ATTACK4_NAME = "Attack4";
static const char* DAMAGE_F_NAME = "DamageF";
static const char* DAMAGE_B_NAME = "DamageB";
static const char* DAMAGE_L_NAME = "DamageL";
static const char* DAMAGE_R_NAME = "DamageR";


DsActionCtrl::DsActionCtrl(DsActionRequest& actReq, const DsAnimEventFlags& animFlags, const std::vector<DsLib::DsAnimClip*>& animClip)
	: m_pASCtrl(NULL)
	, m_state()
	, m_pCurrentAnim(NULL)
{
	m_pASCtrl = new DsLib::DsActionStateCtrl;
	DS_ASSERT(m_pASCtrl, "メモリ確保失敗");


	struct CREATE_INFO
	{
		const CHR_STATE state;
		const char* animTypeName;
	};
	const CREATE_INFO createInfo[] =
	{
		{ CHR_STATE::IDLE, IDEL_ANIM_NAME },
		{ CHR_STATE::RUN, RUN_ANIM_NAME },
		{ CHR_STATE::ATTACK1, ATTACK1_NAME },
		{ CHR_STATE::ATTACK2, ATTACK2_NAME },
		{ CHR_STATE::ATTACK3, ATTACK3_NAME },
		{ CHR_STATE::ATTACK4, ATTACK4_NAME },
		{ CHR_STATE::DAMAGE_F, DAMAGE_F_NAME },
		{ CHR_STATE::DAMAGE_B, DAMAGE_B_NAME },
		{ CHR_STATE::DAMAGE_L, DAMAGE_L_NAME },
		{ CHR_STATE::DAMAGE_R, DAMAGE_R_NAME },
	};

	for (const CREATE_INFO& info : createInfo) {
		//アニメデータからステートに該当するものを見つける
		for (DsAnimClip* pAnim : animClip) {
			if ( std::string::npos != pAnim->RefAnimName().find(info.animTypeName) ) {
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