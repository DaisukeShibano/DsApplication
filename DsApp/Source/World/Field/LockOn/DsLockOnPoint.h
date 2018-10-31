#pragma once

namespace DsApp
{
	class DsLockOnPoint
	{
	public:
		DsLockOnPoint();
		virtual ~DsLockOnPoint();

		void SetPos(const DsVec3d pos) { m_pos = pos; }
		DsVec3d GetPos() const { return m_pos; }

	private:
		DsVec3d m_pos;

	};
}