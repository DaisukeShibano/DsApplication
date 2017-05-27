#include "DsPch.h"
#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyFrameAnim.h"
#endif

using namespace DsLib;


//１フレで２回呼ぶとずれるので注意
void DsKeyFrameAnim::Pose::Update(double dt)
{
	m_currentTimeMs += (dt*1000.0);

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

void DsKeyFrameAnim::Pose::Reset()
{
	m_currentTimeMs = 0.0;
	m_currentIdxRot = 0;
	m_currentIdxPos = 0;
	m_currentIdxScale = 0;
	m_endFlag = 0;
}

bool DsKeyFrameAnim::Pose::IsEnd() const
{
	return (m_endFlag == END_FLAG_ALL);
}








DsKeyFrameAnim::DsKeyFrameAnim()
	: m_name()
	, m_pBone(NULL)
	, m_boneNum(0)
{
}

DsKeyFrameAnim::~DsKeyFrameAnim()
{
	delete[] m_pBone; 
	m_pBone = NULL;
}

void DsKeyFrameAnim::Update(double dt)
{
	for (int i = 0; i < m_boneNum; ++i)
	{
		m_pBone[i].Update(dt);
	}
}

void DsKeyFrameAnim::Reset()
{
	for (int i = 0; i < m_boneNum; ++i)
	{
		m_pBone[i].Reset();
	}
}

bool DsKeyFrameAnim::IsEnd() const
{
	for (int i = 0; i < m_boneNum; ++i)
	{
		if (!m_pBone[i].IsEnd())
		{
			return false;
		}
	}
	return true;
}