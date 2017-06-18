#include "DsPhysicsPch.h"
#ifndef __DS_HINGE2_CONSTRAINT__
#include "Constraint/DsHinge2Constraint.h"
#endif
#include "DsMass.h"
#include "DsPhysicsWorld.h"

using namespace DsLib;
using namespace DsPhysics;


DsHinge2Constraint::DsHinge2Constraint(const DsActorId masterId, const DsActorId subId, const DsPhysicsWorld& world, const DsVec3d& posE, const DsVec3d& ex, const DsVec3d& ey, const DsVec3d& ez, const double dt, const DsVec3d& errorP, const DsMat33d& errorR)
	:DsBallConstraint(masterId, subId, world, posE, ex, ey, ez, dt, errorP, errorR)
	, m_axisMas1(ez)
	, m_axisSub1(ez)
	, m_axisMas2(ey)
	, m_axisSub2(ey)
	, m_lambda()
{
	//z������]��

}
DsHinge2Constraint::DsHinge2Constraint(const DsPhysicsWorld& world)
	:DsBallConstraint(world)
	, m_axisMas1(DsVec3d::Zero())
	, m_axisSub1(DsVec3d::Zero())
	, m_axisMas2(DsVec3d::Zero())
	, m_axisSub2(DsVec3d::Zero())
	, m_lambda()
{
}

//virtual
void DsHinge2Constraint::SetUp()
{
	DsConstraint::SetUp();
	
	double eRX = 0;
	{
		//yz��]�����x��]���������܂�Ă��܂��̂ŉ����덷�Ƃ��邩�̔��ʂ����Ȃ�
		const DsVec3d error = DsVec3d::Cross(m_axisMas1, m_axisSub1);
		eRX = DsVec3d::Dot(m_ex, error);
		
		const DsVec3d error2 = DsVec3d::Cross(m_axisMas2, m_axisSub2);
		double eRX2 = DsVec3d::Dot(m_ex, error2);
		eRX += eRX2;
		
		double check = fabs(fabs(eRX) - fabs(eRX2));
		if (0.00001 < check) {
			DS_ASSERT(true, "");
		}
	}
	
	const DsVec3d eP = { m_errorP.x, m_errorP.y, m_errorP.z };
	const double kp = 0.2;//�덷�C���Q�C��
	const double kr = 0.02;//�덷�C���Q�C���B��]//�����������Ȃ����낤���ǁA�t���[�ɂȂ����Ƃ��������ƌ��ɖ߂���x
	const double e[4] = { DsVec3d::Dot(m_ex, eP), DsVec3d::Dot(m_ey, eP), DsVec3d::Dot(m_ez, eP), eRX};
	const double ke[4] = { -e[0] * kp / m_dt, -e[1] * kp / m_dt, -e[2] * kp / m_dt, -e[3] * kr / m_dt}; //K=1�łP�t���Ō덷���C�����鑬�x�B

	//�ڕW���x���������Ă��鐬������
	//[4][5]���s�肾���A�g���N�͋��܂��Ă���̂Ŏg���邱�Ƃ͂Ȃ�
	RefEq().min_wplus1[0] = ke[0];
	RefEq().min_wplus1[1] = ke[1];
	RefEq().min_wplus1[2] = ke[2];
	RefEq().min_wplus1[3] = ke[3];
	
	RefEq().max_wplus1[0] = ke[0];
	RefEq().max_wplus1[1] = ke[1];
	RefEq().max_wplus1[2] = ke[2];
	RefEq().max_wplus1[3] = ke[3];
}

//virtual 
void DsHinge2Constraint::_SetEquation(const double w[6])
{
	//�q���W��z���̃g���N�������m�B����ȊO�͖��m
	RefEq().isConstLamdba[0] = false;
	RefEq().isConstLamdba[1] = false;
	RefEq().isConstLamdba[2] = false;
	RefEq().isConstLamdba[3] = false;
	RefEq().isConstLamdba[4] = true;
	RefEq().isConstLamdba[5] = true;

	//z���̃g���N�͂킩���Ă���̂ő��
	//1,2,3,4�͑O��̒l�������l�Ƃ��Ďg���̂ŃN���A���Ȃ�
	//RefEq().lambda[0] = 0;
	//RefEq().lambda[1] = 0;
	//RefEq().lambda[2] = 0;
	//RefEq().lambda[3] = 0;
	RefEq().lambda[4] = 0;
	RefEq().lambda[5] = 0;

	const double maxLambda = FLT_MAX;
	if (1){//�ꔭ�ŉ���
		//A^1�����߂�
		double A44[4][4];
		for (int i = 0; i < 4; ++i){
			for (int j = 0; j < 4; ++j){
				A44[i][j] = RefEq().A[i][j];
			}
		}
		double invA44[4][4];
		const bool isInvOK = DsInverseMatrix<4, double>(A44, invA44);
		if (!isInvOK){
			return;//�t�s�񋁂߂��Ȃ������̂ōS���͂͂������ɏI���
		}

		const double wt1_b4[4] = {
			RefEq().max_wplus1[0] - RefEq().b[0],
			RefEq().max_wplus1[1] - RefEq().b[1],
			RefEq().max_wplus1[2] - RefEq().b[2],
			RefEq().max_wplus1[3] - RefEq().b[3],
		};

		double lambda4[4];
		DsMathUtil::MultiVec<4, 4>(lambda4, invA44, wt1_b4);
		RefEq().lambda[0] = Clamp(lambda4[0], -maxLambda, maxLambda);
		RefEq().lambda[1] = Clamp(lambda4[1], -maxLambda, maxLambda);
		RefEq().lambda[2] = Clamp(lambda4[2], -maxLambda, maxLambda);
		RefEq().lambda[3] = Clamp(lambda4[3], -maxLambda, maxLambda);
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
