#include "DsPhysicsPch.h"
#ifndef __DS_RIGID_SPHERE__
#include "Actor/DsRigidSphere.h"
#endif

#ifndef __DS_BOUNDING_TREE_BASE__
#include "Collision/BoundingTree/DsBoundingTreeAabb.h"
#endif

using namespace DsPhysics;


namespace
{
	DsVec3d _GetMaxVector(const DsVec3d* pVec, const int vn)
	{
		double maxX = 0.0;
		double maxY = 0.0;
		double maxZ = 0.0;

		for(int i=0; vn > i; ++i)
		{
			maxX = ( maxX > fabs(pVec[i].x) ) ? maxX : fabs(pVec[i].x);
			maxY = ( maxY > fabs(pVec[i].y) ) ? maxY : fabs(pVec[i].y);
			maxZ = ( maxZ > fabs(pVec[i].z) ) ? maxZ : fabs(pVec[i].z);
		}

		const DsVec3d ret = {maxX*2.0, maxY*2.0, maxZ*2.0};
		return ret;
	}

	DsVec3d _GetCenterOfGravity(const DsVec3d* v, const int vertexNum)
	{
		DsVec3d sum = {0.0, 0.0, 0.0};
		for(int vn=0; vertexNum > vn; ++vn)
		{
			sum += v[vn];
		}
		const DsVec3d ave = {sum.x/vertexNum, sum.y/vertexNum, sum.z/vertexNum};
		return ave;
	}
}



DsRigidSphere::DsRigidSphere(const DsActorId& id, const char* name )
:DsActor(id)
,m_geomInfo()
,m_physicsInfo()
,m_IsInit(false)
,m_addPos(DsVec3d::Zero())
,m_addRot(DsMat33d::Identity())
,m_initPos(DsVec3d::Zero())
,m_initRot(DsMat33d::Identity())
,m_material()
,m_isForceUpdate(false)
,m_isForceRotation(false)
,m_name(name)
,m_referenceF(m_physicsInfo.exForce)
,m_referenceT(m_physicsInfo.exTorque)
,m_exVel(DsVec3d::Zero())
,m_constraintF(DsVec3d::Zero())
,m_constraintT(DsVec3d::Zero())
,m_aabb()
,m_sideSize(DsVec3d::Zero())
,m_restTimer(0)
,m_pCollisionGeometry(NULL)
{
}

DsRigidSphere::~DsRigidSphere()
{
	delete m_pCollisionGeometry; m_pCollisionGeometry = NULL;
}

void DsRigidSphere::Update()
{
	_IntegralF();
	_UpdateForRest();
	
	if (!IsRest()){
		_IntegralVel();
		_Update(m_addPos, m_addRot);
		m_addPos = DsVec3d::Zero();
		m_addRot = DsMat33d::Identity();
	}

	m_isRestContagion = false;
}

void DsRigidSphere::Create(const double r, const double mass )
{
	DS_ASSERT(!( 0.0 >= mass), "重さが0以下です");

	{
		m_geomInfo.r = r;
	}

	{
		DsRigidSpherePhysicsInfo& pi = m_physicsInfo;

		//mass
		{
			const double M = m_option.isStatic ? DsMathUtil::DS_INFINITY_D : mass;
			const double bias = 1.0;
			const double Ixx = M / 3.0 * (r*r + r*r) * bias;
			const double Iyy = M / 3.0 * (r*r + r*r) * bias;
			const double Izz = M / 3.0 * (r*r + r*r) * bias;
			pi.mass.inertia = DsMat33d::Identity();
			pi.mass.inertia[0] = Ixx;
			pi.mass.inertia[4] = Iyy;
			pi.mass.inertia[8] = Izz;
			pi.mass.mass = M;

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
		DsRigidSphereGeometryInfo& gi = m_geomInfo;
		//pi.centerOfGravity = _GetCenterOfGravity(gi.vertex, DsRigidSphereGeometryInfo::VERTEX_NUM);
		pi.centerOfGravity = GetPosition();//中心って決まってるし、計算の無駄だし、頂点数に依存してしまうので。

		
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

	_Update(m_initPos, m_initRot);

	m_pCollisionGeometry = new DsCollisionGeometry(NULL, 0, NULL, 0,
		NULL, 0, GetId(), m_physicsInfo.centerOfGravity, NULL, m_sideSize, NULL, &m_aabb, GetRotation());
}


void DsRigidSphere::_Update( const DsVec3d& deltaPos, const DsMat33d& deltaRot )
{
	if (m_option.isStatic && (!m_isForceUpdate) && (!m_isForceRotation))
	{
		return;
	}

	DsRigidSphereGeometryInfo& gi = m_geomInfo;
	DsRigidSpherePhysicsInfo& pi = m_physicsInfo;

	//初期姿勢反映のため回転成分を有効化
	if (m_option.isRotation || m_isForceRotation)
	{
		//姿勢更新
		{
			pi.rotation = deltaRot * pi.rotation;
		}
	}

	const bool isChangeRot = m_isForceRotation || (!deltaRot.IsNearEqual(DsMat33d::Identity()));
	if (isChangeRot || m_isForceUpdate)
	{
		//位置更新
		{
			pi.pos += deltaPos;
		}
	}
	else if (!deltaPos.IsNearZero())
	{
		//位置更新
		{
			pi.pos += deltaPos;
		}
	}

	//重心更新
	//pi.centerOfGravity = _GetCenterOfGravity(gi.vertex, DsRigidSphereGeometryInfo::VERTEX_NUM);
	pi.centerOfGravity = GetPosition();//中心って決まってるし、計算の無駄だし、頂点数に依存してしまうので。

	//AABB
	{
		if (isChangeRot)
		{
			DsVec3d maxLen = DsVec3d(gi.r, gi.r, gi.r);
			m_aabb.Setup(maxLen.x, maxLen.y, maxLen.z, pi.centerOfGravity);
		}
		else
		{
			m_aabb.SetPos(pi.centerOfGravity);
		}
	}

	m_isForceUpdate = false;
	m_isForceRotation = false;
}

void DsRigidSphere::_UpdateForRest()
{
	m_isRest = false;
	bool isRest = false;
	if ((!m_isForceUpdate) && (!m_isForceRotation)){
		const double restVel = 0.2;//重力加速度による速度が大体0.024くらい
		const double vel = GetVelocity().LengthSq();//２乗なので注意
		if (vel < restVel){
			const double angVel = GetAngularVel().LengthSq();//２乗なので注意
			const double restAngVel = 0.02;
			if (angVel < restAngVel){
				isRest = true;
			}
		}
	}

	if (isRest){
		m_restTimer += GetDT();
	}
	else{
		m_restTimer = 0;
	}

	if (0.5 < m_restTimer){
		m_isRest = true;
		m_physicsInfo.vel = DsVec3d::Zero();
		m_physicsInfo.angVel = DsVec3d::Zero();
		m_physicsInfo.exForce = DsVec3d::Zero();
		m_physicsInfo.exTorque = DsVec3d::Zero();
	}

}

void DsRigidSphere::_IntegralF()
{
	const double dt = GetDT();
	{//並進
		m_physicsInfo.vel = CalcVelocity(m_physicsInfo.exForce);
		m_physicsInfo.exForce = DsVec3d::Zero();
	}
	{//回転
		m_physicsInfo.angVel = CalcAngularVel(m_physicsInfo.exTorque);
		m_physicsInfo.exTorque = DsVec3d::Zero();
	}
}
void DsRigidSphere::_IntegralVel()
{
	const double dt = GetDT();
	{//並進
		m_addPos += ((m_physicsInfo.vel+m_exVel) * dt);
		m_physicsInfo.exForce = DsVec3d::Zero();
	}
	{//回転
		m_addRot = DsMat33d::RotateVec(m_physicsInfo.angVel*dt)*m_addRot;
		m_physicsInfo.exTorque = DsVec3d::Zero();
	}
}

/*
	デバッグ用描画
*/
//virtual
void DsRigidSphere::Draw(DsDrawCommand& com)
{
	com.SetColor(m_dbgColor);
	com.DrawSphere(GetPosition(), m_geomInfo.r);
}

//virtual 
void DsRigidSphere::AddForce(const DsVec3d& force)
{
	if (!m_option.isStatic)
	{
		m_physicsInfo.exForce += force;
		//const double max = 300;
		//m_physicsInfo.exForce = DsVec3d(Clamp(m_physicsInfo.exForce.x, -max, max), Clamp(m_physicsInfo.exForce.y, -max, max), Clamp(m_physicsInfo.exForce.z, -max, max));
		m_referenceF = m_physicsInfo.exForce;
	}
}

//virtual 
void DsRigidSphere::AddTorque(const DsVec3d& torque )
{
	if (!m_option.isStatic)
	{
		m_physicsInfo.exTorque += torque;
		//const double max = 300;
		//m_physicsInfo.exTorque = DsVec3d(Clamp(m_physicsInfo.exTorque.x, -max, max), Clamp(m_physicsInfo.exTorque.y, -max, max), Clamp(m_physicsInfo.exTorque.z, -max, max));
		m_referenceT = m_physicsInfo.exTorque;
	}
}

//virtual 
void DsRigidSphere::AddConstraintForce(const DsVec3d& f, const DsVec3d& t)
{
	m_constraintF += f;
	m_constraintT += t;
}

//virtual 
void DsRigidSphere::IntegrateConstraintForce()
{
	const double dt = GetDT();
	
	//並進
	const DsVec3d addVel = (m_constraintF / m_physicsInfo.mass.mass) * dt;
	const DsVec3d addPos = ((addVel)* dt);
	m_constraintF = DsVec3d::Zero();
	
	//回転
	const DsVec3d addAngVel = (DsMat33d::Inverse(m_physicsInfo.mass.inertia)*m_constraintT) * dt;
	const DsMat33d addRot = DsMat33d::RotateVec(addAngVel*dt);
	m_constraintT = DsVec3d::Zero();
	
	_Update(addPos, addRot);
}

//virtual 
DsVec3d DsRigidSphere::CalcVelocity(const DsVec3d& f) const
{
	if (m_option.isStatic)
	{
		return DsVec3d::Zero();
	}
	else
	{
		DsVec3d ret = m_physicsInfo.vel;
		DsVec3d regist = m_physicsInfo.vel * 0.000018;//粘性抵抗
		ret += (( (f / m_physicsInfo.mass.mass) - regist) * GetDT());
		//const double max = 10;
		//ret = DsVec3d(Clamp(ret.x, -max, max), Clamp(ret.y, -max, max), Clamp(ret.z, -max, max));
		return ret;
	}
}

//virtual 
DsVec3d DsRigidSphere::CalcAngularVel(const DsVec3d& t) const
{
	if (m_option.isStatic)
	{
		return DsVec3d::Zero();
	}
	else
	{
		DsVec3d ret = m_physicsInfo.angVel;
		DsVec3d regist = m_physicsInfo.angVel * 0.000018;
		DsMat33d invInertia = DsMat33d::Inverse(m_physicsInfo.mass.inertia);
		ret += ((invInertia*t) - regist) * GetDT();
		//const double max = 10;
		//ret = DsVec3d(Clamp(ret.x, -max, max), Clamp(ret.y, -max, max), Clamp(ret.z, -max, max));
		return ret;
	}
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