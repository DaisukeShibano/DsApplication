#ifndef _DS_KEYFRAME_ANIM_SET_
#define _DS_KEYFRAME_ANIM_SET_

namespace DsLib
{
	class DsKeyframeAnim;
	class DsAnimSkeleton;
}

namespace DsLib
{
	class DsKeyframeAnimSet
	{
	public:
		DsKeyframeAnimSet(DsKeyframeAnim* pAnim, int animNum);
		virtual ~DsKeyframeAnimSet();

	public:
		const DsKeyframeAnim& RefKeyframeAnim(int idx) const;
		const DsKeyframeAnim* GetKeyframeAnim(const char* name) const;
		DsKeyframeAnim* GetKeyframeAnims(){ return m_pAnim; }
		int GetKeyframeAnimNum() const { return m_animNum; }

	private:
		DsKeyframeAnim* m_pAnim;
		const int m_animNum;
	};
}

#endif