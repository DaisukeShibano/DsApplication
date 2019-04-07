#include "DsPhysicsPch.h"
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif

#include "Actor/DsActorFactory.h"
#include "Collision/DsCollisionResult.h"
#include "Actor/DsRay.h"
#include "Actor/DsRigidCapsule.h"
#include "Actor/DsRigidSphere.h"
#include "DsMass.h"
#include "Tool/DsActorSelector.h"
#ifndef _DS_JOINT_
#include "Joint/DsJoint.h"
#endif
#ifndef __DS_COLLISION_LISTENER__
#include "Collision/DsCollisionListener.h"
#endif 
#ifndef __DS_CONSTRAINT_SOLVER__
#include "Constraint/ConstraintSolver/DsConstraintSolver.h"
#endif

using namespace DsPhysics;


static const double DEFAULT_DT = 0.016;
static const double GRAVITY = 9.8;

DsPhysicsWorldSetting DsPhysicsWorldSetting::Default()
{
	DsPhysicsWorldSetting ret;
	ret.m_gravity = GRAVITY;
	ret.m_dt = DEFAULT_DT;

	return ret;
}



DsPhysicsWorld::DsPhysicsWorld(const DsPhysicsWorldSetting setting)
	: m_pListener(NULL)
	, m_group()
	, m_actors()
	, m_joints()
	, m_setting(setting)
	, m_pConstraintSolver(NULL)
	, m_pCollisionCallback(NULL)
	, m_isGravity(true)
	, m_gravity(0, -setting.GetGravity(), 0)
{
	m_pListener = new DsCollisionListener(*this);
	DS_ASSERT(m_pListener, "�������m�ێ��s");
	m_pConstraintSolver = new DsConstraintSolver(*this);
	DS_ASSERT(m_pConstraintSolver, "�������m�ێ��s");
	m_pDriveActorSolver = new DsConstraintSolver(*this);
	DS_ASSERT(m_pDriveActorSolver, "�������m�ێ��s");
	m_actors.clear();
};

DsPhysicsWorld::~DsPhysicsWorld()
{
	Clear();
	delete m_pDriveActorSolver; m_pDriveActorSolver = NULL;
	delete m_pConstraintSolver; m_pConstraintSolver = NULL;
	delete m_pListener; m_pListener = NULL;
}

DsActorId DsPhysicsWorld::CreateActor( const DsActorFactory& factory)
{
	DsActorId id;
	DsActor* pActor = factory.CreateIns(id);
	if( pActor )
	{
		pActor->SetDT( m_setting.GetDT() );
		m_actors.push_back( pActor );
		m_group.AddGroup( pActor );
		return DsActorId(pActor);
	}
	else
	{
		return DsActorId();
	}
}

DsActorId DsPhysicsWorld::DeleteActor(const DsActorId& id)
{
	auto end = m_actors.end();
	auto it = m_actors.begin();
	for( ; end != it; ++it)
	{
		if( (*it)->GetId()==id)
		{
			m_group.RemoveGroup(id);
			DsActor* pActor = (*it);
			m_actors.erase(it);
			delete pActor;
			return DsActorId();
		}
	}

	return id;//�폜�o���Ȃ������炻�̂܂ܕԂ�
}

DsJoint* DsPhysicsWorld::CreateJoint(DsJointFactory& jointFactory)
{
	DsJoint* pJoint = jointFactory.CreateIns();
	if (pJoint) {
		m_joints.push_back(pJoint);
	}
	return pJoint;
}

void DsPhysicsWorld::DeleteJoint(DsJoint* pJoint)
{
	auto end = m_joints.end();
	for (auto it = m_joints.begin(); end != it; ++it) {
		if (pJoint == (*it)) {
			m_joints.erase(it);
			break;
		}
	}
	delete pJoint;
}


void DsPhysicsWorld::Update(double dt)
{
	m_setting.SetDT(dt);

	_ApplyGravity();

	_UpdateJoint(dt);

	_UpdateConstraint(dt);
	
	_UpdateActor();

	m_pListener->Collide(m_group);

	_DeleteNoLifeActor();
}

void DsPhysicsWorld::DriveActor(double dt, DsActorId id, DsVec3d move)
{
	//DsPhysicsWorld::Update()�ł̍S���́E�d�͂͐ϕ��ς݂łO�B
	//����ȍ~�̊O�͍͂��݁B

	DsActor* pActor = id.GetActor();

	const DsVec3d moveVel = (0.0 < dt) ? (move / dt) : DsVec3d::Zero();

	double velY = pActor->GetVelocity().y;

	//�Ώۍ��̂͑��x���N���A����B�O�͂ŉ������������N���A�ł��Ȃ����߁B
	pActor->SetVelocity(DsVec3d(0, 0, 0));

	{//move������
		const double mass = pActor->GetMass().mass;
		const DsVec3d force = moveVel * (mass / dt);
		pActor->AddForce(force);
		pActor->Update();
	}

	{//�Փ˔���
		m_pListener->OneColide(*m_pDriveActorSolver, id, m_group);
	}

	//���x�͏��Ȃ��̂ł��̂���ōS���͂����߂�(Drive���͏d�͂����Ȃ�)
	pActor->SetVelocity(DsVec3d(0, 0, 0));

	{//�Փˉ���
		const DsVec3d colFroce = m_pDriveActorSolver->SolveCollision(id, m_pDriveActorSolver->GetIterationNum(), dt);
		pActor->Update();
	}

	//move���������̂Ŏ��̃t���[������͓����ĂȂ��Ă����̂ŃN���A
	//�d�͕����������A
	pActor->SetVelocity(DsVec3d(0, velY, 0));
}

void DsPhysicsWorld::_ApplyGravity()
{
	for (DsActor* pActor : m_actors)
	{
		//�����ŏd�͂�������
		if (m_isGravity && pActor->RefOption().isGravity)
		{
			pActor->AddForce(m_gravity*pActor->GetMass().mass);
		}
	}
}

void DsPhysicsWorld::_UpdateJoint(double dt)
{
	for (DsJoint* pJoint : m_joints) {
		pJoint->Update(dt);
	}
}

void DsPhysicsWorld::_UpdateConstraint(double dt)
{
	const int iteNum = m_pConstraintSolver->GetIterationNum();
	m_pConstraintSolver->Solve(iteNum, dt);
}

void DsPhysicsWorld::_UpdateActor()
{
	for ( DsActor* pActor : m_actors )
	{
		pActor->Update();
	}
}

void DsPhysicsWorld::_DeleteNoLifeActor()
{
	auto end = m_actors.end();
	auto it = m_actors.begin();
	for( ; end != it; ++it)
	{
		if( (*it)->GetLifeTime() <= 0  )
		{
			DsActor* pActor = (*it);
			m_group.RemoveGroup(pActor->GetId());
			m_actors.remove(*it);
			delete pActor;
		}
	}	
}


void DsPhysicsWorld::Clear()
{
	auto end = m_actors.end();
	auto it = m_actors.begin();
	for( ; end != it; ++it)
	{
		delete (*it);
	} 
	m_actors.clear();
	m_group.Clear();
}

DsActor* DsPhysicsWorld::GetActor(const DsActorId& id) const
{
	//auto end = m_actors.end();
	//auto it = m_actors.begin();
	//for( ; end != it; ++it)
	//{
	//	if( (*it)->GetId()==id)
	//	{
	//		return (*it);
	//	}
	//}
	//return NULL;
	return const_cast<DsActor*>(id.GetActor());
}

/*
	@param[out] depth �̗p�����Փ˓_�܂ł̋���
*/
DsActor*  DsPhysicsWorld::RayCast_CollectNear(const DsVec3d& startPos, const DsVec3d& endPos, double* depth/* = NULL*/, DsVec3d* hitPos/* = NULL*/) const
{
	DsRay ray;
	ray.Create(startPos, endPos);
	return RayCast_CollectNear(ray, depth, hitPos);
}

DsActor*  DsPhysicsWorld::RayCast_CollectNear(DsRay& ray, double* depth/* = NULL*/, DsVec3d* hitPos/* = NULL*/) const
{
	typedef std::vector<DsCollisionResult> resultVec;
	
	DsActorId dummyId = DsActorId(&ray);
	const DsVec3d startPos = ray.GetCollisionGeometry()->GetVertex()[0];

	resultVec results;
	m_pListener->Cast(ray, m_group, results);

	DsActorId retId;
	double minLen = DsMathUtil::DS_INFINITY_D;
	auto end = results.end();

	DsVec3d retNearPos;

	if( !results.empty() )
	{
		for(auto it=results.begin(); end!=it; ++it)
		{
			if( (*it).GetColNum() == 0 )continue;

			//startPos�Ɉ�ԋ߂��Փ˓_
			DsVec3d nearPos;
			const bool isExitNear = (*it).GetNearColPoint(startPos, nearPos);
			if( isExitNear )
			{
				const double len = (startPos-nearPos).Length();
				//Ray����Ȃ�����ActorId�擾
				const DsActorId nearId = ( (*it).GetHitActorId_1() != dummyId ) ? (*it).GetHitActorId_1() : (*it).GetHitActorId_2();

				if( len < minLen )
				{
					minLen = len;
					retId = nearId;
					retNearPos = nearPos;
					if (depth) *depth = len;
					if (hitPos) *hitPos = retNearPos;
				}
			}
		}
		return GetActor(retId);
	}
	else
	{
		return NULL;
	}
}


bool DsPhysicsWorld::SphereCast(DsVec3d start, DsVec3d end, double r, DsCollisionFilter filter, void* pUserData, DsVec3d* pOutHitPos/*=NULL*/, DsActorId* pOutActor/* = NULL*/) const
{
	typedef std::vector<DsCollisionResult> resultVec;

	bool ret = false;
	resultVec results;
	const DsVec3d vec = end - start;
	const DsVec3d capDir = DsVec3d::Normalize(vec);

	char buffer[ max(sizeof(DsRigidSphere), sizeof(DsRigidCapsule)) ];
	const DsActor* pActor = _GetSphereCastShape(start, end, r, filter, pUserData, buffer);

	if (pOutHitPos) {
		m_pListener->Cast(*pActor, m_group, results);
	}
	else {
		ret = m_pListener->Cast(*pActor, m_group);
	}


	if (pOutHitPos) {
		double depth = DBL_MAX;
		DsVec3d hPos;
		for (const DsCollisionResult& result : results) {
			const int colNum = result.GetColNum();
			for (int ci = 0; ci < colNum; ++ci) {
				const DsVec3d colPos = result.GetPos()[ci] - start;
				const double colLen = DsVec3d::Dot(capDir, colPos);
				if (colLen < depth) {
					depth = colLen;
					ret = true;
				}
			}
		}

		if (ret) {
			*pOutHitPos = start + capDir * depth;
		}
	}

	return ret;
}

bool DsPhysicsWorld::SphereCast(DsVec3d start, DsVec3d end, double r, DsCollisionFilter filter, void* pUserData, std::vector<DsCollisionResult>& outResult) const
{
	bool ret = false;
	char buffer[max(sizeof(DsRigidSphere), sizeof(DsRigidCapsule))];
	const DsActor* pActor = _GetSphereCastShape(start, end, r, filter, pUserData, buffer);
	m_pListener->Cast(*pActor, m_group, outResult);
	return !outResult.empty();
}

DsActor* DsPhysicsWorld::_GetSphereCastShape(DsVec3d start, DsVec3d end, double r, DsCollisionFilter filter, void* pUserData, char* pActorBuffer) const
{
	const DsVec3d vec = end - start;
	const DsVec3d capDir = DsVec3d::Normalize(vec);
	DsActor* pActor = NULL;
	const double lenSq = vec.LengthSq();
	if (lenSq < (r*r*4.0)) {
		//���a�ȉ��Ȃ̂ŋ��Ŕ���
		DsRigidSphere::DsRigidSphereFactory factory(r, 1.0, "�X�t�B�A�L���X�g");
		factory.InitPos(start);
		pActor = factory.CreateIns(DsActorId((DsActor*)(pActorBuffer)), pActorBuffer);
	}
	else {
		//�J�v�Z���Ŕ���
		const DsVec3d baseDir = DsRigidCapsule::GetInitExtendDir();
		const DsVec3d axis = DsVec3d::Cross(capDir, baseDir);
		const double rad = DsVec3d::GetRelativeAng(baseDir, capDir);
		const DsMat33d rot = DsMat33d::RotateAxis(axis, rad);
		const DsVec3d pos = (start + end)*0.5;

		//�J�v�Z���p��
		double halfLen = sqrt(lenSq)*0.5;
		halfLen -= r;
		DsRigidCapsule::DsRigidCapsuleFactory factory(r, halfLen, 1.0, "�X�t�B�A�L���X�g");
		factory.InitPos(pos);
		factory.InitRot(rot);
		pActor = factory.CreateIns(DsActorId((DsActor*)(pActorBuffer)), pActorBuffer);
		//DsDbgSys::GetIns().RefDrawCom().SetColor(0., 0., 1.0);
		//pActor->Draw(DsDbgSys::GetIns().RefDrawCom());
	}

	pActor->SetUserData(pUserData);
	pActor->SetCollisionFilter(filter);
	return pActor;
}

double DsPhysicsWorld::GetDt() const
{
	return m_setting.GetDT();
}
