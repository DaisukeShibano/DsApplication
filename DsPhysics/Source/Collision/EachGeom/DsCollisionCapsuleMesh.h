#ifndef _DS_COLLISION_CAPSULE_MESH_H_
#define _DS_COLLISION_CAPSULE_MESH_H_

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
	class DsCollisionCapsuleMesh : public ICollisionGeom
	{
	public:
		DsCollisionCapsuleMesh(const DsPhysicsWorld& world)
			: ICollisionGeom(world)
			, m_pCapsule(NULL)
			, m_pMesh(NULL)
			, m_info()
		{}
		void Initialize(const DsCollisionGeometry* pCapsule, const DsCollisionGeometry* pMesh)
		{
			m_pCapsule = pCapsule;
			m_pMesh = pMesh;
			m_info.Clear();
		}
		virtual ~DsCollisionCapsuleMesh(){}

	public:
		virtual DsCollisionResult& Collide() override;

	private:
		bool _ColideAABB();

	private:
		const DsCollisionGeometry* m_pCapsule;
		const DsCollisionGeometry* m_pMesh;
		DsCollisionResult m_info;
	};

}

#endif