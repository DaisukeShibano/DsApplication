#ifndef __DS_ICOLLISION_GEOM__
#define __DS_ICOLLISION_GEOM__

namespace DsPhysics
{
	class DsCollisionResult;
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	class ICollisionGeom
	{
	public:
		ICollisionGeom(const DsPhysicsWorld& world) :m_world(world) {}
		virtual DsCollisionResult& Collide() = 0;

	protected:
		const DsPhysicsWorld& m_world;
	};
}

#endif