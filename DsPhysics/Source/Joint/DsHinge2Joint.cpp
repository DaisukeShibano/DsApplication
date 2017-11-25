#include "DsPhysicsPch.h"
#ifndef _DS_HINGE2_JOINT_
#include "Joint/DsHinge2Joint.h"
#endif
#include "DsPhysicsManager.h"
#include "DsPhysicsWorld.h"
#ifndef __DS_HINGE_CONSTRAINT__
#include "Constraint/DsHinge2Constraint.h"
#endif
#ifndef __DS_CONSTRAINT_SOLVER__
#include "Constraint/ConstraintSolver/DsConstraintSolver.h"
#endif

using namespace DsPhysics;

DsHinge2Joint::DsHinge2Joint(DsPhysicsWorld& world)
:DsJoint(world)
, m_masterId()
, m_subId()
, m_pConstarint(NULL)
, m_distance(DsVec3d::Zero())
, m_distanceSub(DsVec3d::Zero())
, m_actorRot(DsMat33d::Identity())
, m_subActorRot(DsMat33d::Identity())
, m_cRot(DsMat33d::Identity())
, m_cRotSub(DsMat33d::Identity())
, m_initDiffRot(DsMat33d::Identity())
, m_isAttach(false)
{
	m_pConstarint = new DsHinge2Constraint(world);
}

DsHinge2Joint::~DsHinge2Joint()
{
	delete m_pConstarint;
	m_pConstarint = NULL;
}

void DsHinge2Joint::AttachJoint(const DsActorId act1, const DsActorId act2, const DsVec3d& pos, const DsVec3d& axis1, const DsVec3d& axis2)
{
	const DsActor* pMas = m_world.GetActor(act1);
	const DsActor* pSub = m_world.GetActor(act2);
	if (pMas && pSub)
	{
		if (!m_isAttach){
			m_masterId = act1;
			m_subId = act2;
			m_distance = pos - pMas->GetPosition();
			m_distanceSub = pos - pSub->GetPosition();
			m_actorRot = pMas->GetRotation();
			m_subActorRot = pSub->GetRotation();

			//�S���̎p�������߂�
			//z���ȊO�͉��ł������B�K���ɒ��s�����߂�
			DsVec3d cZ = DsVec3d::Normalize(axis1);
			DsVec3d cY = DsVec3d::Normalize(axis2);
			DsVec3d cX = DsVec3d::Normalize(DsVec3d::Cross(cY, cZ));
			m_cRot = DsMat33d::SetAxis(cX, cY, cZ);
			m_cRotSub = m_cRot;//�ŏ���mas��sub����v

			m_initDiffRot = m_cRot.ToTransposition()*m_subActorRot;//�S���ƃT�u�̑��Ίp�x

			m_cRot = m_actorRot.ToTransposition()*m_cRot;//�}�X�^�[����̑��΂ɂ��Ă����B���t���}�X�^�[�̕ω��ʂƓ������|���Z���Ă��������ǁA���ꂪ�~�ς�����
			m_cRotSub = m_subActorRot.ToTransposition()*m_cRotSub;

			m_pConstarint->RequestAttach(m_masterId, m_subId, pos, cX, cY, cZ, m_world.GetDt(), DsVec3d::Zero());
			m_pConstarint->UpdateErrorAxis1(cZ, cZ);
			m_pConstarint->UpdateErrorAxis2(cY, cY);
			m_world.GetConstraintSolver()->AddConstraint(m_pConstarint);

			
			m_isAttach = true;
		}
		else
		{
			//���ɒǉ��ς�
		}
	}
}

void DsHinge2Joint::DetachJoint()
{
	m_isAttach = false;
	m_world.GetConstraintSolver()->RemoveConstraint(m_pConstarint);
}

//virtual
void DsHinge2Joint::Update(double dt)
{
	const DsActor* pMas = m_world.GetActor(m_masterId);
	const DsActor* pSub = m_world.GetActor(m_subId);
	if (pSub)
	{
		//sub�̃A�^�b�`�ʒu���X�V
		const DsMat33d curRot = pSub->GetRotation();
		const DsMat33d invPreRot = m_subActorRot.ToTransposition();
		const DsMat33d deltaRot = curRot*invPreRot;
		m_subActorRot = curRot;
		m_distanceSub = deltaRot*m_distanceSub;
	}
	if (pMas && pSub)
	{
		//master�̓����ɒǏ]����
		const DsMat33d curRot = pMas->GetRotation();
		const DsMat33d invPreRot = m_actorRot.ToTransposition();
		const DsMat33d deltaRot = curRot*invPreRot;
		m_actorRot = curRot;
		m_distance = deltaRot*m_distance;
		const DsVec3d pos = pMas->GetPosition() + m_distance;
		const DsVec3d posSub = pSub->GetPosition() + m_distanceSub;
		const DsVec3d errorP = (posSub - pos);
		
		
		const DsMat33d cRot = m_actorRot*m_cRot;//�S�����W�̓}�X�^�[����̑��΂Ȃ̂�,�O���[�o���ɖ߂�
		const DsMat33d cSubRot = m_subActorRot*m_cRotSub;

		m_pConstarint->RequestAttach(m_masterId, m_subId, pos, cRot.GetAxisX(), cRot.GetAxisY(), cRot.GetAxisZ(), m_world.GetDt(), errorP);
		m_pConstarint->UpdateErrorAxis1(cRot.GetAxisZ(), cSubRot.GetAxisZ());
		m_pConstarint->UpdateErrorAxis2(cRot.GetAxisY(), cSubRot.GetAxisY());
	}
}

//virtual 
void DsHinge2Joint::DbgDraw(DsDrawCommand& com) const
{
	if (m_pConstarint)
	{
		const DsVec3d pos = m_pConstarint->GetPosition();
		const DsMat33d rot = m_pConstarint->GetRotation();
		com.DrawAxis(rot, pos);
	}
}
