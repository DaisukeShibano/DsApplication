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
		double GetLocalTime()const;
		bool IsEnd() const;
		bool IsEndBlend() const;
		double GetBlendRate() const { return m_blendRate; }
		void SetInterpolationTime(double time) { m_interpolationTime = time; }
		void ClearInterpolationTime();

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
		double m_interpolationTime;
	};
}


#endif

