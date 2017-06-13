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
	//�q���e �̒����łP�̃��W�b�h�����B�I�[�̃{�[���̓_�~�[�B �Œ�ł�2�K�w�Ȃ��Ɛ��藧���Ȃ�
	//�{�[���̒������Ƃ���1�{�[��1���W�b�h�ł��������A���Ȃ������̂ŁB

	DsAnimBone** pAnimBone = skeleton.RefBoneArray().data();
	for (const DsAnimCustomProperty::RagdollParamId& param : customProperty.ragdollParamIds) {
		DS_ASSERT(pAnimBone[param.boneIndex]->arrayIdx != param.boneIndex, "�{�[���C���f�b�N�X�����~�X");
		const DsAnimBone* pParent = pAnimBone[param.boneIndex]->pParent;
		if (pParent) {
			const DsVec3d dist = pParent->initWorldPose.GetPos() - pAnimBone[param.boneIndex]->initWorldPose.GetPos();
			const DsVec3d pos = (pParent->initWorldPose.GetPos() + pAnimBone[param.boneIndex]->initWorldPose.GetPos())*0.5;
			const DsMat33d rot = pParent->initWorldPose.ToMat33();

			DsVec3d vertex[8];
			DsRigidBodyCube::GetVertex(vertex, 0.1, 0.1, dist.Length());//�����͌�Ńp�����[�^������
			DsRigidBodyCube::DsRigidBodyFactory factory(vertex, 1.0, pParent->name.c_str());
			factory.InitPos(pos);
			factory.InitRot(rot);
			factory.SetOption(DsActor::Option::Default());
			DsActor* pActor = m_world.CreateActor(factory).GetActor();
			pActor->SetMaterial(DsActorMaterial::Aluminum());
			m_actors.push_back(pActor);

			//joint��world�ɓo�^���ăA�b�v�f�[�g�͏���ɂ���Ă����悤�ɂ���B�f�^�b�`�A�^�b�`�͂�����ōs��
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