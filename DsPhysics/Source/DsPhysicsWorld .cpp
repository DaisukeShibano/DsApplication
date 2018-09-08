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
	DS_ASSERT(m_pListener, "メモリ確保失敗");
	m_pConstraintSolver = new DsConstraintSolver(*this);
	DS_ASSERT(m_pConstraintSolver, "メモリ確保失敗");
	m_actors.clear();
};

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

	return id;//削除出来なかったらそのまま返す
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

void DsPhysicsWorld::_ApplyGravity()
{
	for each(DsActor* pActor in m_actors)
	{
		//ここで重力をかける
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
	for each( DsActor* pActor in m_actors )
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

DsPhysicsWorld::~DsPhysicsWorld()
{
	Clear();
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
	@param[out] depth 採用した衝突点までの距離
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

			//startPosに一番近い衝突点
			DsVec3d nearPos;
			const bool isExitNear = (*it).GetNearColPoint(startPos, nearPos);
			if( isExitNear )
			{
				const double len = (startPos-nearPos).Length();
				//Rayじゃない方のActorId取得
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


bool DsPhysicsWorld::SphereCast(DsVec3d start, DsVec3d end, double r, DsCollisionFilter filter, void* pUserData) const
{
	bool ret = false;
	const DsVec3d vec = end - start;
	const double lenSq = vec.LengthSq();
	if (lenSq < (r*r*4.0)) {
		//半径以下なので球で判定
		DsRigidSphere::DsRigidSphereFactory factory(r, 1.0, "スフィアキャスト");
		factory.InitPos(start);
		char buffer[sizeof(DsRigidSphere)];
		DsActor* pActor = factory.CreateIns(DsActorId((DsActor*)(buffer)), buffer);
		pActor->SetUserData(pUserData);
		pActor->SetCollisionFilter(filter);
		ret = m_pListener->Cast(*pActor, m_group);

		//DsDbgSys::GetIns().RefDrawCom().SetColor(0., 0., 1.0);
		//pActor->Draw(DsDbgSys::GetIns().RefDrawCom());
	}
	else {
		//カプセルで判定
		const DsVec3d baseDir = DsRigidCapsule::GetInitExtendDir();
		const DsVec3d capDir = DsVec3d::Normalize(vec);
		const DsVec3d axis = DsVec3d::Cross(capDir, baseDir);
		const double rad = DsVec3d::GetRelativeAng(baseDir, capDir);
		const DsMat33d rot = DsMat33d::RotateAxis(axis, rad);
		const DsVec3d pos = (start + end)*0.5;

		//カプセルパラ
		double halfLen = sqrt(lenSq)*0.5;
		halfLen -= r;
		DsRigidCapsule::DsRigidCapsuleFactory factory(r, halfLen, 1.0, "スフィアキャスト");
		factory.InitPos(pos);
		factory.InitRot(rot);
		char buffer[sizeof(DsRigidCapsule)];
		DsActor* pActor = factory.CreateIns(DsActorId( (DsActor*)(buffer) ), buffer);
		pActor->SetUserData(pUserData);
		pActor->SetCollisionFilter(filter);
		ret = m_pListener->Cast(*pActor, m_group);

		//DsDbgSys::GetIns().RefDrawCom().SetColor(0., 0., 1.0);
		//pActor->Draw(DsDbgSys::GetIns().RefDrawCom());
	}

	return ret;
}

double DsPhysicsWorld::GetDt() const
{
	return m_setting.GetDT();
}
