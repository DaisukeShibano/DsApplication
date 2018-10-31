#pragma once

namespace DsApp
{
	class DsLockOnPoint;
}

namespace DsApp
{
	class DsLockOn
	{
	public:
		DsLockOn();
		virtual ~DsLockOn();

	public:
		void Register(DsLockOnPoint* pPoint);
		void Unregister(DsLockOnPoint* pPoint);
		void LockOn(const DsVec3d& pos, const DsVec3d dir);
		bool GetLockOnPos(DsVec3d& pos)const;

	private:
		std::set<DsLockOnPoint*> m_points;
		DsLockOnPoint* m_pBestPoint;
		double m_maxRange;
	};

}