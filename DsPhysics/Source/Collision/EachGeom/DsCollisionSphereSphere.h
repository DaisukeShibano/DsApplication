#ifndef _DS_COLLISION_SPHERE_SPHERE_H_
#define _DS_COLLISION_SPHERE_SPHERE_H_

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
	class DsCollisionSphereSphere : public ICollisionGeom
	{
	public:
		DsCollisionSphereSphere()
			: m_pSphere1(NULL)
			, m_pSphere2(NULL)
			, m_info()
		{}
		void Initialize(const DsCollisionContext* pSphere1, const DsCollisionContext* pSphere2)
		{
			m_pSphere1 = pSphere1;
			m_pSphere2 = pSphere2;
			m_info.Clear();
		}
		virtual ~DsCollisionSphereSphere(){}

	public:
		virtual DsCollisionResult& Colide();

	private:
		bool _ColideAABB();

	private:
		const DsCollisionContext* m_pSphere1;
		const DsCollisionContext* m_pSphere2;
		DsCollisionResult m_info;
	};

}

#endif