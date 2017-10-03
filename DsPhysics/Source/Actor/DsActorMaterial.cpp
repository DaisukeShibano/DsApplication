#include "DsPhysicsPch.h"
#include "Actor/DsActorMaterial.h"

using namespace DsPhysics;

namespace
{
	static DsActorMaterial s_null;
}

//static 
const DsActorMaterial& DsPhysics::DsActorMaterial::Null()
{
	s_null.m_bundCoef = 1.0;
	return s_null;
}

//static 
const DsActorMaterial DsPhysics::DsActorMaterial::Aluminum()
{
	DsActorMaterial ret;
	ret.m_bundCoef = 0.6;//ƒAƒ‹ƒ~‚ª‚±‚ê‚­‚ç‚¢‚ç‚µ‚¢
	ret.m_staticFrictionCoef = 0.85;
	ret.m_kinematicFricCoef = 0.80;
	return ret;
}