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
	class DsJoint;
	class DsBallJoint;
	class DsPhysicsWorld;
}

namespace DsPhysics
{
	struct DsRagdollParts
	{
		int skeletonBoneIdx;//このアクターに紐づく剛体
		int ragdollParamId;
		int innerPartsIdx;
	};

	class DsRagdoll
	{
	private:
		struct InnerPartsInfo
		{
			DsActor* pActor;
			DsVec3d offset;//親(この剛体)と子をつなぐ中間の親からのオフセット
			DsJoint* pJoint;
			DsVec3d jointAttachPos;
		};

	public:
		DsRagdoll(const std::vector<DsLib::DsAnimRagdollParamId>& ragdollParamIds, DsAnimSkeleton& skeleton, DsPhysicsWorld& world, void* pUserData);
		virtual ~DsRagdoll();

	public:
		const std::vector<DsRagdollParts>& RefParts() const { return m_parts; }
		std::vector<DsRagdollParts>& RefParts() { return m_parts; }

		//リジッドをboneに合わせる
		void FixToKeyframeAnim(const std::vector<DsAnimBone*>& bones, const DsRagdollParts& parts);

		//boneをリジッドに合わせる
		void FixToPhysics(std::vector<DsAnimBone*>& bones, const DsRagdollParts& parts);

	private:
		void _ConstractRagdoll(const DsAnimBone* pBone, DsActor* pParentpActor, const DsVec3d attachPos, const std::map<int, DsAnimRagdollParamId>& params, void* pUserData);

	private:
		DsAnimSkeleton& m_animSkeleton;
		DsPhysicsWorld& m_world;
		std::vector<DsRagdollParts> m_parts;
		std::vector<InnerPartsInfo> m_innerParts;
		std::vector<DsBallJoint*> m_joints;
	};
}

#endif