#include "DsPhysicsPch.h"
#ifndef __DS_RAY__
#include "Actor/DsRay.h"
#endif
using namespace DsPhysics;


DsRay::DsRay( const DsActorId& id )
: DsActor(id)
 , m_line()
 , m_aabb()
 , m_pCollisionGeometry(NULL)
{
	m_vertex[0] = DsVec3d::Zero();
	m_vertex[1] = DsVec3d::Zero();
	m_line.index[0]=0;
	m_line.index[1]=1;
}

DsRay::DsRay()
: DsActor(DsActorId())
, m_line()
, m_aabb()
, m_pCollisionGeometry(NULL)
{
	m_vertex[0] = DsVec3d::Zero();
	m_vertex[1] = DsVec3d::Zero();
	m_line.index[0] = 0;
	m_line.index[1] = 1;
}

DsRay::~DsRay()
{
	delete m_pCollisionGeometry; m_pCollisionGeometry = NULL;
}

//virtual
void DsRay::Update()
{

}

//virtual 
const DsVec3d& DsRay::GetPosition() const
{
	return m_aabb.GetPos();
}

void DsRay::Create( const DsVec3d& start, const DsVec3d& end )
{
	m_vertex[0] = start;
	m_vertex[1] = end;
	m_line.index[0]=0;
	m_line.index[1]=1;

	const DsVec3d center = (start + end)*0.5;
	//AABB
	DsVec3d maxLen = DsVec3d::Zero();
	for (int i = 0; i < 2; ++i)
	{
		const DsVec3d len = m_vertex[i] - center;
		maxLen.x = max(maxLen.x, fabs(len.x));
		maxLen.y = max(maxLen.y, fabs(len.y));
		maxLen.z = max(maxLen.z, fabs(len.z));
	}
	m_aabb.Setup(maxLen.x, maxLen.y, maxLen.z, center);
	m_pCollisionGeometry = new DsCollisionGeometry(m_vertex, GetVertexNum(), NULL, 0, &m_line, 1, GetId(), m_vertex[0], m_vertex, DsVec3d::Zero(), NULL, &m_aabb, GetRotation());

}