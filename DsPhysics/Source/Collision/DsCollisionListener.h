#ifndef __DS_COLLISION_LISTENER__
#define __DS_COLLISION_LISTENER__

namespace DsPhysics
{
	class DsCollisionGroup;
	class DsCollisionResult;
	class DsActor;
	class DsPhysicsWorld;
	class DsBoundingOctree;
}

namespace DsPhysics
{
	class DsCollisionListener
	{
	public:
		DsCollisionListener( DsPhysicsWorld& world );
		virtual ~DsCollisionListener();
		void Collide( DsCollisionGroup& group );
		void Cast( const DsActor& actor, const DsCollisionGroup& group, std::vector<DsCollisionResult>& resultVec ) const;

	private:
		bool _IsUseBoundingGroup(int actorNum) const;

	private:
		DsPhysicsWorld& m_world;
		DsBoundingOctree* m_pBoundingTree;
	};

}

#endif