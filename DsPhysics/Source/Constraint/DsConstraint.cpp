#include "DsPhysicsPch.h"
#ifndef __DS_CONSTRAINT__
#include "Constraint/DsConstraint.h"
#endif
#include "Actor/DsActor.h"
#include "DsMass.h"
#include "DsPhysicsWorld.h"


using namespace DsPhysics;


DsConstraint::DsConstraint(const DsActorId masterId, const DsActorId subId, const DsPhysicsWorld& world, const DsVec3d& pos, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt)
	: m_masterId(masterId)
	, m_subId(subId)
	, m_world(world)
	, m_pos(pos)
	, m_ex(ex)
	, m_ey(ey)
	, m_ez(ez)
	, m_dt(dt)
	, m_errorP(DsVec3d::Zero())
	, m_masterF(DsVec3d::Zero())
	, m_masterT(DsVec3d::Zero())
	, m_subF(DsVec3d::Zero())
	, m_subT(DsVec3d::Zero())
	, m_eq(this)
{
}

DsConstraint::DsConstraint(const DsActorId masterId, const DsActorId subId, const DsPhysicsWorld& world, const DsVec3d& pos, const DsVec3d& ez, const double dt)
	: m_masterId(masterId)
	, m_subId(subId)
	, m_world(world)
	, m_pos(pos)
	, m_ex(DsVec3d::Zero())
	, m_ey(DsVec3d::Zero())
	, m_ez(ez)
	, m_dt(dt)
	, m_errorP(DsVec3d::Zero())
	, m_masterF(DsVec3d::Zero())
	, m_masterT(DsVec3d::Zero())
	, m_subF(DsVec3d::Zero())
	, m_subT(DsVec3d::Zero())
	, m_eq(this)
{
	ez.GetVerticalPlane(m_ex, m_ey);
}

DsConstraint::DsConstraint(const DsPhysicsWorld& world)
	: m_masterId()
	, m_subId()
	, m_world(world)
	, m_pos(DsVec3d::Zero())
	, m_ex(DsVec3d::Zero())
	, m_ey(DsVec3d::Zero())
	, m_ez(DsVec3d::Zero())
	, m_dt(0)
	, m_errorP(DsVec3d::Zero())
	, m_masterF(DsVec3d::Zero())
	, m_masterT(DsVec3d::Zero())
	, m_subF(DsVec3d::Zero())
	, m_subT(DsVec3d::Zero())
	, m_eq(this)
{

}


void DsConstraint::SetUp()
{
	DsActor* pMas = m_world.GetActor(m_masterId);
	DsActor* pSub = m_world.GetActor(m_subId);

	if (pMas && pSub)
	{
		const DsVec3d& posA = pMas->GetPositionForConstraint();
		const DsVec3d& posB = pSub->GetPositionForConstraint();
		const DsVec3d ra = m_pos - posA;
		const DsVec3d rb = m_pos - posB;
		const DsVec3d ra_ex = DsVec3d::Cross(ra, m_ex);
		const DsVec3d ra_ey = DsVec3d::Cross(ra, m_ey);
		const DsVec3d ra_ez = DsVec3d::Cross(ra, m_ez);
		const DsVec3d rb_ex = DsVec3d::Cross(rb, m_ex);
		const DsVec3d rb_ey = DsVec3d::Cross(rb, m_ey);
		const DsVec3d rb_ez = DsVec3d::Cross(rb, m_ez);

		m_J[0][0] = -m_ex.x; m_J[0][1] = -m_ex.y; m_J[0][2] = -m_ex.z; m_J[0][3] = -ra_ex.x; m_J[0][4] = -ra_ex.y; m_J[0][5] = -ra_ex.z; m_J[0][6] = m_ex.x; m_J[0][7] = m_ex.y; m_J[0][8] = m_ex.z; m_J[0][9] = rb_ex.x; m_J[0][10] = rb_ex.y; m_J[0][11] = rb_ex.z;
		m_J[1][0] = -m_ey.x; m_J[1][1] = -m_ey.y; m_J[1][2] = -m_ey.z; m_J[1][3] = -ra_ey.x; m_J[1][4] = -ra_ey.y; m_J[1][5] = -ra_ey.z; m_J[1][6] = m_ey.x; m_J[1][7] = m_ey.y; m_J[1][8] = m_ey.z; m_J[1][9] = rb_ey.x; m_J[1][10] = rb_ey.y; m_J[1][11] = rb_ey.z;
		m_J[2][0] = -m_ez.x; m_J[2][1] = -m_ez.y; m_J[2][2] = -m_ez.z; m_J[2][3] = -ra_ez.x; m_J[2][4] = -ra_ez.y; m_J[2][5] = -ra_ez.z; m_J[2][6] = m_ez.x; m_J[2][7] = m_ez.y; m_J[2][8] = m_ez.z; m_J[2][9] = rb_ez.x; m_J[2][10] = rb_ez.y; m_J[2][11] = rb_ez.z;
		m_J[3][0] = 0.0; m_J[3][1] = 0.0; m_J[3][2] = 0.0; m_J[3][3] = -m_ex.x; m_J[3][4] = -m_ex.y; m_J[3][5] = -m_ex.z; m_J[3][6] = 0.0; m_J[3][7] = 0.0; m_J[3][8] = 0.0; m_J[3][9] = m_ex.x; m_J[3][10] = m_ex.y; m_J[3][11] = m_ex.z;
		m_J[4][0] = 0.0; m_J[4][1] = 0.0; m_J[4][2] = 0.0; m_J[4][3] = -m_ey.x; m_J[4][4] = -m_ey.y; m_J[4][5] = -m_ey.z; m_J[4][6] = 0.0; m_J[4][7] = 0.0; m_J[4][8] = 0.0; m_J[4][9] = m_ey.x; m_J[4][10] = m_ey.y; m_J[4][11] = m_ey.z;
		m_J[5][0] = 0.0; m_J[5][1] = 0.0; m_J[5][2] = 0.0; m_J[5][3] = -m_ez.x; m_J[5][4] = -m_ez.y; m_J[5][5] = -m_ez.z; m_J[5][6] = 0.0; m_J[5][7] = 0.0; m_J[5][8] = 0.0; m_J[5][9] = m_ez.x; m_J[5][10] = m_ez.y; m_J[5][11] = m_ez.z;

		for (int i = 0; 6 > i; ++i)
		{
			for (int j = 0; 12 > j; ++j)
			{
				m_Jt[j][i] = m_J[i][j];
			}
		}

		DsVec3d mV = pMas->GetVelocity();
		DsVec3d mW = pMas->GetAngularVel();
		DsVec3d sV = pSub->GetVelocity();
		DsVec3d sW = pSub->GetAngularVel();

		const DsVec3d mF = (pMas->RefOption().isStatic) ? (DsVec3d::Zero()) : (pMas->GetForce());
		const DsVec3d mT = (pMas->RefOption().isStatic) ? (DsVec3d::Zero()) : (pMas->GetTorque());
		const DsVec3d sF = (pSub->RefOption().isStatic) ? (DsVec3d::Zero()) : (pSub->GetForce());
		const DsVec3d sT = (pSub->RefOption().isStatic) ? (DsVec3d::Zero()) : (pSub->GetTorque());

		_SetVel(mV, mW, sV, sW);
		_SetMass(*pMas, *pSub);
		_SetExForce(mF, mT, sF, sT);

		m_masterF = DsVec3d::Zero();
		m_masterT = DsVec3d::Zero();
		m_subF = DsVec3d::Zero();
		m_subT = DsVec3d::Zero();
	}
}

void DsConstraint::_SetVel(const DsVec3d& mV, const DsVec3d& mW, const DsVec3d& sV, const DsVec3d& sW)
{
	m_u[0] = mV.x;
	m_u[1] = mV.y;
	m_u[2] = mV.z;
	m_u[3] = mW.x;
	m_u[4] = mW.y;
	m_u[5] = mW.z;
	m_u[6] = sV.x;
	m_u[7] = sV.y;
	m_u[8] = sV.z;
	m_u[9] = sW.x;
	m_u[10] = sW.y;
	m_u[11] = sW.z;
}

void DsConstraint::_SetMass(const DsActor& master, const DsActor& sub)
{
	const DsMass& m1 = master.GetMass();
	const DsMass& m2 = sub.GetMass();

	for (int i = 0; 12>i; ++i)
	{
		for (int j = 0; 12>j; ++j)
		{
			m_M[i][j] = 0.0;
			m_invM[i][j] = 0.0;
		}
	}

	m_M[0][0] = m1.mass; m_M[1][1] = m1.mass; m_M[2][2] = m1.mass;
	m_M[3][3] = m1.inertia[0]; m_M[3][4] = m1.inertia[1]; m_M[3][5] = m1.inertia[2];
	m_M[4][3] = m1.inertia[3]; m_M[4][4] = m1.inertia[4]; m_M[4][5] = m1.inertia[5];
	m_M[5][3] = m1.inertia[6]; m_M[5][4] = m1.inertia[7]; m_M[5][5] = m1.inertia[8];
	m_M[6][6] = m2.mass; m_M[7][7] = m2.mass; m_M[8][8] = m2.mass;
	m_M[9][9] = m2.inertia[0]; m_M[9][10] = m2.inertia[1]; m_M[9][11] = m2.inertia[2];
	m_M[10][9] = m2.inertia[3]; m_M[10][10] = m2.inertia[4]; m_M[10][11] = m2.inertia[5];
	m_M[11][9] = m2.inertia[6]; m_M[11][10] = m2.inertia[7]; m_M[11][11] = m2.inertia[8];

	//DsInverseMatrix<12>(m_M, m_invM);
	const double* im1 = master.GetMassInv10();
	const double* im2 = sub.GetMassInv10();
	m_invM[0][0] = im1[0];  m_invM[1][1] = im1[0];    m_invM[2][2] = im1[0];
	m_invM[3][3] = im1[1];  m_invM[3][4] = im1[2];    m_invM[3][5] = im1[3];
	m_invM[4][3] = im1[4];  m_invM[4][4] = im1[5];    m_invM[4][5] = im1[6];
	m_invM[5][3] = im1[7];  m_invM[5][4] = im1[8];    m_invM[5][5] = im1[9];
	m_invM[6][6] = im2[0];  m_invM[7][7] = im2[0];    m_invM[8][8] = im2[0];
	m_invM[9][9] = im2[1];  m_invM[9][10] = im2[2];    m_invM[9][11] = im2[3];
	m_invM[10][9] = im2[4];  m_invM[10][10] = im2[5];    m_invM[10][11] = im2[6];
	m_invM[11][9] = im2[7];  m_invM[11][10] = im2[8];    m_invM[11][11] = im2[9];
}

void DsConstraint::_SetExForce(const DsVec3d& mF, const DsVec3d& mT, const DsVec3d& sF, const DsVec3d& sT)
{
	//ŠO—Í
	m_Fe[0] = mF.x; m_Fe[1] = mF.y; m_Fe[2] = mF.z; m_Fe[3] = mT.x; m_Fe[4] = mT.y; m_Fe[5] = mT.z;
	m_Fe[6] = sF.x; m_Fe[7] = sF.y; m_Fe[8] = sF.z; m_Fe[9] = sT.x; m_Fe[10] = sT.y; m_Fe[11] = sT.z;
}

void DsConstraint::_CalcConatraintForce()
{
	
	double JinvM[6][12];
	DsMathUtil::MultiMat<6, 12>(JinvM, m_J, m_invM);
	
	double JinvMJt[6][6];
	DsMathUtil::MultiMat<6, 6>(JinvMJt, JinvM, m_Jt);

	double JinvMFe[6];
	DsMathUtil::MultiVec<6, 12>(JinvMFe, JinvM, m_Fe);

	double w[6];
	DsMathUtil::MultiVec<6, 12>(w, m_J, m_u);

	//w[t+1] = w[t] + JinvMJt*dt*ƒÉ + JinvMFe*dt
	//w[t+1] = AƒÉ+b
	//A = JinvMJt * dt
	//b = w[t] + JinvMFe * dt

	double A[6][6];
	for (int i = 0; 6>i; ++i)
	{
		for (int j = 0; 6>j; ++j)
		{
			A[i][j] = JinvMJt[i][j] * m_dt;
		}
	}
	
	double b[6] = { w[0] + JinvMFe[0] * m_dt, w[1] + JinvMFe[1] * m_dt, w[2] + JinvMFe[2] * m_dt,
		w[3] + JinvMFe[3] * m_dt, w[4] + JinvMFe[4] * m_dt, w[5] + JinvMFe[5] * m_dt };


	
	for (int i = 0; i < 6; ++i){
		RefEq().b[i] = b[i];
		for (int j = 0; j < 6; ++j){
			RefEq().A[i][j] = A[i][j];
		}
	}

	//‚±‚±‚Ü‚Å‚Í‚·‚×‚Ä‚ÌS‘©‚Å‹¤’Ê

	_SetEquation(w);
}

//virtual 
void DsConstraint::_SetEquation(const double w[6])
{
}


//virtual 
void DsConstraint::CalclateConstraintForce()
{
	_CalcConatraintForce();
}

//virtual 
void DsConstraint::ApplyConstraintForce()
{
	DsActor* pMas = m_world.GetActor(m_masterId);
	DsActor* pSub = m_world.GetActor(m_subId);

	if (pMas && pSub)
	{
		double Fc[12];
		DsMathUtil::MultiVec<12, 6>(Fc, m_Jt, RefEq().lambda);

		m_masterF.x = Fc[0];
		m_masterF.y = Fc[1];
		m_masterF.z = Fc[2];
		m_masterT.x = Fc[3];
		m_masterT.y = Fc[4];
		m_masterT.z = Fc[5];
		m_subF.x =    Fc[6];
		m_subF.y =    Fc[7];
		m_subF.z =    Fc[8];
		m_subT.x =    Fc[9];
		m_subT.y =    Fc[10];
		m_subT.z =    Fc[11];

		pMas->AddForce(m_masterF);
		pMas->AddTorque(m_masterT);
		pSub->AddForce(m_subF);
		pSub->AddTorque(m_subT);
	}
}




