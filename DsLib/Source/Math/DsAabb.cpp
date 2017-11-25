#include "DsPch.h"
#ifndef _DS_AABB_H_
#include "Math/DsAabb.h"
#endif

using namespace DsLib;

DsAabb::DsAabb()
	: m_maxX(0)
	, m_maxY(0)
	, m_maxZ(0)
{
}

DsAabb::~DsAabb()
{

}

void DsAabb::Setup(double maxX, double maxY, double maxZ, double minX, double minY, double minZ)
{
	m_maxX=maxX;
	m_maxY=maxY;
	m_maxZ=maxZ;
	m_minX = minX;
	m_minY = minY;
	m_minZ = minZ;
}

void DsAabb::Setup(const DsVec3d& max, const DsVec3d& min)
{
	m_maxX = max.x;
	m_maxY = max.y;
	m_maxZ = max.z;
	m_minX = min.x;
	m_minY = min.y;
	m_minZ = min.z;
}

void DsAabb::Draw(DsDrawCommand& com) const
{
	const DsVec3d v[8] =
	{
		//è„ñ 
		DsVec3d(m_maxX, m_maxY, m_maxZ),
		DsVec3d(m_maxX, m_maxY, m_minZ),
		DsVec3d(m_minX, m_maxY, m_minZ),
		DsVec3d(m_minX, m_maxY, m_maxZ),
		//â∫ñ 
		DsVec3d(m_maxX, m_minY, m_maxZ),
		DsVec3d(m_maxX, m_minY, m_minZ),
		DsVec3d(m_minX, m_minY, m_minZ),
		DsVec3d(m_minX, m_minY, m_maxZ),
	};

	for (int i = 0; i < 4; ++i)
	{
		const int next = (i+1) % 4;
		com.DrawLine(v[i], v[next]);
		com.DrawLine(v[i+4], v[next+4]);
		com.DrawLine(v[i], v[i+4]);
	}
}