#ifndef _DS_COLLISION_BOX_MESH_H_
#define _DS_COLLISION_BOX_MESH_H_

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
	class DsCollisionBoxMesh : public ICollisionGeom
	{
	public:
		DsCollisionBoxMesh(const DsPhysicsWorld& world)
			: ICollisionGeom(world)
			, m_pBox(NULL)
			, m_pMesh(NULL)
			, m_info()
		{}
		void Initialize(const DsCollisionGeometry* pBox, const DsCollisionGeometry* pMesh)
		{
			m_pBox = pBox;
			m_pMesh = pMesh;
			m_info.Clear();
		}
		virtual ~DsCollisionBoxMesh(){}

	public:
		virtual DsCollisionResult& Collide() override;

	private:
		DsCollisionResult& _ColideFinal();
		bool _ColideAABB();

	private:
		const DsCollisionGeometry* m_pBox;
		const DsCollisionGeometry* m_pMesh;
		DsCollisionResult m_info;
	};

}

#endif