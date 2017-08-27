#ifndef _DS_ANIM_SKELETON_MODIFIER_
#define _DS_ANIM_SKELETON_MODIFIER_

namespace DsLib
{
	class DsAnimSkeleton;
	class DsKeyframeAnim;
}

namespace DsLib
{
	/*
	@brief �A�j���{�[�������H����
	*/
	class DsAnimSkeletonModifier
	{
	public:
		DsAnimSkeletonModifier() {};
		virtual ~DsAnimSkeletonModifier() {};

	public:
		virtual void ModifyAnim(double dt, DsAnimSkeleton& skeleton, const DsKeyframeAnim& anim, DsVec3d worldPos, DsMat33d worldRot)=0;

	public:
		static void UtilKeyframeAnim(double dt, DsAnimSkeleton& skeleton, const DsKeyframeAnim& anim);
	};
}


#endif