#ifndef _DS_ANIM_BLEND_H_
#define _DS_ANIM_BLEND_H_

#ifndef _DS_KEYFRAME_ANIM_
#include "Animation/DsKeyframeAnim.h"
#endif

namespace DsLib
{
	class DsAnimClip;
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
		//blendRate = 1 �� A�B blendRate = 0 �� B
		const DsKeyframeAnim& Blend(const DsAnimClip* pClipA, const DsAnimClip* pClipB, double blendRate);

	private:
		DsKeyframeAnim* m_pBlendAnim;

	};

}

#endif