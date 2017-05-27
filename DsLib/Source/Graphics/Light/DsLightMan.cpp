#include "DsPch.h"
#include <gl/GL.h>
#ifndef _DS_LIGHT_MAN_H_
#include "Graphics/Light/DsLightMan.h"
#endif
#include "Graphics/Light/DsLight.h"
#include "Graphics/Light/DsSunLight.h"

using namespace DsLib;


namespace
{
	static DsSunLight s_sunLight = DsSunLight(0);
	static DsLight s_lights[] =
	{
		DsLight(1),
		DsLight(2),
		DsLight(3),
		DsLight(4),
		DsLight(5),
		DsLight(6),
		DsLight(7),
	};
}

DsLightMan::DsLightMan()
{
}

DsLightMan::~DsLightMan()
{
}

DsLight& DsLightMan::GetLight(unsigned int lightNo)
{
	DS_ASSERT((lightNo < (sizeof(s_lights)/sizeof(s_lights[0]) )), "ƒ‰ƒCƒg”‚ð’´‚¦‚Ä‚¢‚Ü‚·");
	return s_lights[lightNo];
}

DsLight& DsLightMan::GetSunLight()
{
	return s_sunLight;
}

void DsLightMan::Enable()
{
	glEnable(GL_LIGHTING);
}

void DsLightMan::Disable()
{
	glDisable(GL_LIGHTING);
}

void DsLightMan::Update()
{
	s_sunLight.Update();
}