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
			m_fricVel = virticalVel * (-1.0);//���x�Ƃ͔��Ε����ɖ��C����������
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
	//if ( pMas && pSub ){//�Œ���߂荞�܂Ȃ��͂����߂�
	//	const DsVec3d n = m_colNormal*(-1.0);//�Փ˕����Ɣ��΂��i�s����
	//	const DsVec3d a = n*DsVec3d::Dot(pMas->GetForce(), n) / pMas->GetMass().mass;//�Փ˕����̉����x
	//	const DsVec3d v = n*DsVec3d::Dot(pMas->GetVelocity(), n);//�Փ˕����̑��x
	//	const DsVec3d dp = ((v + a*m_dt)*m_dt) + (n*m_error); //����dt�Ői�ނ߂荞�ݕ����̋����B���̋�����ł�����
	//	const DsVec3d reF = (( v - (-dp/m_dt) )/m_dt - a)*pMas->GetMass().mass;
	//
	//	masterF += -reF;
	//	subF += reF;
	//
	//
	//	//////���x�������ɑł�����
	//	//const DsVec3d vm = n*DsVec3d::Dot(pMas->GetVelocity(), n);//�Փ˕����̑��x
	//	//pMas->SetVelocity(pMas->GetVelocity()-vm);
	//	//const DsVec3d vs = n*DsVec3d::Dot(pSub->GetVelocity(), n);//�Փ˕����̑��x
	//	//pSub->SetVelocity(pSub->GetVelocity()-vs);
	//	////�͂������ɑł�����
	//	//const DsVec3d fm = n*DsVec3d::Dot(pMas->GetForce(), n);//�Փ˕����̗�
	//	//pMas->SetForce(pMas->GetForce() - fm);
	//	//const DsVec3d fs = n*DsVec3d::Dot(pSub->GetForce(), n);//�Փ˕����̗�
	//	//pSub->SetForce(pSub->GetForce() - fs);
	//}


	{//�Փ˕����̌���
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

	//�K���ɖ��C�����ɗ͂�����
	{
		const double fircCoef = 0.4;
		const DsVec3d fricDir = DsVec3d::Normalize(m_fricVel);
		const DsVec3d fricMasF = fricDir*((masterF.Length()*fircCoef));
		const DsVec3d fricSubF = -fricMasF;
		//�����C
		if (0.01 < m_fricVel.Length())
		{
			masterF += fricMasF;
			masterT += DsVec3d::Cross(m_colMasVec, fricMasF);
			subF += fricSubF;
			subT += DsVec3d::Cross(m_colSubVec, fricSubF);
		}
		//�Î~���C
		else
		{
			////�O�͂����C�͂��������Ƃ��������Z
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
	_SetUp(); //Force�����������Ƃ͖��x�X�V����K�v����

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