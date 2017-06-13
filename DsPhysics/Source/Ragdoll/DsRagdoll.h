#ifndef _DS_RAGDOLL_
#define _DS_RAGDOLL_

namespace DsLib
{
	struct DsAnimCustomProperty;
	class DsAnimSkeleton;
}

namespace DsPhysics
{
	class DsActor;
	class DsBallJoint;
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	class DsRagdoll
	{
	public:
		DsRagdoll(const DsAnimCustomProperty& ragdollInfo, DsAnimSkeleton& skeleton, DsPhysicsWorld& world);
		virtual ~DsRagdoll();

	private:
		DsAnimSkeleton& m_animSkeleton;
		DsPhysicsWorld& m_world;
		std::vector<DsActor*> m_actors;
		std::vector<DsBallJoint*> m_joints;
	};
}

#endif