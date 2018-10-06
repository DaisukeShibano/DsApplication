#include "DsPch.h"
#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyframeAnim.h"
#endif

using namespace DsLib;



/*----------------------------------------------------------------------------
DsKeyframeAnim::Pose
-----------------------------------------------------------------------------*/
DsKeyframeAnim::Pose::Pose()
	: m_currentTimeMs(0.0)
	, m_maxLocalTimeMs(0.0)
	, m_pPos(0)
	, m_pRot(0)
	, m_pScale(0)
	, m_keyFrameNumRot(0)
	, m_keyFrameNumPos(0)
	, m_keyFrameNumScale(0)
	, m_currentIdxRot(0)
	, m_currentIdxPos(0)
	, m_currentIdxScale(0)
	, m_endFlag(0)
{
}

DsKeyframeAnim::Pose::~Pose()
{
	delete[] m_pPos; m_pPos = NULL;
	delete[] m_pRot; m_pRot = NULL;
	delete[] m_pScale; m_pScale = NULL;
}

void DsKeyframeAnim::Pose::Initialize()
{
	m_maxLocalTimeMs = max(max(m_pPos[m_keyFrameNumPos - 1].localTime, m_pRot[m_keyFrameNumRot - 1].localTime), (m_pScale) ? (m_pScale[m_keyFrameNumScale - 1].localTime) : (0));
}

void DsKeyframeAnim::Pose::Update(double dt, DsVec4d* pOutDeltaRot, DsVec3d* pOutDeltaPos)
{
	m_currentTimeMs += (dt*1000.0);
	_Update(pOutDeltaRot, pOutDeltaPos);
}

void DsKeyframeAnim::Pose::Reset()
{
	m_currentTimeMs = 0.0;
	m_currentIdxRot = 0;
	m_currentIdxPos = 0;
	m_currentIdxScale = 0;
	m_endFlag = 0;
}

//time[s]
void DsKeyframeAnim::Pose::SetLocalTime(double time, DsVec4d* pOutDeltaRot, DsVec3d* pOutDeltaPos)
{
	m_currentTimeMs = 0.0;
	m_currentIdxRot = 0;
	m_currentIdxPos = 0;
	m_currentIdxScale = 0;
	m_endFlag = 0;

	const double maxTime = m_maxLocalTimeMs;
	m_currentTimeMs = time * 1000.0;

	if ((maxTime-DBL_EPSILON) < m_currentTimeMs) {
		m_endFlag = END_FLAG_ALL;
	}

	_Update(pOutDeltaRot, pOutDeltaPos);
}

bool DsKeyframeAnim::Pose::IsEnd() const
{
	return (m_endFlag == END_FLAG_ALL);
}

void DsKeyframeAnim::Pose::_Update(DsVec4d* pOutDeltaRot, DsVec3d* pOutDeltaPos)
{
	//現在のキーフレームインデックス更新
	//１フレで複数キーフレームまたぐかもしれないので、現在の時間になるまでスキップ
	while (m_pPos[m_currentIdxPos].localTime < m_currentTimeMs)
	{
		const double localTime = m_pPos[m_currentIdxPos].localTime;
		if (pOutDeltaPos) {
			*pOutDeltaPos += m_pPos[m_currentIdxPos].val;
		}

		++m_currentIdxPos;
		if (m_keyFrameNumPos <= m_currentIdxPos)
		{
			m_currentIdxPos = m_keyFrameNumPos - 1;
			m_endFlag |= END_FLAG_POS;
			break;
		}
	}


	while (m_pRot[m_currentIdxRot].localTime < m_currentTimeMs)
	{
		const double localTime = m_pPos[m_currentIdxPos].localTime;
		if (pOutDeltaRot) {
			//*pOutDeltaRot += m_pRot[m_currentIdxRot].val;//q*=dq;
		}

		++m_currentIdxRot;
		if (m_keyFrameNumRot <= m_currentIdxRot)
		{
			m_currentIdxRot = m_keyFrameNumRot - 1;
			m_endFlag |= END_FLAG_ROT;
			break;
		}
	}

	if (m_pScale) 
	{
		while (m_pScale[m_currentIdxScale].localTime < m_currentTimeMs)
		{
			++m_currentIdxScale;
			if (m_keyFrameNumScale <= m_currentIdxScale)
			{
				m_currentIdxScale = m_keyFrameNumScale - 1;
				m_endFlag |= END_FLAG_SCALE;
				break;
			}
		}
	}
	else
	{
		//終わったことにしておいた方が実装上都合がいい
		m_endFlag |= END_FLAG_SCALE;
	}
}







/*----------------------------------------------------------------------------
DsKeyframeAnim
-----------------------------------------------------------------------------*/
DsKeyframeAnim::DsKeyframeAnim()
	: m_masterMove()
	, m_masterMoveDeltaRot(DsVec4d::Zero())
	, m_masterMoveDeltaPos(DsVec3d::Zero())
	, m_masterMoveDeltaRotPre(DsVec4d::Zero())
	, m_masterMoveDeltaPosPre(DsVec3d::Zero())
	, m_name()
	, m_currentTimeMs(0.0)
	, m_maxLocalTimeMs(0.0)
	, m_pBone(NULL)
	, m_boneNum(0)
	, m_isLoop(true)
{
}

DsKeyframeAnim::~DsKeyframeAnim()
{
	delete[] m_pBone; 
	m_pBone = NULL;
}

void DsKeyframeAnim::Initialize()
{
	for (int i = 0; i < m_boneNum; ++i) {
		m_pBone[i].Initialize();
	}
	m_masterMove.Initialize();

	for (int i = 0; i < m_boneNum; ++i) {
		m_maxLocalTimeMs = max(m_pBone[i].GetMaxLocalTime(), m_maxLocalTimeMs);
	}
	m_maxLocalTimeMs = max(m_masterMove.GetMaxLocalTime(), m_maxLocalTimeMs);
}

void DsKeyframeAnim::Update(double dt)
{
	m_currentTimeMs += (dt*1000.0);

	m_masterMoveDeltaRot = DsVec4d::Zero();
	m_masterMoveDeltaPos = DsVec3d::Zero();

	for (int i = 0; i < m_boneNum; ++i){
		m_pBone[i].Update(dt, NULL, NULL);
	}
	m_masterMove.Update(dt, &m_masterMoveDeltaRot, &m_masterMoveDeltaPos);

	//ループアニメは時間を戻してループさせる
	if (m_isLoop && IsEnd()){
		if (0.0 < m_maxLocalTimeMs) {
			while (m_maxLocalTimeMs < m_currentTimeMs) {
				m_currentTimeMs -= m_maxLocalTimeMs;
			}
			for (int i = 0; i < m_boneNum; ++i) {
				m_pBone[i].SetLocalTime(m_currentTimeMs*0.001, NULL, NULL);
			}
			m_masterMove.SetLocalTime(m_currentTimeMs*0.001, &m_masterMoveDeltaRot, &m_masterMoveDeltaPos);
			//0フレ目は前フレとの差分がないので移動量がない。なので前フレのものを引き継ぐ
			//m_masterMoveDeltaRot += m_masterMoveDeltaRotPre;//qの乗算にしなきゃダメな気が...使わないけど
			m_masterMoveDeltaPos += m_masterMoveDeltaPosPre;
		}
		else {
			//最大時刻が0秒？最初のフレームしかポーズがない？
			//IsEnd()をtrueにするか悩みどころ
		}
	}

	m_masterMoveDeltaRotPre = m_masterMoveDeltaRot;
	m_masterMoveDeltaPosPre = m_masterMoveDeltaPos;
}

void DsKeyframeAnim::Reset()
{
	m_currentTimeMs = 0;

	for (int i = 0; i < m_boneNum; ++i){
		m_pBone[i].Reset();
	}
	m_masterMove.Reset();
}

void DsKeyframeAnim::SetLocalTime(double time)
{
	m_currentTimeMs = time;

	for (int i = 0; i < m_boneNum; ++i){
		m_pBone[i].SetLocalTime(time, NULL, NULL);
	}
	m_masterMove.SetLocalTime(time, NULL, NULL);
}

bool DsKeyframeAnim::IsEnd() const
{
	if (!m_masterMove.IsEnd()) {
		return false;
	}

	for (int i = 0; i < m_boneNum; ++i){
		if (!m_pBone[i].IsEnd()){
			return false;
		}
	}
	
	return true;
}

void DsKeyframeAnim::SetLoop(bool isLoop)
{
	m_isLoop = isLoop;
}