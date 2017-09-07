#include "DsPhysicsPch.h"
#ifndef __DS_COLLISION_MESH_RAY__
#include "Collision/EachGeom/DsCollisionMeshRay.h"
#endif

#include "Actor/DsRigidBox.h"
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

DsCollisionResult& DsCollisionMeshRay::Collide()
{
	m_info.Clear();

	if (m_pMesh && m_pRay)
	{
		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*m_pMesh->RefOwnerId().GetActor(), *m_pRay->RefOwnerId().GetActor())) {
				return m_info;
			}
		}

		DsCD_LineFace(m_pRay, m_pMesh, m_info);
	}

	return m_info;
}