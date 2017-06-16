#ifndef _DS_COLLISION_CUBE_MESH_H_
#define _DS_COLLISION_CUBE_MESH_H_

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
	class DsCollisionCubeMesh : public ICollisionGeom
	{
	public:
		DsCollisionCubeMesh(const DsPhysicsWorld& world)
			: ICollisionGeom(world)
			, m_pCube(NULL)
			, m_pMesh(NULL)
			, m_info()
		{}
		void Initialize(const DsCollisionGeometry* pCube, const DsCollisionGeometry* pMesh)
		{
			m_pCube = pCube;
			m_pMesh = pMesh;
			m_info.Clear();
		}
		virtual ~DsCollisionCubeMesh(){}

	public:
		virtual DsCollisionResult& Collide() override;

	private:
		DsCollisionResult& _ColideFinal();
		bool _ColideAABB();

	private:
		const DsCollisionGeometry* m_pCube;
		const DsCollisionGeometry* m_pMesh;
		DsCollisionResult m_info;
	};

}

#endif