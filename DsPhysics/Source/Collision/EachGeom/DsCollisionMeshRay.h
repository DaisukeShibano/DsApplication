#ifndef __DS_COLLISION_MESH_RAY__
#define __DS_COLLISION_MESH_RAY__

#ifndef __DS_ICOLLISION_GEOM__
#include "Collision/EachGeom/ICollisionGeom.h"
#endif
#ifndef __DS_COLLISION_RESULT__
#include "Collision/DsCollisionResult.h"
#endif

namespace DsPhysics
{
	class DsRigidMesh;
	class DsRay;
	class DsCollisionGeometry;
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	class DsCollisionMeshRay : public ICollisionGeom
	{
	public:
		DsCollisionMeshRay(const DsPhysicsWorld& world)
			: ICollisionGeom(world)
			, m_pMesh(NULL)
			, m_pRay(NULL)
			, m_info()
		{}
		void Initialize(const DsCollisionGeometry* pMesh, const DsCollisionGeometry* pRay)
		{
			m_pMesh = pMesh;
			m_pRay = pRay;
			m_info.Clear();
		}
		virtual ~DsCollisionMeshRay(){}
		virtual DsCollisionResult& Collide() override;

	private:
		const DsCollisionGeometry* m_pMesh;
		const DsCollisionGeometry* m_pRay;
		DsCollisionResult m_info;

	};

}

#endif