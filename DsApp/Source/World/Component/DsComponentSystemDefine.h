#ifndef _DS_COMPONENT_SYSTEM_DEFINE_
#define _DS_COMPONENT_SYSTEM_DEFINE_

/*
#ifndef _DS_COMPONENT_SYSTEM_DEFINE_
#include "World\/Component/DsComponentSystemDefine.h"
#endif
*/
namespace DsApp
{
	class DsFieldObj;
}

namespace DsApp
{
	struct COMPONENT_UPDATE_ARG
	{
		COMPONENT_UPDATE_ARG(double _dt, DsFieldObj& _owner, DsLib::DsSys& _sys, DsPhysics::DsPhysicsWorld& _physWorld, DsGameSys* _pGameSys)
			: dt(_dt)
			, owner(_owner)
			, sys(_sys)
			, physWorld(_physWorld)
			, pGameSys(_pGameSys)
		{}
		double dt;
		DsFieldObj& owner;
		DsLib::DsSys& sys;
		DsPhysics::DsPhysicsWorld& physWorld;
		DsGameSys* pGameSys;
	};

}


#endif
