#include "DsAppPch.h"
#ifndef __DS_APP_COLLISION_CALLBACK__
#include "World/Physics/DsAppCollisionCallback.h"
#endif

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif

using namespace DsPhysics;
using namespace DsApp;

//�����邩�ǂ�����Ԃ�
//virtual
bool DsAppCollisionCallback::IsCollide(const DsActor& a1, const DsActor& a2) const
{
	//�����I�[�i�[�͓�����Ȃ�
	if (a1.GetUserData() == a2.GetUserData()) {
		return false;
	}
	else {
		return true;
	}
}
	