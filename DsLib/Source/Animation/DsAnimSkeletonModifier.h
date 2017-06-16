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
	@brief アニメボーンを加工する
	*/
	class DsAnimSkeletonModifier
	{
	public:
		DsAnimSkeletonModifier() {};
		virtual ~DsAnimSkeletonModifier() {};

	public:
		virtual void ModifyAnim(double dt, DsAnimSkeleton& skeleton, const DsKeyframeAnim& anim)=0;

	public:
		static void UtilKeyframeAnim(double dt, DsAnimSkeleton& skeleton, const DsKeyframeAnim& anim);
	};
}


#endif