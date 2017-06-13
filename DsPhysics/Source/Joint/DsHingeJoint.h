#ifndef _DS_HINGE_JOINT_
#define _DS_HINGE_JOINT_

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif

#ifndef _DS_JOINT_
#include "Joint/DsJoint.h"
#endif

namespace DsPhysics
{
	class DsPhysicsWorld;
	class DsHingeConstraint;
}

namespace DsPhysics
{
	class DsHingeJoint : public DsJoint
	{
	public:
		DsHingeJoint(DsPhysicsWorld& world);
		virtual ~DsHingeJoint();

		//ç≈èâÇÃàÍâÒÇæÇØÇ‚ÇÍÇŒÇ¢Ç¢
		void AttachJoint(const DsActorId act1, const DsActorId act2, const DsVec3d& pos, const DsVec3d& axis);
		void DetachJoint();

	public:
		virtual void Update(double dt) override;

	private:
		DsActorId m_masterId;
		DsActorId m_subId;
		DsHingeConstraint* m_pConstarint;
		DsVec3d m_distance;
		DsVec3d m_distanceSub;
		DsMat33d m_actorRot;
		DsMat33d m_subActorRot;
		DsMat33d m_cRot;
		DsMat33d m_cRotSub;
		DsMat33d m_initDiffRot;
		bool m_isAttach;

	public:
		virtual void DbgDraw(DsDrawCommand& com) const override;

	};


	class DsHingeJointFactory : public DsJointFactory
	{
	public:
		DsHingeJointFactory(DsPhysicsWorld& world) : m_world(world) {}
		virtual DsJoint* CreateIns() override
		{
			return new DsHingeJoint(m_world);
		}
	private:
		DsPhysicsWorld& m_world;
	};
}


#endif