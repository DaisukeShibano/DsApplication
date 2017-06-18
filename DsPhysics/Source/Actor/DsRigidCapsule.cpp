#include "DsPhysicsPch.h"
#ifndef __DS_RIGID_CAPSULE__
#include "Actor/DsRigidCapsule.h"
#endif

using namespace DsPhysics;


DsRigidCapsule::DsRigidCapsule(const DsActorId& id, const char* name )
:DsRigidBody(id, name)
,m_pCollisionGeometry(NULL)
{
}

DsRigidCapsule::~DsRigidCapsule()
{
	delete m_pCollisionGeometry; m_pCollisionGeometry = NULL;
}

void DsRigidCapsule::Create(const double r, const double halfLen, const double mass)
{
	DS_ASSERT(!( 0.0 >= mass), "重さが0以下です");

	{
		DsRigidPhysicsInfo& pi = m_physicsInfo;

		//mass
		{
			const double M = m_option.isStatic ? DsMathUtil::DS_INFINITY_D : mass;
			const double h = halfLen*2.0;
			const double m1 = 3.0*h/(4.0*r+3.0*h)*(1.0/12.0)*M;
			const double m2 = 4.0*r / (4.0*r + 3.0*h)*(1.0 / 20.0)*M;
			const double bias = 1.0 + m_biasInertia;
			const double Ixx = (m1*(3.0*r*r + h*h) + m2*(8.0*r*r + 5.0*h*h)) * bias;;
			const double Iyy = (m1*(0.5*r*r) + m2*(8.0*r*r)) * bias;;
			const double Izz = (m1*(3.0*r*r + h*h) + m2*(8.0*r*r + 5.0*h*h)) * bias;;
			pi.mass.inertia = DsMat33d::Identity();
			pi.mass.inertia[0] = Ixx;
			pi.mass.inertia[4] = Iyy;
			pi.mass.inertia[8] = Izz;
			pi.mass.mass = M;
		}

		//初期姿勢
		pi.rotation = DsMat33d::Identity();

		//初期位置
		pi.pos.Set( 0.0, 0.0, 0.0 );

		//重心位置
		pi.centerOfGravity = GetPosition();

		
		//AABB
		m_aabb.Setup(r, r+halfLen, r, pi.centerOfGravity);
		m_sideSize.x = r;
		m_sideSize.y = r+halfLen;
		m_sideSize.z = r;
	}

	
	//静的オブジェでも最初の一回だけUpdateしないと初期姿勢が反映されない
	m_isForceUpdate = true;
	m_isForceRotation = true;

	_Update(m_initPos, m_initRot);

	m_pCollisionGeometry = new DsCollisionGeometry(NULL, 0, NULL, 0,
		NULL, 0, GetId(), m_physicsInfo.centerOfGravity, NULL, m_sideSize, NULL, &m_aabb, GetRotation());
}

//virtual 
void DsRigidCapsule::_UpdateAabb(bool isChangeRot)
{
	if (isChangeRot)
	{
		const DsVec3d tmpX = GetRotation()*DsVec3d(m_sideSize.x, 0, 0);
		const DsVec3d tmpY = GetRotation()*DsVec3d(0, m_sideSize.y, 0);
		const DsVec3d tmpZ = GetRotation()*DsVec3d(0, 0, m_sideSize.z);
		const double maxX = max(fabs(tmpX.x), max(fabs(tmpY.x), fabs(tmpZ.x)));
		const double maxY = max(fabs(tmpX.y), max(fabs(tmpY.y), fabs(tmpZ.y)));
		const double maxZ = max(fabs(tmpX.z), max(fabs(tmpY.z), fabs(tmpZ.z)));
		m_aabb.Setup(maxX, maxY, maxZ, m_physicsInfo.centerOfGravity);
	}
	else
	{
		m_aabb.SetPos(m_physicsInfo.centerOfGravity);
	}
}

/*
	デバッグ用描画
*/
//virtual
void DsRigidCapsule::Draw(DsDrawCommand& com)
{
	const double r = m_sideSize.x;
	const DsVec3d p1 = GetRotation().GetAxisY()*(m_sideSize.y-r) + GetPosition();
	const DsVec3d p2 = GetRotation().GetAxisY()*(-(m_sideSize.y-r)) + GetPosition();
	com.DrawCapsule(p2, p1, r);
}



//////////////////////factory//////////////////////////
DsActor* DsRigidCapsule::DsRigidCapsuleFactory::CreateIns( const DsActorId& id ) const
{
	DsRigidCapsule* pRet = new DsRigidCapsule(id, m_name.c_str());
	if(pRet)
	{
		pRet->m_option = m_initOption;
		pRet->m_initPos = m_initPos;
		pRet->m_initRot = m_initRot;
		pRet->Create(m_r, m_halfLen, m_mass);
		return pRet;
	}
	else
	{
		return NULL;
	}
}