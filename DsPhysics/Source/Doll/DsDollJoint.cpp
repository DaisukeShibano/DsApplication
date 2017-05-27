#include "DsPhysicsPch.h"
#include "Doll/DsDollJoint.h"

using namespace DsPhysics;

DsDollJoint::DsDollJoint(const DsVec3d& pre, const DsVec3d& pos, const DsVec3d& next )
:m_startPos(pos)
,m_endPos(next)
,m_angle(DsVec3d::Zero())
,m_len((pos-next).Length())
{
	const DsVec3d baseDir = pos-pre;
	const DsVec3d dir = next-pos;
	//前のジョイントを基準とした角度
	//今のグローバル角度-前のグローバル角度
	m_angle.x = atan2(dir.z,dir.y) - atan2(baseDir.z, baseDir.y);
	m_angle.y = atan2(dir.x,dir.z) - atan2(baseDir.x, baseDir.z);
	m_angle.z = atan2(dir.y,dir.x) - atan2(baseDir.y, baseDir.x);
}

void DsDollJoint::AddAngle( const DsVec3d& angle )
{
	m_angle += angle;
}

const DsVec3d& DsDollJoint::GetAngleFromLocal() const
{
	return m_angle;
}

void DsDollJoint::Update( const DsDollJoint& parent )
{


}