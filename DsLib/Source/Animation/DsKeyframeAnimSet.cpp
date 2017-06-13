#include "DsPch.h"
#ifndef _DS_KEYFRAME_ANIM_SET_
#include "Animation/DsKeyframeAnimSet.h"
#endif
#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyframeAnim.h"
#endif
#ifndef _DS_AMIM_SKELETON_
#include "Animation/DsAnimSkeleton.h"
#endif
#ifndef __DS_MATH_UTIL__
#include "Math/DsMathUtil.h"
#endif


using namespace DsLib;

DsKeyframeAnimSet::DsKeyframeAnimSet(DsKeyframeAnim* pAnim, int animNum)
	: m_pAnim(pAnim)
	, m_animNum(animNum)
{
}

DsKeyframeAnimSet::~DsKeyframeAnimSet()
{
	delete[] m_pAnim;
	m_pAnim = NULL;
}

const DsKeyframeAnim& DsKeyframeAnimSet::RefKeyframeAnim(int idx) const
{
	DS_ASSERT(idx < m_animNum, "アニメ数を超えたアクセス");
	return m_pAnim[idx];
}

const DsKeyframeAnim* DsKeyframeAnimSet::GetKeyframeAnim(const char* name) const
{
	for (int idx = 0; idx < m_animNum; ++idx)
	{
		if (m_pAnim[idx].RefName() == name)
		{
			return &m_pAnim[idx];
		}
	}
	return NULL;
}
