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
#ifndef __DS_RIGID_BODY__
#include "Actor/DsRigidBody.h"
#endif
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif

using namespace DsLib;
using namespace DsPhysics;


DsRagdoll::DsRagdoll(const DsAnimCustomProperty& customProperty, DsAnimSkeleton& skeleton, DsPhysicsWorld& world)
	: m_animSkeleton(skeleton)
	, m_world(world)
	, m_actors()
	, m_joints()
{
	//子→親 の長さで１つのリジッドを作る。終端のボーンはダミー。 最低でも2階層ないと成り立たない
	//ボーンの長さがとれれば1ボーン1リジッドでいけたが、取れなかったので。

	DsAnimBone** pAnimBone = skeleton.RefBoneArray().data();
	for (const DsAnimCustomProperty::RagdollParamId& param : customProperty.ragdollParamIds) {
		DS_ASSERT(pAnimBone[param.boneIndex]->arrayIdx != param.boneIndex, "ボーンインデックス実装ミス");
		const DsAnimBone* pParent = pAnimBone[param.boneIndex]->pParent;
		if (pParent) {
			const DsVec3d dist = pParent->initWorldPose.GetPos() - pAnimBone[param.boneIndex]->initWorldPose.GetPos();
			const DsVec3d pos = (pParent->initWorldPose.GetPos() + pAnimBone[param.boneIndex]->initWorldPose.GetPos())*0.5;
			const DsMat33d rot = pParent->initWorldPose.ToMat33();

			DsVec3d vertex[8];
			DsRigidBodyCube::GetVertex(vertex, 0.1, 0.1, dist.Length());//太さは後でパラメータ化する
			DsRigidBodyCube::DsRigidBodyFactory factory(vertex, 1.0, pParent->name.c_str());
			factory.InitPos(pos);
			factory.InitRot(rot);
			factory.SetOption(DsActor::Option::Default());
			DsActor* pActor = m_world.CreateActor(factory).GetActor();
			pActor->SetMaterial(DsActorMaterial::Aluminum());
			m_actors.push_back(pActor);

			//jointはworldに登録してアップデートは勝手にやってくれるようにする。デタッチアタッチはこちらで行う
		}
	}


}

DsRagdoll::~DsRagdoll()
{
	for (DsActor* pActor : m_actors) {
		m_world.DeleteActor(pActor->GetId());
	}
	m_actors.clear();
	for (DsBallJoint* pJoint : m_joints) {
		delete pJoint;
	}
	m_joints.clear();
}