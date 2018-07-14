#include "DsPhysicsPch.h"
#ifndef _DS_COLLISION_BOX_CAPSULE_
#include "Collision/EachGeom/DsCollisionBoxCapsule.h"
#endif

using namespace DsLib;
using namespace DsPhysics;

//virtual 
DsCollisionResult& DsCollisionBoxCapsule::Collide()// override
{
	return m_info;
}