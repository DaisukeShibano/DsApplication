#ifndef _DS_RAGDOLL_
#define _DS_RAGDOLL_

namespace DsLib
{
	struct DsAnimRagdollParamId;
	class DsAnimSkeleton;
	struct DsAnimBone;
}

namespace DsPhysics
{
	class DsActor;
	class DsBallJoint;
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	struct DsRagdollParts
	{
		DsActor* pActor;
		int skeletonBoneIdx;
		int ragdollParamId;
	};

	class DsRagdoll
	{
	public:
		DsRagdoll(const std::vector<DsLib::DsAnimRagdollParamId>& ragdollParamIds, DsAnimSkeleton& skeleton, DsPhysicsWorld& world);
		virtual ~DsRagdoll();

	public:
		const std::vector<DsRagdollParts>& RefParts() const { return m_parts; }
		std::vector<DsRagdollParts>& RefParts() { return m_parts; }

	private:
		void _ConstractRagdoll(const DsAnimBone* pBone, DsActor* pParentpActor, const DsVec3d attachPos, const std::map<int, DsAnimRagdollParamId>& params);

	private:
		DsAnimSkeleton& m_animSkeleton;
		DsPhysicsWorld& m_world;
		std::vector<DsRagdollParts> m_parts;
		std::vector<DsBallJoint*> m_joints;
	};
}

#endif