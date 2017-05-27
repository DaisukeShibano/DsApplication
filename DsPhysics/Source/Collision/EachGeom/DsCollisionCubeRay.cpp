#include "DsPhysicsPch.h"
#ifndef __DS_COLLISION_CUBE_RAY__
#include "Collision/EachGeom/DsCollisionCubeRay.h"
#endif

#include "Actor/DsRigidBody.h"
#include "Collision/DsCollisionResult.h"
#include "Collision/DsCollisionDetection.h"
#include "Collision/DsCollisionContext.h"

using namespace DsPhysics;

DsCollisionResult& DsCollisionCubeRay::Colide()
{
	m_info.Clear();

	if( m_pCube && m_pRay )
	{
		//DsCD_LineFace(m_pRay->GetOwnerId(), m_pRay->GetVertex(), m_pRay->GetLine(), m_pRay->GetLineNum(), 
		//			  m_pCube->GetOwnerId(), m_pCube->GetVertex(), m_pCube->GetFace(), m_pCube->GetFaceNum(), m_info);
		DsCD_LineFace(m_pRay, m_pCube, m_info);
	}
	return m_info;
}