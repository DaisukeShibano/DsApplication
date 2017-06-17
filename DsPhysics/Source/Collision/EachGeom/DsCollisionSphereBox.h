#ifndef _DS_COLLISION_SPHERE_BOX_H_
#define _DS_COLLISION_SPHERE_BOX_H_

#ifndef __DS_ICOLLISION_GEOM__
#include "Collision/EachGeom/ICollisionGeom.h"
#endif
#ifndef __DS_COLLISION_RESULT__
#include "Collision/DsCollisionResult.h"
#endif

namespace DsPhysics
{
	class DsCollisionGeometry;
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	class DsCollisionSphereBox : public ICollisionGeom
	{
	public:
		DsCollisionSphereBox(const DsPhysicsWorld& world)
			: ICollisionGeom(world)
			, m_pSphere(NULL)
			, m_pBox(NULL)
			, m_info()
		{}
		void Initialize(const DsCollisionGeometry* pSphere, const DsCollisionGeometry* pBox)
		{
			m_pSphere = pSphere;
			m_pBox = pBox;
			m_info.Clear();
		}
		virtual ~DsCollisionSphereBox(){}

	public:
		virtual DsCollisionResult& Collide() override;

	private:
		bool _ColideAABB();

	private:
		const DsCollisionGeometry* m_pSphere;
		const DsCollisionGeometry* m_pBox;
		DsCollisionResult m_info;
	};

}

#endif