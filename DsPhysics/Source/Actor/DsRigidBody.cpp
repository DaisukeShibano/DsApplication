#include "DsPhysicsPch.h"
#ifndef __DS_RIGID_BODY__
#include "Actor/DsRigidBody.h"
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



DsRigidBodyCube::DsRigidBodyCube(const DsActorId& id, const char* name )
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

DsRigidBodyCube::~DsRigidBodyCube()
{
	delete m_pCollisionContext; m_pCollisionContext = NULL;
}

void DsRigidBodyCube::Update()
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

void DsRigidBodyCube::Create(const DsVec3d* pv, const double mass )
{
	DS_ASSERT(!( 0.0 >= mass), "重さが0以下です");

	{
		DsRigidBodyGeometryInfo& gi = m_geomInfo;

		for(int vn=0; DsRigidBodyGeometryInfo::VERTEX_NUM > vn; ++vn )
		{
			gi.vertex[ vn ] = pv[ vn ];
			gi.vertexOriginal[vn] = pv[vn];
		}

		/*
				  3_________2
				 /         /|
			  0 /________1/ |
				|         | |
				|         | | 
				|	7	  | / 6      y
				|_________|/         |__ x 
				4         5          /
									z
		*/

		
		//反時計回り
		gi.face[0].index[0] = 0;	gi.face[0].index[1] = 1;	gi.face[0].index[2] = 2;	gi.face[0].index[3] = 3;	//上面
		gi.face[1].index[0] = 4;	gi.face[1].index[1] = 7;	gi.face[1].index[2] = 6;	gi.face[1].index[3] = 5;	//下面
		gi.face[2].index[0] = 0;	gi.face[2].index[1] = 4;	gi.face[2].index[2] = 5;	gi.face[2].index[3] = 1;	//前面
		gi.face[3].index[0] = 3;	gi.face[3].index[1] = 2;	gi.face[3].index[2] = 6;	gi.face[3].index[3] = 7;	//後面
		gi.face[4].index[0] = 1;	gi.face[4].index[1] = 5;	gi.face[4].index[2] = 6;	gi.face[4].index[3] = 2;	//右面
		gi.face[5].index[0] = 0;	gi.face[5].index[1] = 3;	gi.face[5].index[2] = 7;	gi.face[5].index[3] = 4;	//左面
	
		//法線
		for(int fn=0; DsRigidBodyGeometryInfo::FACE_NUM > fn; ++fn)
		{
			const DsVec3d normal = DsVec3d::Cross( pv[gi.face[fn].index[1]] - pv[gi.face[fn].index[0]], pv[gi.face[fn].index[3]] - pv[gi.face[fn].index[0]] );
			gi.face[fn].normal = DsVec3d::Normalize(normal);
		}

		gi.line[0].index[0] = 0;	gi.line[0].index[1] = 1;	//上面
		gi.line[1].index[0] = 1;	gi.line[1].index[1] = 2;
		gi.line[2].index[0] = 2;	gi.line[2].index[1] = 3;
		gi.line[3].index[0] = 3;	gi.line[3].index[1] = 0;
		gi.line[4].index[0] = 0;	gi.line[4].index[1] = 4;	//縦
		gi.line[5].index[0] = 1;	gi.line[5].index[1] = 5;
		gi.line[6].index[0] = 2;	gi.line[6].index[1] = 6;
		gi.line[7].index[0] = 3;	gi.line[7].index[1] = 7;
		gi.line[8].index[0] = 4;	gi.line[8].index[1] = 7;	//下面
		gi.line[9].index[0] = 7;	gi.line[9].index[1] = 6;
		gi.line[10].index[0] = 6;	gi.line[10].index[1] = 5;
		gi.line[11].index[0] = 5;	gi.line[11].index[1] = 4;
	}

	{
		DsRigidBodyPhysicsInfo& pi = m_physicsInfo;

		//mass
		{
			const double M = m_option.isStatic ? DsMathUtil::DS_INFINITY_D : mass;
			const DsVec3d maxLength = _GetMaxVector(pv, DsRigidBodyGeometryInfo::VERTEX_NUM);
			const double bias = 1.0;
			const double Ixx = M / 3.0 * (maxLength.y*maxLength.y + maxLength.z*maxLength.z) * bias;
			const double Iyy = M / 3.0 * (maxLength.x*maxLength.x + maxLength.z*maxLength.z) * bias;
			const double Izz = M / 3.0 * (maxLength.x*maxLength.x + maxLength.y*maxLength.y) * bias;
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
		DsRigidBodyGeometryInfo& gi = m_geomInfo;
		//pi.centerOfGravity = _GetCenterOfGravity(gi.vertex, DsRigidBodyGeometryInfo::VERTEX_NUM);
		pi.centerOfGravity = GetPosition();

		
		//AABB
		DsVec3d maxLen = DsVec3d::Zero();
		for (int i = 0; i < DsRigidBodyGeometryInfo::VERTEX_NUM; ++i)
		{
			const DsVec3d len = gi.vertex[i] - pi.centerOfGravity;
			maxLen.x = max(maxLen.x, fabs(len.x));
			maxLen.y = max(maxLen.y, fabs(len.y));
			maxLen.z = max(maxLen.z, fabs(len.z));
		}
		m_aabb.Setup(maxLen.x, maxLen.y, maxLen.z, pi.centerOfGravity);
		m_sideSize.x = maxLen.x;
		m_sideSize.y = maxLen.y;
		m_sideSize.z = maxLen.z;
	}

	for (int vn = 0; vn < DsRigidBodyGeometryInfo::VERTEX_NUM; ++vn)
	{
		m_geomInfo.preVertex[vn] = m_geomInfo.vertex[vn];
	}

	//静的オブジェでも最初の一回だけUpdateしないと初期姿勢が反映されない
	m_isForceUpdate = true;
	m_isForceRotation = true;

	_Update(m_initPos, m_initRot);

	m_pCollisionContext = new DsCollisionContext(m_geomInfo.vertex, DsRigidBodyGeometryInfo::VERTEX_NUM, m_geomInfo.face, DsRigidBodyGeometryInfo::FACE_NUM,
		m_geomInfo.line, DsRigidBodyGeometryInfo::LINE_NUM, GetId(), m_physicsInfo.centerOfGravity, m_geomInfo.preVertex, m_sideSize, NULL, &m_aabb, GetRotation());
}


void DsRigidBodyCube::_Update( const DsVec3d& deltaPos, const DsMat33d& deltaRot )
{
	if (m_option.isStatic && (!m_isForceUpdate) && (!m_isForceRotation))
	{
		return;
	}

	DsRigidBodyGeometryInfo& gi = m_geomInfo;
	DsRigidBodyPhysicsInfo& pi = m_physicsInfo;

	//前回の頂点更新
	for (int vn = 0; vn < DsRigidBodyGeometryInfo::VERTEX_NUM; ++vn)
	{
		m_geomInfo.preVertex[vn] = m_geomInfo.vertex[vn];
	}

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
		for (int vn = 0; DsRigidBodyGeometryInfo::VERTEX_NUM > vn; ++vn)
		{
			gi.vertex[vn] = pi.rotation * gi.vertexOriginal[vn];
		}

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

		//法線更新
		for (int fn = 0; DsRigidBodyGeometryInfo::FACE_NUM > fn; ++fn)
		{
			const DsVec3d rotN = deltaRot * gi.face[fn].normal;
			gi.face[fn].normal = DsVec3d::Normalize(rotN);
		}

		//位置更新
		{
			pi.pos += deltaPos;
		}

		//頂点移動
		for (int vn = 0; DsRigidBodyGeometryInfo::VERTEX_NUM > vn; ++vn)
		{
			gi.vertex[vn] = pi.pos + gi.vertex[vn];
		}
	}
	else if (!deltaPos.IsNearZero())
	{
		//位置更新
		{
			pi.pos += deltaPos;
		}

		//頂点移動
		for (int vn = 0; DsRigidBodyGeometryInfo::VERTEX_NUM > vn; ++vn)
		{
			gi.vertex[vn] += deltaPos;
		}
	}

	

	//重心更新
	//pi.centerOfGravity = _GetCenterOfGravity(gi.vertex, DsRigidBodyGeometryInfo::VERTEX_NUM);
	pi.centerOfGravity = GetPosition();//中心って決まってるし、計算の無駄だし、頂点数に依存してしまうので。

	//AABB
	{
		if (isChangeRot)
		{
			DsVec3d maxLen = DsVec3d::Zero();
			for (int i = 0; i < DsRigidBodyGeometryInfo::VERTEX_NUM; ++i)
			{
				const DsVec3d len = gi.vertex[i] - pi.centerOfGravity;
				maxLen.x = max(maxLen.x, fabs(len.x));
				maxLen.y = max(maxLen.y, fabs(len.y));
				maxLen.z = max(maxLen.z, fabs(len.z));
			}
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

void DsRigidBodyCube::_UpdateForRest()
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

void DsRigidBodyCube::_IntegralF()
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
void DsRigidBodyCube::_IntegralVel()
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
void DsRigidBodyCube::Draw(DsDrawCommand& com)
{
	const DsVec3d* pVertex = GetVertex();
	const int faceNum = GetFaceNum();
	for (int fIdx = 0; faceNum > fIdx; ++fIdx)
	{
		const DsQuad& face = GetFace(fIdx);
		//色は外側で決める
		if (!RefOption().isDrawWireFrame)
		{
			if (0.00000001 < m_dbgColor.w){//アルファ値が０なら描画しない
				com.SetColor(m_dbgColor);
				com.DrawQuad(pVertex[face.index[0]], pVertex[face.index[1]], pVertex[face.index[2]], pVertex[face.index[3]]);
			}
			com.SetColor(m_dbgLineColor);
			com.DrawLine(pVertex[face.index[0]], pVertex[face.index[1]]);
			com.DrawLine(pVertex[face.index[1]], pVertex[face.index[2]]);
			com.DrawLine(pVertex[face.index[3]], pVertex[face.index[0]]);
		}
		else
		{
			com.DrawLine(pVertex[face.index[0]], pVertex[face.index[1]]);
			com.DrawLine(pVertex[face.index[1]], pVertex[face.index[2]]);
			com.DrawLine(pVertex[face.index[3]], pVertex[face.index[0]]);
		}
	}
}


void DsRigidBodyCube::GetVertex( DsVec3d* pv, double xl, double yl, double zl)
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
void DsRigidBodyCube::AddForce(const DsVec3d& force)
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
void DsRigidBodyCube::AddTorque(const DsVec3d& torque )
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
void DsRigidBodyCube::AddConstraintForce(const DsVec3d& f, const DsVec3d& t)
{
	m_constraintF += f;
	m_constraintT += t;
}

//virtual 
void DsRigidBodyCube::IntegrateConstraintForce()
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
DsVec3d DsRigidBodyCube::CalcVelocity(const DsVec3d& f) const
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
DsVec3d DsRigidBodyCube::CalcAngularVel(const DsVec3d& t) const
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
DsActor* DsRigidBodyCube::DsRigidBodyFactory::CreateIns( const DsActorId& id ) const
{
	DsRigidBodyCube* pRet = new DsRigidBodyCube(id, m_name.c_str());
	if(pRet)
	{
		pRet->m_option = m_initOption;
		pRet->m_initPos = m_initPos;
		pRet->m_initRot = m_initRot;
		pRet->Create(m_pv, m_mass);
		return pRet;
	}
	else
	{
		return NULL;
	}
}