#include "DsPhysicsPch.h"
#ifndef __DS_RIGID_BODY__
#include "Actor/DsRigidBody.h"
#endif

using namespace DsPhysics;

static const double AIR_DAMPER = 0.000018;

DsRigidBody::DsRigidBody(const DsActorId& id, const char* name)
: DsActor(id)
, m_geomInfo()
, m_physicsInfo()
, m_addPos(DsVec3d::Zero())
, m_addRot(DsMat33d::Identity())
, m_initPos(DsVec3d::Zero())
, m_initRot(DsMat33d::Identity())
, m_material()
, m_isForceUpdate(false)
, m_isForceRotation(false)
, m_name(name)
, m_referenceF(m_physicsInfo.exForce)
, m_referenceT(m_physicsInfo.exTorque)
, m_constraintF(DsVec3d::Zero())
, m_constraintT(DsVec3d::Zero())
, m_aabb()
, m_sideSize(DsVec3d::Zero())
, m_restTimer(0)
, m_prePos(DsVec3d::Zero())
, m_preRot(DsMat33d::Identity())
, m_damperVel(0)
, m_damperAngVel(0)
, m_biasInertia(DsVec3d::Zero())
, m_pCollisionGeometry(NULL)
{}


//virtual
void DsRigidBody::Update()
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

DsVec3d DsRigidBody::_GetMaxVector(const DsVec3d* pVec, const int vn) const
{
	double maxX = 0.0;
	double maxY = 0.0;
	double maxZ = 0.0;

	for (int i = 0; vn > i; ++i)
	{
		maxX = (maxX > fabs(pVec[i].x)) ? maxX : fabs(pVec[i].x);
		maxY = (maxY > fabs(pVec[i].y)) ? maxY : fabs(pVec[i].y);
		maxZ = (maxZ > fabs(pVec[i].z)) ? maxZ : fabs(pVec[i].z);
	}

	const DsVec3d ret = { maxX*2.0, maxY*2.0, maxZ*2.0 };
	return ret;
}

DsVec3d DsRigidBody::_GetCenterOfGravity(const DsVec3d* v, const int vertexNum) const
{
	DsVec3d sum = { 0.0, 0.0, 0.0 };
	for (int vn = 0; vertexNum > vn; ++vn)
	{
		sum += v[vn];
	}
	const DsVec3d ave = { sum.x / vertexNum, sum.y / vertexNum, sum.z / vertexNum };
	return ave;
}

//virtual
void DsRigidBody::_Update( const DsVec3d& deltaPos, const DsMat33d& deltaRot )
{
	if (m_option.isStatic && (!m_isForceUpdate) && (!m_isForceRotation))
	{
		return;
	}

	DsRigidGeometryInfo& gi = m_geomInfo;
	DsRigidPhysicsInfo& pi = m_physicsInfo;

	
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
		//頂点回転
		for (int vn = 0; vn < gi.vn; ++vn)
		{
			gi.pVertex[vn] = pi.rotation * gi.pVertexOriginal[vn];
		}

		//慣性テンソル更新
		{
			_UpdateInertia(deltaRot);
		}

		//法線更新
		for (int fn = 0; fn < gi.fn; ++fn)
		{
			const DsVec3d rotN = deltaRot * gi.pFace[fn].normal;//なんフレかに一回求め直したほうがいいかも。
			gi.pFace[fn].normal = DsVec3d::Normalize(rotN);//正規化は毎フレやらなくても良い気がする
		}

		//位置更新
		{
			pi.pos += deltaPos;
		}

		//頂点移動
		for (int vn = 0; vn < gi.vn; ++vn)
		{
			gi.pVertex[vn] = pi.pos + gi.pVertex[vn];
		}
	}
	else if (!deltaPos.IsNearZero())
	{
		//位置更新
		{
			pi.pos += deltaPos;
		}

		//頂点移動
		for (int vn = 0; vn < gi.vn; ++vn)
		{
			gi.pVertex[vn] += deltaPos;
		}
	}

	//重心更新
	//pi.centerOfGravity = _GetCenterOfGravity(gi.vertex, DsRigidBodyGeometryInfo::VERTEX_NUM);
	pi.centerOfGravity = GetPosition();

	//AABB
	{
		_UpdateAabb();
	}

	m_isForceUpdate = false;
	m_isForceRotation = false;
}

//virtual 
void DsRigidBody::_UpdateInertia(const DsMat33d& deltaRot)
{
	DsRigidPhysicsInfo& pi = m_physicsInfo;

	//慣性テンソル更新
	{
		const DsMat33d invR = DsMat33d::Inverse(m_physicsInfo.rotation);
		const DsMat33d rotI = (m_physicsInfo.rotation * pi.mass.inertiaOriginal) * invR;
		//const DsMat33d invR = DsMat33d::Inverse(deltaRot);
		//const DsMat33d rotI = (deltaRot * pi.mass.inertia) * invR;
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
}

//virtual 
void DsRigidBody::_UpdateAabb()
{
	DsVec3d maxP = DsVec3d(-DBL_MAX, -DBL_MAX, -DBL_MAX);
	DsVec3d minP = DsVec3d(DBL_MAX, DBL_MAX, DBL_MAX);
	for (int i = 0; i < m_geomInfo.vn; ++i)
	{
		const DsVec3d v = m_geomInfo.pVertex[i];
		maxP.x = max(maxP.x, v.x);
		maxP.y = max(maxP.y, v.y);
		maxP.z = max(maxP.z, v.z);
		minP.x = min(minP.x, v.x);
		minP.y = min(minP.y, v.y);
		minP.z = min(minP.z, v.z);
	}
	m_aabb.Setup(maxP, minP);
	m_sideSize = (maxP - minP)*0.5;
}

//virtual
void DsRigidBody::_UpdateForRest()
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

//virtual
void DsRigidBody::_IntegralF()
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

//virtual
void DsRigidBody::_IntegralVel()
{
	const double dt = GetDT();
	{//並進
		m_addPos += ((m_physicsInfo.vel) * dt);
		m_physicsInfo.exForce = DsVec3d::Zero();
	}
	{//回転
		m_addRot = DsMat33d::RotateVec(m_physicsInfo.angVel*dt)*m_addRot;
		m_physicsInfo.exTorque = DsVec3d::Zero();
	}

	//拘束用に静的なものでも速度を設定(ここで設定した速度は_IntegralFで０になるので位置に反映されない)
	if (RefOption().isStatic) {
		const DsVec3d vel = (m_physicsInfo.pos - m_prePos)/dt;
		m_physicsInfo.vel = vel;
		m_prePos = m_physicsInfo.pos;

		//回転はよく分からん
		//const DsMat33d dr = m_preRot.ToTransposition()*m_physicsInfo.rotation;
		//m_preRot = m_physicsInfo.rotation;
	}
}


//virtual 
void DsRigidBody::AddForce(const DsVec3d& force)
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
void DsRigidBody::AddTorque(const DsVec3d& torque )
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
void DsRigidBody::AddConstraintForce(const DsVec3d& f, const DsVec3d& t)
{
	m_constraintF += f;
	m_constraintT += t;
}

//virtual 
void DsRigidBody::IntegrateConstraintForce()
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
DsVec3d DsRigidBody::CalcVelocity(const DsVec3d& f) const
{
	if (m_option.isStatic)
	{
		return DsVec3d::Zero();
	}
	else
	{
		DsVec3d ret = m_physicsInfo.vel;
		DsVec3d regist = m_physicsInfo.vel * (AIR_DAMPER + m_damperVel);//粘性抵抗
		ret += (( (f / m_physicsInfo.mass.mass) - regist) * GetDT());
		//const double max = 10;
		//ret = DsVec3d(Clamp(ret.x, -max, max), Clamp(ret.y, -max, max), Clamp(ret.z, -max, max));
		return ret;
	}
}

//virtual 
DsVec3d DsRigidBody::CalcAngularVel(const DsVec3d& t) const
{
	if (m_option.isStatic)
	{
		return DsVec3d::Zero();
	}
	else
	{
		DsVec3d ret = m_physicsInfo.angVel;
		DsVec3d regist = m_physicsInfo.angVel * (AIR_DAMPER+m_damperAngVel);
		DsMat33d invInertia = DsMat33d::Inverse(m_physicsInfo.mass.inertia);
		ret += ((invInertia*t) - regist) * GetDT();
		//const double max = 10;
		//ret = DsVec3d(Clamp(ret.x, -max, max), Clamp(ret.y, -max, max), Clamp(ret.z, -max, max));
		return ret;
	}
}
