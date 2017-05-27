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
	class DsCollisionContext;
}

namespace DsPhysics
{
	class DsCollisionCubeMesh : public ICollisionGeom
	{
	public:
		DsCollisionCubeMesh()
			: m_pCube(NULL)
			, m_pMesh(NULL)
			, m_info()
		{}
		void Initialize(const DsCollisionContext* pCube, const DsCollisionContext* pMesh)
		{
			m_pCube = pCube;
			m_pMesh = pMesh;
			m_info.Clear();
		}
		virtual ~DsCollisionCubeMesh(){}

	public:
		virtual DsCollisionResult& Colide();

	private:
		DsCollisionResult& _ColideFinal();
		bool _ColideAABB();

	private:
		const DsCollisionContext* m_pCube;
		const DsCollisionContext* m_pMesh;
		DsCollisionResult m_info;
	};

}

#endif