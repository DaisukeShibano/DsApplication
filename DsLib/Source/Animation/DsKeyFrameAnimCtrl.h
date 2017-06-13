#ifndef _DS_KEYFRAME_ANIM_CTRL_
#define _DS_KEYFRAME_ANIM_CTRL_

namespace DsLib
{
	class DsKeyFrameAnim;
	class DsAnimSkeleton;
}

namespace DsLib
{
	class DsKeyFrameAnimCtrl
	{
	public:
		DsKeyFrameAnimCtrl(DsKeyFrameAnim* pAnim, int animNum);
		virtual ~DsKeyFrameAnimCtrl();

	public:
		const DsKeyFrameAnim& RefKeyFrameAnim(int idx) const;
		const DsKeyFrameAnim* GetKeyFrameAnim(const char* name) const;
		DsKeyFrameAnim* GetKeyFrameAnims(){ return m_pAnim; }
		int GetKeyFrameAnimNum() const { return m_animNum; }

	public:
		void ApplyAnim(double dt, DsAnimSkeleton& skeleton, const DsKeyFrameAnim& anim) const;

	private:
		DsKeyFrameAnim* m_pAnim;
		const int m_animNum;
	};
}

#endif