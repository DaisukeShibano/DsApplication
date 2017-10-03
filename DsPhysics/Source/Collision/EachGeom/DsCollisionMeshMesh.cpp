#include "DsPhysicsPch.h"

#ifndef _DS_COLLISION_MESH_MESH_H_
#include "Collision/EachGeom/DsCollisionMeshMesh.h"
#endif
#ifndef __DS_COLLISION_CALLBACK__
#include "Collision/DsCollisionCallback.h"
#endif
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif

using namespace DsPhysics;



DsCollisionResult& DsCollisionMeshMesh::Collide()
{
	m_info.Clear();
	return m_info;
}
