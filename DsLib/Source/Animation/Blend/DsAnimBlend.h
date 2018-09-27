#ifndef _DS_ANIM_BLEND_H_
#define _DS_ANIM_BLEND_H_

#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyframeAnim.h"
#endif

namespace DsLib
{
	class DsAnimClip;
	class DsAnimSkeleton;
	struct DsAnimBone;
}

namespace DsLib
{
	class DsAnimBlend
	{
	public:
		DsAnimBlend();
		virtual ~DsAnimBlend();

	public:
		void Initialize(const DsKeyframeAnim& src);

	public:
		//blendRate = 1 Ç≈ AÅB blendRate = 0 Ç≈ B
		const DsKeyframeAnim& Blend(const DsAnimSkeleton& skeleton, const DsAnimClip* pClipA, const DsAnimClip* pClipB, double blendRate);

	private:
		void _BlendPoseModel(const DsAnimBone* bone, const DsKeyframeAnim& animA, const DsKeyframeAnim& animB, const DsMat33d& parentRot, const DsVec3d& parentPos, double blendRate);
		void _BlendPoseLocal(const DsKeyframeAnim& animA, const DsKeyframeAnim& animB, double blendRate);

	private:
		DsKeyframeAnim* m_pBlendAnim;

	};

}

#endif