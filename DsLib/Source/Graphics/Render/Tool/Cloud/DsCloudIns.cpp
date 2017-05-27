#include "DsPch.h"
#include <gl/GL.h>
#include "Graphics/Render/Tool/Cloud/DsCloudIns.h"

using namespace DsLib;

DsCloudIns::DsCloudIns( unsigned int texId)
: m_pos(DsVec3d::Zero())
, m_rot(DsMat33d::Identity())
, m_size(0)
, m_texId(texId)
, m_isCreate(false)
, m_lifeTimer(-1)
, m_alpha(0)
, m_speefOffset(static_cast<float>(rand() % 2000) / 8000000.0f)
{
}

DsCloudIns::~DsCloudIns()
{
}

void DsCloudIns::Initialize( const DsVec3d& pos )
{
	m_pos = pos;
	const float initRad = static_cast<float>(rand() % 628) / 100.0f;
	m_rot = DsMat33d::RotateZ(initRad);
	m_size = static_cast<float>(50 + rand()%200);
	m_isCreate = true;
	m_lifeTimer = 100;
	m_alpha = static_cast<float>(rand() % 500) / 5000.0f + 0.5f;
}

void DsCloudIns::Update(const float dt, const DsMat33d& camRot)
{
	const float size = m_size;
	const float dr = 0.001f*dt + m_speefOffset;
	m_pos = DsMat33d::RotateY(dr)*m_pos;
	const DsVec3d pos = m_pos;

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texId);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//下地を透明にするのに必要

	//zのマイナス方向を向くようにする
	DsVec3d p1(-size, -size, 0);
	DsVec3d p2(-size, size, 0);
	DsVec3d p3(size, size, 0);
	DsVec3d p4(size, -size, 0);

	DsVec2d t1(0.0f, 0.0f);				//0, 0
	DsVec2d t2(1, 0);		//1, 0
	DsVec2d t3(1, 1);		//1, 1
	DsVec2d t4(0, 1);			//0, 1

	p1 = camRot*m_rot*p1 + pos;
	p2 = camRot*m_rot*p2 + pos;
	p3 = camRot*m_rot*p3 + pos;
	p4 = camRot*m_rot*p4 + pos;
	//m_com.DrawTexQuad(p1, p2, p3, p4, t1, t2, t3, t4, m_texId);

	const DsVec3d normal = DsVec3d::CalcNormal(p1, p2, p3);
	glColor4f(1.0f, 1.0f, 1.0f, m_alpha);
	glNormal3dv(normal.v);
	glBegin(GL_QUADS);
	glTexCoord2dv(t1.v);  glVertex3dv(p1.v);
	glTexCoord2dv(t2.v);  glVertex3dv(p2.v);
	glTexCoord2dv(t3.v);  glVertex3dv(p3.v);
	glTexCoord2dv(t4.v);  glVertex3dv(p4.v);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	m_lifeTimer += dt;
}

bool DsCloudIns::IsDead() const
{
	return !m_isCreate;
}