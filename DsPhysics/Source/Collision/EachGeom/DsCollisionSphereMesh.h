#ifndef _DS_COLLISION_SPHERE_MESH_H_
#define _DS_COLLISION_SPHERE_MESH_H_

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
	class DsCollisionSphereMesh : public ICollisionGeom
	{
	public:
		DsCollisionSphereMesh(const DsPhysicsWorld& world)
			: ICollisionGeom(world)
			, m_pSphere(NULL)
			, m_pMesh(NULL)
			, m_info()
		{}
		void Initialize(const DsCollisionGeometry* pSphere, const DsCollisionGeometry* pMesh)
		{
			m_pSphere = pSphere;
			m_pMesh = pMesh;
			m_info.Clear();
		}
		virtual ~DsCollisionSphereMesh(){}

	public:
		virtual DsCollisionResult& Collide() override;

	private:
		bool _ColideAABB();

	private:
		const DsCollisionGeometry* m_pSphere;
		const DsCollisionGeometry* m_pMesh;
		DsCollisionResult m_info;
	};

}

#endif