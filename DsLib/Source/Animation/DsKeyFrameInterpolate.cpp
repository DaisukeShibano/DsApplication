#include "DsPch.h"
#ifndef _DS_KEYFRAME_INTERPOLATE_
#include "Animation/DsKeyframeInterpolate.h"
#endif

using namespace DsLib;

void DsKeyframeInterpolate::Update(float dt)
{
	if (m_maxFrameNum <= m_index)
	{
		//最大まで来たのでこれ以降は処理しない
		m_val = m_pKeyframeVal[m_maxFrameNum - 1];
		return;
	}

	m_localTime += dt;
	
	if (m_pKeyframeTime[m_index] <= m_localTime)
	{
		++m_index;
		if (m_maxFrameNum <= m_index)
		{
			//最大まで来たのでこれ以降は処理しない
			m_val = m_pKeyframeVal[m_maxFrameNum-1];
			return;
		}
	}

	if (0 < m_index)
	{
		//キーフレーム間を線形補間
		const float maxLen = m_pKeyframeTime[m_index] - m_pKeyframeTime[m_index - 1];
		const float len = m_localTime - m_pKeyframeTime[m_index - 1];
		const float rate = (0.000000001f < maxLen) ? (len / maxLen) : 0.0f;
		m_val = ((m_pKeyframeVal[m_index] - m_pKeyframeVal[m_index - 1])*rate) + m_pKeyframeVal[m_index - 1];
	}
	else
	{
		m_val = m_pKeyframeVal[0];
	}
}

void DsKeyframeInterpolate::Reset()
{
	m_val = m_pKeyframeVal[0];
	m_localTime = m_pKeyframeTime[0];
	m_index = 0;
}