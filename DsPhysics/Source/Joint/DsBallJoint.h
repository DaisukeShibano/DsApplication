#ifndef _DS_BALL_JOINT_H_
#define _DS_BALL_JOINT_H_

#ifndef __DS_ACTOR__
#include "Joint/DsJoint.h"
#endif

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif

namespace DsPhysics
{
	class DsPhysicsWorld;
	class DsBallConstraint;
}

namespace DsPhysics
{
	class DsBallJoint : public DsJoint
	{
	public:
		DsBallJoint(DsPhysicsWorld& world);
		virtual ~DsBallJoint();

		//ç≈èâÇÃàÍâÒÇæÇØÇ‚ÇÍÇŒÇ¢Ç¢
		void AttachJoint(const DsActorId act1, const DsActorId act2, const DsVec3d& pos);
		void DetachJoint();

	public:
		virtual void Update(double dt) override;

	private:
		DsActorId m_masterId;
		DsActorId m_subId;
		DsBallConstraint* m_pConstarint;
		DsVec3d m_distance;
		DsVec3d m_distanceSub;
		DsMat33d m_rot;
		DsMat33d m_rotSub;
		bool m_isAttach;


	public:
		virtual void DbgDraw(DsDrawCommand& com) const override;
	};


	class DsBallJointFactory : public DsJointFactory
	{
	public:
		DsBallJointFactory(DsPhysicsWorld& world) : m_world(world) {}
		virtual DsJoint* CreateIns() override
		{
			return new DsBallJoint(m_world);
		}
	private:
		DsPhysicsWorld& m_world;
	};
}


#endif