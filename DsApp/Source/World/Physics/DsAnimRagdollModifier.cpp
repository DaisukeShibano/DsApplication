#include "DsAppPch.h"
#ifndef _DS_ANIM_RAGDOLL_MODIFIER_
#include "World/Physics/DsAnimRagdollModifier.h"
#endif

#ifndef _DS_AMIM_SKELETON_
#include "Animation/DsAnimSkeleton.h"
#endif
#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyframeAnim.h"
#endif
#ifndef _DS_RAGDOLL_
#include "Ragdoll/DsRagdoll.h"
#endif

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;


//virtual
void DsAnimRagdollModifier::ModifyAnim(double dt, DsAnimSkeleton& skeleton, const DsKeyframeAnim& anim)
{
	//キーフレームアニメでスケルトンを変更
	UtilKeyframeAnim(dt, skeleton, anim);

	//スケルトンでラグドールを動かす
	const std::vector<DsAnimBone*>& bones = skeleton.RefBoneArray();
	for (DsRagdollParts& parts : m_ragdoll.RefParts()) {
		const DsMat44d mat = bones[parts.skeletonBoneIdx]->worldPose;
		const DsVec3d pos = mat.GetPos();
		const DsMat33d rot = mat.ToMat33();

		DsDbgSys::GetIns().RefDrawCom().DrawSphere(pos, 0.1);

		parts.pActor->SetPosition(pos);
		//parts.pActor->SetRotation(rot);
	}

}