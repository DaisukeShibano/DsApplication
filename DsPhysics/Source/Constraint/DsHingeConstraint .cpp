#include "DsPhysicsPch.h"
#ifndef __DS_HINGE_CONSTRAINT__
#include "Constraint/DsHingeConstraint.h"
#endif
#include "DsMass.h"
#include "DsPhysicsWorld.h"

using namespace DsLib;
using namespace DsPhysics;


DsHingeConstraint::DsHingeConstraint(const DsActorId masterId, const DsActorId subId, const DsPhysicsWorld& world, const DsVec3d& posE, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt, const DsVec3d& errorP, const DsMat33d& errorR)
	:DsBallConstraint(masterId, subId, world, posE, ex, ey, ez, dt, errorP, errorR)
	, m_axisMas(ez)
	, m_axisSub(ez)
	, m_lambda()
{
	//z������]��

}
DsHingeConstraint::DsHingeConstraint(const DsPhysicsWorld& world)
	:DsBallConstraint(world)
	, m_axisMas(DsVec3d::Zero())
	, m_axisSub(DsVec3d::Zero())
	, m_lambda()
{
}

void DsHingeConstraint::SetAxis(const DsVec3d& maserAxis, const DsVec3d& subAxis)
{
	m_axisMas = maserAxis;
	m_axisSub = subAxis;
}

//virtual
void DsHingeConstraint::ClampConstraint()
{
	DsActor* pMas = m_world.GetActor(m_masterId);
	DsActor* pSub = m_world.GetActor(m_subId);

	if (pMas && pSub)
	{
		double Fc[12];
		DsMathUtil::MultiVec<12, 6>(Fc, m_Jt, m_lambda);
		DsVec3d masterF;
		DsVec3d masterT;
		DsVec3d subF;
		DsVec3d subT;
		masterF.x = Fc[0];
		masterF.y = Fc[1];
		masterF.z = Fc[2];
		masterT.x = Fc[3];
		masterT.y = Fc[4];
		masterT.z = Fc[5];
		subF.x = Fc[6];
		subF.y = Fc[7];
		subF.z = Fc[8];
		subT.x = Fc[9];
		subT.y = Fc[10];
		subT.z = Fc[11];

		pMas->AddForce(-masterF);
		pMas->AddTorque(-masterT);
		pSub->AddForce(-subF);
		pSub->AddTorque(-subT);

		for (int i = 0; i < 6; ++i){
			m_lambda[i] = Clamp(m_lambda[i], -10.0, 10.0);
		}

		DsMathUtil::MultiVec<12, 6>(Fc, m_Jt, m_lambda);
		masterF.x = Fc[0];
		masterF.y = Fc[1];
		masterF.z = Fc[2];
		masterT.x = Fc[3];
		masterT.y = Fc[4];
		masterT.z = Fc[5];
		subF.x = Fc[6];
		subF.y = Fc[7];
		subF.z = Fc[8];
		subT.x = Fc[9];
		subT.y = Fc[10];
		subT.z = Fc[11];

		pMas->AddForce(masterF);
		pMas->AddTorque(masterT);
		pSub->AddForce(subF);
		pSub->AddTorque(subT);

		for (int i = 0; i < 6; ++i){
			m_lambda[i] = 0;
		}
	}
}

//virtual 
void DsHingeConstraint::ApplyConstraintForce()
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

		//ClampConstraint�Ńe�X�g�Ŏg���Ă���B����ȊO�͊��Ɠ���
		for (int i = 0; i < 6; ++i){
			m_lambda[i] += RefEq().lambda[i];
		}

		pMas->AddForce(m_masterF);
		pMas->AddTorque(m_masterT);
		pSub->AddForce(m_subF);
		pSub->AddTorque(m_subT);
	}
}

//virtual
void DsHingeConstraint::SetUp()
{
	DsConstraint::SetUp();
	double eRX = 0;
	double eRY = 0;
	const double diffZ = DsVec3d::Dot(m_axisMas, m_axisSub);
	if (fabs(diffZ) < 0.999)//��]���ł���z�����Ɍ덷��������
	{
		const DsVec3d error = DsVec3d::Cross(m_axisMas, m_axisSub);
		eRX = DsVec3d::Dot(m_ex, error);
		eRY = DsVec3d::Dot(m_ey, error);
	}
	const DsVec3d eP = { m_errorP.x, m_errorP.y, m_errorP.z };
	const double kp = 0.2;//�덷�C���Q�C��
	const double kr = 0.2;//�덷�C���Q�C���B��]
	const double e[5] = { DsVec3d::Dot(m_ex, eP), DsVec3d::Dot(m_ey, eP), DsVec3d::Dot(m_ez, eP), eRX, eRY };
	const double ke[5] = { -e[0] * kp / m_dt, -e[1] * kp / m_dt, -e[2] * kp / m_dt, -e[3] * kr / m_dt, -e[4] * kr / m_dt }; //K=1�łP�t���Ō덷���C�����鑬�x�B

	//�ڕW���x���������Ă��鐬������
	//[5]���s�肾���A�g���N�͋��܂��Ă���̂Ŏg���邱�Ƃ͂Ȃ�
	RefEq().min_wplus1[0] = ke[0];
	RefEq().min_wplus1[1] = ke[1];
	RefEq().min_wplus1[2] = ke[2];
	RefEq().min_wplus1[3] = ke[3];
	RefEq().min_wplus1[4] = ke[4];
	RefEq().max_wplus1[0] = ke[0];
	RefEq().max_wplus1[1] = ke[1];
	RefEq().max_wplus1[2] = ke[2];
	RefEq().max_wplus1[3] = ke[3];
	RefEq().max_wplus1[4] = ke[4];
}

//virtual 
void DsHingeConstraint::_SetEquation(const double w[6])
{
	//�q���W��z���̃g���N�������m�B����ȊO�͖��m
	RefEq().isConstLamdba[0] = false;
	RefEq().isConstLamdba[1] = false;
	RefEq().isConstLamdba[2] = false;
	RefEq().isConstLamdba[3] = false;
	RefEq().isConstLamdba[4] = false;
	RefEq().isConstLamdba[5] = true;

	//z���̃g���N�͂킩���Ă���̂ő��
	//1,2,3,4,5�͑O��̒l�������l�Ƃ��Ďg���̂ŃN���A���Ȃ�
	//RefEq().lambda[0] = 0;
	//RefEq().lambda[1] = 0;
	//RefEq().lambda[2] = 0;
	//RefEq().lambda[3] = 0;
	//RefEq().lambda[4] = 0;
	RefEq().lambda[5] = 0;

	const double maxLambda = FLT_MAX;
	if (1){//�ꔭ�ŉ���
		//A^1�����߂�
		double A55[5][5];
		for (int i = 0; i < 5; ++i){
			for (int j = 0; j < 5; ++j){
				A55[i][j] = RefEq().A[i][j];
			}
		}
		double invA55[5][5];
		const bool isInvOK = DsInverseMatrix<5, double>(A55, invA55);
		if (!isInvOK){
			return;//�t�s�񋁂߂��Ȃ������̂ōS���͂͂������ɏI���
		}

		const double* pDbgLambda = RefEq().lambda;

		const double wt1_b5[5] = {
			RefEq().max_wplus1[0] - RefEq().b[0],
			RefEq().max_wplus1[1] - RefEq().b[1],
			RefEq().max_wplus1[2] - RefEq().b[2],
			RefEq().max_wplus1[3] - RefEq().b[3],
			RefEq().max_wplus1[4] - RefEq().b[4],
		};

		double lambda5[5];
		DsMathUtil::MultiVec<5, 5>(lambda5, invA55, wt1_b5);
		RefEq().lambda[0] = Clamp(lambda5[0], -maxLambda, maxLambda);
		RefEq().lambda[1] = Clamp(lambda5[1], -maxLambda, maxLambda);
		RefEq().lambda[2] = Clamp(lambda5[2], -maxLambda, maxLambda);
		RefEq().lambda[3] = Clamp(lambda5[3], -maxLambda, maxLambda);
		RefEq().lambda[4] = Clamp(lambda5[4], -maxLambda, maxLambda);
	}
	else{//��񕪂̌J��Ԃ��B
		DsConstraintEquation& eq = RefEq();
		//�P�s������6*6����̂ł���������Ă����BA�̒��̍s���[�v
		for (int row = 0; row < 6; ++row){	
			if (!eq.isConstLamdba[row]){//�ɂ��Œ�l�̍s�͌v�Z���Ȃ��Ă����ɋ��܂��Ă�̂ŏ��O

				if (0.0000001 < fabs(eq.A[row][row]))
				{
					double forward = 0;//�X�V�ς�K+1
					for (int col = 0; col < row; ++col){//����A�����̗�̃��[�v�Bforward�́A���ɉ����Ă���row�����܂ŉ��Z
						forward += eq.A[row][col] * eq.lambda[col];
					}

					double back = 0;//���X�V��k
					for (int col = row + 1; col < 6; ++col){ //forward+�������X�L�b�v���Ă���ȍ~�̗�����Z
						back += eq.A[row][col] * eq.lambda[col];
					}

					//dst[col] = (srcVec[col] - forward - back) / srcMat[col][col];
					const double w_b = eq.max_wplus1[row] - eq.b[row];
					const double weight = 1.0;
					const double old_lamdba = eq.lambda[row];
					double new_lambda = (w_b - forward - back) / eq.A[row][row];
					eq.lambda[row] = old_lamdba + weight*( new_lambda - old_lamdba );//SOR�@�B�K�E�X�U�C�f���̉��̕ω��ʂ�weight�{����
					
					eq.lambda[row] = Clamp(eq.lambda[row], -maxLambda, maxLambda);
				}
			}
		}
	}
}
