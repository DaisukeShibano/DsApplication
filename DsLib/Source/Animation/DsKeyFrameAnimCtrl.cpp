#include "DsPch.h"
#ifndef _DS_KEYFRAME_ANIM_CTRL_
#include "Animation/DsKeyFrameAnimCtrl.h"
#endif
#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyFrameAnim.h"
#endif
#ifndef _DS_AMIM_SKELETON_
#include "Animation/DsAnimSkeleton.h"
#endif
#ifndef __DS_MATH_UTIL__
#include "Math/DsMathUtil.h"
#endif


using namespace DsLib;

DsKeyFrameAnimCtrl::DsKeyFrameAnimCtrl(DsKeyFrameAnim* pAnim, int animNum)
	: m_pAnim(pAnim)
	, m_animNum(animNum)
{
}

DsKeyFrameAnimCtrl::~DsKeyFrameAnimCtrl()
{
	delete[] m_pAnim;
	m_pAnim = NULL;
}

const DsKeyFrameAnim& DsKeyFrameAnimCtrl::RefKeyFrameAnim(int idx) const
{
	DS_ASSERT(idx < m_animNum, "アニメ数を超えたアクセス");
	return m_pAnim[idx];
}

const DsKeyFrameAnim* DsKeyFrameAnimCtrl::GetKeyFrameAnim(const char* name) const
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

void DsKeyFrameAnimCtrl::ApplyAnim(double dt, DsAnimSkeleton& skeleton, const DsKeyFrameAnim& anim) const
{
	auto setPose = [&anim](DsAnimBone* pBone)
	{
		int idx = pBone->arrayIdx;
		const DsVec4d& rot = anim.RefCurrentRot(idx);
		
		const DsQuaterniond q(rot.x, rot.y, rot.z, rot.w);
		const DsMat33d mat = DsMathUtil::ToMat33d<double>(q);
		
		const DsVec3d& pos = anim.RefCurrentPos(idx);
		const DsVec3d& scale = anim.RefCurrentScale(idx);
		pBone->localPose = DsMat44d::Get(mat, pos);
	};

	skeleton.GetAllBone(setPose);
}