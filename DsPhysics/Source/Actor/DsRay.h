#ifndef __DS_RAY__
#define __DS_RAY__

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif
#ifndef __DS_LINE__
#include "DsLine.h"
#endif
#ifndef __DS_COLLISION_GEOMETRY__
#include "Collision/DsCollisionGeometry.h"
#endif
#ifndef __DS_ACTOR_FACTORY__
#include "Actor/DsActorFactory.h"
#endif


namespace DsPhysics
{
	class DsRay : public DsActor
	{
	public:
		DsRay( const DsActorId& id );
		virtual ~DsRay();

	public:
		virtual DsActor::ACTOR_TYPE GetType() const override{ return DsActor::RAY; }

		virtual const DsCollisionGeometry* GetCollisionGeometry() const override{ return m_pCollisionGeometry; }

		virtual void Update() override;
	
	public:
		void Create( const DsVec3d& start, const DsVec3d& end );

	public: //�`��̂��߂̃��\�b�h
		int GetVertexNum()const{ return 2; }

		const DsVec3d* GetVertex()const{ return m_vertex; }

	private:
		DsVec3d m_vertex[2];
		DsLine m_line;
		DsCollisionGeometry* m_pCollisionGeometry;
	};

	/////////////Factor////////////
	class DsRayFactor : public DsActorFactory
	{
	public:
		DsRayFactor( const DsVec3d& start, const DsVec3d& end )
		:m_start(start)
		,m_end(end)
		{}
		virtual DsActor* CreateIns( const DsActorId& id ) const override
		{
			DsRay* pRet = new DsRay(id);
			if(pRet)
			{
				pRet->Create(m_start, m_end);
				return pRet;
			}
			else
			{
				return 0;
			}
		}

	private:
		const DsVec3d& m_start;
		const DsVec3d& m_end;

	};

}

#endif