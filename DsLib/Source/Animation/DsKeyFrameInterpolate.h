#ifndef _DS_KEYFRAME_INTERPOLATE_
#define _DS_KEYFRAME_INTERPOLATE_

namespace DsLib
{
	class DsKeyFrameInterpolate
	{
	public:
		DsKeyFrameInterpolate(const float* pTimes, const float* pVals, int maxFrame)
			: m_val(pVals[0])
			, m_localTime(pTimes[0])
			, m_index(0)
			, m_pKeyFrameTime(pTimes)
			, m_pKeyFrameVal(pVals)
			, m_maxFrameNum(maxFrame)
		{}
		virtual ~DsKeyFrameInterpolate(){};

	public:
		float GetVal() const { return m_val; }
		void Update(float dt);
		void Reset();

	private:
		float m_val;
		float m_localTime;
		int m_index;

		const float* m_pKeyFrameTime;
		const float* m_pKeyFrameVal;
		const int m_maxFrameNum;
	};
}

#endif