#ifndef _DS_KEYFRAME_INTERPOLATE_
#define _DS_KEYFRAME_INTERPOLATE_

namespace DsLib
{
	class DsKeyframeInterpolate
	{
	public:
		DsKeyframeInterpolate(const float* pTimes, const float* pVals, int maxFrame)
			: m_val(pVals[0])
			, m_localTime(pTimes[0])
			, m_index(0)
			, m_pKeyframeTime(pTimes)
			, m_pKeyframeVal(pVals)
			, m_maxFrameNum(maxFrame)
		{}
		virtual ~DsKeyframeInterpolate(){};

	public:
		float GetVal() const { return m_val; }
		void Update(float dt);
		void Reset();

	private:
		float m_val;
		float m_localTime;
		int m_index;

		const float* m_pKeyframeTime;
		const float* m_pKeyframeVal;
		const int m_maxFrameNum;
	};
}

#endif