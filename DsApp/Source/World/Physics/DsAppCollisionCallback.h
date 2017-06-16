#ifndef __DS_APP_COLLISION_CALLBACK__
#define __DS_APP_COLLISION_CALLBACK__

#ifndef __DS_COLLISION_CALLBACK__
#include "Collision/DsCollisionCallback.h"
#endif

namespace DsPhysics
{
	class DsActor;
}

namespace DsApp
{
	class DsAppCollisionCallback : public DsPhysics::DsCollisionCallback
	{
	public:
		//“–‚½‚é‚©‚Ç‚¤‚©‚ð•Ô‚·
		virtual bool IsCollide(const DsPhysics::DsActor& a1, const DsPhysics::DsActor& a2) const override;
	};
}

#endif