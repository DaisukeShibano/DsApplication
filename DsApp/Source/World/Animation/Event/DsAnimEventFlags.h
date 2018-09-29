#pragma once

/*コピペ用
#include "World/Animation/Event/DsAnimEventFlags.h"
*/

namespace DsApp
{
	class DsAnimEventFlags
	{
	public:
		DsAnimEventFlags()
			: m_animInterpolationTime(-1.0)
		{}

		~DsAnimEventFlags()
		{}

		void Clear()
		{
			m_animInterpolationTime = -1.0;
		}

		bool GetAnimInterpolationTime(double& time) const
		{
			bool ret = false;
			if (m_animInterpolationTime < 0.0) {
				//マイナスは無効
			}
			else {
				time = m_animInterpolationTime;
				ret = true;
			}
			return ret;
		}

		void SetAnimInterpolationTime(double time)
		{
			m_animInterpolationTime = time;
		}

	private:
		double m_animInterpolationTime;
	};

}