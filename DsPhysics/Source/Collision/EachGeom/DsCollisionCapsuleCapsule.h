#ifndef _DS_COLLISION_CAPSULE_CAPSULE_
#define _DS_COLLISION_CAPSULE_CAPSULE_

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
	class DsCollisionCapsuleCapsule : public ICollisionGeom
	{
	public:
		DsCollisionCapsuleCapsule(const DsPhysicsWorld& world)
			: ICollisionGeom(world)
			, m_pCapsule1(NULL)
			, m_pCapsule2(NULL)
			, m_info()
		{}
		virtual ~DsCollisionCapsuleCapsule() {}

	public:
		void Initialize(const DsCollisionGeometry* pCapsule1, const DsCollisionGeometry* pCapsule2)
		{
			m_pCapsule1 = pCapsule1;
			m_pCapsule2 = pCapsule2;
		}

	public:
		virtual DsCollisionResult& Collide() override;

	private:
		int _dCollideSpheres(const DsVec3d& p1, double r1, const DsVec3d& p2, double r2, DsCollisionResult& c) const;
		int _dCollideCapsuleCapsule();


	private:
		const DsCollisionGeometry* m_pCapsule1;
		const DsCollisionGeometry* m_pCapsule2;
		DsCollisionResult m_info;
	};
}

#endif