#ifndef _DS_ANIM_CLIP_H_
#define _DS_ANIM_CLIP_H_

namespace DsLib
{
	class DsKeyFrameAnim;
}

namespace DsLib
{
	class DsAnimClip
	{
	public:
		DsAnimClip(DsKeyFrameAnim& anim);
		virtual ~DsAnimClip();

	public:
		void Activate(double dt);
		void Update(double dt);
		void Deactivate(double dt);
		void SetLoop(bool loop){ m_isLoop = loop; }
		void ResetAnim();
		bool IsEnd() const;
		double GetBlendRate() const { return m_blendRate; }

	public:
		const DsKeyFrameAnim& RefAnim() const { return m_anim; }

	private:
		DsKeyFrameAnim& m_anim;
		bool m_isLoop;
		bool m_isActive;
		double m_blendRate;
		bool m_isRequestEnd;
		double m_localTime;
	};
}


#endif

