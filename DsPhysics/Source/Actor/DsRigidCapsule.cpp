#include "DsPhysicsPch.h"
#ifndef __DS_RIGID_CAPSULE__
#include "Actor/DsRigidCapsule.h"
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



DsRigidCapsule::DsRigidCapsule(const DsActorId& id, const char* name )
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
,m_pCollisionContext(NULL)
{
}

DsRigidCapsule::~DsRigidCapsule()
{
	delete m_pCollisionContext; m_pCollisionContext = NULL;
}

void DsRigidCapsule::Update()
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

void DsRigidCapsule::Create(const double r, const double halfLen, const double mass)
{
	DS_ASSERT(!( 0.0 >= mass), "重さが0以下です");

	{
		DsRigidCapsuleGeometryInfo& gi = m_geomInfo;
		gi.r = r;
		gi.halfLen = halfLen;
	}

	{
		DsRigidCapsulePhysicsInfo& pi = m_physicsInfo;

		//mass
		{
			const double M = m_option.isStatic ? DsMathUtil::DS_INFINITY_D : mass;
			const double h = halfLen*2.0;
			const double m1 = 3.0*h/(4.0*r+3.0*h)*(1.0/12.0)*M;
			const double m2 = 4.0*r / (4.0*r + 3.0*h)*(1.0 / 20.0)*M;
			const double bias = 1.0;
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
		DsRigidCapsuleGeometryInfo& gi = m_geomInfo;
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

	m_pCollisionContext = new DsCollisionContext(NULL, 0, NULL, 0,
		NULL, 0, GetId(), m_physicsInfo.centerOfGravity, NULL, m_sideSize, NULL, &m_aabb, GetRotation());
}


void DsRigidCapsule::_Update( const DsVec3d& deltaPos, const DsMat33d& deltaRot )
{
	if (m_option.isStatic && (!m_isForceUpdate) && (!m_isForceRotation))
	{
		return;
	}

	DsRigidCapsuleGeometryInfo& gi = m_geomInfo;
	DsRigidCapsulePhysicsInfo& pi = m_physicsInfo;

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
		//慣性テンソル更新
		{
			const DsMat33d invR = DsMat33d::Inverse(deltaRot);
			const DsMat33d rotI = (deltaRot * pi.mass.inertia) * invR;
			pi.mass.inertia = rotI;
		}
		//拘束用慣性テンソル逆行列
		{
			const DsMat33d&& invRI = DsMat33d::Inverse(pi.mass.inertia);
			pi.massInv[0] = 1.0 / pi.mass.mass;
			pi.massInv[1] = invRI[0]; pi.massInv[2] = invRI[1]; pi.massInv[3] = invRI[2];
			pi.massInv[4] = invRI[3]; pi.massInv[5] = invRI[4]; pi.massInv[6] = invRI[5];
			pi.massInv[7] = invRI[6]; pi.massInv[8] = invRI[7]; pi.massInv[9] = invRI[8];
		}

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
	pi.centerOfGravity = GetPosition();

	//AABB
	{
		if (isChangeRot)
		{
			const DsVec3d tmpX = GetRotation()*DsVec3d(m_sideSize.x, 0, 0);
			const DsVec3d tmpY = GetRotation()*DsVec3d(0, m_sideSize.y, 0);
			const DsVec3d tmpZ = GetRotation()*DsVec3d(0, 0, m_sideSize.z);
			const double maxX = max( fabs(tmpX.x), max( fabs(tmpY.x), fabs(tmpZ.x)) );
			const double maxY = max(fabs(tmpX.y), max(fabs(tmpY.y), fabs(tmpZ.y)));
			const double maxZ = max(fabs(tmpX.z), max(fabs(tmpY.z), fabs(tmpZ.z)));
			m_aabb.Setup(maxX, maxY, maxZ, pi.centerOfGravity);
		}
		else
		{
			m_aabb.SetPos(pi.centerOfGravity);
		}
	}

	m_isForceUpdate = false;
	m_isForceRotation = false;
}

void DsRigidCapsule::_UpdateForRest()
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

void DsRigidCapsule::_IntegralF()
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
void DsRigidCapsule::_IntegralVel()
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
void DsRigidCapsule::Draw(DsDrawCommand& com)
{
	const double r = m_sideSize.x;
	const DsVec3d p1 = GetRotation().GetAxisY()*(m_sideSize.y-r) + GetPosition();
	const DsVec3d p2 = GetRotation().GetAxisY()*(-(m_sideSize.y-r)) + GetPosition();
	com.DrawCapsule(p2, p1, r);
}


void DsRigidCapsule::GetVertex( DsVec3d* pv, double xl, double yl, double zl)
{
	pv[0].Set( -xl/2.0, yl/2.0, zl/2.0 );
	pv[1].Set(  xl/2.0, yl/2.0, zl/2.0 );
	pv[2].Set(  xl/2.0, yl/2.0, -zl/2.0 );
	pv[3].Set( -xl/2.0, yl/2.0, -zl/2.0 );
	pv[4].Set( -xl/2.0, -yl/2.0, zl/2.0 );
	pv[5].Set(  xl/2.0, -yl/2.0, zl/2.0 );
	pv[6].Set(  xl/2.0, -yl/2.0, -zl/2.0 );
	pv[7].Set( -xl/2.0, -yl/2.0, -zl/2.0 );
}

//virtual 
void DsRigidCapsule::AddForce(const DsVec3d& force)
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
void DsRigidCapsule::AddTorque(const DsVec3d& torque )
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
void DsRigidCapsule::AddConstraintForce(const DsVec3d& f, const DsVec3d& t)
{
	m_constraintF += f;
	m_constraintT += t;
}

//virtual 
void DsRigidCapsule::IntegrateConstraintForce()
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
DsVec3d DsRigidCapsule::CalcVelocity(const DsVec3d& f) const
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
DsVec3d DsRigidCapsule::CalcAngularVel(const DsVec3d& t) const
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