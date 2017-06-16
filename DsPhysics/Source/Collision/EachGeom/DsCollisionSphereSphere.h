#ifndef _DS_COLLISION_SPHERE_SPHERE_H_
#define _DS_COLLISION_SPHERE_SPHERE_H_

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
	class DsCollisionSphereSphere : public ICollisionGeom
	{
	public:
		DsCollisionSphereSphere(const DsPhysicsWorld& world)
			: ICollisionGeom(world)
			, m_pSphere1(NULL)
			, m_pSphere2(NULL)
			, m_info()
		{}
		void Initialize(const DsCollisionGeometry* pSphere1, const DsCollisionGeometry* pSphere2)
		{
			m_pSphere1 = pSphere1;
			m_pSphere2 = pSphere2;
			m_info.Clear();
		}
		virtual ~DsCollisionSphereSphere(){}

	public:
		virtual DsCollisionResult& Collide() override;

	private:
		bool _ColideAABB();

	private:
		const DsCollisionGeometry* m_pSphere1;
		const DsCollisionGeometry* m_pSphere2;
		DsCollisionResult m_info;
	};

}

#endif