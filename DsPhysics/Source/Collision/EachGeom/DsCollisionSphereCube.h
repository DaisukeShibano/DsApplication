#ifndef _DS_COLLISION_SPHERE_CUBE_H_
#define _DS_COLLISION_SPHERE_CUBE_H_

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
	class DsCollisionSphereCube : public ICollisionGeom
	{
	public:
		DsCollisionSphereCube(const DsPhysicsWorld& world)
			: ICollisionGeom(world)
			, m_pSphere(NULL)
			, m_pCube(NULL)
			, m_info()
		{}
		void Initialize(const DsCollisionGeometry* pSphere, const DsCollisionGeometry* pCube)
		{
			m_pSphere = pSphere;
			m_pCube = pCube;
			m_info.Clear();
		}
		virtual ~DsCollisionSphereCube(){}

	public:
		virtual DsCollisionResult& Collide() override;

	private:
		bool _ColideAABB();

	private:
		const DsCollisionGeometry* m_pSphere;
		const DsCollisionGeometry* m_pCube;
		DsCollisionResult m_info;
	};

}

#endif