#include "DsPhysicsPch.h"
#ifndef _DS_BALL_JOINT_H_
#include "Joint/DsBallJoint.h"
#endif

#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif
#ifndef __DS_BALL_CONSTRAINT__
#include "Constraint/DsBallConstraint.h"
#endif
#ifndef __DS_CONSTRAINT_SOLVER__
#include "Constraint/ConstraintSolver/DsConstraintSolver.h"
#endif
#ifndef __DS_CONSTRAINT_SOLVER__
#include "Constraint/ConstraintSolver/DsConstraintSolver.h"
#endif

using namespace DsPhysics;

DsBallJoint::DsBallJoint(DsPhysicsWorld& world)
	: DsJoint(world)
	, m_masterId()
	, m_subId()
	, m_pConstarint(NULL)
	, m_distance(DsVec3d::Zero())
	, m_distanceSub(DsVec3d::Zero())
	, m_rot(DsMat33d::Identity())
	, m_rotSub(DsMat33d::Identity())
	, m_isAttach(false)
{
	m_pConstarint = new DsBallConstraint(world);
}

//virtual 
DsBallJoint::~DsBallJoint()
{
	delete m_pConstarint; m_pConstarint = NULL;
}

//最初の一回だけやればいい
void DsBallJoint::AttachJoint(const DsActorId act1, const DsActorId act2, const DsVec3d& pos)
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
			m_rot = pMas->GetRotation();
			m_rotSub = pSub->GetRotation();
			m_pConstarint->RequestAttach(m_masterId, m_subId, pos, m_rot.GetAxisX(), m_rot.GetAxisY(), m_rot.GetAxisZ(), m_world.GetDt(), DsVec3d::Zero());
			m_world.GetConstraintSolver()->AddConstraint(m_pConstarint);
			m_isAttach = true;
		}
		else
		{
			//既に追加済み
		}
	}
}

//virtual 
void DsBallJoint::Update(double dt)
{
	const DsActor* pMas = m_world.GetActor(m_masterId);
	const DsActor* pSub = m_world.GetActor(m_subId);
	if (pSub)
	{
		//subのアタッチ位置を更新
		const DsMat33d curRot = pSub->GetRotation();
		const DsMat33d invPreRot = m_rotSub.ToTransposition();
		const DsMat33d deltaRot = curRot*invPreRot;
		m_rotSub = curRot;
		m_distanceSub = deltaRot*m_distanceSub;
	}
	if (pMas && pSub)
	{
		//masterの動きに追従する
		const DsMat33d curRot = pMas->GetRotation();
		const DsMat33d invPreRot = m_rot.ToTransposition();
		const DsMat33d deltaRot = curRot*invPreRot;
		m_rot = curRot;
		m_distance = deltaRot*m_distance;
		const DsVec3d pos = pMas->GetPosition() + m_distance;
		const DsVec3d posSub = pSub->GetPosition() + m_distanceSub;
		const DsVec3d error = posSub - pos;
		//ボールジョイントはposだけ合ってれば、軸は適当で大丈夫なはず
		m_pConstarint->RequestAttach(m_masterId, m_subId, pos, m_rot.GetAxisX(), m_rot.GetAxisY(), m_rot.GetAxisZ(), m_world.GetDt(), error);
	}	
}

void DsBallJoint::DetachJoint()
{
	m_isAttach = false;
	m_world.GetConstraintSolver()->RemoveConstraint(m_pConstarint);
}



//virtual 
void DsBallJoint::DbgDraw(DsDrawCommand& com) const
{
	if (m_pConstarint)
	{
		const DsVec3d pos = m_pConstarint->GetPosition();
		const DsMat33d rot = m_pConstarint->GetRotation();
		com.DrawAxis(rot, pos);
	}
	const DsActor* pSub = m_world.GetActor(m_subId);
	if (pSub)
	{
		const DsVec3d posSub = pSub->GetPosition() + m_distanceSub;
		const DsMat33d rotSub = pSub->GetRotation();
		com.DrawAxis(rotSub, posSub);
	}
}