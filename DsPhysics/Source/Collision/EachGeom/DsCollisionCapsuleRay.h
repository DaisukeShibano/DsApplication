#pragma

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
	class DsCollisionCapsuleRay : public ICollisionGeom
	{
	public:
		DsCollisionCapsuleRay(const DsPhysicsWorld& world)
			: ICollisionGeom(world)
			, m_pCapsule(NULL)
			, m_pRay(NULL)
			, m_info()
		{}
		void Initialize(const DsCollisionGeometry* pCapsule, const DsCollisionGeometry* pRay)
		{
			m_pCapsule = pCapsule;
			m_pRay = pRay;
			m_info.Clear();
		}
		virtual ~DsCollisionCapsuleRay(){}

	public:
		virtual DsCollisionResult& Collide() override;

	private:
		bool _ColideAABB();

	private:
		const DsCollisionGeometry* m_pCapsule;
		const DsCollisionGeometry* m_pRay;
		DsCollisionResult m_info;
	};

}