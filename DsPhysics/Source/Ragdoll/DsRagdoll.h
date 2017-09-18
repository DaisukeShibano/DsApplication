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
	class DsHinge2Joint;
	class DsPhysicsWorld;
	typedef unsigned long long DsCollisionFilter;
}

namespace DsPhysics
{
	struct DsRagdollParts
	{
		const int skeletonBoneIdx;//このアクターに紐づく剛体
		const int ragdollParamId;
		const int innerPartsIdx;
		double damperV;
		double damperA;
		double mass;
		DsCollisionFilter collisionFilter;
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
		DsRagdoll(const std::vector<DsLib::DsAnimRagdollParamId>& ragdollParamIds, DsAnimSkeleton& skeleton, DsPhysicsWorld& world, void* pUserData, DsVec3d pos, DsMat33d rot);
		virtual ~DsRagdoll();

	public:
		const std::vector<DsRagdollParts>& RefParts() const { return m_parts; }
		std::vector<DsRagdollParts>& RefParts() { return m_parts; }

		void SetParam(const DsRagdollParts& parts);

		//リジッドをboneに合わせる
		void FixToKeyframeAnim(double dt, const std::vector<DsAnimBone*>& bones, const DsRagdollParts& parts, DsVec3d worldPos, DsMat33d worldRot);

		//boneをリジッドに合わせる
		void FixToPhysics(double dt, std::vector<DsAnimBone*>& bones, const DsRagdollParts& parts, DsVec3d worldPos, DsMat33d worldRot);

	private:
		void _ConstractRagdoll(const DsAnimBone* pBone, DsActor* pParentpActor, const DsVec3d attachPos, const std::map<int, DsAnimRagdollParamId>& params, void* pUserData, DsVec3d initPos, DsMat33d initRot);

	private:
		DsAnimSkeleton& m_animSkeleton;
		DsPhysicsWorld& m_world;
		std::vector<DsRagdollParts> m_parts;
		std::vector<InnerPartsInfo> m_innerParts;
		std::vector<DsHinge2Joint*> m_joints;
	};
}

#endif