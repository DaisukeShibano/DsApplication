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
void DsAnimSkeletonModifier::UtilKeyframeAnim(double dt, const DsKeyframeAnim& anim, DsAnimSkeleton& skeleton)
{
	//キーフレームアニメ
	auto setPose = [&anim](DsAnimBone* pBone)
	{
		const int idx = pBone->arrayIdx;
		const DsVec4d& rot = anim.RefCurrentRot(idx);
		const DsQuaterniond q(rot.x, rot.y, rot.z, rot.w);
		const DsMat33d mat = (pBone->isMaster) ? (DsMat33d::Identity()) : (DsQuaterniond::ToMat33d(q));
		const DsVec3d pos = (pBone->isMaster) ? (DsVec3d::Zero()) : (anim.RefCurrentPos(idx));
		//const DsVec3d& scale = anim.RefCurrentScale(idx);//使わない
		pBone->localPose = DsMat44d::Get(mat, pos);
	};

	skeleton.GetAllBone(setPose);

	//ローカルを更新したのでモデル座標にも反映
	skeleton.UpdatePose();
}

//static
void DsAnimSkeletonModifier::UtilGetMasterMove(double dt, const DsLib::DsKeyframeAnim& anim, const DsLib::DsAnimSkeleton& skeleton, DsVec3d* pPos, DsMat33d* pRot)
{
	const DsAnimBone* pBone = skeleton.GetMasterBone();
	if (pBone) {
		const DsVec4d& rot = anim.RefMasterMoveRot();
		const DsQuaterniond q(rot.x, rot.y, rot.z, rot.w);
		//FBX変換
		const DsMat33d fbxR = DsMat33d::RotateX(-M_PI*0.5);
		if (pRot) {
			*pRot = fbxR * DsQuaterniond::ToMat33d(q);
		}
		if (pPos) {
			*pPos = fbxR * anim.RefMasterMovePos();
		}
	}
}