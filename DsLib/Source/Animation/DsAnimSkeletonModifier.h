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
		virtual void ModifyAnim(double dt, const DsKeyframeAnim& anim, DsVec3d worldPos, DsMat33d worldRot, DsAnimSkeleton& skeleton)=0;

	public:
		static void UtilKeyframeAnim(double dt, const DsKeyframeAnim& anim, DsAnimSkeleton& skeleton);
		static void UtilGetMasterMove(double dt, const DsLib::DsKeyframeAnim& anim, const DsLib::DsAnimSkeleton& skeleton, DsVec3d* pPos, DsMat33d* pRot);
	};
}


#endif