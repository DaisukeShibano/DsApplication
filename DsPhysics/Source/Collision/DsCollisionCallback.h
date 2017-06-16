#ifndef __DS_COLLISION_CALLBACK__
#define __DS_COLLISION_CALLBACK__

/*
#ifndef __DS_COLLISION_CALLBACK__
#include "Collision/DsCollisionCallback.h"
#endif
*/
namespace DsPhysics
{
	class DsActor;
}

namespace DsPhysics
{
	class DsCollisionCallback
	{
	public:
		//“–‚½‚é‚©‚Ç‚¤‚©‚ð•Ô‚·
		virtual bool IsCollide( const DsActor& a1, const DsActor& a2 ) const { return true; }
	};
}

#endif