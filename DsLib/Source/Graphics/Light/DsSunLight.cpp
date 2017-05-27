#include "DsPch.h"
#include <gl/GL.h>
#include"Graphics/Light/DsSunLight.h"

using namespace DsLib;

DsSunLight::DsSunLight(unsigned int lightNo)
:DsLight(lightNo)
{
}

DsSunLight::~DsSunLight()
{

}

//virtual
void DsSunLight::SetPos(const DsVec3f& pos)
{
	m_pos = pos;
	ApplyPos(pos.x, pos.y, pos.z, 0.0f);
}

//virtual
void DsSunLight::SetDir(const DsVec3f& dir)
{
	m_dir = DsVec3f::Normalize(dir);
	ApplyDir(dir.x, dir.y, dir.z, 1.0f);
}

void DsSunLight::Update()
{
	SetPos(DsVec3f(10, 10, 10));
	SetDir(DsVec3f(-1, -1, -1));
}