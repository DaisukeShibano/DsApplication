#ifndef _DS_ANIM_RAGDOLL_MODIFIER_
#define _DS_ANIM_RAGDOLL_MODIFIER_

#ifndef _DS_ANIM_SKELETON_MODIFIER_
#include "Animation/DsAnimSkeletonModifier.h"
#endif

namespace DsPhysics
{
	class DsRagdoll;
}

namespace DsApp
{
	/*
	@brief アニメボーンを加工する
	*/
	class DsAnimRagdollModifier : public DsLib::DsAnimSkeletonModifier
	{
	public:
		DsAnimRagdollModifier(DsPhysics::DsRagdoll& ragdoll)
			:m_ragdoll(ragdoll){};
		
		virtual ~DsAnimRagdollModifier() {};

	public:
		virtual void ModifyAnim(double dt, DsLib::DsAnimSkeleton& skeleton, const DsLib::DsKeyframeAnim& anim);

	private:
		DsPhysics::DsRagdoll& m_ragdoll;
	};
}


#endif