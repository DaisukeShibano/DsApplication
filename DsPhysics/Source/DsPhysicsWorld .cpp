#include "DsPhysicsPch.h"
#ifndef __DS_PHYSICS_WORLD__
#include "DsPhysicsWorld.h"
#endif

#include "Actor/DsActorFactory.h"
#include "Collision/DsCollisionResult.h"
#include "Actor/DsRay.h"
#include "DsMass.h"
#include "Tool/DsActorSelector.h"

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
	: m_listener(*this)
	, m_group()
	, m_actors()
	, m_setting(setting)
	, m_constraintSolver(*this)
	, m_isGravity(true)
	, m_gravity(0, -setting.GetGravity(), 0)
{
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
			m_actors.remove(*it);
			delete pActor;
			return DsActorId();
		}
	}

	return id;//削除出来なかったらそのまま返す
}


void DsPhysicsWorld::Update(double dt)
{
	m_setting.SetDT(dt);

	_ApplyGravity();

	_UpdateConstraint(dt);
	
	_UpdateActor();

	m_listener.Collide(m_group);

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

void DsPhysicsWorld::_UpdateConstraint(double dt)
{
	const int iteNum = m_constraintSolver.GetIterationNum();
	m_constraintSolver.Solve(iteNum, dt);
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
DsActor*  DsPhysicsWorld::RayCast_CollectNear(const DsVec3d& startPos, const DsVec3d& endPos, double* depth/* = NULL*/) const
{
	typedef std::vector<DsCollisionResult> resultVec;
	
	const DsActorId dummyId;
	DsRay ray(dummyId);
	ray.Create(startPos, endPos);

	resultVec results;
	m_listener.Cast(ray, m_group, results);

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
				const double len = fabs(startPos.Length()-nearPos.Length());
				//Rayじゃない方のActorId取得
				const DsActorId nearId = ( (*it).GetHitActorId_1() != dummyId ) ? (*it).GetHitActorId_1() : (*it).GetHitActorId_2();

				if( len < minLen )
				{
					minLen = len;
					retId = nearId;
					retNearPos = nearPos;
					if (depth) *depth = len;
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

double DsPhysicsWorld::GetDt() const
{
	return m_setting.GetDT();
}
