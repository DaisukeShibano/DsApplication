#ifndef __DS_COLLISION_BOX_RAY__
#define __DS_COLLISION_BOX_RAY__

#ifndef __DS_ICOLLISION_GEOM__
#include "Collision/EachGeom/ICollisionGeom.h"
#endif
#ifndef __DS_COLLISION_RESULT__
#include "Collision/DsCollisionResult.h"
#endif

namespace DsPhysics
{
	class DsRigidBox;
	class DsCollisionGeometry;
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	class DsCollisionBoxRay : public ICollisionGeom
	{
	public:
		DsCollisionBoxRay(const DsPhysicsWorld& world)
		:ICollisionGeom(world)
		,m_pBox(NULL)
		,m_pRay(NULL)
		, m_info()
		{}

		void Initialize(const DsCollisionGeometry* pBox, const DsCollisionGeometry* pRay)
		{
			m_pBox = pBox;
			m_pRay = pRay;
			m_info.Clear();
		}

		virtual ~DsCollisionBoxRay(){}

		virtual DsCollisionResult& Collide() override;

	private:
		const DsCollisionGeometry* m_pBox;
		const DsCollisionGeometry* m_pRay;
		DsCollisionResult m_info;

	};

}

#endif