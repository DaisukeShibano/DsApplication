#ifndef _DS_HINGE2_JOINT_
#define _DS_HINGE2_JOINT_

#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif

#ifndef _DS_JOINT_
#include "Joint/DsJoint.h"
#endif

namespace DsPhysics
{
	class DsPhysicsWorld;
	class DsHinge2Constraint;
}

namespace DsPhysics
{
	class DsHinge2Joint : public DsJoint
	{
	public:
		DsHinge2Joint(DsPhysicsWorld& world);
		virtual ~DsHinge2Joint();

		/*
		axis1,axis2 âÒì]é≤ÅBact1Ç©ÇÁÇÃëäëŒç¿ïW
		*/
		void AttachJoint(const DsActorId act1, const DsActorId act2, const DsVec3d& pos, const DsVec3d& axis1, const DsVec3d& axis2);
		void DetachJoint();

	public:
		virtual void Update(double dt) override;

	private:
		DsActorId m_masterId;
		DsActorId m_subId;
		DsHinge2Constraint* m_pConstarint;
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


	class DsHinge2JointFactory : public DsJointFactory
	{
	public:
		DsHinge2JointFactory(DsPhysicsWorld& world) : m_world(world) {}
		virtual DsJoint* CreateIns() override
		{
			return new DsHinge2Joint(m_world);
		}
	private:
		DsPhysicsWorld& m_world;
	};
}


#endif