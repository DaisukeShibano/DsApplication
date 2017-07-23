#include "DsAppPch.h"
#ifndef __DS_APP_COLLISION_CALLBACK__
#include "World/Physics/DsAppCollisionCallback.h"
#endif
#ifndef __DS_APP_COLLISION_FILTER__
#include "World/Physics/DsAppCollisionFilter.h"
#endif

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif

using namespace DsPhysics;
using namespace DsApp;

//当たるかどうかを返す
//virtual
bool DsAppCollisionCallback::IsCollide(const DsActor& a1, const DsActor& a2) const
{
	bool ret = true;

	if((a1.GetUserData() != NULL) && (a1.GetUserData() == a2.GetUserData()) ) {
		//同じ内部所属グループなら当たらない
		ret = DsAppCollisionFilter::IsInsideGroupCollision(a1.GetCollisionFilter(), a2.GetCollisionFilter());
	}
	else {
		//同じ所属グループなら当たらない
		ret = DsAppCollisionFilter::IsGroupCollision(a1.GetCollisionFilter(), a2.GetCollisionFilter());
	}
	return ret;
}
	