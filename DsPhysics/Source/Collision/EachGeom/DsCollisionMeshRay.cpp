#include "DsPhysicsPch.h"
#ifndef __DS_COLLISION_MESH_RAY__
#include "Collision/EachGeom/DsCollisionMeshRay.h"
#endif

#include "Actor/DsRigidBody.h"
#include "Collision/DsCollisionResult.h"
#include "Collision/DsCollisionDetection.h"
#include "Collision/DsCollisionContext.h"

using namespace DsPhysics;

DsCollisionResult& DsCollisionMeshRay::Colide()
{
	m_info.Clear();

	if (m_pMesh && m_pRay)
	{
		DsCD_LineFace(m_pRay, m_pMesh, m_info);
	}

	return m_info;
}