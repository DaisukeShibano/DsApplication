#include "DsPhysicsPch.h"
#include "Actor/DsRigidCapsule.h"

using namespace DsPhysics;


DsRigidCapsule::DsRigidCapsule(const DsActorId& id, const char* name )
:DsRigidBody(id, name)
{
}

DsRigidCapsule::~DsRigidCapsule()
{
	delete m_pCollisionGeometry; m_pCollisionGeometry = NULL;
}

void DsRigidCapsule::Create(double r, double halfLen, double mass)
{
	DS_ASSERT(!( 0.0 >= mass), "重さが0以下です");

	r = max(r, 0.0);
	halfLen = max(halfLen, 0.0);

	DS_ASSERT(!(0.0 >= r), "半径が0以下です");
	
	{
		DsRigidPhysicsInfo& pi = m_physicsInfo;

		//mass
		{
			const double M = m_option.isStatic ? DS_MAX_MASS : mass;
			const double h = halfLen*2.0;
			const double m1 = 3.0*h/(4.0*r+3.0*h)*(1.0/12.0)*M;
			const double m2 = 4.0*r / (4.0*r + 3.0*h)*(1.0 / 20.0)*M;
			const DsVec3d bias = DsVec3d(1.0, 1.0, 1.0) + m_biasInertia;
			const double Ixx = (m1*(3.0*r*r + h*h) + m2*(8.0*r*r + 5.0*h*h)) * bias.x;
			const double Iyy = (m1*(0.5*r*r) + m2*(8.0*r*r)) * bias.y;
			const double Izz = (m1*(3.0*r*r + h*h) + m2*(8.0*r*r + 5.0*h*h)) * bias.z;
			pi.mass.inertia = DsMat33d::Identity();
			pi.mass.inertia[0] = Ixx;
			pi.mass.inertia[4] = Iyy;
			pi.mass.inertia[8] = Izz;
			pi.mass.mass = M;
			pi.mass.inertiaOriginal = pi.mass.inertia;
		}

		//初期姿勢
		pi.rotation = DsMat33d::Identity();

		//初期位置
		pi.pos.Set( 0.0, 0.0, 0.0 );

		//重心位置
		pi.centerOfGravity = GetPosition();

	}

	
	//静的オブジェでも最初の一回だけUpdateしないと初期姿勢が反映されない
	m_isForceUpdate = true;
	m_isForceRotation = true;

	m_prePos = GetPosition() + m_initPos;
	m_preRot = GetRotation() * m_initRot;
	SetPosition(GetPosition() + m_initPos);
	SetRotation(GetRotation() * m_initRot);
	_Update(DsVec3d::Zero(), DsMat33d::Identity());

	m_sideSize.x = r;
	m_sideSize.y = r + halfLen;
	m_sideSize.z = r;
	//AABB
	const DsVec3d max = GetPosition()+m_sideSize;
	const DsVec3d min = GetPosition()-m_sideSize;
	m_aabb.Setup(max, min);
	

	m_pCollisionGeometry = new DsCollisionGeometry(NULL, 0, NULL, 0,
		NULL, 0, GetId(), NULL, m_sideSize, NULL, &m_aabb, GetRotation());
}

//virtual 
void DsRigidCapsule::_UpdateAabb()
{
	const DsVec3d len = DsVec3d::Abs( GetRotation().GetAxisY()*(m_sideSize.y - m_sideSize.x) ) + DsVec3d(m_sideSize.x, m_sideSize.x, m_sideSize.x);
	const DsVec3d max = GetPosition() + len;
	const DsVec3d min = GetPosition() - len;
	m_aabb.Setup(max, min);
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
		pRet->SetInertiaBias(m_biasI);
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