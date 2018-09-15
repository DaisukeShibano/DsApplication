#pragma once

namespace DsLib
{
	enum class DS_MAP_OBJ_TYPE : char;
}

namespace DsLib
{
	class DsHitRes;
}

namespace DsPhysics
{
	class DsActorId;
	class DsPhysicsWorld;
}

namespace DsApp
{
	class DsPhysicsCreator
	{
	public:
		static DsPhysics::DsActorId Create(DsPhysics::DsPhysicsWorld& world, const DsLib::DsHitRes& hitRes, DS_MAP_OBJ_TYPE type, const DsVec3d& pos, const DsVec3d& ang, const char* pInsName);

	};

}