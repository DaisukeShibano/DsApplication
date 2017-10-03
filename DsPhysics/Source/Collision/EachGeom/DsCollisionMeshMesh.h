#ifndef _DS_COLLISION_MESH_MESH_H_
#define _DS_COLLISION_MESH_MESH_H_

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
	class DsCollisionMeshMesh : public ICollisionGeom
	{
	public:
		DsCollisionMeshMesh(const DsPhysicsWorld& world)
			: ICollisionGeom(world)
			, m_pMesh1(NULL)
			, m_pMesh2(NULL)
			, m_info()
		{}
		void Initialize(const DsCollisionGeometry* pMesh1, const DsCollisionGeometry* pMesh2)
		{
			m_pMesh1 = pMesh1;
			m_pMesh2 = pMesh2;
			m_info.Clear();
		}
		virtual ~DsCollisionMeshMesh(){}

	public:
		virtual DsCollisionResult& Collide() override;

	private:
		const DsCollisionGeometry* m_pMesh1;
		const DsCollisionGeometry* m_pMesh2;
		DsCollisionResult m_info;
	};

}

#endif