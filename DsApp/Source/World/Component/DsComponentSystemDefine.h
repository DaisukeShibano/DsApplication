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
	class DsComponentSystem;
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

	struct COMPONENT_UPDATE_RESULT
	{
		struct DAMAGE
		{
			DAMAGE()
				: hitPos()
				, hitDir()
				, hitEffectId(-1)
				, toAtk()
				, kLenDef(0)
				, kLenAtk(0)
				, kTime(0)
				, pAtk(NULL)
				, hitStopTime(0)
			{}

			DsVec3d hitPos;
			DsVec3d hitDir;
			int hitEffectId;

			DsVec3d toAtk;
			double kLenDef;
			double kLenAtk;
			double kTime;
			DsComponentSystem* pAtk;

			double hitStopTime;
		};

		COMPONENT_UPDATE_RESULT()
			: damage()
			{}

		std::vector<DAMAGE> damage;
	};

}


#endif
