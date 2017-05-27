#include "DsPch.h"
#ifndef _DS_KEYFRAME_INTERPOLATE_
#include "Animation/DsKeyFrameInterpolate.h"
#endif

using namespace DsLib;

void DsKeyFrameInterpolate::Update(float dt)
{
	if (m_maxFrameNum <= m_index)
	{
		//最大まで来たのでこれ以降は処理しない
		m_val = m_pKeyFrameVal[m_maxFrameNum - 1];
		return;
	}

	m_localTime += dt;
	
	if (m_pKeyFrameTime[m_index] <= m_localTime)
	{
		++m_index;
		if (m_maxFrameNum <= m_index)
		{
			//最大まで来たのでこれ以降は処理しない
			m_val = m_pKeyFrameVal[m_maxFrameNum-1];
			return;
		}
	}

	if (0 < m_index)
	{
		//キーフレーム間を線形補間
		const float maxLen = m_pKeyFrameTime[m_index] - m_pKeyFrameTime[m_index - 1];
		const float len = m_localTime - m_pKeyFrameTime[m_index - 1];
		const float rate = (0.000000001f < maxLen) ? (len / maxLen) : 0.0f;
		m_val = ((m_pKeyFrameVal[m_index] - m_pKeyFrameVal[m_index - 1])*rate) + m_pKeyFrameVal[m_index - 1];
	}
	else
	{
		m_val = m_pKeyFrameVal[0];
	}
}

void DsKeyFrameInterpolate::Reset()
{
	m_val = m_pKeyFrameVal[0];
	m_localTime = m_pKeyFrameTime[0];
	m_index = 0;
}