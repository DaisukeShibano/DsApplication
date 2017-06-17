#ifndef __DS_COLLISION_BOX_BOX__
#define __DS_COLLISION_BOX_BOX__

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
	class DsCollisionBoxBox : public ICollisionGeom
	{
	public:
		DsCollisionBoxBox(const DsPhysicsWorld& world)
		:ICollisionGeom(world)
		,m_pBox1(NULL)
		,m_pBox2(NULL)
		,m_info()
		{}

		void Initialize(const DsCollisionGeometry* pBox1, const DsCollisionGeometry* pBox2)
		{
			m_pBox1 = pBox1;
			m_pBox2 = pBox2;
			m_info.Clear();
		}

		virtual ~DsCollisionBoxBox(){}

		virtual DsCollisionResult& Collide() override;

	private:
		DsCollisionResult& _ColideFinal();

	private:
		const DsCollisionGeometry* m_pBox1;
		const DsCollisionGeometry* m_pBox2;
		DsCollisionResult m_info;
	};

}

#endif