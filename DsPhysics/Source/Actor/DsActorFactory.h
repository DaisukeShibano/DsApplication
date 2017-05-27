#ifndef __DS_ACTOR_FACTORY__
#define __DS_ACTOR_FACTORY__

namespace DsPhysics
{
	class DsActor;
	class DsActorId;
}

namespace DsPhysics
{
	class DsActorFactory
	{
	public:
		virtual DsActor* CreateIns( const DsActorId& id ) const = 0;
	};
}
#endif