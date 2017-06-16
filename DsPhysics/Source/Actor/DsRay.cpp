#include "DsPhysicsPch.h"
#ifndef __DS_RAY__
#include "Actor/DsRay.h"
#endif
using namespace DsPhysics;


DsRay::DsRay( const DsActorId& id )
: DsActor(id)
, m_line()
, m_pCollisionGeometry(NULL)
{
	m_vertex[0] = DsVec3d::Zero();
	m_vertex[1] = DsVec3d::Zero();
	m_line.index[0]=0;
	m_line.index[1]=1;
}

DsRay::~DsRay()
{
	delete m_pCollisionGeometry; m_pCollisionGeometry = NULL;
}

void DsRay::Update()
{

}

void DsRay::Create( const DsVec3d& start, const DsVec3d& end )
{
	m_vertex[0] = start;
	m_vertex[1] = end;
	m_line.index[0]=0;
	m_line.index[1]=1;
	m_pCollisionGeometry = new DsCollisionGeometry(m_vertex, GetVertexNum(), NULL, 0, &m_line, 1, GetId(), m_vertex[0], m_vertex, DsVec3d::Zero(), NULL, NULL, GetRotation());
}