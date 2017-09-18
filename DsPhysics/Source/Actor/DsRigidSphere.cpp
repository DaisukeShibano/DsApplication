#include "DsPhysicsPch.h"
#ifndef __DS_RIGID_SPHERE__
#include "Actor/DsRigidSphere.h"
#endif

#ifndef __DS_BOUNDING_TREE_BASE__
#include "Collision/BoundingTree/DsBoundingTreeAabb.h"
#endif

using namespace DsPhysics;


DsRigidSphere::DsRigidSphere(const DsActorId& id, const char* name )
:DsRigidBody(id, name)
,m_pCollisionGeometry(NULL)
{
}

DsRigidSphere::~DsRigidSphere()
{
	delete m_pCollisionGeometry; m_pCollisionGeometry = NULL;
}


void DsRigidSphere::Create(const double r, const double mass )
{
	DS_ASSERT(!( 0.0 >= mass), "重さが0以下です");

	{
		DsRigidPhysicsInfo& pi = m_physicsInfo;

		//mass
		{
			const double M = m_option.isStatic ? DS_MAX_MASS : mass;
			const DsVec3d bias = DsVec3d(1.0, 1.0, 1.0) + m_biasInertia;
			const double Ixx = M / 3.0 * (r*r + r*r) * bias.x;
			const double Iyy = M / 3.0 * (r*r + r*r) * bias.y;
			const double Izz = M / 3.0 * (r*r + r*r) * bias.z;
			pi.mass.inertia = DsMat33d::Identity();
			pi.mass.inertia[0] = Ixx;
			pi.mass.inertia[4] = Iyy;
			pi.mass.inertia[8] = Izz;
			pi.mass.mass = M;
			pi.mass.inertiaOriginal = pi.mass.inertia;

			//拘束用慣性テンソル逆行列
			{
				pi.massInv[0] = 1.0 / pi.mass.mass;
				pi.massInv[1] = 1.0 / pi.mass.mass; pi.massInv[2] = 0;                  pi.massInv[3] = 0;
				pi.massInv[4] = 0;                  pi.massInv[5] = 1.0 / pi.mass.mass; pi.massInv[6] = 0;
				pi.massInv[7] = 0;                  pi.massInv[8] = 0;                  pi.massInv[9] = 1.0 / pi.mass.mass;
			}
		}

		//初期姿勢
		pi.rotation = DsMat33d::Identity();

		//初期位置
		pi.pos.Set( 0.0, 0.0, 0.0 );

		//重心位置
		pi.centerOfGravity = GetPosition();

		
		//AABB
		DsVec3d maxLen = DsVec3d(r, r, r);
		m_aabb.Setup(maxLen.x, maxLen.y, maxLen.z, pi.centerOfGravity);
		m_sideSize.x = maxLen.x;
		m_sideSize.y = maxLen.y;
		m_sideSize.z = maxLen.z;
	}

	//静的オブジェでも最初の一回だけUpdateしないと初期姿勢が反映されない
	m_isForceUpdate = true;
	m_isForceRotation = true;

	m_prePos = GetPosition() + m_initPos;
	m_preRot = GetRotation() * m_initRot;
	SetPosition(GetPosition() + m_initPos);
	SetRotation(GetRotation() * m_initRot);
	_Update(DsVec3d::Zero(), DsMat33d::Identity());
	
	m_pCollisionGeometry = new DsCollisionGeometry(NULL, 0, NULL, 0,
		NULL, 0, GetId(), m_physicsInfo.centerOfGravity, NULL, m_sideSize, NULL, &m_aabb, GetRotation());
}

//virtual 
void DsRigidSphere::_UpdateInertia(const DsMat33d& deltaRot)
{
	//何もしない
}

//virtual
void DsRigidSphere::_UpdateAabb(bool isChangeRot)
{
	DsRigidPhysicsInfo& pi = m_physicsInfo;
	if (isChangeRot)
	{
		m_aabb.Setup(m_sideSize.x, m_sideSize.y, m_sideSize.z, pi.centerOfGravity);
	}
	else
	{
		m_aabb.SetPos(pi.centerOfGravity);
	}
}


/*
	デバッグ用描画
*/
//virtual
void DsRigidSphere::Draw(DsDrawCommand& com)
{
	com.SetColor(m_dbgColor);
	com.DrawSphere(GetPosition(), m_sideSize.x);
}


//////////////////////factory//////////////////////////
DsActor* DsRigidSphere::DsRigidSphereFactory::CreateIns(const DsActorId& id) const
{
	DsRigidSphere* pRet = new DsRigidSphere(id, m_name.c_str());
	if(pRet)
	{
		pRet->m_option = m_initOption;
		pRet->m_initPos = m_initPos;
		pRet->m_initRot = m_initRot;
		pRet->Create(m_r, m_mass);
		return pRet;
	}
	else
	{
		return NULL;
	}
}