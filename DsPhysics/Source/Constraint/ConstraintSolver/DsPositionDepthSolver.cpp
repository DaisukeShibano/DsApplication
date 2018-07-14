#include "DsPhysicsPch.h"
#ifndef _DS_POSITION_DEPTH_SOLVER_
#include "Constraint/ConstraintSolver/DsPositionDepthSolver.h"
#endif
//���̑��̃w�b�_
#ifndef __DS_ACTOR__
#include "Actor/DsActor.h"
#endif
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif
#ifndef __DS_COLLISION_RESULT__
#include "Collision/DsCollisionResult.h"
#endif

using namespace DsPhysics;

DsPositionDepthSolver::DsPositionDepthSolver(const DsPhysicsWorld& world)
	:m_world(world)
{
}

DsPositionDepthSolver::~DsPositionDepthSolver()
{
}


void DsPositionDepthSolver::AddDepth(const DsCollisionResult& c)
{
	const int colNum = c.GetColNum();
	for (int i = 0; i < colNum; ++i) {

		const DsVec3d depth = c.GefNormal()[i]*c.GefDepth()[i];
		//������owner1���߂荞�݉�����������Ȃ̂ŁA�߂荞�ݗʂƂ��Ă͔��Α��ɂȂ�

		//���v�x�N�g���ɂ��悤���Ǝv�������A���������ɕ����߂荞��ł����ꍇ�ߏ�ɒ����Ȃ��Ă��܂��̂ň�ԗʂ������̂��̗p����
		//�������Ƃɂ�������������H�悭�����������ɂȂ邪�A���Ȃ��H
		{
			DsActor* pOwner1 = m_world.GetActor(c.GefOwnerId1()[i]);
			const DsVec3d add = -depth*0.5;
			const DsVec3d cur = pOwner1->GetCollisionDepth();
			//if (cur.LengthSq() < add.LengthSq()) {
			//	pOwner1->SetCollisionDepth(add);//�߂荞�ݗʂ�ǉ�
			//}
			DsVec3d set = cur;
			if ( fabs(cur.x) < fabs(add.x)) {
				set.x = add.x;
			}
			if (fabs(cur.y) < fabs(add.y)) {
				set.y = add.y;
			}
			if (fabs(cur.z) < fabs(add.z)) {
				set.z = add.z;
			}
			pOwner1->SetCollisionDepth(set);
		}
		{
			DsActor* pOwner2 = m_world.GetActor(c.GefOwnerId2()[i]);
			const DsVec3d add = depth*0.5;
			const DsVec3d cur = pOwner2->GetCollisionDepth();
			//if (cur.LengthSq() < add.LengthSq()) {
			//	pOwner2->SetCollisionDepth(add);//�߂荞�ݗʂ�ǉ�
			//}
			DsVec3d set = cur;
			if (fabs(cur.x) < fabs(add.x)) {
				set.x = add.x;
			}
			if (fabs(cur.y) < fabs(add.y)) {
				set.y = add.y;
			}
			if (fabs(cur.z) < fabs(add.z)) {
				set.z = add.z;
			}
			pOwner2->SetCollisionDepth(set);
		}

		//����̑��x�̂��Ƃ��l����ƁA�߂荞�ݕ����ɐi��ł����Ȃ��ꍇ������
		//�ŏI�I�ȑ��x���o���A�ړ��ʂ̗\�����o���A����ł��߂荞��ł��܂��Ȃ�A���̐������ړ��֎~�ɂ���Ƃ��H

		//����ł������Œǂ��z���Ƃ��ɋ}��~�����Ⴄ�C�����邪�A1�t���ł߂荞�݉����ł��鑬�x���S���v�Z���o���΁A
		//�߂荞�݉����ł��Ȃ����x�ɂȂ�̂͏㉺�ɋ��܂�Ă���悤�Ȃǂ����Ă������ł��Ȃ��ꍇ�Ȃ̂ŁA�}��~���Ă����Ȃ��H
	}
}