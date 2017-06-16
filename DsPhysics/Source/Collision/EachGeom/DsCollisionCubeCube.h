#ifndef __DS_COLLISION_CUBE_CUBE__
#define __DS_COLLISION_CUBE_CUBE__

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
	class DsCollisionCubeCube : public ICollisionGeom
	{
	public:
		DsCollisionCubeCube(const DsPhysicsWorld& world)
		:ICollisionGeom(world)
		,m_pCube1(NULL)
		,m_pCube2(NULL)
		,m_info()
		{}

		void Initialize(const DsCollisionGeometry* pCube1, const DsCollisionGeometry* pCube2)
		{
			m_pCube1 = pCube1;
			m_pCube2 = pCube2;
			m_info.Clear();
		}

		virtual ~DsCollisionCubeCube(){}

		virtual DsCollisionResult& Collide() override;

	private:
		DsCollisionResult& _ColideFinal();

	private:
		const DsCollisionGeometry* m_pCube1;
		const DsCollisionGeometry* m_pCube2;
		DsCollisionResult m_info;
	};

}

#endif