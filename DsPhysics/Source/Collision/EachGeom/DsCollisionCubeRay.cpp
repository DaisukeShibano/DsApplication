#include "DsPhysicsPch.h"
#ifndef __DS_COLLISION_CUBE_RAY__
#include "Collision/EachGeom/DsCollisionCubeRay.h"
#endif

#include "Actor/DsRigidCube.h"
#include "Collision/DsCollisionResult.h"
#include "Collision/DsCollisionDetection.h"
#include "Collision/DsCollisionGeometry.h"
#ifndef __DS_COLLISION_CALLBACK__
#include "Collision/DsCollisionCallback.h"
#endif
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif

using namespace DsPhysics;

DsCollisionResult& DsCollisionCubeRay::Collide()
{
	m_info.Clear();

	if( m_pCube && m_pRay )
	{
		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*m_pRay->RefOwnerId().GetActor(), *m_pCube->RefOwnerId().GetActor())) {
				return m_info;
			}
		}

		//DsCD_LineFace(m_pRay->GetOwnerId(), m_pRay->GetVertex(), m_pRay->GetLine(), m_pRay->GetLineNum(), 
		//			  m_pCube->GetOwnerId(), m_pCube->GetVertex(), m_pCube->GetFace(), m_pCube->GetFaceNum(), m_info);
		DsCD_LineFace(m_pRay, m_pCube, m_info);
	}
	return m_info;
}