#ifndef __DS_COLLISION_GROUP__
#define __DS_COLLISION_GROUP__


namespace DsPhysics
{
	class DsActor;
	class DsActorId;
}

namespace DsPhysics
{
	/*
	@brief	コリジョングループ
	*/
	class DsCollisionGroup
	{
	
	public:
		
		DsCollisionGroup();
		virtual ~DsCollisionGroup();
		void AddGroup( DsActor* pActor );
		void RemoveGroup( const DsActorId& id);
		int GetActorNumber() const { return m_totalActorNum; }
		const DsActor*const* GetActors() const { return m_actorVec; }
		DsActor** GetActors() { return m_actorVec; }
		void SortActor();
		void Clear();
	
	private:
		enum
		{
			MAX_ACTOR = 10000000,
		};
		
	private:
		void _ReArrangement();

	private:
		DsActor* m_actorVec[MAX_ACTOR];
		int m_totalActorNum;
	};
}

#endif