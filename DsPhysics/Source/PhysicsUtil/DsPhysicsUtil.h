#ifndef __DS_PHYSICS_UTIL__
#define __DS_PHYSICS_UTIL__

namespace DsPhysics
{
	class DsActorId;
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	namespace DsPhysicsUtil
	{
		DsVec3d CalcForceTargetPos( const DsVec3d& pos, const DsPhysicsWorld& world, const DsActorId& id, const DsVec3d& offset=DsVec3d::Zero() );
	}
}

#endif