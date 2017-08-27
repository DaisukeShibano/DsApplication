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

#ifndef _DS_RES_PARAM_DS_RAGDOLL_PARAM_
#include "Res/Param/DsRagdollParam.h"
#endif

using namespace DsLib;
using namespace DsPhysics;
using namespace DsApp;


//virtual
void DsAnimRagdollModifier::ModifyAnim(double dt, DsAnimSkeleton& skeleton, const DsKeyframeAnim& anim, DsVec3d worldPos, DsMat33d worldRot)
{
	//キーフレームアニメでスケルトンを変更
	UtilKeyframeAnim(dt, skeleton, anim);
	
	std::vector<DsAnimBone*>& bones = skeleton.RefBoneArray();
	for (DsRagdollParts& parts : m_ragdoll.RefParts()) {
		DsRagdollParam param(parts.ragdollParamId);
		if (param.GetAnimType() == DS_RAGDOLL_PARAM_ANIM_TYPE::KEYFRAME) {
			//スケルトンでラグドールを動かす
			m_ragdoll.FixToKeyframeAnim(dt, bones, parts, worldPos, worldRot);
		}
		else {
			//ラグドールでスケルトンを動かす
			m_ragdoll.FixToPhysics(dt, bones, parts, worldPos, worldRot);
		}
	}

}