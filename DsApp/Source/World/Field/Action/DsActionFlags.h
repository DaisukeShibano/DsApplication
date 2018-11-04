#pragma once

/*�R�s�y�p
#include "World/Field/Action/DsActionFlags.h"
*/

namespace DsApp
{
	class DsActionFlags
	{
	public:
		DsActionFlags()
			: m_animInterpolationTime(-1.0)
		{}

		~DsActionFlags()
		{}

		void Clear()
		{
			m_animInterpolationTime = -1.0;
			m_flags = 0;
		}

		bool GetAnimInterpolationTime(double& time) const
		{
			bool ret = false;
			if (m_animInterpolationTime < 0.0) {
				//�}�C�i�X�͖���
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

		void SetLockOnTurn(bool isSet) { m_isLockOnTurn = isSet; }
		bool IsLockOnTurn() const { return m_isLockOnTurn; }

	private:
		double m_animInterpolationTime;

		union
		{
			ds_uint64 m_flags;
			ds_uint64 m_isLockOnTurn : 1;
		};

	};

}