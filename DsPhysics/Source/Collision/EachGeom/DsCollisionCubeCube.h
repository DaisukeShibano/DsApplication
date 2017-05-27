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
	class DsCollisionContext;
}

namespace DsPhysics
{
	class DsCollisionCubeCube : public ICollisionGeom
	{
	public:
		DsCollisionCubeCube()
		:m_pCube1(NULL)
		,m_pCube2(NULL)
		,m_info()
		{}

		void Initialize(const DsCollisionContext* pCube1, const DsCollisionContext* pCube2)
		{
			m_pCube1 = pCube1;
			m_pCube2 = pCube2;
			m_info.Clear();
		}

		virtual ~DsCollisionCubeCube(){}

		virtual DsCollisionResult& Colide();

	private:
		DsCollisionResult& _ColideFinal();
		bool _ColideAABB();

	private:
		const DsCollisionContext* m_pCube1;
		const DsCollisionContext* m_pCube2;
		DsCollisionResult m_info;
	};

}

#endif