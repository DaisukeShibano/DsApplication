#include "DsPch.h"
#ifndef _DS_AABB_H_
#include "Math/DsAabb.h"
#endif

using namespace DsLib;

DsAabb::DsAabb()
	: m_maxX(0)
	, m_maxY(0)
	, m_maxZ(0)
	, m_maxLen(0)
	, m_pos(DsVec3d::Zero())
{
}

DsAabb::~DsAabb()
{

}

void DsAabb::Setup(double maxX, double maxY, double maxZ, const DsVec3d& pos)
{
	m_maxX=maxX;
	m_maxY=maxY;
	m_maxZ=maxZ;
	m_pos=pos;
	m_maxLen = max(maxX, max(maxY,maxZ));
}

void DsAabb::Draw(DsDrawCommand& com) const
{
	const DsVec3d v[8] =
	{
		//è„ñ 
		DsVec3d(m_maxX, m_maxY, m_maxZ) + m_pos,
		DsVec3d(m_maxX, m_maxY, -m_maxZ) + m_pos,
		DsVec3d(-m_maxX, m_maxY, -m_maxZ) + m_pos,
		DsVec3d(-m_maxX, m_maxY, m_maxZ) + m_pos,
		//â∫ñ 
		DsVec3d(m_maxX, -m_maxY, m_maxZ) + m_pos,
		DsVec3d(m_maxX, -m_maxY, -m_maxZ) + m_pos,
		DsVec3d(-m_maxX, -m_maxY, -m_maxZ) + m_pos,
		DsVec3d(-m_maxX, -m_maxY, m_maxZ) + m_pos,
	};

	for (int i = 0; i < 4; ++i)
	{
		const int next = (i+1) % 4;
		com.DrawLine(v[i], v[next]);
		com.DrawLine(v[i+4], v[next+4]);
		com.DrawLine(v[i], v[i+4]);
	}
}