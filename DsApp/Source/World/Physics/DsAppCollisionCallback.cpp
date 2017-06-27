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

//�����邩�ǂ�����Ԃ�
//virtual
bool DsAppCollisionCallback::IsCollide(const DsActor& a1, const DsActor& a2) const
{
	//�����I�[�i�[�͓�����Ȃ�
	if (a1.GetUserData() != NULL) {
		if (a1.GetUserData() == a2.GetUserData()) {
			//�������������O���[�v�Ȃ瓖����Ȃ�
			return DsAppCollisionFilter::IsInsideGroupCollision(a1.GetCollisionFilter(), a2.GetCollisionFilter());
		}
	}
	else {
		//���������O���[�v�Ȃ瓖����Ȃ�
		return DsAppCollisionFilter::IsGroupCollision(a1.GetCollisionFilter(), a2.GetCollisionFilter());
	}
	return true;
}
	