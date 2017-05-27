#ifndef __DS_ICOLLISION_GEOM__
#define __DS_ICOLLISION_GEOM__

namespace DsPhysics
{
	class DsCollisionResult;
}

namespace DsPhysics
{
	class ICollisionGeom
	{
		virtual DsCollisionResult& Colide() = 0;
	};
}

#endif