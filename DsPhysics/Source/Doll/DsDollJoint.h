#ifndef _DS_DOLL_JOINT_
#define _DS_DOLL_JOINT_

namespace DsPhysics
{
	class DsDollJoint
	{
	public:
		DsDollJoint(const DsVec3d& pre, const DsVec3d& pos, const DsVec3d& next );

		void AddAngle( const DsVec3d& angle );

		const DsVec3d& GetAngleFromLocal() const;

		void Update( const DsDollJoint& parent );

	private:
		const DsVec3d m_startPos;
		const DsVec3d m_endPos;
		DsVec3d m_angle;  //èâä˙îzíuÇ©ÇÁÇÃäpìx
		const double m_len;
	};
}

#endif