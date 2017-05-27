#include "DsPhysicsPch.h"
#include "PhysicsUtil/DsPhysicsUtil.h"
#include "DsPhysicsWorld.h"
#include "Actor/DsActor.h"

using namespace DsPhysics;
using namespace DsPhysicsUtil;


DsVec3d DsPhysicsUtil::CalcForceTargetPos( const DsVec3d& pos, const DsPhysicsWorld& world, const DsActorId& id, const DsVec3d& offset )
{
	const double K = 0.6;
	const double C = 0.8;
	DsActor* pActor = world.GetActor(id);
	if( pActor )
	{
		const DsVec3d diff = pos - (pActor->GetPosition()+offset);
		const DsVec3d vel = pActor->GetVelocity();
		const DsVec3d force = diff*K - vel*C;
		return force;
	}
	return DsVec3d::Zero();
}