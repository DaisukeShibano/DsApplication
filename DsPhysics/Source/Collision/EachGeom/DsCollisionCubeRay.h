#ifndef __DS_COLLISION_CUBE_RAY__
#define __DS_COLLISION_CUBE_RAY__

#ifndef __DS_ICOLLISION_GEOM__
#include "Collision/EachGeom/ICollisionGeom.h"
#endif
#ifndef __DS_COLLISION_RESULT__
#include "Collision/DsCollisionResult.h"
#endif

namespace DsPhysics
{
	class DsRigidCube;
	class DsCollisionContext;
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	class DsCollisionCubeRay : public ICollisionGeom
	{
	public:
		DsCollisionCubeRay(const DsPhysicsWorld& world)
		:ICollisionGeom(world)
		,m_pCube(NULL)
		,m_pRay(NULL)
		, m_info()
		{}

		void Initialize(const DsCollisionContext* pCube, const DsCollisionContext* pRay)
		{
			m_pCube = pCube;
			m_pRay = pRay;
			m_info.Clear();
		}

		virtual ~DsCollisionCubeRay(){}

		virtual DsCollisionResult& Collide() override;

	private:
		const DsCollisionContext* m_pCube;
		const DsCollisionContext* m_pRay;
		DsCollisionResult m_info;

	};

}

#endif