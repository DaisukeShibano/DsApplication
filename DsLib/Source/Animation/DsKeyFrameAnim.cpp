#include "DsPch.h"
#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyframeAnim.h"
#endif

using namespace DsLib;



/*----------------------------------------------------------------------------
DsKeyframeAnim::Pose
-----------------------------------------------------------------------------*/

void DsKeyframeAnim::Pose::Update(double dt)
{
	m_currentTimeMs += (dt*1000.0);
	_Update();
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
void DsKeyframeAnim::Pose::SetLocalTime(double time)
{
	m_currentTimeMs = 0.0;
	m_currentIdxRot = 0;
	m_currentIdxPos = 0;
	m_currentIdxScale = 0;

	const double maxTime = max(max(m_pPos[m_keyFrameNumPos - 1].localTime, m_pRot[m_keyFrameNumRot - 1].localTime), (m_pScale) ? (m_pScale[m_keyFrameNumScale - 1].localTime) : (0));
	m_currentTimeMs = time * 1000.0;

	if (m_currentTimeMs < DBL_EPSILON) {
		m_endFlag = 0;
	}
	else if ((maxTime-DBL_EPSILON) < m_currentTimeMs) {
		m_endFlag = END_FLAG_ALL;
	}

	_Update();
}


bool DsKeyframeAnim::Pose::IsEnd() const
{
	return (m_endFlag == END_FLAG_ALL);
}
void DsKeyframeAnim::Pose::_Update()
{
	//現在のキーフレームインデックス更新
	//１フレで複数キーフレームまたぐかもしれないので、現在の時間になるまでスキップ
	while (m_pPos[m_currentIdxPos].localTime < m_currentTimeMs)
	{
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
}







/*----------------------------------------------------------------------------
DsKeyframeAnim
-----------------------------------------------------------------------------*/


DsKeyframeAnim::DsKeyframeAnim()
	: m_name()
	, m_pBone(NULL)
	, m_boneNum(0)
	, m_masterMove()
{
}

DsKeyframeAnim::~DsKeyframeAnim()
{
	delete[] m_pBone; 
	m_pBone = NULL;
}

void DsKeyframeAnim::Update(double dt)
{
	for (int i = 0; i < m_boneNum; ++i){
		m_pBone[i].Update(dt);
	}
	m_masterMove.Update(dt);
}

void DsKeyframeAnim::Reset()
{
	for (int i = 0; i < m_boneNum; ++i){
		m_pBone[i].Reset();
	}
	m_masterMove.Reset();
}

void DsKeyframeAnim::SetLocalTime(double dt)
{
	for (int i = 0; i < m_boneNum; ++i){
		m_pBone[i].SetLocalTime(dt);
	}
	m_masterMove.SetLocalTime(dt);
}

bool DsKeyframeAnim::IsEnd() const
{
	for (int i = 0; i < m_boneNum; ++i){
		if (!m_pBone[i].IsEnd()){
			return false;
		}
		//マスター移動量は関係なし
		//if (!m_masterMove.IsEnd()) {
		//	return false;
		//}
	}
	return true;
}