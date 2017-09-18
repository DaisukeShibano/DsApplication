#include "DsPch.h"
#ifndef _DS_ANIM_SKELETON_MODIFIER_
#include "Animation/DsAnimSkeletonModifier.h"
#endif

#ifndef _DS_AMIM_SKELETON_
#include "Animation/DsAnimSkeleton.h"
#endif
#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyframeAnim.h"
#endif

using namespace DsLib;

//static 
void DsAnimSkeletonModifier::UtilKeyframeAnim(double dt, DsAnimSkeleton& skeleton, const DsKeyframeAnim& anim)
{
	//キーフレームアニメ
	auto setPose = [&anim](DsAnimBone* pBone)
	{
		const int idx = pBone->arrayIdx;
		const DsVec4d& rot = anim.RefCurrentRot(idx);

		const DsQuaterniond q(rot.x, rot.y, rot.z, rot.w);
		const DsMat33d mat = DsMathUtil::ToMat33d<double>(q);

		const DsVec3d& pos = anim.RefCurrentPos(idx);
		const DsVec3d& scale = anim.RefCurrentScale(idx);
		pBone->localPose = DsMat44d::Get(mat, pos);
	};

	skeleton.GetAllBone(setPose);

	//ローカルを更新したのでモデル座標にも反映
	skeleton.UpdatePose();
}