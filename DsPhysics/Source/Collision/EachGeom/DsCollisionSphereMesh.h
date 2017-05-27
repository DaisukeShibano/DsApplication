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
	class DsCollisionContext;
}

namespace DsPhysics
{
	class DsCollisionSphereMesh : public ICollisionGeom
	{
	public:
		DsCollisionSphereMesh()
			: m_pSphere(NULL)
			, m_pMesh(NULL)
			, m_info()
		{}
		void Initialize(const DsCollisionContext* pSphere, const DsCollisionContext* pMesh)
		{
			m_pSphere = pSphere;
			m_pMesh = pMesh;
			m_info.Clear();
		}
		virtual ~DsCollisionSphereMesh(){}

	public:
		virtual DsCollisionResult& Colide();

	private:
		bool _ColideAABB();

	private:
		const DsCollisionContext* m_pSphere;
		const DsCollisionContext* m_pMesh;
		DsCollisionResult m_info;
	};

}

#endif