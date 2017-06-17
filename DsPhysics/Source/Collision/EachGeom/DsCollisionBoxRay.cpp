#include "DsPhysicsPch.h"
#ifndef __DS_COLLISION_BOX_RAY__
#include "Collision/EachGeom/DsCollisionBoxRay.h"
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

DsCollisionResult& DsCollisionBoxRay::Collide()
{
	m_info.Clear();

	if( m_pBox && m_pRay )
	{
		if (m_world.GetCollisionCallback()) {
			if (!m_world.GetCollisionCallback()->IsCollide(*m_pRay->RefOwnerId().GetActor(), *m_pBox->RefOwnerId().GetActor())) {
				return m_info;
			}
		}

		//DsCD_LineFace(m_pRay->GetOwnerId(), m_pRay->GetVertex(), m_pRay->GetLine(), m_pRay->GetLineNum(), 
		//			  m_pBox->GetOwnerId(), m_pBox->GetVertex(), m_pBox->GetFace(), m_pBox->GetFaceNum(), m_info);
		DsCD_LineFace(m_pRay, m_pBox, m_info);
	}
	return m_info;
}