#include "DsPhysicsPch.h"
#ifndef _DS_RAGDOLL_
#include "Ragdoll/DsRagdoll.h"
#endif

#ifndef _DS_AMIM_RAGDOLL_INFO_
#include "Animation/DsAnimCustomProperty.h"
#endif
#ifndef _DS_AMIM_SKELETON_
#include "Animation/DsAnimSkeleton.h"
#endif
#ifndef __DS_RIGID_CUBE__
#include "Actor/DsRigidCube.h"
#endif
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif
#ifndef _DS_BALL_JOINT_H_
#include "Joint/DsBallJoint.h"
#endif

using namespace DsLib;
using namespace DsPhysics;



DsRagdoll::DsRagdoll(const std::vector<DsAnimRagdollParamId>& ragdollParamIds, DsAnimSkeleton& skeleton, DsPhysicsWorld& world)
	: m_animSkeleton(skeleton)
	, m_world(world)
	, m_parts()
	, m_joints()
{
	//子→親 の長さで１つのリジッドを作る。終端のボーンはダミー。 最低でも2階層ないと成り立たない
	//ボーンの長さがとれれば1ボーン1リジッドでいけたが、取れなかったので。

	//アニメボーンのルートから辿り、ragdollParamIdsに自分を登録しているものがないか探す。親→子→孫 で 子がラグドール設定されていない場合、親→孫で拘束するから
	//リジッドの長さ、姿勢は、親→子でいい。

	//親がいないリジッドはどこかに固定しておく必要がある？
	const size_t ragNum = ragdollParamIds.size();
	m_parts.reserve(ragNum);
	m_joints.reserve(ragNum);

	std::map<int, DsAnimRagdollParamId> tmpParams;
	for (const DsAnimRagdollParamId& param : ragdollParamIds) {
		tmpParams[param.boneIndex] = param;
	}

	const std::vector<DsAnimBone*>& bones = skeleton.RefRootBone();
	for (const DsAnimBone* pBone : bones){
		_ConstractRagdoll(pBone, NULL, DsVec3d::Zero(), tmpParams);
	}
}

void DsRagdoll::_ConstractRagdoll(const DsAnimBone* pBone, DsActor* pParentpActor, const DsVec3d attachPos, const std::map<int, DsAnimRagdollParamId>& params)
{
	if (!pBone->child.empty()) {
		DsActor* pActor = pParentpActor;
		DsVec3d nextAttachPos = attachPos;
		//ボーンのインデックスを持ったラグドールIDを探す
		const int arrayIdx = pBone->arrayIdx;
		auto it = params.find(arrayIdx);
		if (params.end() != it) {
			const DsVec3d parentPos = pBone->initWorldPose.GetPos();
			const DsVec3d childPos = pBone->child[0]->initWorldPose.GetPos();//どの子供でも根っこは同じ座標、でしょ？
			const DsVec3d rigidPos = (parentPos + childPos)*0.5;
			const DsVec3d dist = childPos - parentPos;
			DsVec3d vertex[8];
			DsRigidCube::GetVertex(vertex, 0.1, 0.1, dist.Length());//太さは後でパラメータ化する
			DsRigidCube::DsRigidCubeFactory factory(vertex, 1.0, pBone->name.c_str());
			factory.InitPos(rigidPos);
			factory.InitRot(pBone->initWorldPose.ToMat33());
			factory.SetOption(DsActor::Option::Default());
			pActor = m_world.CreateActor(factory).GetActor();
			if (pActor) {
				pActor->SetMaterial(DsActorMaterial::Aluminum());
				DsRagdollParts parts;
				parts.pActor = pActor;
				parts.ragdollParamId = it->second.ragdollParamId;
				parts.skeletonBoneIdx = it->second.boneIndex;
				m_parts.push_back(parts);

				if (pParentpActor) {
					//一つ前の親剛体（アニメボーンは一つ前の親とは限らない）と今回の剛体をつなぐ
					DsBallJoint* pJoint = static_cast<DsBallJoint*>( m_world.CreateJoint(DsBallJointFactory(m_world)) );
					if (pJoint) {
						//pJoint->AttachJoint(pParentpActor->GetId(), pActor->GetId(), attachPos);
						//m_joints.push_back(pJoint);
					}
				}
				nextAttachPos = childPos;//次のリジッドとのアタッチ位置更新
			}
		}
		for (const DsAnimBone* pChild : pBone->child) {
			_ConstractRagdoll(pChild, pActor, nextAttachPos, params);
		}
	}
	else {
		//ボーン１本で長さを取る手段がなかったので子がいないのであればリジッドは作れない
	}
}

DsRagdoll::~DsRagdoll()
{
	for (DsRagdollParts parts : m_parts) {
		m_world.DeleteActor(parts.pActor->GetId());
	}
	m_parts.clear();
	for (DsBallJoint* pJoint : m_joints) {
		m_world.DeleteJoint(pJoint);
	}
	m_joints.clear();
}