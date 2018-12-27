#include "DsPch.h"
#include <gl/GL.h>
#ifndef _DS_LIGHT_H_
#include "Graphics/Light/DsLight.h"
#endif
using namespace DsLib;

namespace
{
	static GLenum s_glLightNo[] =
	{
		GL_LIGHT0,
		GL_LIGHT1,
		GL_LIGHT2,
		GL_LIGHT3, 
		GL_LIGHT4,
		GL_LIGHT5,
		GL_LIGHT6,
		GL_LIGHT7,
	};
}


DsLight::DsLight(unsigned int lightNo)
:m_pos(DsVec3f(0,0,0))
,m_dir(DsVec3f(0,0,0))
,m_lightNo(lightNo)
{
	DS_ASSERT((lightNo < (sizeof(s_glLightNo) / sizeof(s_glLightNo[0]))), "ƒ‰ƒCƒg”‚ð’´‚¦‚Ä‚¢‚Ü‚·");
}

DsLight::~DsLight()
{
}

void DsLight::Enable()
{
	const GLfloat diff[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat spec[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const GLfloat ambi[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(s_glLightNo[m_lightNo], GL_DIFFUSE, diff);
	glLightfv(s_glLightNo[m_lightNo], GL_SPECULAR, spec);
	glLightfv(s_glLightNo[m_lightNo], GL_AMBIENT, ambi);
	glEnable(s_glLightNo[m_lightNo]);
}

void DsLight::Disable()
{
	glDisable(s_glLightNo[m_lightNo]);
}

//virtual
void DsLight::SetPos(const DsVec3f& pos)
{
	m_pos = pos;
	ApplyPos(pos.x, pos.y, pos.z, 1.0f);
}

//virtual 
void DsLight::SetDir(const DsVec3f& dir)
{
	m_dir = DsVec3f::Normalize(dir);
	ApplyDir(dir.x, dir.y, dir.z, 1.0f);
}

void DsLight::ApplyPos(float x, float y, float z, float w)
{
	const GLfloat light_pos[4] = { x, y, z, w };
	glLightfv(s_glLightNo[m_lightNo], GL_POSITION, light_pos);
}

void DsLight::ApplyDir(float x, float y, float z, float w)
{
	const GLfloat light_dir[4] = { x, y, z, w };
	glLightfv(s_glLightNo[m_lightNo], GL_SPOT_DIRECTION, light_dir);
}