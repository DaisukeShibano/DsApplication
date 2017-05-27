#include "DsPhysicsPch.h"
#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif
#ifndef __DS_MASS__
#include "DsMass.h"
#endif

using namespace DsPhysics;

namespace
{
	static DsVec3d s_zero = DsVec3d::Zero();
	static DsMass s_unit = DsMass::Unit();
	static DsMat33d s_identity = DsMat33d::Identity();
	static DsAabb s_aabb = DsAabb();
}

const DsVec3d& DsActor::GetPosition()const
{ 
	return s_zero;
}
			   
const DsVec3d& DsActor::GetVelocity()const
{ 
	return s_zero;
}
			   
const DsVec3d& DsActor::GetForce()const
{ 
	return s_zero; 
}

const DsMat33d& DsActor::GetRotation()const
{
	return s_identity;
}
			   
const DsVec3d& DsActor::GetAngularVel()const
{
	return s_zero;
}
			   
const DsVec3d& DsActor::GetTorque()const
{
	return s_zero;
}

const DsMass& DsActor::GetMass()const
{
	return s_unit;
}

const double* DsActor::GetMassInv10()const
{
	static double identity[10]={
		1,
		   0, 0, 0,
		   1, 0, 0,
		   0, 1, 0,
	};
	return identity;
}

const DsAabb& DsActor::RefAabb() const
{
	return s_aabb;
}