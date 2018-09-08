#pragma once

#include "DsPhysicsDefine.h"
#include "Actor/DsActorId.h"


namespace DsPhysics
{
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	class DsChrProxy
	{
	public:
		DsChrProxy(DsPhysicsWorld& world);
		virtual ~DsChrProxy();

	public:
		void Initialize(double r, double height, double mass, DsVec3d pos, DsMat33d rot);

		DsVec3d GetPosition() const { return m_pos; }
		void SetPosition(const DsVec3d& pos) { m_pos = pos; }

		DsAabb GetAabb()const;

		void SetUserData(void *pData);

		void SetCollisionFilter(DsCollisionFilter filter);

		void Drive( DsVec3d move);

	private:
		DsPhysicsWorld& m_world;
		DsActorId m_actorId;
		DsVec3d m_pos;
	};
}