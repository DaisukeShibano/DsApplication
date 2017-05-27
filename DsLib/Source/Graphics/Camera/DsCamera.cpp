#include "DsPch.h"
#ifndef _DS_CAMERA_H_
#include "Graphics/Camera/DsCamera.h"
#endif

#include "Math/DsInverseMatrix.h"
#include "Mouse/DsMouse.h"

using namespace DsLib;

DsCamera::DsCamera()
	: m_aspect(0.0f)
	, m_zoom(0.0f)
	, m_pos(DsVec3f::Zero())
{
	for(int row=0; row<4; ++row)
	{
		for(int col=0; col<4; ++col)
		{
			if( col == row )
			{
				m_mat[col*4 + row] = 1.0f;
				m_matInv[col*4 + row] = 1.0f;
			}
			else
			{
				m_mat[col*4 + row] = 0.0f;
				m_matInv[col*4 + row] = 0.0f;
			}
		}
	}
}

DsCamera::~DsCamera()
{
}

void DsCamera::Update( const double dt)
{
	DsMat33d mat = DsMat33d::ToMat33(m_mat);
	const DsVec3d camPos = mat*DsVec3d::ToVec3(0.0, 0.0, m_zoom);
	DsMat33d::ToMat16(mat, m_mat);
	m_mat[3] = m_pos.x + camPos.x;
	m_mat[7] = m_pos.y + camPos.y;
	m_mat[11] = m_pos.z + camPos.z;
	DsInverseMatrix<double>(m_mat, m_matInv, 4);
	
}

void DsCamera::SetPos(const DsVec3d& pos)
{
	m_pos = pos;
	m_mat[3] = pos.x;
	m_mat[7] = pos.y;
	m_mat[11] = pos.z;
}

void DsCamera::SetRot(const DsMat33d& rot)
{
	DsMat33d::ToMat16(rot, m_mat);
}

const double* DsCamera::GetCamMat16() const
{
	return m_mat;
}

const double *DsCamera::GetCamMatInv16() const
{
	return m_matInv;
}

double DsCamera::GetNear() const
{
	return 1.0f;
}

double DsCamera::GetFar() const
{
	return 1000.0f;
}

/*
	@return ‰æŠp[“x]
*/
double DsCamera::GetFovy() const
{
	return 50.0f;
}