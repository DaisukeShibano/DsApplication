#ifndef _DS_COLLISION_BOX_CAPSULE_
#define _DS_COLLISION_BOX_CAPSULE_

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
	class DsCollisionBoxCapsule : public ICollisionGeom
	{
	public:
		DsCollisionBoxCapsule(const DsPhysicsWorld& world)
			: ICollisionGeom(world)
			, m_pBox(NULL)
			, m_pCapsule(NULL)
			, m_info()
		{}
		virtual ~DsCollisionBoxCapsule() {}

	public:
		void Initialize(const DsCollisionGeometry* pBox, const DsCollisionGeometry* pCapsule)
		{
			m_pBox = pBox;
			m_pCapsule = pCapsule;
		}

	public:
		virtual DsCollisionResult& Collide() override;


	private:
		const DsCollisionGeometry* m_pBox;
		const DsCollisionGeometry* m_pCapsule;
		DsCollisionResult m_info;
	};
}

#endif