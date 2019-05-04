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

	class COMPONENT_UPDATE_RESULT
	{
	public:
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
				, pDef(NULL)
				, hitStopTime(0)
			{}

			DsVec3d hitPos;
			DsVec3d hitDir;
			int hitEffectId;

			DsVec3d toAtk;
			double kLenDef;
			double kLenAtk;
			double kTime;
			DsComponentSystem* pDef;

			double hitStopTime;
		};

		COMPONENT_UPDATE_RESULT()
			: m_damage()
		{}

		void AddDamage(const DAMAGE& damage)
		{
			m_damage.push_back(damage);
		}

		void Clear()
		{
			m_damage.clear();
		}

		const std::vector<DAMAGE>& RefDamage() const { return m_damage; }

	private:
		std::vector<DAMAGE> m_damage;
	};

}


#endif
