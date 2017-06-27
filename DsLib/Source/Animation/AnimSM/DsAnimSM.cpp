#include "DsPch.h"
#ifndef _DS_ANIM_SM_H_
#include "Animation/AnimSM/DsAnimSM.h"
#endif

#ifndef _DS_ANIM_CONTROLLER_H_
#include "Animation/ActionRequest/DsAnimController.h"
#endif
#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyframeAnim.h"
#endif
#ifndef _DS_ANIM_CLIP_H_
#include "Animation/DsAnimClip.h"
#endif

using namespace std;
using namespace DsLib;


DsAnimSM::DsAnimSM(const DsAnimController& animController)
	: m_animController(animController)
	, m_activeState(IDLE)
	, m_prevActiveState(IDLE)
	//, m_pClips()
{
	for (int ci = 0; ci < STATE_NUM; ++ci)
	{
		m_pClips[ci] = NULL;
	}
}

DsAnimSM::~DsAnimSM()
{
	for (int ci = 0; ci < STATE_NUM; ++ci)
	{
		delete m_pClips[ci];
		m_pClips[ci] = NULL;
	}
}

void DsAnimSM::Initialize(DsKeyframeAnim* pAnim, int animNum)
{
	struct TmpClipSetting
	{
		STARE state;
		const char* name;
		bool isLoop;
	};

	const TmpClipSetting settings[] =
	{
		{ IDLE, "idle", true },
		{ WALK, "walk", true },
		{ RUN, "run", true },
	};

	for (int ai = 0; ai < animNum; ++ai)
	{
		for each(const TmpClipSetting& setting in settings)
		{
			//アニメの中から↑の名前のモーション探して状態と紐付ける
			if (string::npos != pAnim[ai].RefName().find(setting.name))
			{
				delete m_pClips[setting.state];
				m_pClips[setting.state] = new DsAnimClip(pAnim[ai]);
				m_pClips[setting.state]->SetLoop(setting.isLoop);
				break;
			}
		}
	}
}

void DsAnimSM::Update(double dt)
{
	switch (m_activeState)
	{
	case IDLE:
		_Idle(dt);
		break;
	case WALK:
		_Walk(dt);
		break;
	case RUN:
		_Run(dt);
		break;
	}

	for (int si = 0; si < STATE_NUM; ++si)
	{
		if (m_pClips[si])
		{
			if (si == m_activeState)
			{
				m_pClips[si]->Update(dt);
			}
			else if (si == m_prevActiveState)
			{
				m_pClips[si]->Update(dt);
				if (m_pClips[si]->IsEnd())
				{
					m_pClips[si]->ResetAnim();
					m_prevActiveState = NONE;
				}
			}
			else
			{
				m_pClips[si]->ResetAnim();
			}
		}
	}
}
const DsAnimClip* DsAnimSM::GetActiveClip() const
{
	return m_pClips[m_activeState];
}

const DsAnimClip* DsAnimSM::GetPreActiveClip() const
{
	return m_pClips[m_prevActiveState];
}

const DsAnimClip* DsAnimSM::GetDefaultClip() const
{
	return m_pClips[IDLE];
}

void DsAnimSM::_SetActiveState(STARE state)
{
	m_prevActiveState = m_activeState;
	m_activeState = state;
}

void DsAnimSM::_Idle(double dt)
{
	const bool isMove = m_animController.IsMove();
	if (isMove)
	{
		_SetActiveState(RUN);
		if (m_pClips[IDLE]) {
			m_pClips[IDLE]->Deactivate(dt);
		}
		if (m_pClips[RUN]) {
			m_pClips[RUN]->Activate(dt);
		}
		
	}
}

void DsAnimSM::_Walk(double dt)
{

}

void DsAnimSM::_Run(double dt)
{
	const bool isMove = m_animController.IsMove();
	if (!isMove)
	{
		_SetActiveState(IDLE);
		if (m_pClips[RUN]) {
			m_pClips[RUN]->Deactivate(dt);
		}
		if (m_pClips[IDLE]) {
			m_pClips[IDLE]->Activate(dt);
		}
	}
}