#pragma once

/*コピペ用
#include "World/Field/Action/DsActionFlags.h"
*/

namespace DsApp
{
	enum class DsDmgDir : ds_uint8
	{
		None=0,
		F,
		B,
		L,
		R,
	};

	class DsActionFlags
	{
	public:
		DsActionFlags()
			: m_animInterpolationTime(-1.0)
			, m_isLockOnTurn(false)
			, m_flagsFirst(0)
			, m_flagsLast(0)
		{}

		~DsActionFlags()
		{}

		void ClearFirst()
		{
			m_animInterpolationTime = -1.0;
			m_flagsFirst = 0;
		}

		void ClearLast()
		{
			m_flagsLast = 0;
		}

		//アニメーション補間
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

		//ロックオン
		void SetLockOnTurn(bool isSet) { m_isLockOnTurn = isSet; }
		bool IsLockOnTurn() const { return m_isLockOnTurn; }

		//ダメージ
		void SetDmgDir(DsDmgDir type) { m_dmgType = static_cast<ds_uint8>(type); }
		DsDmgDir GetDmgDir() const { return static_cast<DsDmgDir>(m_dmgType); }

	private:
		double m_animInterpolationTime;
		bool m_isLockOnTurn;

		union
		{
			ds_uint64 m_flagsFirst;
			struct {

			};
		};

		union
		{
			ds_uint64 m_flagsLast;
			struct {
				ds_uint64 m_dmgType : 3;
			};
		};
	};

}