#include "DsPhysicsPch.h"
#include "Constraint/DsImpulseConstraint.h"
#include "DsMass.h"
#include "DsPhysicsWorld.h"

using namespace DsPhysics;



DsImpulseConstraint::DsImpulseConstraint(const DsActorId master, const DsActorId sub, const DsPhysicsWorld& world, const DsVec3d& colPos, const DsVec3d& colNormal, const double dt, const double error)
	:m_masterId(master)
	, m_subId(sub)
	, m_world(world)
	, m_colPos(colPos)
	, m_colNormal(colNormal)
	, m_dt(dt)
	, m_error(error)
	, m_e1(0.0)
	, m_e2(0.0)
	, m_colVel()
	, m_fricVel()
	, m_colMasVec()
	, m_colSubVec()
	, m_exMasF()
	, m_exSubF()
{

}

void DsImpulseConstraint::_SetUp()
{

	DsActor* pMas = m_world.GetActor(m_masterId);
	DsActor* pSub = m_world.GetActor(m_subId);

	if (pMas && pSub)
	{
		const DsVec3d& posA = pMas->GetPosition();
		const DsVec3d& posB = pSub->GetPosition();
		const DsVec3d ra = m_colPos - posA;
		const DsVec3d rb = m_colPos - posB;
		
		m_e1 = pMas->GetMaterial().m_bundCoef;
		m_e2 = pSub->GetMaterial().m_bundCoef;

		{
			const DsVec3d rVel = (pMas->GetVelocity() + DsVec3d::Cross(pMas->GetAngularVel(), ra)) - (pSub->GetVelocity() + DsVec3d::Cross(pSub->GetAngularVel(), rb));
			const DsVec3d colDirVel = m_colNormal *  DsVec3d::Dot(rVel, m_colNormal);
			const DsVec3d virticalVel = rVel - colDirVel;
			m_colVel = colDirVel;
			m_fricVel = virticalVel * (-1.0);//速度とは反対方向に摩擦が発生する
			m_colMasVec = ra;
			m_colSubVec = rb;
		}

		m_exMasF = pMas->GetForce();
		m_exSubF = pMas->GetForce();
	}
}


void DsImpulseConstraint::_CalcConatraintForce(DsVec3d& masterF, DsVec3d& masterT, DsVec3d& subF, DsVec3d& subT)
{
	//DsActor* pMas = m_world.GetActor(m_masterId);
	//DsActor* pSub = m_world.GetActor(m_subId);
	//if ( pMas && pSub ){//最低限めり込まない力を求める
	//	const DsVec3d n = m_colNormal*(-1.0);//衝突方向と反対が進行方向
	//	const DsVec3d a = n*DsVec3d::Dot(pMas->GetForce(), n) / pMas->GetMass().mass;//衝突方向の加速度
	//	const DsVec3d v = n*DsVec3d::Dot(pMas->GetVelocity(), n);//衝突方向の速度
	//	const DsVec3d dp = ((v + a*m_dt)*m_dt) + (n*m_error); //次のdtで進むめり込み方向の距離。この距離を打ち消す
	//	const DsVec3d reF = (( v - (-dp/m_dt) )/m_dt - a)*pMas->GetMass().mass;
	//
	//	masterF += -reF;
	//	subF += reF;
	//
	//
	//	//////速度を強引に打ち消す
	//	//const DsVec3d vm = n*DsVec3d::Dot(pMas->GetVelocity(), n);//衝突方向の速度
	//	//pMas->SetVelocity(pMas->GetVelocity()-vm);
	//	//const DsVec3d vs = n*DsVec3d::Dot(pSub->GetVelocity(), n);//衝突方向の速度
	//	//pSub->SetVelocity(pSub->GetVelocity()-vs);
	//	////力も強引に打ち消す
	//	//const DsVec3d fm = n*DsVec3d::Dot(pMas->GetForce(), n);//衝突方向の力
	//	//pMas->SetForce(pMas->GetForce() - fm);
	//	//const DsVec3d fs = n*DsVec3d::Dot(pSub->GetForce(), n);//衝突方向の力
	//	//pSub->SetForce(pSub->GetForce() - fs);
	//}


	{//衝突方向の撃力
		const double coefK = 160.0;
		const double coefC = 8.0;

		const DsVec3d spring = m_colNormal*m_error*coefK;
		const DsVec3d dumper = m_colVel*coefC;
		masterF += spring - dumper;
		subF += -masterF;

		masterT = DsVec3d::Cross(m_colMasVec, masterF);
		subT = DsVec3d::Cross(m_colSubVec, subF);
	}
	return;

	//適当に摩擦方向に力かける
	{
		const double fircCoef = 0.4;
		const DsVec3d fricDir = DsVec3d::Normalize(m_fricVel);
		const DsVec3d fricMasF = fricDir*((masterF.Length()*fircCoef));
		const DsVec3d fricSubF = -fricMasF;
		//動摩擦
		if (0.01 < m_fricVel.Length())
		{
			masterF += fricMasF;
			masterT += DsVec3d::Cross(m_colMasVec, fricMasF);
			subF += fricSubF;
			subT += DsVec3d::Cross(m_colSubVec, fricSubF);
		}
		//静止摩擦
		else
		{
			////外力が摩擦力を上回ったときだけ加算
			//if ( fabs(DsVec3d::Dot(m_exMasF+m_exSubF, m_colNormal)) > fricMasF.Length())
			//{
			//	masterF += fricMasF;
			//	//masterT += DsVec3d::Cross(m_colMasVec, fricMasF);
			//	subF += fricSubF;
			//	//subT += DsVec3d::Cross(m_colSubVec, fricSubF);
			//}
		}
	}
}

void DsImpulseConstraint::CalclateConstraintForce(int iteNum)
{
	_SetUp(); //Forceを加えたあとは毎度更新する必要あり

	DsActor* pMas = m_world.GetActor(m_masterId);
	DsActor* pSub = m_world.GetActor(m_subId);

	if (pMas && pSub)
	{
		DsVec3d masterF = DsVec3d::Zero();
		DsVec3d masterT = DsVec3d::Zero();
		DsVec3d subF = DsVec3d::Zero();
		DsVec3d subT = DsVec3d::Zero();
		_CalcConatraintForce(masterF, masterT, subF, subT);
		pMas->AddForce(masterF);
		pMas->AddTorque(masterT);
		pSub->AddForce(subF);
		pSub->AddTorque(subT);
	}
}