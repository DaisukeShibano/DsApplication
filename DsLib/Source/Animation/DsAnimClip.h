#ifndef _DS_ANIM_CLIP_H_
#define _DS_ANIM_CLIP_H_

namespace DsLib
{
	class DsKeyframeAnim;
}

namespace DsLib
{
	class DsAnimClip
	{
	public:
		DsAnimClip(DsKeyframeAnim& anim);
		virtual ~DsAnimClip();

	public:
		void Activate(double dt);
		void Update(double dt);
		void Deactivate(double dt);
		void SetLoop(bool loop){ m_isLoop = loop; }
		void ResetAnim();
		void SetLocalTime(double time);
		bool IsEnd() const;
		bool IsEndWithoutBlend() const;
		double GetBlendRate() const { return m_blendRate; }

	public:
		const DsKeyframeAnim& RefAnim() const { return m_anim; }
		const std::string& RefAnimName() const;

	private:
		DsKeyframeAnim& m_anim;
		bool m_isLoop;
		bool m_isActive;
		double m_blendRate;
		bool m_isRequestEnd;
		double m_localTime;
	};
}


#endif

