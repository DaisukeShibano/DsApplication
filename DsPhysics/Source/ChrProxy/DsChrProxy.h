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
		DsChrProxy(DsPhysicsWorld& world, void* pOwner);
		virtual ~DsChrProxy();

	public:
		void Initialize(double r, double height, double mass, DsVec3d pos, DsMat33d rot);

		DsVec3d GetPosition() const { return m_pos; }
		void SetPosition(const DsVec3d& pos);

		DsAabb GetAabb()const;

		void SetCollisionFilter(DsCollisionFilter filter);

		void Drive( double dt, DsVec3d move);

	private:
		DsPhysicsWorld& m_world;
		DsActorId m_actorId;
		DsVec3d m_pos;
		void* m_pOwner;
		DsCollisionFilter m_filter;
		double m_height;
		double m_radius;
		bool m_isGround;
	};
}