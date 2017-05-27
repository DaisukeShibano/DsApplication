#ifndef __DS_ACTOR_FIXER__
#define __DS_ACTOR_FIXER__

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif

namespace DsPhysics
{
}

namespace DsPhysics
{
	/*
		ï°êîÇÃActorÇçáëÃÇµÇƒå≈íËÇ∑ÇÈÉNÉâÉX
	*/
	class DsActorFixer
	{
	public:
		DsActorFixer()
			: m_actors()
			, m_position(DsVec3d::Zero())
			, m_velocity(DsVec3d::Zero())
			, m_force(DsVec3d::Zero())
		{};
		virtual ~DsActorFixer() {};

	public:
		void Update();
		void AddActor(DsActorId actor) { m_actors.push_back(actor); }

		const DsVec3d& GetPosition() const { return m_position; }
		const DsVec3d& GetVelocity() const { return m_velocity; }
		const DsVec3d& GetForce() const { return m_force; }
		
	private:
		std::vector<DsActorId> m_actors;
		DsVec3d m_position;
		DsVec3d m_velocity;
		DsVec3d m_force;

	};
}
#endif